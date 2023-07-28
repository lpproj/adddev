readme for adddev/deldev v2.55 patch
====================================

ADDDEV/DELDEVは、本来DOS起動時しかインストールできない文字型デバイスを、
コマンドライン上からインストールできるようにするためのコマンドで、主に
メモリ常駐量の多い日本語FEPを一時的に利用可能にする目的で使われます。

動作は日本語版MS-DOSに付属しているADDDRV/DELDRVとほぼ同じです。
使用法に関しては、オリジナルのバージョン2.55に付属していたマニュアル
（adddev.man）を参照してください。

v2.55 patch版はADDDEV/DELDEVのバージョン2.55に対して多少の修正を行った
ものです。DOSバージョン7以上とDOSBox方面の対策、そのほか若干の利便性改善が
含まれています。

主な変更点：

(v2.55 p2)

  - DEVICEHIGH 暫定対応（動作はDEVICEと完全に同じでUMBにはロードされない。また/L、/Sオプションにも未対応）
  - REM 対応（#でのコメントと異なり、SILENTオプション未対応）
  - DOSBox専用版を通常版と統合
  - ドライバが常駐せず終了した場合のハングアップ／メモリ破壊の修正
  - 日本語エラーメッセージ
  - ADDDEV作成補助ツール（exehigh.c）

(v2.55 p1)

  - ソースのTurbo Assembler対応
  - COMMAND.COM環境変数領域が見つからない場合は親プロセスのものを使う
  - DOSBoxのPSP作成ファンクション対策
  - バージョンチェック緩和（DOS 9.xまで許容。ついでにOS/2 MVDMも許容）
  - adddev /? でヘルプ表示（エラー時はヘルプメッセージを出さない）


ディレクトリ構成
----------------

  adddev.exe              adddev/deldev v2.55 patched（日本語版）
  deldev.exe
  english/
      adddev.exe          (messages in English)

実のところ、deldev.exe はバージョン表記以外のソースコードを全く変更して
いないため、どれを使っても結果は同じです（オリジナルの v2.55 でも）。


再構築
------

オリジナルのadddev/deldevはSLR SystemsのOPTASMを使ってアセンブルされている
ようですが、現在は入手が困難であるため、BorlandのTurbo Assemblerを使って
アセンブルを行っています。リアルモードのTASMではメモリ不足でアセンブルが
失敗するため、tasmxかtasm32が必要です。
またadddev.exeのヘッダ調整のためTurbo C(++)もしくはBorland C++が必要です。

    make -f Makefile.bor


