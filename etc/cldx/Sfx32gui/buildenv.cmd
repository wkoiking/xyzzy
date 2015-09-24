@echo off

echo ****************************************************************
echo *      この Windows NT コマンドスクリプトを実行するには、      *
echo *      patch.exe, unzip.exe をパスの通ったディレクトリに       *
echo *                  用意する必要があります。                    *
echo *コマンドスクリプトを中止する場合は、Ctrl + Cを押してください。*
echo ****************************************************************
pause

if not exist unzip550.zip goto nofile
if not exist sfx32gui.dif goto nofile
if not exist sfx32gui.ico goto nofile

echo 'unzip551.zip' を 'origsrc' へ解凍します。

if exist origsrc rmdir /S /Q origsrc
unzip unzip551.zip -d .\origsrc

echo 'origsrc' に解凍されたソースを 'src' へコピーします。

if exist src rmdir /S /Q src
mkdir src
xcopy origsrc\*.* src /S /E /V /Q

echo 'src' 配下のソースにパッチを当てます。

cd src
patch --dos < ..\sfx32gui.dif
rem patch < ..\sfx32gui.dif

echo ビルドに必要なアイコンファイルをコピーします。

copy ..\sfx32gui.ico .

cd ..

echo 正常終了！

goto exitlabel
:nofile
echo この Windows NT コマンドスクリプト を実行するには、以下のファイルがこのディレクトリに必要です。
echo unzip551.zip, sfx32gui.dif, sfx32gui.ico
:exitlabel
pause
