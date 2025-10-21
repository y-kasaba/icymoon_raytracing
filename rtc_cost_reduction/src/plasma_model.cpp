// basic_plasma_model.cpp: basic_plasma_model クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "basic_model.h"

using namespace rtc;
//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

basic_plasma_model::basic_plasma_model()
: m_mother( NULL )
{}

basic_plasma_model::~basic_plasma_model()
{}

int basic_plasma_model::create( basic_planet& mother )
{
	if( m_mother )
		throw std::logic_error(
			"basic_plasma_model::create : "
			"was called two or more times.\n"
			"NOTE: cannot call multiple cosmos::registerPlanet() with single planet."
	);
	m_mother = &mother;

#ifdef RTC_ENABLE_PLASMA_CAVITY
	// キャビティの構築 //
	// addCavity()がcreate()より前に呼び出されたものに付いては
	// ここで面倒を見る。
	cavity_list_t::iterator it;
	for( it = m_cavity.begin(); it != m_cavity.end(); ++it )
	{
		it->create( mother );
	}
#endif

	return 0;
}

double basic_plasma_model::operator ()( const vector& pos ) const
{
	double dens = getDensity(pos);

#ifdef RTC_ENABLE_PLASMA_CAVITY
	cavity_list_t::const_iterator it;
	for( it = m_cavity.begin(); it != m_cavity.end(); ++it )
	{
		dens *= it->factor(pos);
	}
#endif
	
	return dens;
}

void basic_plasma_model::addCavity( const cavity& c )
{
#ifdef RTC_ENABLE_PLASMA_CAVITY
	m_cavity.push_back(c);

	// cavity::create()は m_motherが必要であるが、これは
	// basic_plasma_model::create()が呼び出された後でないと取得できない。
	// addCavity()はいつ呼び出されるかわからないため、
	// basic_plasma_modell::create()内部でまとめて呼び出すのと同時に
	// その後にaddCavity()が呼び出されたものについてはここでも呼び出す。
	// create()前かあとかは、m_motherがNULLかどうかで判断できる。
	// NULLで無い場合、cavity::create()を呼び出す。
	if( m_mother ){
		m_cavity.back().create( getMother() );
	}
	
#endif
}

void basic_plasma_model::clearCavity()
{
#ifdef RTC_ENABLE_PLASMA_CAVITY
	m_cavity.clear();
#endif
}

void basic_plasma_model::plotModel(
    cnst::plot_style ps,
	const double     step,
	const double     range,
	const double     other_param
) const throw() {

	const double Re = getCosmos().getPlanet().getRadius();

	for( double a = -range*Re; a < range*Re; a+=step*Re )
	{
		for( double b =-range*Re; b < range*Re; b+=step*Re )
		{
			rtc::vector p = boost::numeric::ublas::zero_vector<double>(3);
			switch(ps)
			{
			case cnst::plot_xy:
				p(0) = a; p(1) = b; p(2) = other_param;
				break;
			case cnst::plot_xz:
				p(0) = a; p(1) = other_param; p(2) = b;
				break;
			case cnst::plot_yz:
				p(0) = other_param; p(1) = a; p(2) = b;
				break;
			}

			const double n = getDensity(p);
			std::cout
				<< a/Re << " "
				<< b/Re << " "
				<< n    << "\n"
			;
		}
		std::cout << std::endl;
	}
}
