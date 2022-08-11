@echo off
::============================================================================
:: Batch file to program the flash of Arduino-UNO
:: 
:: NOTE: requires the AVRDUDE uitlity which is included in the QTools
:: collection
:: 
setlocal

@echo off
@echo Load a given hex file to the flash of Arduino-UNO
@echo usage:   flash hex-file [COM-port]
@echo example: flash dbg\blinky-qk.hex
@echo example: flash dbg\blinky-qk.hex COM5

::----------------------------------------------------------------------------
:: NOTE: Adjust the following symbol to the location of the
:: AVRDUDE utility on your machine 
::
set AVRDUDE=avrdude.exe

set COM_PORT=COM5
if [%2] NEQ [] set COM_PORT=%2

if ["%~1"]==[""] (
    @echo The hex file missing
    @goto end
)
if not exist %~s1 (
    @echo The hex file '%1' does not exist
    @goto end
)

@echo on
%AVRDUDE% -p atmega328p -c arduino -P %COM_PORT% -b 115200 -D -v -U flash:w:%1:i
@echo off

:end

endlocal