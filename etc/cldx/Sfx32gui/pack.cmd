@echo off
echo ****************************************************************
echo *      ���� Windows NT �R�}���h�X�N���v�g�����s����ɂ́A      *
echo *   upx.exe, diff.exe, zip.exe ���p�X�̒ʂ����f�B���N�g����    *
echo *                  �p�ӂ���K�v������܂��B                    *
echo *�R�}���h�X�N���v�g�𒆎~����ꍇ�́ACtrl + C�������Ă��������B*
echo ****************************************************************
pause

echo �t�@�C���̃`�F�b�N
if not exist src\release\unzipsfx.exe goto nofile
if not exist src\sfx32gui.ico goto nofile
if not exist buildenv.cmd goto nofile

set arcname=zips3004

echo �s�v�ȃt�@�C���̍폜
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

echo �Ĕz�z�p�p�b�P�[�W�f�B���N�g���̏���
if exist package rmdir /S /Q package
mkdir package

echo �����t�@�C���쐬
if exist sfx32gui.dif del sfx32gui.dif
rem diff -urN --dos origsrc src> sfx32gui.dif
diff -urN origsrc src> sfx32gui.dif

echo �Ĕz�z�p�t�@�C���̏���
copy sfx32gui.txt package
upx --best src\release\unzipsfx.exe
copy src\release\unzipsfx.exe package\sfx32gui.dat
copy sfx32gui.dif package
copy src\sfx32gui.ico package
copy pack.cmd package
copy buildenv.cmd package
rem copy ReadMe.Txt package

echo ���ɉ�
cd package
dir /B | zip -9 -v -@ %arcname%.zip
copy %arcname%.zip ..
cd ..

echo �����I

goto endlabel
:nofile
echo ���� Windows NT �R�}���h�X�N���v�g�����s����ɂ́A�ȉ��̃t�@�C�����K�v�ł��B
echo src\release\unzipsfx.exe
echo src\sfx32gui.ico
echo buildenv.cmd

:endlabel
pause
