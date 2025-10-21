////////////////////////////////////////////////////////////////////////
// basic_planet.cpp
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#include "StdAfx.h"
#include "basic_planet.h"

using namespace rtc;

////////////////////////////////////////////////////////////////////////
basic_planet::basic_planet(
	const double       radius,
	const double          VDM,
	const axis_info&     axis,
	basic_magnet_model&   mag,
	basic_plasma_model&  plsm
) : m_radius( radius ),
    m_vdm   (    VDM ),
	m_axisInfo( axis ),
    m_magnet(    mag ),
    m_plasma(   plsm )
{};

void basic_planet::create()
{
	m_magnet.create( *this );
	m_plasma.create( *this );

#ifndef NDEBUG
	test();
#endif
}


// 惑星磁場
void basic_planet::setMagneticalAxis(
	const double magnet_latitude, // 磁軸の緯度 [deg]
	const double magnet_longitude // 磁軸の経度 [deg]
){
	m_axisInfo.setAxis( magnet_latitude, magnet_longitude );
}


// 惑星に対する座標系 --------------------------------------------------
vector basic_planet::getFootPrint(
	const vector& source_ptr,
	double      trace_factor
) const {
	return m_magnet.getFootPrint(
		source_ptr,
		trace_factor
	);
}

vector basic_planet::getEquatorPrint(
	const vector& source_ptr,
	double      trace_factor
) const {
	return m_magnet.getEquatorPrint(
		source_ptr,
		trace_factor
	);
}

double basic_planet::getFLAT(
	const vector& point,
	double trace_factor
) const {

	// 直交座標の point から、地表における緯度(FLAT)を求め、返す。
	const vector fp = convertToPolar(
		getFootPrint(
			point,
			trace_factor
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


double basic_planet::getEqLAT(
	const vector&  point,
	double  trace_factor
) const {
	
	// L値を導出し、双極子磁場を仮定して求める。
	const double L = norm_2(
		getEquatorPrint( point, trace_factor )
	) / getRadius();

	const double ilat = rad2deg(
		std::acos( std::sqrt(1/L) )
	);

	return std::min( ilat, 89.9999999999999 );
}


double basic_planet::getMLT( const vector& point ) const
{
	// ユークリッド空間 point から、磁気ローカル時刻[h]を求め、返す。
	double mlt = 0;

	vector p = convertToPolar(point);
	const double rad = std::min(
		std::max( p(2), -cnst::pi )+cnst::pi,
		2*cnst::pi
	);

	mlt = std::fmod(
		rad * 24.0/(2*cnst::pi),
		24.0
	);

	assert( 0 <= mlt && mlt < 24 );
	return mlt;
}


vector basic_planet::getLocation(
	const double MLAT,      // MLAT値を指定[degree]
	const double MLT,       // MLT値を指定 [h]
	const double altitude,  // 惑星表面からの高度を指定
	const double trace_factor, /* = 1e3 */ // 磁場トレースの精度
	std::vector<vector>* const out_trace_line /* = NULL */
) const {
	assert( trace_factor > 0.0 );

	if( MLAT < -90.0 || 90.0 < MLAT )
		throw std::out_of_range(
			"basic_magnet_model::getLocation : MLAT is out of range."
	);
	if( MLT < 0.0 || 24.0 < MLT )
		throw std::out_of_range(
			"basic_magnet_model::getLocation : MLT is out of range."
	);

	const double radius = getRadius();

	vector r = boost::numeric::ublas::zero_vector<double>(3);
	r[0] = radius;
	r[1] = mlat2rad(MLAT);
	r[2] = mlt2rad(MLT);
	r = convertToCartesian(r);

	if( out_trace_line ) {
		out_trace_line->clear();
		out_trace_line->push_back(r);
	}

	// 位置rから磁力線を伝っていき、
	// radius+altitudeに達した点で終了する。
	vector B = m_magnet.getField(r);

	// トレースする方向を定める。
	const double
		dist      = radius+altitude - norm_2(r),
		dist_plus = radius+altitude - norm_2(r+B);

	const double sence =
		dist > dist_plus ? +1.0 : -1.0;

	// senceの方向へトレース。
	double lest = altitude;
	while( lest > trace_factor )
	{
		if( lest < 0.0 )
			throw std::logic_error(
				"basic_magnet_model::getLocation : Tries to derive the height of the underground."
		);

		const vector r_new = r +(
			B * ( sence * trace_factor/norm_2(B) )
		);

		if( norm_2(r_new) < norm_2(r) )
			throw std::logic_error(
				"basic_magnet_model::getLocation : magnetic field line doesn't reach up to the height of the target."
		);

		r    = r_new;
		lest = altitude+radius - norm_2(r_new);
		B    = m_magnet.getField(r);

		if( out_trace_line ) {
			out_trace_line->push_back(r);
		}
	};

	// |r|が少しだけ足りない位置で止まるので、
	// のこりの距離を計算して、目的の高さにぴっちり持ってくる。
	// thetaは Bベクトルとrベクトルが成す角。
	r += B/norm_2(B) * (
	  lest  * (
	    norm_2(B)*norm_2(r)/inner_prod(B,r)// == 1/cos(theta);
	  )
	);
	if( out_trace_line ) {
		out_trace_line->push_back(r);
	}

#ifdef RTC_RAYTRACE_ENABLE_DETAIL_LOG
	std::clog << "location precision:" << norm_2(r) - (radius+altitude) << std::endl;;
#endif

	return r;
}


// 太陽方向、自転軸、磁軸 -------------------------------------
const vector basic_planet::getRotationalAxisInGSE() const
{
	// 自転軸方向を、GSEの三次元直交座標系で表現し
	// 単位ベクトルにして返す。

	// GSE座標系は、
	//  x軸：太陽方向
	//  y軸：黄道面のdusk側
	//  z軸：黄道軸
	// で表現される。原点は惑星中心。


	// 自転軸は、経度0h、緯度+90°のGEO座標系から
	// GEI系に変換し、次にGSE系に変換する。
	const vector& ptr = m_axisInfo.getGeometricRotationalAxis();

	const matrix
		t1 = makeMatrixInverse( getGEI2GEO() ),
		t2 = getGEI2GSE();

	return rotation_prod( 
		boost::numeric::ublas::prod(t2,t1),
		ptr
	);
}

const vector basic_planet::getMagneticalAxisInGSE() const
{
	// 磁軸の方向を、GSEの三次元直交座標系で表現し
	// 単位ベクトルにして返す。

	// 磁軸は、地理経度・緯度からGEI系に変換し、GSEに変換する。
	const vector& ptr = m_axisInfo.getGeometricMagneticalAxis();

	const matrix t = boost::numeric::ublas::prod(
		getGEI2GSE(),
		makeMatrixInverse( getGEI2GEO() )
	);

	return rotation_prod( 
		t,
		ptr
	);
}


// 回転行列 --------------------------------------------------
matrix basic_planet::getGEI2GEO() const
{ 
#ifdef RTC_RAYTRACE_ENABLE_LOG
	std::clog <<
		"warning core::basic_planet : GEI2GEO transfer is not implemented."
	"\n";
#endif
	return boost::numeric::ublas::identity_matrix<double>(4);
}

matrix basic_planet::getGEI2GSE() const
{ 
#ifdef RTC_RAYTRACE_ENABLE_LOG
	std::clog <<
		"warning core::basic_planet : GEI2GSE transfer is not implemented."
	"\n";
#endif
	return boost::numeric::ublas::identity_matrix<double>(4);
}

matrix basic_planet::getGSE2GSM() const
{ 
#ifdef RTC_RAYTRACE_ENABLE_LOG
	std::clog <<
		"warning core::basic_planet : GSE2GSM transfer is not implemented."
	"\n";
#endif
	return boost::numeric::ublas::identity_matrix<double>(4);
}

matrix basic_planet::getGSM2SM () const
{ 
#ifdef RTC_RAYTRACE_ENABLE_LOG
	std::clog <<
		"warning core::basic_planet : GSM2SM transfer is not implemented."
	"\n";
#endif
	return boost::numeric::ublas::identity_matrix<double>(4);
}


////////////////////////////////////////////////////////////////////////
// basic_planet::axis_info
basic_planet::axis_info::axis_info(
	const double magnet_latitude, // 磁軸の緯度 [deg]
	const double magnet_longitude // 磁軸の経度 [deg]
) : m_magAxis(3),
    m_rotAxis(3)
{
	setAxis( magnet_latitude, magnet_longitude );
}

basic_planet::axis_info::axis_info(
	const basic_planet::axis_info& r
) : m_magAxis(r.m_magAxis),
    m_rotAxis(r.m_rotAxis)
{}

void basic_planet::axis_info::setAxis(
	const double magnet_latitude, // 磁軸の緯度 [deg]
	const double magnet_longitude // 磁軸の経度 [deg]
) {
	// 地軸は、z軸と同じである。
	m_rotAxis[0] = 0;
	m_rotAxis[1] = 0;
	m_rotAxis[2] = 1.0;

	// 磁軸は、渡された緯度経度から算出する。
	double lat, lng;

	// 磁気モーメントが南向きの場合でも
	// 磁軸は北向きを常に正として保持する。
	if( magnet_latitude < 0.0 )
	{
		lat = deg2rad( -magnet_latitude       );
		lng = deg2rad( magnet_longitude+180.0 );
	}
	else 
	{
		lat = deg2rad( magnet_latitude  );
		lng = deg2rad( magnet_longitude );
	}

	m_magAxis[0] = std::cos(lat) * std::cos(lng);
	m_magAxis[1] = std::cos(lat) * std::sin(lng);
	m_magAxis[2] = std::sin(lat);

	assert( m_magAxis[2] >= 0.0 );
}

// テスト用のメソッド --------------------------------------------------
#ifndef NDEBUG

void basic_planet::test() const
{
	// getMagneticalAxisInGSE()と
	// SM座標系のZ軸単位ベクトルをGESに変換し
	// 結果が同じになるかをチェックする。
	vector m = boost::numeric::ublas::zero_vector<double>(3);
	m[2] = 1.0;

	matrix t = boost::numeric::ublas::prod(
		makeMatrixInverse( getGSE2GSM() ),
		makeMatrixInverse( getGSM2SM()  )
	);
	const vector r1 = rotation_prod(
		t, m
	);
	const vector r2 = getMagneticalAxisInGSE();

	std::clog <<
		"MagneticalAxis\n"
		"                 in SM->GSE " << r1 << "\n"
		"in getMagneticalAxisInGSE() " << r2 << std::endl;
}

#endif//NDEBUG
