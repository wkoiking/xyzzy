#include <windows.h>
#include <stdio.h>

typedef BOOL (WINAPI * ZIP_QUERYENCRYPTION)(VOID); // ZipQueryEncryption

void main()
{
	char t[64] = "ZIP32J.DLL �Í������\�����ׂ�̂͂ǂ����̂��H";
	char m[64] = "�G���[";
	
	HINSTANCE hDll = LoadLibrary("zip32j.dll");
    if (hDll)
    {
		ZIP_QUERYENCRYPTION fpZipQueryEncryption = (ZIP_QUERYENCRYPTION) GetProcAddress(hDll, "ZipQueryEncryption");
		if (fpZipQueryEncryption)
		{
			sprintf(m, "ZIP�`���̈��k���Ƀp�X���[�h��t���ł�%s�B", ((*fpZipQueryEncryption)()) ? "�܂�" : "�܂���");
		} else strcpy(m, "�����ɕK�v�ȁAAPI�̃G���g���[�A�h���X�������܂���B");
		FreeLibrary(hDll);
    }
	else strcpy(m, "zip32j.dll������܂���");
	MessageBox(NULL, m, t, MB_OK);
}