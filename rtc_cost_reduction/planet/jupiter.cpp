////////////////////////////////////////////////////////////////////////
// jupiter.cpp
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#include "StdAfx.h"
#include "jupiter.h"

using namespace rtc;
using namespace rtc::planet;

jupiter::jupiter(
	basic_magnet_model&   mag,        // 磁場モデルのインスタンスを指定する。
	basic_plasma_model&  plsm         // プラズマモデルのインスタンスを指定する。
) : basic_planet (
	71372.e3, // 地球半径[m]
	8.43e22,  // 地磁気の仮想双極子モーメント[Am^2]（北向きで正）
	axis_info( 90.0, 0.0 ), // 磁軸の位置
	mag,
	plsm
)
{};

/*
matrix jupiter::getGEI2GEO() const
{
	// Z軸を中心に回転する行列を渡す。
	// 回転角をもとめる式は、referenceの中を参照。
	// referenceの中では、T1として定義されている行列である。

	const std::tm& t = getCosmos().getUniversalTime();
	const double
		T0 = (getMJD() - 51544.5)/ 36525.0,
		H  = t.tm_hour + (t.tm_min/60.0) + (t.tm_sec/3600);

	const double theta = deg2rad(
		100.461
		+ 36000.770 * T0
		+ 15.04107  * H
	);

	return makeMatrixRotationZ(-theta);
}

matrix jupiter::getGEI2GSE() const
{
	// X軸を中心に epsilon 回転した後に、
	// Z軸を中心に lambda 回転する。
	// referenceの中では、T2として定義されている行列である。
	const std::tm& t = getCosmos().getUniversalTime();

	const double
		T0 = (getMJD() - 51544.5)/ 36525.0,
		H  = t.tm_hour + (t.tm_min/60.0) + (t.tm_sec/3600);

	const double
		M = 357.528 + 35999.050*T0 + 0.04107*H,
		A = 280.460 + 36000.772*T0 + 0.04107*H;

	const double
		epsilon = 23.439 - 0.013*T0,
		lambda  = A + (1.915 - 0.0048*T0)*std::sin( deg2rad(M) ) + 0.020*std::sin( deg2rad(2*M) );

	return boost::numeric::ublas::prod( 
		makeMatrixRotationZ( deg2rad(-lambda)  ),
		makeMatrixRotationX( deg2rad(-epsilon) )
	);
}
*/
matrix jupiter::getGSE2GSM() const
{
	// X軸を中心に、psiだけ回転する行列を返す。
	// psi は GSE座標での磁軸方向から求める。
	const vector re = getMagneticalAxisInGSE();
	assert( re[2] >= 0.0 );

	const double psi = std::atan2( re[1], re[2] );
	return makeMatrixRotationX(psi);
}

matrix jupiter::getGSM2SM () const
{
	// Y軸を中心に、-muだけ回転する行列を返す。
	// mu は GSE座標系での磁軸方向から求める。
	const vector re = getMagneticalAxisInGSE();
	assert( re[2] >= 0.0 );

	const double mu = std::atan2(
		re[0],
		std::sqrt( re[1]*re[1] + re[2]*re[2] )
	);

	return makeMatrixRotationY(-mu);
}

