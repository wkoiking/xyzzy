# Installation

���ϐ�XYZZYHOME��ݒ肷��B

## ���[���[

### KamailV3

�p�X��ǉ��F

`~\xyzzy\hyperestraier\`

�f�[�^�t�@�C����`C:/Mail`�ɒu���B

�ݒ��`C:\Mail\config.l`������������

��Gmail�ɓY�t��������R�s�y����Ɨǂ�

### stunnel�̐ݒ�

* google��stunnel���������ăC���X�g�[��
* �ݒ��`C:\Program Files (x86)\stunnel\config\stunnel.conf`������������

* stunnel�ŃG���[���o��ꍇ
    * stunnel���A�b�v�f�[�g���Ă݂�

��Gmail�ɓY�t��������R�s�y����Ɨǂ�

### Gmail�Ƃ̓������x���グ��

�ȉ����g���F

* tamper monkey
* Gmail POP3 Checker

### Mail�t�H���_�̊T�v

* Main
    * attachments
        * ���b�Z�[�W�̓]�����s���ꍇ�ɁA���̃��b�Z�[�W�̓Y�t�t�@�C���������I�ɕۑ������ꏊ
        * M-x: km3::message-cleanup-attachments�ō폜�ł���
    * mail
        * ���܂�fetch�������ׂă��[���̎��̂��ۊǂ���Ă���
        * �Y�t�t�@�C���̏���MIME�Ƃ��Ė��ߍ��܂�Ă���
    * data
        * fetch-count.dat
            * ���܂�fetch�������[���̑���
        * he-gather.txt
            * �Ō��fetch�������[���̕ۑ��ӏ����L������Ă���
    * hedump
        * casket�̒��g���e�L�X�g�̃t�H�[�}�b�g��dump��������
        * M-x: km3::he-dump�Ő��������
        * M-x: km3::he-restore��dump����casket���č\�z�����
    * casket
        * Hyper Estrailer�̃C���f�b�N�X���ۊǂ���Ă���

### ���[���̊Ǘ�

* Mail/mail�̃o�b�N�A�b�v�����I�ɂƂ��Ă���
* �������[�����d�����܂����A�s�������������ꍇ��kamail-analyse�Ȃǂ��g���ă��[������́E��������
    * Mail/mail�̃��[���{�̂���Mail/hedump�̃��[���̃_�C�W�F�X�g����͂��������y
    * ����digest�Ń��[���̎��̂����ꂩ�ǂ�������ł���
* ��́E�������I�������
    * mail��casket���o�b�N�A�b�v����
    * ���L�̃R�}���h�Ő�����̃��[���̎��̂���Mail/mail�����Mail/casket���č\�z����

~~~
M-x: kamail3                       <= KaMailV3���N�����Ă���
M-x: load-library
Load library: kamail3/import
M-x: import-mh-folder
Folder: C:/some/backup/mail        <= ����������̃��[�����i�[���ꂽ�t�H���_
Ignore Folders: trash spam draft   <= �C���|�[�g���Ȃ��t�H���_���X�y�[�X��؂�Ŏw��i���K�\���j
~~~

4���Ԃ��炢������̂Ŗ�ɂ��ׂ��I

## �t�@�C���i�A�[�J�C�o�̃C���X�g�[���j

���k��𓀂��s���ɂ́C�����A�[�J�C�o�v���W�F�N�g�̃��C�u�������K�v�ƂȂ�
�܂��B������ Windows �� System �f�B���N�g��(`C:/Windows/system/` �Ƃ� 
`C:/WINNT/system32/` �Ƃ�)�ɓ���Ă����Ă��������B

DLL �́C���c����̓����A�[�J�C�o�v���W�F�N�g����_�E�����[�h�ł��܂��B

`http://www.csdinc.co.jp/archiver/`

�܂��CK.INABA ����� caldix ���g���ƊȒP�ɂ܂Ƃ߂ăC���X�g�[���ł��܂��B

`http://www.kmonos.net/lib/`

���ݎg�p�ł����Ԃɂ��郉�C�u�����́C[�w���v]��[�ɂ���] �Ŋm�F���邱�Ƃ��ł��܂��B

## Graphics Magik�̃C���X�g�[��

1.3.36�͓����Ȃ������̂�1.3.31�����L����C���X�g�[������F

https://sourceforge.net/projects/graphicsmagick/files/graphicsmagick-binaries/1.3.31/

## filter-for-xyzzy�̃C���X�g�[��

`stack install`����

## ���̑�

�ȉ���`stack install`����

* pandoc.exe
* pandoc-crossref.exe
* hasktags.exe
