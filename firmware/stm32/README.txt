This is Makefile project for STM32F107RC, soldered on STM32VL-Discovery board and changed quartz to 14.7456 MHz

The firmware is not yet supports controlling SI4711 by HID-interface (it's written only partially).
Frontend init load sequence is located in Project/Audio_Streaming/src/audio_codec_si4711.c:92

There is lot of TODO:
------------

1). Change project to use [GNU toolchain:](https://launchpad.net/gcc-arm-embedded) and [ST-LINK utils](http://github.com/texane/stlink).
2). Provide schematic
3). Implement pseudo-EEPROM.
4). Finish HID-API.
5). Add device support to host-machine software.

