set arcname=zip3j025
rm -rf package
mkdir package

cp zip32j.txt zip32j_e.txt package\
cp c:\windows\system\zip32j.dll package\

mkdir package\sdk
mkdir package\src
mkdir package\src\zip32j
mkdir package\src\zip32jex
mkdir package\src\minizip

cp sdk\*.txt package\sdk
cp zip32j\release\zip32j.lib sdk\
cp zip32j\zip32j.h sdk\

cp pack.bat package\src
cp zip32j\{*.c,*.h,*.rc,*.def,*.dsw,*.dsp} package\src\zip32j\
cp zip32jex\*.dsp package\src\zip32jex\
cp minizip\*.c minizip\*.dsp minizip\debug\minizip.exe package\src\minizip\

cp sdk\*.txt package\sdk\

cd package
lha a -d %arcname%.lzh
' lha s -d %arcname%
'cp %arcname%.exe ..
cp %arcname%.lzh ..
cd ..

