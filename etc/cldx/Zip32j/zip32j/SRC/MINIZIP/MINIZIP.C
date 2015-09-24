#include <windows.h>
#include <stdio.h>

typedef BOOL (WINAPI * ZIP_QUERYENCRYPTION)(VOID); // ZipQueryEncryption

void main()
{
	char t[64] = "ZIP32J.DLL 暗号化が可能か調べるのはどうやるのさ？";
	char m[64] = "エラー";
	
	HINSTANCE hDll = LoadLibrary("zip32j.dll");
    if (hDll)
    {
		ZIP_QUERYENCRYPTION fpZipQueryEncryption = (ZIP_QUERYENCRYPTION) GetProcAddress(hDll, "ZipQueryEncryption");
		if (fpZipQueryEncryption)
		{
			sprintf(m, "ZIP形式の圧縮時にパスワードを付加でき%s。", ((*fpZipQueryEncryption)()) ? "ます" : "ません");
		} else strcpy(m, "処理に必要な、APIのエントリーアドレスが得られません。");
		FreeLibrary(hDll);
    }
	else strcpy(m, "zip32j.dllがありません");
	MessageBox(NULL, m, t, MB_OK);
}
