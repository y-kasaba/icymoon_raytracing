////////////////////////////////////////////////////////////////////////
// rtc_func.cpp
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#include "StdAfx.h"
#include "rtc_func.h"


// 唯一のグローバル変数
namespace rtc {
	// この変数をexternしてはならない。
	// 必ず、rtc::getUniv()を使って値を取得すること。
	cosmos* g_cosmo = NULL;
};

rtc::cosmos& rtc::getCosmos()
{
	assert( rtc::g_cosmo );
	return *rtc::g_cosmo;
}

double rtc::deg2rad( const double deg )
{ return cnst::pi/180.0 * deg; }

double rtc::rad2deg( const double rad )
{ return 180.0/cnst::pi * rad; }

double rtc::mlat2rad( const double mlat )
{ return (cnst::pi/180.0) * (90.0-mlat); }

double rtc::rad2mlat( const double rad )
{ return 90. - 180./cnst::pi * rad; }

double rtc::mlt2rad( const double mlt )
{ return -(2*cnst::pi/24) * (12-mlt); }

double rtc::rad2mlt( const double rad )
{ return 12*( rad/cnst::pi - 1 ); }

// 座標変換系 ----------------------------------------------------------
rtc::vector rtc::convertToPolar( const rtc::vector& cartesian )
{
	// ユークリッド空間から極座標への変換
	rtc::vector polar = boost::numeric::ublas::zero_vector<double>(3);// (r,theta,fai)

	const double dr = cartesian(0)*cartesian(0) + cartesian(1)*cartesian(1);

	polar(0) = rtc::norm_2( cartesian );
	polar(1) = std::atan2( std::sqrt(dr), cartesian(2) );
	polar(2) = std::atan2( cartesian(1), cartesian(0) );

	return polar;
}

rtc::vector rtc::convertToCartesian( const rtc::vector& polar )
{
	// 極座標からユークリッド空間への変換
	rtc::vector e = boost::numeric::ublas::zero_vector<double>(3);// (x,y,z);

	e(0) = polar(0) * std::sin( polar(1) ) * std::cos( polar(2) );
	e(1) = polar(0) * std::sin( polar(1) ) * std::sin( polar(2) );
	e(2) = polar(0) * std::cos( polar(1) );

	return e;
}


#ifdef _MSC_VER
int rtc::isnan( double x ){
	return ::_isnan(x);
}
#endif

#ifdef RTC_RAYTRACE_ENABLE_CLEARNAN
double rtc::clearNaN( double x )
{ 
	const bool is = isnan(x);
#ifdef RTC_RAYTRACE_LOGGING_CLEARNAN
	if( is ) {
		std::clog << "rtc::clearNaN() : NaN was cleared.\n";
	}
#endif
	return is ? 0.0 : x;
}
#endif


rtc::vector rtc::rotation(
	const rtc::vector& ptr,
	const rtc::vector& axis,
	const double       angle
){
	// あるベクトル v を任意の軸で回転するためにクォータニオン(４次元数)を利用する。
	// クォータニオン v(0,x,y,z) を (nx,ny,nz)ベクトルを軸として thetaだけ回転するには、
	// q = w + ai + bj + ck,
	//   w = cos(theta/2);
	//   a = nx * sin(theta/2);
	//   b = ny * sin(theta/2);
	//   c = nz * sin(theta/2);
	//の、回転quaternion(q)とその共役なquaternion(qc)が必要。
	// 共役なquaternionは、(w, -a, -b, -c)で定義される。
	//
	// 回転quaternionが得られたら、
	//  q * v * qc
	//を計算することで、回転結果が得られる。
	rtc::quaternion v( 0, ptr[0], ptr[1], ptr[2] );

	// 回転。回転軸ベクトルは単位ベクトルでないといけない。
	rtc::vector n = axis;
	n /= rtc::norm_2(n); 

	const double
		s = std::sin(angle/2),
		c = std::cos(angle/2);

	const rtc::quaternion
		q(
			c,
			n(0) * s,
			n(1) * s,
			n(2) * s
		),
		qc(
			 c,
			-n(0) * s,
			-n(1) * s,
			-n(2) * s
		);

	v = q * v * qc;

	rtc::vector result = boost::numeric::ublas::zero_vector<double>(3);
	result[0] = v.R_component_2();
	result[1] = v.R_component_3();
	result[2] = v.R_component_4();

	return result;
}



#if defined (_MSC_VER) && _MSC_VER < 1300
// VC6だとコンパイルできない部分がある。
// 以下ではそれを補う。
#include <boost/numeric/ublas/lu.hpp>

template<class M, class E>
void lu_substitute(
	const M &m, boost::numeric::ublas::matrix_expression<E> &e
) {
	using namespace boost::numeric::ublas;

    typedef const M const_matrix_type;
    typedef matrix<E::value_type> matrix_type;

#ifdef BOOST_UBLAS_TYPE_CHECK
    matrix_type cm1 (e);
#endif
    inplace_solve (m, e, unit_lower_tag ());
#ifdef BOOST_UBLAS_TYPE_CHECK
    BOOST_UBLAS_CHECK (equals (prod (triangular_adaptor<const_matrix_type, unit_lower> (m), e), cm1), internal_logic ());
    matrix_type cm2 (e);
#endif
    inplace_solve (m, e, upper_tag ());
#ifdef BOOST_UBLAS_TYPE_CHECK
    BOOST_UBLAS_CHECK (equals (prod (triangular_adaptor<const_matrix_type, upper> (m), e), cm2), internal_logic ());
#endif
}

template<class M, class PMT, class PMA, class MV>
void lu_substitute (const M &m, const boost::numeric::ublas::permutation_matrix<PMT, PMA> &pm, MV &mv) {
	using namespace boost::numeric::ublas;

    swap_rows (pm, mv);
    ::lu_substitute (m, mv);
}

#else // VC6�ȊO
template<class M, class PMT, class PMA, class MV>
void lu_substitute (const M &m, const boost::numeric::ublas::permutation_matrix<PMT,PMA> &pm, MV &mv) {
	boost::numeric::ublas::lu_substitute(m,pm,mv);
}

#endif

rtc::matrix rtc::makeMatrixRotationX( const double theta )
{
	const double
		c = std::cos(theta),
		s = std::sin(theta);

	matrix rot(4,4);
	rot(0,0) = 1; rot(0,1) = 0; rot(0,2) =  0; rot(0,3) = 0;
	rot(1,0) = 0; rot(1,1) = c; rot(1,2) = -s; rot(1,3) = 0;
	rot(2,0) = 0; rot(2,1) = s; rot(2,2) =  c; rot(2,3) = 0;
	rot(3,0) = 0; rot(3,1) = 0; rot(3,2) =  0; rot(3,3) = 1;

	return rot;
}

rtc::matrix rtc::makeMatrixRotationY( const double theta )
{
	const double
		c = std::cos(theta),
		s = std::sin(theta);

	matrix rot(4,4);
	rot(0,0) =  c; rot(0,1) = 0; rot(0,2) = s; rot(0,3) = 0;
	rot(1,0) =  0; rot(1,1) = 1; rot(1,2) = 0; rot(1,3) = 0;
	rot(2,0) = -s; rot(2,1) = 0; rot(2,2) = c; rot(2,3) = 0;
	rot(3,0) =  0; rot(3,1) = 0; rot(3,2) = 0; rot(3,3) = 1;

	return rot;
}

rtc::matrix rtc::makeMatrixRotationZ( const double theta )
{
	const double
		c = std::cos(theta),
		s = std::sin(theta);

	matrix rot(4,4);
	rot(0,0) = c; rot(0,1) = -s; rot(0,2) = 0; rot(0,3) = 0;
	rot(1,0) = s; rot(1,1) =  c; rot(1,2) = 0; rot(1,3) = 0;
	rot(2,0) = 0; rot(2,1) =  0; rot(2,2) = 1; rot(2,3) = 0;
	rot(3,0) = 0; rot(3,1) =  0; rot(3,2) = 0; rot(3,3) = 1;

	return rot;
}

rtc::matrix rtc::makeMatrixInverse( const matrix& m )
{
	// 逆行列をつくり、返す。
	// このへん参照：http://www.page.sannet.ne.jp/d_takahashi/boost/ublas/#SEC17
	rtc::matrix
		a = m,
		b = boost::numeric::ublas::identity_matrix<double>(4);

	boost::numeric::ublas::permutation_matrix<> pm( a.size1() );

	boost::numeric::ublas::lu_factorize( a, pm );
	::lu_substitute( a, pm, b );

	return b;
}

rtc::vector rtc::rotation_prod(
	const rtc::matrix& rot,
	const rtc::vector& ptr
){
	assert( ptr.size() == 3 );

	// ベクトルを回転行列に後ろから掛け合わせ、
	// 三次元的に回転した結果を返す。
	vector m = boost::numeric::ublas::zero_vector<double>(4);
	m[0] = ptr[0]; m[1] = ptr[1]; m[2] = ptr[2];
	m[3] = 1;

	m = boost::numeric::ublas::prod( rot, m );

	vector r = boost::numeric::ublas::zero_vector<double>(3);
	r[0] = m[0]; r[1] = m[1]; r[2] = m[2];

	return r;
}
