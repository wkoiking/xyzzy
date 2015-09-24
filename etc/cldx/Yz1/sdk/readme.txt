
【開発者向けドキュメント】


１．ファイル

　　readme.txt    : この文書

　　yz1.h         : YZ1.DLLのC/C++用ヘッダーファイル
　　yz1api.txt    : YZ1.DLLのAPI説明書
　　yz1cmd.txt    : YZ1.DLLのコマンド説明書
　　yz1static.yz1 : YZ1.DLLをスタティックリンクで使うための説明

　　unyz1.h       : UNYZ1.DLLのC/C++用ヘッダーファイル
　　unyz1api.txt  : UNYZ1.DLLのAPI説明書
　　unyz1cmd.txt  : UNYZ1.DLLのコマンド説明書
　　unyz1src.yz1  : UNYZ1.DLLのソース
　　unyz1set.exe  : UNYZ1.DLLの設定のためのアプリケーション


２．使い方

　　　LoadLibrary/GetProcAddress によってダイナミックリンクを
　　行う方法をお薦めします。
　　yz1static.yz1内のyz1.libや、unyz1src.yz1内のunyz1.libを
　　リンクすることで静的リンクで使うことも可能です。


３．DLL間の関係

　　YZ1.DLLが圧縮用、UNYZ1.DLLが解凍用…では『ありません』。(^^;
　どちらのDLLでも圧縮・解凍・パスワード利用・自己解凍書庫作成など、
　一通りの操作は可能となっています。
　UNYZ1.DLL 0.03 以降では、内部で YZ1.DLL を呼び出します。


４．32bit制限について

　　両DLLは、2GB越のファイルに対応していません。
　2GBを超えるサイズのファイルを扱おうとしたときの動作は未定義です。
