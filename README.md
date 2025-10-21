# icymoon_raytracing

### レイトレーシング実行
* rtc_cost_reduction で “make”を実行
* rtc_cost_reduction/testing で “make”を実行
* rtc_cost_reduction/testing で “bash mkray3.sh” を実行

### 対応OS: Ubuntu

### deimos上で動かすには
#### (1) ライブラリのインストール (参照先: https://ja.linux-console.net/?p=13955)
* sudo apt install aptitude
* sudo aptitude install gcc
* sudo aptitude install libboost-all-dev
* sudo aptitude update

#### (2) ライブラリ場所を探す 
* sudo find / -name “libpthread.so” 2>/dev/null
* sudo find / -name “libboost_thread.a” 2>/dev/null

#### (3) 回すコードのコンパイル時にboost 等でエラーが出る場合 
* tesing内の makefile.gcc、makefile、common.mak を以下のように編集

[makefile.gcc] \
LIBRT = ../lib/core.a ../lib/model.a ../lib/planet.a \
/usr/lib/x86_64-linux-gnu/libboost_thread.a \ #マシンによって異なる \
/usr/lib/x86_64-linux-gnu/libpthread.so \#マシンによって異なる \
INCLUDE_OPT= /usr/include \
STDLIB = pthread boost_thread \

[makefile]
LIBRT = ../lib/core.a ../lib/model.a ../lib/planet.a /usr/lib/x86_64-linux-gnu/libboost_thread.a (マシンによって異なる) /usr/lib/x86_64-linux-gnu/libpthread.so (# マシンによって異なる) \
INCLUDE_OPT= \
STDLIB = pthread boost_thread \

[common.mak]
LD_PATH = /usr/lib/x86_64-linux-gnu   # マシンによって異なる
