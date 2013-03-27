all:
	-@echo "================================================================"
	-@echo "!!! Do it on Win32 system with uVision2 (ver.7.5) installed!"
	-@echo "================================================================"

clean:
	rm -f *.lst *.obj *.LST *.OBJ *.BIN *.M51 *.m51 *.hex *.HEX *.LNP *.lnp *.PLG *.plg *.__i *.__I *.SRC *.src *.Bak Wm9707 Si4711

cleanup:
	cat /usr/src/DFU/clean0.bin | /usr/src/DFU/i2c-tools/eepromer/eeprog -16 /dev/i2c-2 0x50 -w 0

install:
	/usr/src/DFU/dfu-util/src/dfu-util -D APP7.BIN -v -R

run:
	:/usr/src/DFU/cfg-app-console/src/si471xCfg -c /usr/src/DFU/cfg-app-console/conf/fmtrans-digital.conf -vvvvv