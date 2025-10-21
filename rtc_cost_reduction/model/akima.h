////////////////////////////////////////////////////////////////////////
// akima.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_akima_INTERPOLATION_H
#define RTC_RAYTRACE_akima_INTERPOLATION_H

namespace rtc { // -----------------------------------------------------

	template < class D = double >
	class akima_interpolation
	{
	    class akima_element
	    {
	    public:
	    	akima_element( const D& _x, const D& _y )
	    	: x(_x), y(_y){}
	    	
			// P(x) = A[0]+A[1]*x+A[2]*x^2+A[3]*x^3
			D get( const D& X ) const {
			    return  a + X*( b + X*( c + X*d ));
			}

	    	// parameter
	    	D x,y;
		    D a,b,c,d;
	    };
		struct akima_search
		{
			bool operator ()(
				const D&             b,
				const akima_element& a
			) const {
				return a.x > b;
			}
		};
		
	public:
		typedef akima_element            element_t;
		typedef std::vector< element_t > dataset_t;
		
	public:
		void add( const D& x, const D& y );
		void establish();
		
		const D get( const D& x ) const;
		
	private:
		void mkSlope( std::vector<D>& out_slope ) const;
		void mkDerivative(
			std::vector<D>&       out_deri, 
			const std::vector<D>& slope
		) const;
		dataset_t m_data;
	};

	#include "akima.inl"
}

#endif
