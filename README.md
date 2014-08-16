AVR-ISP
=======

An AVR serial programmer based on the STM32F4 Discovery board.

Currently this works with its own PC side client, but will make it avrdude compatible, if I have enough time.
At this time it has a limited functionality, but can be used to program bootloader, or make your own USB programmer.
(Solve the chicken and egg problem).

Total hardware is cheap as it includes only the STM32F4-Discovery board, which can be had for 15-20 USD.

Usage
-----

Download and install the ST-LINK utility and STM32 VCP driver, both are available at ST website.
Use the ST-LINK to flash the ISP/release/ISP.hex on the Discovery board. Connect the ISP pins to the AVR and use the client program to burn AVR.

For PC client use prog\prog\bin\Debug\prog.exe

```sh
prog.exe --hex="PATH/TO/HEX" --mcu=MCU_TYPE --port=PORT:BAUDRATE
```
E.g.
```sh
prog.exe --hex="C:\a.hex" --mcu=8 --port=COM13:115200
```

License
-------

GNU GPLv3

isp.h and isp.c are modified from the USBASP project, which is licensed under GPLv2, for more details see USBASP project.
