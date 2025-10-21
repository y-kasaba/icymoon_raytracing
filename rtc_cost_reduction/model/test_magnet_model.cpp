// basic_magnetic_model.cpp: basic_magnetic_model クラスのインプリメンテーション

//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "test_model.h"

using namespace rtc;
using namespace model;

// test_null model ////////////////////////////////////////////////////////
vector magnet::test_null_magnet::getField      ( const vector& pos ) const
{
	return boost::numeric::ublas::zero_vector<double>(3);
}
matrix magnet::test_null_magnet::getDerivativeB( const vector& pos ) const
{
	matrix r(3,3);
	r(0,0) = r(0,1) = r(0,2) =
	r(1,0) = r(1,1) = r(1,2) =
	r(2,0) = r(2,1) = r(2,2) = 0;

	return r;
}

// test_simple model //////////////////////////////////////////////////////
/*magnet::test_simple::test_simple()                            //////////////本来のシンプルモデル（双極子磁場）
{}

vector magnet::test_simple::getField( const vector& pos ) const
{
	const vector& m = getMagneticMoment();

	const double
		r  = norm_2(pos),
		r3 = r*r*r,
		mr = m(0)*pos(0) + m(1)*pos(1) + m(2)*pos(2);

	return
		-1.0/(4*cnst::pi) * (
			 m/r3
			-pos*(
				3*mr*r/(r3*r3)
			)
	);
}
*/
magnet::test_simple::test_simple()

{}

//////////////追加したシンプルモデル（x軸方向一定）                   磁場の影響は無視したいため磁場弱・波数ベクトルの向きを決めるため磁場は必要..

vector magnet::test_simple::getField      ( const vector& pos ) const
{

	boost::numeric::ublas::vector<double> x_vector(3);

	x_vector[0] = 1e-20;  //x
	x_vector[1] = 0;  //y
	x_vector[2] = 0;  //z

	return x_vector;
}

// getFootPrint() --------------------------------
vector magnet::test_simple::getFootPrint(
	const vector&           sp,
	double
) const {

	const basic_planet& mother = getMother();

	// trace_factorは無視
	const double r = mother.getRadius();
	vector ptr = convertToPolar(sp);

	ptr[1] = std::asin(
		std::sqrt( r/ptr[0] ) * std::sin( ptr[1] )
	);
	ptr[0] = r;

	if( sp[2] < 0.0 ) {
		ptr[1] = cnst::pi-ptr[1];
	}
	return convertToCartesian(ptr);
}

// getEquatorPrint ------------------------------
vector magnet::test_simple::getEquatorPrint(
	const vector&           sp,
	double
) const {
	const basic_planet& mother = getMother();
	vector ptr = convertToPolar(sp);

	const double s = std::sin(ptr[1]);
	ptr[0] = ptr[0] /( s*s ); // r_eq
	ptr[1] = cnst::pi/2;      // 90°

	return convertToCartesian(ptr);
}

