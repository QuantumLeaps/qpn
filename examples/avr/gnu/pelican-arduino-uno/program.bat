
set ARDUINO=C:\tools\arduino-0021

%ARDUINO%\hardware\tools\avr\bin\avrdude -C%ARDUINO%\hardware\tools\avr\etc\avrdude.conf -v -v -v -v -patmega328p -cstk500v1 -P%1 -b115200 -D -Uflash:w:%2:i 