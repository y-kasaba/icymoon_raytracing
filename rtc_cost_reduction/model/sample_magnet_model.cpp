// basic_magnetic_model.cpp: basic_magnetic_model クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "sample_model.h"

using namespace rtc;
using namespace model;

// null model ////////////////////////////////////////////////////////
vector magnet::null_magnet::getField      ( const vector& pos ) const
{
	return boost::numeric::ublas::zero_vector<double>(3);
}
matrix magnet::null_magnet::getDerivativeB( const vector& pos ) const
{
	matrix r(3,3);
	r(0,0) = r(0,1) = r(0,2) =
	r(1,0) = r(1,1) = r(1,2) =
	r(2,0) = r(2,1) = r(2,2) = 0;

	return r;
}

// simple model //////////////////////////////////////////////////////
magnet::simple::simple()
{}

vector magnet::simple::getField( const vector& pos ) const
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

// getFootPrint() --------------------------------
vector magnet::simple::getFootPrint(
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
vector magnet::simple::getEquatorPrint(
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
