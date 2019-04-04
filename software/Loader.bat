mode
echo Enter the com port number that connected to the board (e.g: 4,5,6)
set /p a=COM
REM set a=8
set rec=program.rec

mode com%a% baud=115200 parity=n data=8 stop=1 to=off xon=off odsr=off octs=off dtr=off rts=off idsr=off
type %rec% >\\.\COM%a% && exit
echo SOMETHING WENT WRONG! are you sure %rec% exists HERE?
echo this must be ran from within the same directory as %rec%
pause
