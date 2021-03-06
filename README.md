# Installation

環境変数XYZZYHOMEを設定する。

## メーラー

### KamailV3

パスを追加：

`~\xyzzy\hyperestraier\`

データファイルを`C:/Mail`に置く。

設定は`C:\Mail\config.l`を書き換える

※Gmailに添付したやつをコピペすると良い

### stunnelの設定

* googleでstunnelを検索してインストール
* 設定は`C:\Program Files (x86)\stunnel\config\stunnel.conf`を書き換える

※Gmailに添付したやつをコピペすると良い

### Gmailとの同期速度を上げる

以下を使う：

* tamper monkey
* Gmail POP3 Checker

### Mailフォルダの概要

* Main
    * mail
        * 今までfetchしたすべてメールの実体が保管されている
    * data
        * fetch-count.dat
            * 今までfetchしたメールの総数
        * he-gather.txt
            * 最後にfetchしたメールの保存箇所が記憶されている
    * hedump
        * casketの中身をテキストのフォーマットでdumpしたもの
        * M-x: km3::he-dumpで生成される
        * M-x: km3::he-restoreでdumpからcasketが再構築される
    * casket
        * Hyper Estrailerのインデックスが保管されている

### メールの管理

* Mail/mailのバックアップを定期的にとっておく
* 同じメールが重複しまう等、不整合が生じた場合はkamail-analyseなどを使ってメールを解析・整理する
    * Mail/mailのメール本体よりもMail/hedumpのメールのダイジェストを解析した方が楽
    * 特にdigestでメールの実体が同一かどうか判定できる
* 解析・整理が終わったら下記のコマンドで整理後のメールの実体からMail/mailおよびMail/casketを再構築する

~~~
M-x: kamail3                       <= KaMailV3を起動しておく
M-x: load-library
Load library: kamail3/import
M-x: import-mh-folder
Folder: C:/some/backup/mail        <= 整理した後のメールが格納されたフォルダ
Ignore Folders: trash spam draft   <= インポートしないフォルダをスペース区切りで指定（正規表現可）
~~~

## ファイラ（アーカイバのインストール）

圧縮や解凍を行うには，統合アーカイバプロジェクトのライブラリが必要となり
ます。これらを Windows の System ディレクトリ(`C:/Windows/system/` とか 
`C:/WINNT/system32/` とか)に入れておいてください。

DLL は，庄田さんの統合アーカイバプロジェクトからダウンロードできます。

`http://www.csdinc.co.jp/archiver/`

また，K.INABA さんの caldix を使うと簡単にまとめてインストールできます。

`http://www.kmonos.net/lib/`

現在使用できる状態にあるライブラリは，[ヘルプ]→[について] で確認することができます。

## その他

以下を`stack install`する

* pandoc.exe
* pandoc-crossref.exe
* hasktags.exe
