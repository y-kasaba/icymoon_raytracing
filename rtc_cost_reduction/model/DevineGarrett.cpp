//////////////////////////////////////////////////////////////////////
// DevineGarrett.cpp
//  This is a library for software uses RAY-TRACING.
//  Copyright (C) 2006 Miyamoto Luisch
#include "StdAfx.h"
#include "DevineGarrett.h"

using namespace rtc;
using namespace model::plasma;

int DevineGarrett::create( basic_planet& mother )
{
	// 回転行列をあらかじめ取得しておく。
	m_sm2geo = boost::numeric::ublas::prod(
		mother.getGEI2GEO(),
		makeMatrixInverse( mother.getGEI2GSE() )
	);
	m_sm2geo = boost::numeric::ublas::prod(
		m_sm2geo,
		makeMatrixInverse( mother.getGSE2GSM() )
	);
	m_sm2geo = boost::numeric::ublas::prod(
		m_sm2geo,
		makeMatrixInverse( mother.getGSM2SM() )
	);

	return basic_plasma_model::create(mother);
}

double DevineGarrett::getDensity( const vector& point ) const
{
	// Devine and Garretの計算式による密度分布モデルを返す。
	// 以下のコードでは常に電子の密度分布モデル( gk == 1.0 )を返すので、
	// 他の原子も考慮した密度を考える場合は、関数から値を返す所で
	// 適切なファクターを掛け合わせるとよい。
	
	const vector ptr = convertToPolar(rotation_prod(m_sm2geo,point));
	const double
		Rj    = getMother().getRadius(),
		tan_p = 0.122784560902905, // == std::tan(deg2rad(7.0)), 7.0 == alpha
		l0    = deg2rad(21.),
		radii = ptr[0]/Rj,
		lat   = deg2rad(90.0-rad2deg(ptr[1])), //緯度
		lng   = -ptr[2];                       //西経
	assert( 0.0 <= radii );

	double n  = 0.0;

	// fix me //
	// 以下、ひたすら長い if else 文が続く。
	// このため（特にradiiが大きな値の時）非常に低速である。
	// 高速化のために別のアルゴリズムを考えた方がよい。
	// static_cast<int>(radii * 10)でもして、テーブルから係数を拾うとか。
	
	// radii < 1.0 ------------------------------/
	if( radii < 1.0 )
	{ n = 10.0; }

	// 1.0 <= radii < 3.8 -----------------------/
	else if( radii < 3.8 )
	{
		assert( 1.0 <= radii );
		
		const double
			n0    = 4.65,
			r0    = 7.68;
		//	h0    = 1.0;

		const double
			lac = tan_p * std::cos(lng-l0),
			a = radii/* /h0 */ - 1.0,
			b = lat-lac;

		n = n0 * std::exp(
			(r0/radii) - (a*a * b*b)
		);
	}
	
	// 3.8 <= radii < 5.5 -----------------------/
	else if( radii < 5.5 )
	{
		assert( 3.8 <= radii );
		double a = 0, b = 0;

		if( radii < 4.9 )
		{
			const double
				r     = radii-3.8,
				denom = 1.0/(4.9-3.8);

			a = 1.55 + r*( 2.75-1.55)*denom;
			b = 1.67 + r*(-0.31-1.67)*denom;
		}
		else if( radii < 5.1 )
		{
			const double
				r     = radii-4.9,
				denom = 1.0/(5.1-4.9);

			a = 2.75 + r*( 2.91-2.75)*denom;
			b = -.31 + r*(-0.18+0.31)*denom;
		}
		else if( radii < 5.3 )
		{
			const double
				r     = radii-5.1,
				denom = 1.0/(5.3-5.1);

			a = 2.91 + r*( 3.27-2.91)*denom;
			b = -.18 + r*( 0.37+0.18)*denom;
		}
		else if( radii < 5.5 )
		{
			const double
				r     = radii-5.3,
				denom = 1.0/(5.5-5.3);

			a = 3.27 + r*( 2.88-3.27)*denom;
			b = 0.37 + r*( 0.92-0.37)*denom;
		}
		else assert(false);

		const double
			h0 = 0.2,
		//	e0 = 1.0,
			n0 = std::pow( 10.0, a ),
			kt = std::pow( 10.0, b );

		const double
			z0 = radii * tan_p * std::cos(lng-l0),
			h  = h0 * std::sqrt(kt/* /e0 */),
			c  = (radii*lat - z0)/h;
		n = n0 * std::exp(-c*c);
	}

	// 5.5 <= radii < 7.9 -----------------------/
	else if( radii < 7.9 )
	{
		assert( 5.5 <= radii );
		double a = 0, b = 0;

		if( radii < 5.65 )
		{
			const double
				r     = radii-5.5,
				denom = 1.0/(5.65-5.5);

			a = 2.88 + r*( 3.57-2.88)*denom;
			b = 0.92 + r*( 1.15-0.92)*denom;
		}
		else if( radii < 5.8 )
		{
			const double
				r     = radii-5.65,
				denom = 1.0/(5.8-5.65);

			a = 3.57 + r*( 3.31-3.57)*denom;
			b = 1.15 + r*( 1.33-1.15)*denom;
		}
		else if( radii < 5.9 )
		{
			const double
				r     = radii-5.8,
				denom = 1.0/(5.9-5.8);

			a = 3.31 + r*( 3.35-3.31)*denom;
			b = 1.33 + r*( 1.54-1.33)*denom;
		}
		else if( radii < 6.4 )
		{
			const double
				r     = radii-5.9,
				denom = 1.0/(6.4-5.9);

			a = 3.35 + r*( 3.18-3.35)*denom;
			b = 1.54 + r*( 1.63-1.54)*denom;
		}
		else if( radii < 7.4 )
		{
			const double
				r     = radii-6.4,
				denom = 1.0/(7.4-6.4);

			a = 3.18 + r*( 2.78-3.18)*denom;
			b = 1.63 + r*( 1.67-1.63)*denom;
		}
		else if( radii < 7.9 )
		{
			const double
				r     = radii-7.4,
				denom = 1.0/(7.9-7.4);

			a = 2.78 + r*( 2.25-2.78)*denom;
			b = 1.67 + r*( 1.75-1.67)*denom;
		}
		else assert(false);

		const double
			h0 = 0.2,
		//	e0 = 1.0,
			n0 = std::pow( 10.0, a ),
			kt = std::pow( 10.0, b );

		const double
			z0 = radii * tan_p * std::cos(lng-l0),
			h  = h0 * std::sqrt(kt/* /e0 */),
			c  = (radii*lat - z0)/h;
		n = n0 * std::exp(-c*c);
	}

	// 7.9 <= radii < 20 ------------------------/
	else if( radii < 20.0 )
	{
		assert( 7.9 <= radii );

		double a = 0, b = 0;
		if( radii < 10. )
		{
			const double
				r     = radii-7.9,
				denom = 1.0/(10.-7.9);

			a = 2.25 + r*( 1.48-2.25)*denom;
			b = 1.75 + r*( 2.0 -1.75)*denom;
		}
		else if( radii < 20. )
		{
			const double
				r     = radii-10.,
				denom = 1.0/(20.-10.);

			a = 1.48 + r*( 0.2 -1.48)*denom;
			b = 2.0;
		}
		else assert(false);

		const double 
			h  = 1.82-.041 * radii,
			z0 = (7.*radii-26.)/30. * std::cos(lng-l0),
			n0 = std::pow( 10.0, a ),
			c  = (radii*lat - z0)/h;

		n = n0 * std::exp(-c*c);
	}

	// 20.0 <= radii < 170 ----------------------/
	else if( radii < 170.0 )
	{
		assert( 20.0 <= radii );

		double a=0, b=0;
		if( radii < 60. )
		{
			const double
				r     = radii-20.,
				denom = 1.0/(60.-20.);

			a = 0.2  + r*( -2.0-0.2)*denom;
			b = 2.0;
		}
		else if( radii < 100. )
		{
			const double
				r     = radii-60.,
				denom = 1.0/(100.-60.);

			a = -2.0 + r*( -2.0+2.0)*denom;
			b = 2.0;
		}
		else if( radii < 170. )
		{
			const double
				r     = radii-100.,
				denom = 1.0/(170.-100.);

			a = -2.0 + r*( -3.0+2.0)*denom;
			b = 2.0;
		}

		const double
			// fix me //
			//  10.77は磁軸の傾斜角。本来は磁場モデルから取得すべき。
			tan_p = std::tan(deg2rad(10.77)),
			r0 = 20.0,
		//	h  = 1.0,
			z0 = r0 * tan_p * std::cos( lng-l0 - 0.9*deg2rad(radii-r0) ),
			n0 = std::pow( 10.0, a ),
			c  = (radii*lat - z0) /* /h */;

		n = n0 * std::exp(-c*c);
	}
//	else /*if( 170.0 <= radii )*/
//	{ n = 0; }

	return n*1e6;
}
