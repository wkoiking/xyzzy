@echo off

echo ****************************************************************
echo *      ���� Windows NT �R�}���h�X�N���v�g�����s����ɂ́A      *
echo *      patch.exe, unzip.exe ���p�X�̒ʂ����f�B���N�g����       *
echo *                  �p�ӂ���K�v������܂��B                    *
echo *�R�}���h�X�N���v�g�𒆎~����ꍇ�́ACtrl + C�������Ă��������B*
echo ****************************************************************
pause

if not exist unzip550.zip goto nofile
if not exist sfx32gui.dif goto nofile
if not exist sfx32gui.ico goto nofile

echo 'unzip551.zip' �� 'origsrc' �։𓀂��܂��B

if exist origsrc rmdir /S /Q origsrc
unzip unzip551.zip -d .\origsrc

echo 'origsrc' �ɉ𓀂��ꂽ�\�[�X�� 'src' �փR�s�[���܂��B

if exist src rmdir /S /Q src
mkdir src
xcopy origsrc\*.* src /S /E /V /Q

echo 'src' �z���̃\�[�X�Ƀp�b�`�𓖂Ă܂��B

cd src
patch --dos < ..\sfx32gui.dif
rem patch < ..\sfx32gui.dif

echo �r���h�ɕK�v�ȃA�C�R���t�@�C�����R�s�[���܂��B

copy ..\sfx32gui.ico .

cd ..

echo ����I���I

goto exitlabel
:nofile
echo ���� Windows NT �R�}���h�X�N���v�g �����s����ɂ́A�ȉ��̃t�@�C�������̃f�B���N�g���ɕK�v�ł��B
echo unzip551.zip, sfx32gui.dif, sfx32gui.ico
:exitlabel
pause
