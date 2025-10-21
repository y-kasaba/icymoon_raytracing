////////////////////////////////////////////////////////////////////////
// rtc_type.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_TYPE_H
#define RTC_RAYTRACE_TYPE_H

namespace rtc { // -----------------------------------------------------

	// libraytrace 内部で使用するベースクラスのテンプレ宣言。
	class cosmos;
	class basic_planet;
	class basic_magnet_model;
	class basic_plasma_model;
	class ray;
	class wave_parameter;

	// libraytrace で利用する、特別な型の定義。
	typedef boost::numeric::ublas::vector<double> vector;
	typedef boost::numeric::ublas::matrix<double> matrix;
	typedef boost::math::quaternion<double>       quaternion;

	typedef std::pair<vector,vector>              vector_pair;

	using   boost::numeric::ublas::inner_prod;
	using   boost::numeric::ublas::outer_prod;
	using   boost::numeric::ublas::norm_1; // ノルムの合計を得る。これはユークリッドノルムの二乗値である。
	using   boost::numeric::ublas::norm_2; // ベクトルのユークリッドノルム（≒ベクトルの絶対値）を計算する。

}// namespace rtc; -----------------------------------------------------

#if defined _MSC_VER && _MSC_VER < 1400
namespace std {
	using ::sin;
	using ::cos;
	using ::tan;
	using ::sqrt;
	using ::asin;
	using ::acos;
	using ::atan;
	using ::atan2;
	using ::pow;
	using ::abs;
	using ::fabs;
	using ::log;
	using ::log10;
	using ::exp;
	using ::fmod;
	using ::floor;
	using ::ceil;
	using ::memcmp;
}
#endif

#endif//RTC_RAYTRACE_TYPE_H
