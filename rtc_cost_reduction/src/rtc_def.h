////////////////////////////////////////////////////////////////////////
// rtc_def.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_DEF_H
#define RTC_RAYTRACE_DEF_H

namespace rtc { namespace cnst {// -------------------------------------
	// ライブラリが内部で利用している定数。/////////////////////
	enum plot_style {
		plot_xy,
		plot_xz,
		plot_yz,
	}; // 各モデルのプロットを、どの軸に沿って行うか指定する。

	// 物理定数 ////////////////////////////////////////////////
	const static double pi = 3.1415926535897932; // 円周率
	const static double c  = 2.99792458e8;       // 光速度[m/s]
	const static double e  = 1.60217759e-19;     // 電気素量
	const static double e0 = 8.85418782e-12;     // 誘電率
	const static double u0 = 4*pi*1e-7;          // 透磁率
	const static double me = 9.1093922e-31;      // 電子の質量[kg]
	const static double mp = 1.6726217e-27;      // 陽子の質量[kg]
	const static double k  = 1.3806505e-23;      // ボルツマン定数

	const static double Rj = 71372.e3;           // 木星半径[m]

}}// namespace rtc::cnst; ----------------------------------------------

#endif//RTC_RAYTRACE_DEF_H
