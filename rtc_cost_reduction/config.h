//////////////////////////////////////////////////////////////////////
// config.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_CORE_CONFIG_H
#define RTC_RAYTRACE_CORE_CONFIG_H

//
// raytraceの調整可能なパラメータはここで記述。
//

//
// 密度勾配、磁場勾配を計算するときの幅をメートル単位で指定する。
//
#define RTC_DERIVATIVE_DISTANCE 1.0


//
// basic_magnet_model::getFootPrint()���A
// 過去の計算結果を保持した高速化を図る場合に定義する。
// 定義された個数分だけ過去の計算結果を保持する。
//
#define RTC_BASIC_MAGNET_MODEL_STORE_PAST 5


//
// cavity によるプラズマ密度への影響を与える場合、定義する。
// これを定義しないことによって、basic_plasma_model::operator ()の結果は
// プラズマキャビティを含まない、生の値を返す。
// その結果、ほんの少しだけの高速化を見込むことができる。
// 未定義の状態でも cavityクラスのインスタンスをつくることはできるし
// setCavity()を呼び出すことも可能だが、プラズマには全く影響しない。
//
#define RTC_ENABLE_PLASMA_CAVITY


//
// raytrace時に、パラメータ指定の時間より少ない経過時間が必要と
// 判断された時をエラーと判断し、例外をなげるようにする。
// モデルの不具合などで無限ループに陥る場合、有効にする。
//
#define RTC_RAYTRACE_ENABLE_EXCEPTION_WHEN_TIMESTEP_UNDERFLOW


// -----------------------------------------------
// 以下、デバッグ時にのみ有効にする。
// 非デバッグ時も有効にする場合は
// ifndef節をコメントアウトする。
#ifndef NDEBUG

//
// rtc::clearNaN()を有効にする場合、定義する。
//
#define RTC_RAYTRACE_ENABLE_CLEARNAN


//
// rtc::clearNaN()が有効であった時、
// 実際にNaNをクリアしたときにレポート表示する場合に定義する。
//
#define RTC_RAYTRACE_LOGGING_CLEARNAN


//
// ray::calc_dt()で、パラメータ指定の時間範囲外に到達
// したときにログをとる場合、定義する。
//
#define RTC_RAYTRACE_RAY_LOGS_OUT_OF_TIME_RANGE


//
// デバッグ用のログを出力する場合、定義する。
//
#define RTC_RAYTRACE_ENABLE_LOG


//
// より詳細なデバッグログを出力する場合、定義する。
//
// #define RTC_RAYTRACE_ENABLE_DETAIL_LOG

#endif//NDEBUG -----------------------------------


#endif//RTC_RAYTRACE_CORE_CONFIG_H
