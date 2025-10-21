////////////////////////////////////////////////////////////////////////
// akima.cpp
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch

#ifndef TYPENAME

#ifdef _MSC_VER
#	define TYPENAME
#else
#	define TYPENAME typename
#endif

#endif

// akima_interpolation --------------------------
template< class D >
void akima_interpolation<D>::add( const D& x, const D& y )
{
	m_data.push_back(
		akima_element(x,y)
	);
}

template< class D >
void akima_interpolation<D>::establish()
{
	int i;
	const int data_size = m_data.size();
//	std::sort( m_data.begin(), m_data.end() );

    // compute slopes
	std::vector<D> slope;
	mkSlope( slope );
	
	// construct derivatives
	std::vector<D> deri;
	mkDerivative( deri, slope );
	
	// construct polynomials
	for( i = 0; i < data_size-1; ++i )
	{
		const D
			dy  = m_data[i+1].y - m_data[i].y,
			dx  = m_data[i+1].x - m_data[i].x,
			dx2 = dx*dx,
			dx3 = dx2*dx;
		
		m_data[i].a = m_data[i].y;
		m_data[i].b = deri[i];
		m_data[i].c = (
			  D(3.0) * dy - dx * (deri[i+1] + D(2.0) * deri[i])
			) / dx2;
		m_data[i].d = (
			  dx * ( deri[i+1] + deri[i] ) - ( D(2.0) * dy )
			) / dx3;
	}
}

template < class D >
const D akima_interpolation<D>::get( const D& x ) const
{
	//out_of_range チェック
	if( x < m_data.begin()->x || m_data.rbegin()->x < x ) {
		throw std::out_of_range( boost::str( boost::format(
			"akima_interpolation::get() %1% out of range."
			) % x
		).c_str() );
	}
	
	// で、xに対応するyを得る。
	// upper_bound()は指定した値「より大きい」点を、
	// lower_bound()は指定した値「以上」の点を返す。
	// 間違えやすいので気をつけて。
	typename dataset_t::const_iterator
		ptr = std::upper_bound(
			m_data.begin(),
			m_data.end(),
			x,
			akima_search()
		)-1;

	assert( ptr->x <= x );

	return ptr->get(x - ptr->x);
}

//////////////////////////////////////////////////////////////////////
template < class D >
void akima_interpolation<D>::mkSlope(
	std::vector<D>& out_slope
) const {
	const unsigned data_size = m_data.size();

	out_slope.clear();
	out_slope.push_back( D(0.0) );
	out_slope.push_back( D(0.0) );

	typename dataset_t::const_iterator it;
	for( it = m_data.begin(); it+1 != m_data.end(); ++it )
	{
		out_slope.push_back( 
			((it+1)->y - it->y) / ((it+1)->x - it->x)
		);
	}
	out_slope[1] = D(2.0) * out_slope[2] - out_slope[3];
	out_slope[0] = D(2.0) * out_slope[1] - out_slope[2];

	assert( out_slope.size() == data_size+1 );

	out_slope.push_back(
		D(2.0) * out_slope[data_size] - out_slope[data_size-1]
	);
	out_slope.push_back(
		D(2.0) * out_slope[data_size+1] - out_slope[data_size]
	);
}

template < class D >
void akima_interpolation<D>::mkDerivative(
	std::vector<D>&       out_deri, 
	const std::vector<D>& slope
) const {
	
	assert( slope.size() == m_data.size()+3 );

	out_deri.clear();
	
	typename std::vector<D>::const_iterator it;
	for( it = slope.begin(); it+3 != slope.end(); ++it )
	{
		if( *(it+1) == *(it+2) )
		{
			out_deri.push_back( *(it+1) );
		}
		
		else if( *it == *(it+1) )
		{
			if( *(it+2) == *(it+3) )
			{
				out_deri.push_back(
					D(0.5) * ( *(it+1) + *(it+2) )
				);
			}
			else {
				out_deri.push_back( *(it+1) );
			}
		}
		
		else if( *(it+2) == *(it+3) )
		{
			out_deri.push_back( *(it+2) );
		}
		
		else
		{
			D ad0 = std::fabs(D( *(it+3) - *(it+2 ) ));
			D ad1 = std::fabs(D( *it - *(it+1) ));
			out_deri.push_back(
				( ad0 * *(it+1)
				+ ad1 * *(it+2)
				) / (ad0+ad1)
			);
		}
	}

	assert( out_deri.size() == m_data.size() );
}
