@echo off
echo ****************************************************************
echo *      この Windows NT コマンドスクリプトを実行するには、      *
echo *   upx.exe, diff.exe, zip.exe をパスの通ったディレクトリに    *
echo *                  用意する必要があります。                    *
echo *コマンドスクリプトを中止する場合は、Ctrl + Cを押してください。*
echo ****************************************************************
pause

echo ファイルのチェック
if not exist src\release\unzipsfx.exe goto nofile
if not exist src\sfx32gui.ico goto nofile
if not exist buildenv.cmd goto nofile

set arcname=zips3004

echo 不要なファイルの削除
cd src
if exist *.bak del *.bak
if exist *.~?? del *.~??
if exist *.$$$ del *.$$$
if exist *.obr del *.obr
if exist *.!?? del *.!??
if exist *.mbt del *.mbt
if exist *.mrt del *.mrt
if exist *.r$p del *.r$p
if exist *.csm del *.csm
if exist *.il* del *.il*
if exist *.tds del *.tds
if exist *.aps del *.aps
if exist *.pch del *.pch
if exist *.idb del *.idb
if exist *.pdb del *.pdb
if exist *.ilk del *.ilk
if exist *.ncb del *.ncb
if exist *.opt del *.opt
if exist *.plg del *.plg
if exist *.orig del *.orig
if exist defug rmdir /S /Q debug
cd Release
if exist *.obj del *.obj
if exist *.sbr del *.sbr
if exist *.pch del *.pch
if exist *.ich del *.ich
if exist *.res del *.res
if exist *.idb del *.idb
if exist *.bsc del *.bsc
cd ..\..

echo 再配布用パッケージディレクトリの準備
if exist package rmdir /S /Q package
mkdir package

echo 差分ファイル作成
if exist sfx32gui.dif del sfx32gui.dif
rem diff -urN --dos origsrc src> sfx32gui.dif
diff -urN origsrc src> sfx32gui.dif

echo 再配布用ファイルの準備
copy sfx32gui.txt package
upx --best src\release\unzipsfx.exe
copy src\release\unzipsfx.exe package\sfx32gui.dat
copy sfx32gui.dif package
copy src\sfx32gui.ico package
copy pack.cmd package
copy buildenv.cmd package
rem copy ReadMe.Txt package

echo 書庫化
cd package
dir /B | zip -9 -v -@ %arcname%.zip
copy %arcname%.zip ..
cd ..

echo 成功！

goto endlabel
:nofile
echo この Windows NT コマンドスクリプトを実行するには、以下のファイルが必要です。
echo src\release\unzipsfx.exe
echo src\sfx32gui.ico
echo buildenv.cmd

:endlabel
pause
