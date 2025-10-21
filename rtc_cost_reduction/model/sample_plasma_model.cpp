// basic_plasma_model.cpp: basic_plasma_model クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "sample_model.h"

using namespace rtc;
using namespace model;

// null model ////////////////////////////////////////////////////////
double plasma::null_plasma::getDensity( const vector& point ) const
{
	return 0;
}

// simple model //////////////////////////////////////////////////////
double plasma::simple::getDensity( const vector& point ) const
{
	const double Re = getCosmos().getPlanet().getRadius();

	vector pa = point, pb = point;
	pa(0) -= 3*Re;
	pb(0) += 3*Re;

	const double
		r = std::fabs( 
			  (point(0)*point(0) - Re*Re/2)
			+ (point(1)*point(1) - Re*Re/2)
			+ (point(2)*point(2) - Re*Re/2)
		);

	return 5e14*(
		Re/r
		+ 1.5*Re/(inner_prod(pa,pa))
		+ 1.5*Re/(inner_prod(pb,pb))
	);
}

