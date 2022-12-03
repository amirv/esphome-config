Guru Meditation Error: Core  1 panic'ed (InstrFetchProhibited). Exception was unhandled.
Core 1 register dump:
PC      : 0xffffffff  PS      : 0x00060c30  A0      : 0x800e0eb5  A1      : 0x3ffb1f40
A2      : 0x3ffb9460  A3      : 0x0001236c  A4      : 0x00000000  A5      : 0x3ffd3a00
A6      : 0x3ffd3a4c  A7      : 0x00000000  A8      : 0x8016b8d0  A9      : 0x00000002
A10     : 0x3ffb9460  A11     : 0x3ffcec8c  A12     : 0x3ffcec8c  A13     : 0x3ffcec8c
A14     : 0x00000000  A15     : 0x3ffb0060  SAR     : 0x0000000a  EXCCAUSE: 0x00000014
EXCVADDR: 0xfffffffc  LBEG    : 0x4000c2e0  LEND    : 0x4000c2f6  LCOUNT  : 0xffffffff

ELF file SHA256: 0000000000000000

Backtrace: 0x7fffffff:0x3ffb1f40 0x400e0eb2:0x3ffb1f60 0x400e368e:0x3ffb1f90 0x400f1139:0x3ffb1fb0 0x40089ce6:0x3ffb1fd0

Rebooting...
ets Jun  8 2016 00:22:57

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0018,len:4
load:0x3fff001c,len:1044
load:0x40078000,len:10124
load:0x40080400,len:5828
entry 0x400806a8
