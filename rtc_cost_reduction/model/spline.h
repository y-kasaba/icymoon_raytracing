////////////////////////////////////////////////////////////////////////
// spline.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_SPLINE_INTERPOLATION_H
#define RTC_RAYTRACE_SPLINE_INTERPOLATION_H

namespace rtc { // -----------------------------------------------------

	template < class D >
	class spline_interpolation;
	
	template < class D >
	class spline_element
	{
		friend class spline_interpolation<D>;

	public:
		spline_element( const D& _x, const D& _y );
		D x, y;

	public:
		// algorithmópÇÃÉÅÉìÉoä÷êî
		bool operator <( const spline_element& r ) const
		{ return x < r.x; }

		struct upper_search
		{
			bool operator ()(
				const D                  b,
				const spline_element<D>& a
			) const {
				return a.x > b;
			}
		};

	private:
		D b, c, d;
	};
	
	template < class D = double >
	class spline_interpolation
	{
	public:
		typedef spline_element<D>        element_t;
		typedef std::vector< element_t > dataset_t;
		
	public:
		void add( const D& x, const D& y );
		void establish();
		
		const D get( const D& x ) const;
		
	private:
		dataset_t m_data;
	};

	#include "spline.inl"
}

#endif
