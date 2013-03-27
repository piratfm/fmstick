#include <hid.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> /* for getopt() */

#include "logging.h"
#define P1 0
#define P3 0
#define sbit int
#include "gpio.h"


#include "eepromConfig.h"

bool match_serial_number(struct usb_dev_handle* usbdev, void* custom, unsigned int len)
{
  bool ret;
  char* buffer = (char*)malloc(len);
  usb_get_string_simple(usbdev, usb_device(usbdev)->descriptor.iSerialNumber,
      buffer, len);
  ret = strncmp(buffer, (char*)custom, len) == 0;
  free(buffer);
  return ret;
}

int main(int argc, char *argv[])
{
  HIDInterface* hid;
  int iface_num = 2;
  hid_return ret;
  int index=0;
  char buff[42];
  unsigned short vendor_id  = 0x0451;
  unsigned short product_id = 0x2100;
  int flag;
  uint16_t addr=0;

  uint8_t eepromBuff[512];
  memset(eepromBuff, 0x00, 512);

  // HIDInterfaceMatcher matcher = { 0x0925, 0x1237, NULL, NULL, 0 };
  HIDInterfaceMatcher matcher = { vendor_id, product_id, NULL, NULL, 0 };

#if 1
  /* see include/debug.h for possible values */
  hid_set_debug(HID_DEBUG_ALL);
  hid_set_debug_stream(stderr);
  /* passed directly to libusb */
  hid_set_usb_debug(0);
  
  ret = hid_init();
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_init failed with return code %d\n", ret);
    return 1;
  }

  hid = hid_new_HIDInterface();
  if (hid == 0) {
    fprintf(stderr, "hid_new_HIDInterface() failed, out of memory?\n");
    return 1;
  }

  ret = hid_force_open(hid, iface_num, &matcher, 3);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_force_open failed with return code %d\n", ret);
    return 1;
  }

  ret = hid_write_identification(stdout, hid);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_write_identification failed with return code %d\n", ret);
    return 1;
  }

  ret = hid_dump_tree(stdout, hid);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_dump_tree failed with return code %d\n", ret);
    return 1;
  }

//#define _READ
#ifdef _READ
  memset(eepromBuff, 0x00, 512);
  for (index=0;index < 64;index++) {
      memset(buff, 0x00, 42);
      addr = 8*index;
      buff[0] = PCTransfer;
      buff[1] = RequestEepromSectionRead;
      buff[2] = (0xFF00 & addr) >> 8; //M:offset
      buff[3] = 0x00FF & addr; //L:offset
      buff[4] = 8; //bytes

      fprintf(stderr, "sending:\n");
      print_bytes(LOG_ERROR, buff, 42);

      ret = hid_interrupt_write(hid, 0x04, buff, 42, 1000);
      if (ret != HID_RET_SUCCESS) {
        fprintf(stderr, "hid_set_output_report failed with return code %d\n", ret);
      }
      ret = hid_interrupt_read(hid, 0x03, buff, 42, 1000);
      if (ret != HID_RET_SUCCESS) {
        fprintf(stderr, "hid_set_output_report failed with return code %d\n", ret);
      }

      fprintf(stderr, "readed:\n");
      print_bytes(LOG_ERROR, buff, 42);

      if(buff[0] & PCRequestError) {
        logwrite(LOG_ERROR, "USB: request error");
      }

      if(!(buff[1] & RequestDone)) {
        logwrite(LOG_ERROR, "USB: request is not done!");
      }

      logwrite(LOG_ERROR, "USB: retval: %02x", buff[2]);
      memcpy(&eepromBuff[8*index], &buff[3], 8);
  }
#endif

#define _WRITE
#ifdef _WRITE
  memset(eepromBuff, 0xFF, 512);
  int commands = eeprom_data_size()/8;
  if(commands < 0 || commands > 62) {
    logwrite(LOG_ERROR, "WRONG COMMANDS NUMBER: %d", commands);
    exit(1);
  }
  logwrite(LOG_ERROR, "total commands: %d", commands);
  /* 2 bytes of sync + number of commands! */
  eepromBuff[0] = 0xb0;
  eepromBuff[1] = 0x0b;
  eepromBuff[2] = commands;

  for (index=0;index < commands;index++) {
      fprintf(stderr, "w%d: eepromBuff[%d], eepromData[%d]\n", index, (index+1)*8, index*8);
      memcpy(&eepromBuff[(index+1)*8], &eepromData[8*index], 8);
  }

  for (index=0;index <= commands;index++) {
      addr = 8*index;
      memset(buff, 0x00, 42);
      buff[0] = PCTransfer;
      buff[1] = RequestEepromSectionWrite;
      buff[2] = (0xFF00 & addr) >> 8; //M:offset
      buff[3] = 0x00FF & addr; //L:offset
      buff[4] = 8; //bytes
      memcpy(&buff[5], &eepromBuff[8*index], 8);

      fprintf(stderr, "sending:\n");
      print_bytes(LOG_ERROR, buff, 42);

      ret = hid_interrupt_write(hid, 0x04, buff, 42, 1000);
      if (ret != HID_RET_SUCCESS) {
        fprintf(stderr, "hid_set_output_report failed with return code %d\n", ret);
      }
      ret = hid_interrupt_read(hid, 0x03, buff, 42, 1000);
      if (ret != HID_RET_SUCCESS) {
        fprintf(stderr, "hid_set_output_report failed with return code %d\n", ret);
      }

      fprintf(stderr, "readed:\n");
      print_bytes(LOG_ERROR, buff, 42);

      if(buff[0] & PCRequestError) {
        logwrite(LOG_ERROR, "USB: request error");
      }

      if(!(buff[1] & RequestDone)) {
        logwrite(LOG_ERROR, "USB: request is not done!");
      }

      logwrite(LOG_ERROR, "USB: retval: %02x", buff[2]);
  }
#endif

  ret = hid_close(hid);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_close failed with return code %d\n", ret);
    return 1;
  }

  hid_delete_HIDInterface(&hid);

  ret = hid_cleanup();
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_cleanup failed with return code %d\n", ret);
    return 1;
  }
#endif

  fprintf(stderr, "EEPROM DATA:\n");
  for (index=0;index < 64;index++) {
    print_bytes(LOG_ERROR, &eepromBuff[8*index], 8);
  }


  return 0;
}
