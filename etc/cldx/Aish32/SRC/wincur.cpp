// wincur.cpp - Windows Cursor Control Class
//
// Copyright (C) 1999 Hiroyuki Shimada. All rights reserved.

#include "wincur.h"

namespace shimaden {

TWinCursor::TWinCursor(HWND hwnd, HINSTANCE hinst, bool recover)
	: f_hwnd(hwnd), f_hinst(hinst), f_recover(recover)
// ����
//     hwnd   : �E�B���h�E�E�n���h��
//     hinst  : �A�v���P�[�V�����E�C���X�^���X�̃n���h��
//              ����̈Ӗ��ɂ��Ă� LoadCursor WIN32 API �̃w���v���Q�Ƃ̂���
//     recover: �f�X�g���N�^���s���ɁA�J�[�\����������Ԃɖ߂����ǂ���
// �߂�l
//     �Ȃ�
{
	f_hCurs = 0;
}

TWinCursor::~TWinCursor()
{
	if(f_hCurs) {
		if(f_recover) {
			::SetClassLong(f_hwnd, GCL_HCURSOR,
				reinterpret_cast<LONG>(f_hCurs));
		}
	}
}

HCURSOR TWinCursor::SetCursor(LPCTSTR cursor)
// �J�[�\���� cursor �ɕύX����
// ����
//     cursor: �J�[�\����������܂��̓J�[�\���E���\�[�X���ʎq
//             ����̈Ӗ��� LoadCursor WIN32 API �̃w���v���Q�Ƃ̂���
// �߂�l
//     ���s: 0
//     ����: �ȑO�̃J�[�\���̃n���h��
{
	HCURSOR hCurs;
	HCURSOR ret;

	hCurs = ::LoadCursor(f_hinst, cursor); // �J�[�\���̃n���h�������[�h
	if(hCurs) {
		if(f_hCurs == 0)
			f_hCurs = hCurs;
		ret = reinterpret_cast<HCURSOR>(::SetClassLong(f_hwnd, GCL_HCURSOR,
			reinterpret_cast<LONG>(hCurs)));
	} else {
		ret = 0;
	}

	return ret;
}

HCURSOR TWinCursor::GetCursor()
// ���݂̃J�[�\���̃n���h����Ԃ�
// �߂�l
//     ���s: 0
//     ����: ���݂̃J�[�\���̃n���h��
{
	return reinterpret_cast<HCURSOR>(::GetClassLong(f_hwnd, GCL_HCURSOR));
}


} // namespace shimaden
