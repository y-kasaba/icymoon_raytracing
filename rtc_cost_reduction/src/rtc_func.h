////////////////////////////////////////////////////////////////////////
// rtc_func.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_LIBRAYTRACE_RTC_FUNC_H
#define RTC_LIBRAYTRACE_RTC_FUNC_H

namespace rtc {
	// グローバルなシステムへのアクセス
	extern cosmos* g_cosmo;
	cosmos&   getCosmos();

	double deg2rad( const double deg );
	double rad2deg( const double rad );

	double mlat2rad( const double mlat );
	double rad2mlat( const double rad  );

	double mlt2rad( const double mlt );
	double rad2mlt( const double rad );

	vector convertToCartesian( const vector& polar );
	vector convertToPolar( const vector& cartesian );

	vector rotation(
		const vector& ptr,
		const vector& axis,
		const double  angle
	);

	vector rotation_prod(
		const matrix& rot,
		const vector& ptr
	);

	matrix makeMatrixRotationX( const double theta );
	matrix makeMatrixRotationY( const double theta );
	matrix makeMatrixRotationZ( const double theta );
	matrix makeMatrixInverse( const matrix& m );

	// 非数値のチェック関数 /////////////////////
#ifdef _MSC_VER
	int isnan( double x );
#endif
	
#ifdef RTC_RAYTRACE_ENABLE_CLEARNAN
	double clearNaN( double x );
#else
#	define clearNaN
#endif
}

#endif//RTC_LIBRAYTRACE_RTC_FUNC_H
