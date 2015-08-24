/* main() in aish.cpp ���Ԃ��߂�l */

#if !defined(WDN_AISHERR_H)
#define WDN_AISHERR_H

// �x�� - ���Y�t�@�C���ɑ΂��Ă̏������X�L�b�v���邾���ŁA���s�𒆎~��
//        �邱�Ƃ͂Ȃ�
//
// �� #define �㕔�́u//�v�ɂ��R�����g�́A�o���������x�̂��̂ł��B
// �@ ��萳�m�ȉ���́AAISHERR.txt ���������������B
//
#define AISH_DISK_SPACE        0x05 // �����̂��߂̃f�B�X�N�̋󂫂�����܂���
#define AISH_READ_ONLY         0x06 // ������̃t�@�C���̓��[�h�E�I�����[�ł�
#define AISH_USER_SKIP         0x07 // ���[�U�ɂ���ĕ������X�L�b�v����܂���
#define AISH_UNKNOWN_TYPE      0x08 // �i�[�t�@�C���̎�ނ�MS-DOS�ň����Ȃ��`��
#define AISH_METHOD            0x09 // AISH32.DLL �ň����Ȃ��i�[���[�h�ł̊i�[
#define AISH_PASSWORD_FILE     0x0A // �i�[�t�@�C���Ƀp�X���[�h���������Ă���
#define AISH_ERROR_VERSION     0x0B // �i�[�t�@�C���̌`���� AISH32.DLL �Ŗ��Ή�
#define AISH_FILE_CRC          0x0C // �i�[�t�@�C���̃`�F�b�N�E�T�����s��
#define AISH_FILE_OPEN         0x0D // �������Ƀt�@�C�����J���܂���ł���
#define AISH_MORE_FRESH        0x0E // ���V�����t�@�C����������ɑ��݂��Ă���
#define AISH_NOT_EXIST         0x0F // �t�@�C���͕�����ɑ��݂��Ă��܂���
#define AISH_ALREADY_EXIST     0x10 // �t�@�C���͕�����ɂ��łɑ��݂��Ă��܂�
#define AISH_TOO_MANY_FILES    0x11 // �i�[�t�@�C���������E�𒴂��Ă���

// �G���[ - �v���I�ȃG���[�ŁA���̎��_�Ŏ��s�𒆎~����
//
// �� #define �㕔�́u//�v�ɂ��R�����g�́A�o���������x�̂��̂ł��B
// �@ ��萳�m�ȉ���́AAISHERR.txt ���������������B
//
#define AISH_MAKEDIRECTORY     0x12 // �f�B���N�g��������ł��܂���
#define AISH_CANNOT_WRITE      0x13 // �������ɏ������݃G���[�������܂���
#define AISH_HUFFMAN_CODE      0x14 // ISH file �̃n�t�}���E�R�[�h�����Ă�
#define AISH_COMMENT_HEADER    0x15 // ISH file �̃R�����g�E�R�[�h�����Ă�
#define AISH_HEADER_CRC        0x16 // ISH file �̃w�b�_�� check sum �����Ă�
#define AISH_HEADER_BROKEN     0x17 // ISH file �̃w�b�_�����Ă���
#define AISH_ARC_FILE_OPEN     0x18 // ISH file ���J�����Ƃ��ł��܂���
#define AISH_NOT_ARC_FILE      0x19 // �w�肵���t�@�C���� ISH file �ł͂Ȃ�
#define AISH_CANNOT_READ       0x1A // ISH file �̓ǂݍ��ݎ��ɃG���[����������
#define AISH_FILE_STYLE        0x1B // �w�肳�ꂽ�t�@�C���� ISH file �ł͂Ȃ�
#define AISH_COMMAND_NAME      0x1C // �R�}���h�w�肪�Ԉ���Ă��܂�
#define AISH_MORE_HEAP_MEMORY  0x1D // ��Ɨp�̃q�[�v�E���������s�����Ă��܂�
#define AISH_ENOUGH_MEMORY     0x1E // �O���[�o���E���������s�����Ă��܂�
#define AISH_ALREADY_RUNNING   0x1F // ���łɑ��̃v���Z�X�� AISH32.DLL ���g�p��
#define AISH_USER_CANCEL       0x20 // ���[�U�ɂ���ĕ����𒆎~����܂���
#define AISH_HARC_ISNOT_OPENED 0x21 // �܂� AIshOpenArchive() ����ĂȂ��̂�
#define AISH_NOT_SEARCH_MODE   0x22 // �s���� AIshFindNext()/AIshGetFileName()
#define AISH_NOT_SUPPORT       0x23 // AISH32.DLL ���T�|�[�g�� API �Ăяo��
#define AISH_TIME_STAMP        0x24 // ���t/���Ԃ̌`�����ԈႦ�Ă��܂�
#define AISH_TMP_OPEN          0x25 // ��ƃt�@�C�����J���܂���
#define AISH_LONG_FILE_NAME    0x26 // �f�B���N�g���̃p�X���������܂�
#define AISH_ARC_READ_ONLY     0x27 // �������ݐ�p�� ISH file �͑���ł��Ȃ�
#define AISH_SAME_NAME_FILE    0x28 // ���łɓ����t�@�C���� ISH �Ɋi�[����Ă�
#define AISH_NOT_FIND_ARC_FILE 0x29 // �w�肳�ꂽ dir �� ISH file �͂Ȃ�����
#define AISH_RESPONSE_READ     0x2A // ���X�|���X�E�t�@�C���ǂݍ��݃G���[
#define AISH_NOT_FILENAME      0x2B // �t�@�C�����̎w�肪�K�v�ł�
#define AISH_TMP_COPY          0x2C // Tmp �t�@�C���̏��ɂւ̏����߂��ł���

/* AISH �Ǝ��̌x���R�[�h�i�����𒆒f���Ȃ��j */
//                             0x81

/* AISH �Ǝ��̃G���[�E�R�[�h�i�����𒆒f����j */
#define AISH_CANT_SPLIT        0xA1 // �G���R�[�h���ɕ����ł��Ȃ�
#define AISH_CANT_READ_INFILE  0xA2 // encode ���邽�߂� file ���J���܂���
#define AISH_TOO_MANY_VOLUMES  0xA3 // �������̎w�肪��������


#endif /* aisherr.h */
