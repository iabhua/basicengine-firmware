## 豊四季タイニーBASIC for Arduino STM32 V0.2

![サンプル画像](./image/sample.jpg)


本プログラムは、下記オリジナル版をArduino STM32向けに移植・機能版です.  
STM32F103C8T6搭載のBlue Pillボード、Black Pillボードでの動作を確認しています.  
※機能の大幅追加のため、Arduino MEGEは非対応となりました.  

- オリジナル版配布サイト  
 https://github.com/vintagechips/ttbasic_arduino  
 関連情報 [電脳伝説 Vintagechips - 豊四季タイニーBASIC確定版](https://vintagechips.wordpress.com/2015/12/06/%E8%B1%8A%E5%9B%9B%E5%AD%A3%E3%82%BF%E3%82%A4%E3%83%8B%E3%83%BCbasic%E7%A2%BA%E5%AE%9A%E7%89%88/)

**「豊四季タイニーBASIC」**の著作権は開発者**のTetsuya Suzuki**氏にあります.  
プログラム利用については、オリジナル版の著作権者の配布条件に従うものとします.  
著作権者の同意なしに経済的な利益を得てはいけません.  
この条件のもとで、利用、複写、改編、再配布を認めます.  

**修正内容**
- ラインエディタ部の差し換え  
 オリジナルのラインエディタ部分をフルスリーンテキストエディタに差し換えました.  
 ターミナル上で昔のBASICっぽい編集操作を出来るようにしました.  

- コマンドの追加  
  - **RENUME** ：行番号再割り当て  
  - **CLS** ：画面クリア  
  - **LOCATE**：カーソル移動  
  - **COLOR**： 文字色の指定  
  - **ATTR**：文字装飾の指定  
  - **WAIT**：時間待ち  
  - **VPEEK**：スクリーン位置の文字コード参照  
  - **CHR$()**、**ASC()**、**INKEY()**関数の追加  
  - **?**：**PRINT**文の省略系を追加  
  - **GPIO**、**OUT**、**IN**、**ANA**：GPIO利用コマンドの追加  
- 文法の変更
  - 命令文区切りを';'から':'に変更  
  - **PRINT**文の行継続を';'でも可能に変更  
  - **IF**文の不一致判定を"<>"でも可能に変更  
  - 演算子 剰余計算'%'を追加  
  - **STOP**命令を**END**に変更  
  - **SIZE**を**FREE**に変更
- 編集機能の変更  
  - 実行中プログラムの中断を**[ESC]**を2回押し、**[CTRL-C]**に変更  
- その他  
  - プログラム領域を256バイトから2048バイトに拡大  
  - 配列サイズを32から100に拡大  
  - エラーメッセージ**Illegal value**の追加  
  - 定数:HIGH、LOW、PA00、PA01、..、PC15のピン名の追加  

本スケッチの利用には、  
別途、mcursesライブラリ(https://github.com/ChrisMicro/mcurses) が必要です。  


## フルスリーンテキストエディタの機能
※利用にはTeraTerm用のシリアル接続可能なターミナルソフトが必要です.  

**スクリーンサイス**  
40列ｘ20行  

**利用可能キー**  
- [←][→][↑][↓] ： カーソル移動 カーソルキー  
- [Delete]、[CTRL-X] ：カーソル位置の文字削除  
- [BackSpace]：カーソル前の文字削除と前に移動  
- [PageUP]、[PageDown]、[CTRL-R]：画面の再表示  
- [HOME]、[END]：行内でカーソルを左端、右端に移動  
- [INS]：挿入・上書きのトグル切り替え  
- [Enter]：行入力確定  
- [ESC]2回押し、[CTRL-C]：実行プログラムの中断
- [CTRL-L]：画面のクリア

## 追加コマンドの説明

### 行番号の再振り付け
- 書式  
  RENUM  
  RENUM 開始行番号  
  RENUM 開始行番号, 増分  

- 引数  
 開始番号：再振り付けをする行番号の開始番号
 増分：行番号間の増分
 
- 説明  
 行番号を再振り付けを行います.  
 引数を省略した場合は、行番号を10行から10刻みで再振り付けを行います.  
 開始番号を指定した場合は、指定した開始番号から10刻みで再振り付けを行います.  
 開始番号と増分を指定した場合は、指定した開始番号から増分で指定した刻みで再振り付けを行います.  
 再振り付けにおいて、GOTO文、GOSUB文で指定した行番号も正しく更新されます.  
 
### 画面クリア
- 書式  
 CLS

- 引数  
 なし  

- 説明  
 画面（ターミナル上の表示）をクリア（表示内容消去）します.  
 カーソルは画面左上の座標(0,0)に移動します.  
 直前にCOLORコマンドで背景色の設定を行っている場合は、背景色でクリアされます。  

### カーソル移動
- 書式  
 LOCATE 横位置, 縦位置  
- 引数  
 横位置: 0～39
 縦位置: 0～19

- 説明  
 文字を表示するカーソルを指定した位置に移動します.  
 指定した座標値が範囲外の場合は範囲の下限または上限に移動します.  

### 文字色の指定

- 書式  
 COLOR 文字色  
 COLOR 文字色, 背景色  

- 引数  
 文字色: 色コード　0～9
 背景色: 色コード　0～9

- 説明  
 文字色の指定を行います.
 指定した色は以降の文字表示に反映されます.  
 利用するターミナルソフトにより正しく表示できない場合があります.  
 画面を[HOME]キーで再表示した場合、色情報は欠落します.  
 
 **色コード**  

 |色コード|意味|
 |:-----:|:-:|
 |0      |黒|
 |1      |赤|
 |2      |緑|
 |3      |茶|
 |4      |青|
 |5      |マゼンタ|
 |6      |シアン|
 |7      |白|
 |8      |黄| 


### 文字表示属性の設定

- 書式  
 ATTR 属性  

- 引数  
 属性: 属性コード　0～4

- 説明  
 文字の表示属性を指定します.  
 指定した表示属性は以降の文字表示に反映されます.  
 利用するターミナルソフトにより正しく表示できない場合があります.  
 画面を[HOME]キーで再表示した場合、色情報は欠落します.  

 **属性コード**  

 |属性コード|意味|
 |:-----:|:-:|
 |0      |標準|
 |1      |下線|
 |2      |反転|
 |3      |ブリンク|
 |4      |ボールド|


### 時間待ち
- 書式  
 WAIT 待ち時間(ミリ秒)  

- 引数  
 待ち時間: 0～32767 (単位ミリ秒)
- 説明  
 引数で指定した時間(ミリ秒単位)、時間待ち(ウェイト)を行います.  
 **(注意)**  
 ウェイト中は[ESC]によるプログラム中断を行うことは出来ません.  


### スクリーン位置の文字コード参照(関数) 
- 書式  
 VPEEK(横位置,縦位置)

- 引数  
 横位置: 0～39  
 縦位置: 0～19  

- 戻り値    
 指定位置の文字コード  

- 説明  
 画面上の指定位置の文字コードを取得します.  
 引数の指定位置が範囲外の場合は0を返します.  


### 文字コードから文字への変換
- 書式  
 CHR$(文字コード)  

- 引数  
 文字コード: 0～127,160～255
 
- 戻り値    
 指定文字コードに対応する文字

- 説明  
 指定した文字コードに対応する文字を返します.  
 PRINT文にのみ利用可能です.  
 範囲外の値を指定した場合は空白文字(" ")を返します.  


### 文字から文字コードへの変換
- 書式  
 ASC(文字列)  

- 引数  
 文字列: 例:"A"  
 
- 戻り値  
 指定文字に対応する文字コード  

- 説明  
 指定した文字に対応する文字コードを返します.  
 指定した文字列が1文字で無い場合は、エラーとなります.  


### GPIOピン機能設定
- 書式  
 GPIO ピン番号, モード  

- 引数  
 ピン番号：  
 **0 ～ 34 または以下の定数**  
 PA00, PA01, PA02, PA03, PA04, PA05, PA06, PA07, PA08,  
 PA09, PA10, PA11, PA12, PA13, PA14,PA15,  
 PB00, PB01, PB02, PB03, PB04, PB05, PB06, PB07, PB08,  
 PB09, PB10, PB11, PB12, PB13,PB14,PB15,  
 PC13, PC14,PC15    
 **モード：以下の定数**    
 OUTPUT_OD：デジタル出力（オープンドレイン）  
 OUTPUT：デジタル出力  
 INPUT_FL ：デジタル入力（プルアップ無し、フロート状態：ArduinoのINPUT指定に同じ）  
 INPUT_PU：デジタル入力（内部プルアップ抵抗有効）  
 INPUT_PD：デジタル（内部プルダウン抵抗有効）  
 ANALOG：アナログ入力  
 
- 戻り値  
 なし  

- 説明  
 ボード上のGPIOピンの機能設定を行います.  
 ArduinoのpinMode()の機能に相当します.  
 GPIOピンを使って信号の入出力を行う場合は、必ず本コマンドにて設定を行って下さい.  

### デジタル出力
- 書式  
 OUT ピン番号, 出力値  

- 引数  
 ピン番号：  
 **0 ～ 34 または以下の定数**  
 PA00, PA01, PA02, PA03, PA04, PA05, PA06, PA07, PA08,  
 PA09, PA10, PA11, PA12, PA13, PA14,PA15,  
 PB00, PB01, PB02, PB03, PB04, PB05, PB06, PB07, PB08,  
 PB09, PB10, PB11, PB12, PB13,PB14,PB15,  
 PC13, PC14,PC15     
 出力値：  
 **LOW** または 0： 0Vを出力する  
 **HIGH** or 0以外の値：3.3Vを出力する  
  
- 戻り値  
 なし  

- 説明  
 指定ピンから、指定した出力値をを出力します.  
 GPIOピンを使って信号の入出力を行う場合は、必ず本コマンドにて設定を行って下さい.  

### デジタル入力(関数)  
- 書式  
 IN(ピン番号)  

- 引数  
 ピン番号：  
 **0 ～ 34 または以下の定数**  
 PA00, PA01, PA02, PA03, PA04, PA05, PA06, PA07, PA08,  
 PA09, PA10, PA11, PA12, PA13, PA14,PA15,  
 PB00, PB01, PB02, PB03, PB04, PB05, PB06, PB07, PB08,  
 PB09, PB10, PB11, PB12, PB13,PB14,PB15,  
 PC13, PC14,PC15     
 出力値：  
 **LOW** または 0： 0Vを出力する  
 **HIGH** or 0以外の値：3.3Vを出力する  
  
- 戻り値  
 なし  

- 説明  
 指定ピンから、指定した出力値をを出力します.  
 GPIOピンを使って信号の入出力を行う場合は、必ず本コマンドにて設定を行って下さい.  

## サンプルプログラム
```
10 FOR I=0 TO 10
20 FOR J=0 TO 10
30 COLOR RND(8): ? "*";
35 WAIT 100
40 NEXT J
50 ?
60 NEXT I
```
## 以降はオリジナルのドキュメントです



TOYOSHIKI Tiny BASIC for Arduino

The code tested in Arduino Uno R3.<br>
Use UART terminal, or temporarily use Arduino IDE serial monitor.

Operation example

&gt; list<br>
10 FOR I=2 TO -2 STEP -1; GOSUB 100; NEXT I<br>
20 STOP<br>
100 REM Subroutine<br>
110 PRINT ABS(I); RETURN

OK<br>
&gt;run<br>
2<br>
1<br>
0<br>
1<br>
2

OK<br>
&gt;

The grammar is the same as<br>
PALO ALTO TinyBASIC by Li-Chen Wang<br>
Except 3 point to show below.

(1)The contracted form of the description is invalid.

(2)Force abort key<br>
PALO ALTO TinyBASIC -> [Ctrl]+[C]<br>
TOYOSHIKI TinyBASIC -> [ESC]<br>
NOTE: Probably, there is no input means in serial monitor.

(3)Other some beyond my expectations.

(C)2012 Tetsuya Suzuki<br>
GNU General Public License
