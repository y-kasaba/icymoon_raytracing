#include "plasma_trace.h"

void trace_plasma(
	const rtc::basic_plasma_model& p,
	const rtc::vector& start_ptr,
	const rtc::vector&   end_ptr,
	const int               step
){
	const double Re = p.getMother().getRadius();
	rtc::vector ds = end_ptr - start_ptr;
	ds /= step;

	rtc::vector ptr = start_ptr;
	for( int s = 0; s < step; ++s, ptr+=ds )
	{
		const double dens = p(ptr);
		
		std::cout
			<< s << " " << dens << " "
			<< ptr[0]/Re << " "
			<< ptr[1]/Re << " "
			<< ptr[2]/Re << " "
			<< p.getMother().getFLAT(ptr,Re*1e-3)  << " "
			<< p.getMother().getMLT(ptr)
		<< std::endl;
	}
}

int main( int argc, char* argv[] )
{
	rtc::cosmos c( 1980,5,6, 0,0,0 );

	rtc::model::magnet::IGRF       m(4);
	rtc::model::plasma::sato_earth p;
	rtc::planet::earth e(m,p);

	rtc::getCosmos().registerPlanet(e);

	rtc::vector
		sp = boost::numeric::ublas::zero_vector<double>(3),
		ep = boost::numeric::ublas::zero_vector<double>(3);
	sp[0] = boost::lexical_cast<double>(argv[1])*e.getRadius();
	sp[1] = boost::lexical_cast<double>(argv[2])*e.getRadius();
	sp[2] = boost::lexical_cast<double>(argv[3])*e.getRadius();
	ep[0] = boost::lexical_cast<double>(argv[4])*e.getRadius();
	ep[1] = boost::lexical_cast<double>(argv[5])*e.getRadius();
	ep[2] = boost::lexical_cast<double>(argv[6])*e.getRadius();
	
	trace_plasma( p, sp, ep, 10000 );

	return 0;
}
