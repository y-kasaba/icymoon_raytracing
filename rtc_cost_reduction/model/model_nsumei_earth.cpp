////////////////////////////////////////////////////////////////////////
// nsumei_earth.cpp
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#include "StdAfx.h"
#include "model_nsumei_earth.h"

using namespace rtc;
using namespace model;

plasma::nsumei_earth::nsumei_earth(
	const double kp
) : m_Kp   (kp)
{}

double plasma::nsumei_earth::getDensity( const vector& point ) const
{
	const double ILAT = getILAT(point);

	// 単位を合わせなければならない。
	// この式では、R は 地球半径単位、Ne は[コ/cc]となっている。
	const double R = norm_2(point) / getCosmos().getPlanet().getRadius();

	if( 1.4 < R && R < 5.0 &&
	    70 < ILAT
	){
		const double
			N0    =  68510,
			alpha = -0.038,
			beta  =  0.220,
			gamma =  4.95;
		return 1e6 *(
			 N0 * std::pow(R, -gamma) * std::exp( beta*m_Kp + alpha*ILAT)
		);
	}

	else {
		const double
			N0    = 3433.0,
			beta  = 0.229,
			gamma = 5.09;

		return 1e6 * (
			N0 * std::pow(R, -gamma) * std::exp( beta * m_Kp )
		);
	}
}
double plasma::nsumei_earth::getILAT( const vector& point ) const
{
	// ユークリッド空間 point から、不変磁気緯度(ILAT)を求め、返す。
	const vector fp = convertToPolar(
		getCosmos().getPlanet().getFootPrint(
			point,
			3e-2*getCosmos().getPlanet().getRadius()
		)
	);

	const double ilat = std::fabs( rad2deg( fp(1) )-90.0 );

	// --fix me--
	// ilat が 90.0000000001等の数値になっていることがある。
	// これによる影響を避けるため、90.0以上の場合は
	// すべて 89.9999999999999 にして返す。
	// FPUによっては、正常に動作しなくなる恐れもあるので注意すること。
	return std::min( ilat, 89.9999999999999 );
}

