@echo off
REM #################################################
REM Edit this line below to make it match your setup.

set PS2GCC=C:\ps2dev\gcc

REM #################################################


set path=%path%;%ps2gcc%\share;%ps2gcc%\ee;%ps2gcc%\ee\bin;%ps2gcc%\iop;%ps2gcc%\iop\bin;
set path=%path%;%ps2gcc%\ee\lib\gcc-lib\mips64r5900-sce-elf\2.9-ee-991111b;%ps2gcc%\ee\mips64r5900-sce-elf\bin
set path=%path%;c:\ps2dev\gcc\share;

set PS2DEV=c:/ps2dev/gcc
rem set PS2DEV=c:/ps2lib
set EE=%PS2DEV%/ee
set IOP=%PS2DEV%/iop
set PS2LIB=%PS2DEV%
rem export PS2DEV EE IOP PS2LIB PATH
