////////////////////////////////////////////////////////////////////////
// spline.cpp
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch

// spline_element --------------------------------
template< class D >
spline_element<D>::spline_element( const D& _x, const D& _y )
: x(_x),
  y(_y),
  b(0),c(0),d(0)
{}

// spline_interpolation --------------------------
template< class D >
void spline_interpolation<D>::add( const D& x, const D& y )
{
	m_data.push_back(
		spline_element<D>(x,y)
	);
}

template< class D >
void spline_interpolation<D>::establish()
{
	const int end_n = m_data.size()-1;
	assert( end_n > 3 );

	int i;
	std::sort( m_data.begin(), m_data.end() );

	// h1 の計算
	m_data[0].d = m_data[1].x - m_data[0].x;

	// β2の計算
	m_data[1].c = (m_data[1].y -m_data[0].y) / m_data[0].d;

	// hi, αi, βiの計算
	for( int n = 1;	n < end_n; ++n )
	{
		//hi
		m_data[n].d = m_data[n+1].x - m_data[n].x;

		//αi
		m_data[n].b = 2.0 * ( m_data[n-1].d + m_data[n].d );

		//βi
		m_data[n+1].c = (m_data[n+1].y-m_data[n].y) / m_data[n].d;
		m_data[n].c = m_data[n+1].c - m_data[n].c;
	}

	//α1の計算
	m_data[0].b     = -m_data[0].d;
	m_data[end_n].b = -m_data[end_n-1].d;

	m_data[0].c = m_data[2].c / (m_data[3].x - m_data[1].x)
	         - m_data[1].c / (m_data[2].x - m_data[0].x) ;
	m_data[end_n].c = m_data[end_n-1].c / (m_data[end_n].x   - m_data[end_n-2].x)
	             - m_data[end_n-2].c / (m_data[end_n-1].x - m_data[end_n-3].x);
	m_data[0].c = m_data[0].c * std::pow(m_data[0].d,2.0) / ( m_data[3].x - m_data[1].x );
	m_data[end_n].c = -m_data[end_n].c
		* std::pow( m_data[end_n-1].d, 2.0 )
		/ ( m_data[end_n].x - m_data[end_n-3].x );

	for( i = 1; i <= end_n; ++i )
	{
		const double t = m_data[i-1].d / m_data[i-1].b;
		m_data[i].b = m_data[i].b - t*m_data[i-1].d;
		m_data[i].c = m_data[i].c - t*m_data[i-1].c;
	}

	m_data[end_n].c = m_data[end_n].c / m_data[end_n].b;

	for( i = end_n-1; i >= 0; --i )
	{
		m_data[i].c = (m_data[i].c - m_data[i].d * m_data[i+1].c) / m_data[i].b;
	}

	m_data[end_n].b = ( m_data[end_n].y - m_data[end_n-1].y ) / m_data[end_n-1].d
		+ m_data[end_n-1].d * (
			m_data[end_n-1].c + 2.0 * m_data[end_n].c
		)
	;

	for( i = 0; i < end_n; ++i )
	{
		m_data[i].b = ( m_data[i+1].y - m_data[i].y ) / m_data[i].d
		         - m_data[i].d * ( m_data[i+1].c + 2.0 * m_data[i].c );
		m_data[i].d = ( m_data[i+1].c - m_data[i].c ) / m_data[i].d;
		m_data[i].c = 3.0 * m_data[i].c;
	}
	m_data[end_n].c = m_data[end_n].c;
	m_data[end_n].d = m_data[end_n-1].d;
}

template < class D >
const D spline_interpolation<D>::get( const D& x ) const
{
#ifdef _MSC_VER
#	define TYPENAME
#else
#	define TYPENAME typename
#endif
	
	// out_of_range チェック
	if( x < m_data.begin()->x || m_data.rbegin()->x < x ) {
		throw std::out_of_range( boost::str( boost::format(
			"spline_interpolation::get() %1% out of range."
			) % x
		).c_str() );
	}
	
	// で、xに対応するyを得る。
	// upper_bound()は指定した値「より大きい」点を、
	// lower_bound()は指定した値「以上」の点を返す。
	// 間違えやすいので気をつけて。
	TYPENAME dataset_t::const_iterator
		ptr = std::upper_bound(
			m_data.begin(),
			m_data.end(),
			x,
			TYPENAME element_t::upper_search()
		)-1;

	assert( ptr->x <= x );

	return
		ptr->y
		+ ptr->b * ( x - ptr->x )
		+ ptr->c * std::pow( x-ptr->x, 2.0 )
		+ ptr->d * std::pow( x-ptr->x, 3.0 )
	;
}
