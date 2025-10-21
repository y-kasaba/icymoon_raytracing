// basic_magnetic_model.cpp: basic_magnetic_model クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "basic_model.h"

using namespace rtc;
//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

basic_magnet_model::basic_magnet_model()
 :
#if defined RTC_BASIC_MAGNET_MODEL_STORE_PAST && RTC_BASIC_MAGNET_MODEL_STORE_PAST > 0
  m_pastFootPrint( 
	RTC_BASIC_MAGNET_MODEL_STORE_PAST,
	past_result_element(
		boost::numeric::ublas::zero_vector<double>(3),
		boost::numeric::ublas::zero_vector<double>(3)
	)
  ),
#endif
  m_mother( NULL )
{}

basic_magnet_model::~basic_magnet_model()
{}

int basic_magnet_model::create( basic_planet& mother )
{
	if( m_mother )
		throw std::logic_error(
			"basic_magnet_model::create : "
			"was called two or more times.\n"
			"NOTE: cannot call multiple cosmos::registerPlanet() with single planet."
	);
	m_mother = &mother;

	return 0;
}

const vector basic_magnet_model::getMagneticMoment() const
{
	// 磁極は常にZ軸方向を向く
	vector m = boost::numeric::ublas::zero_vector<double>(3);
	m[2] = getCosmos().getPlanet().getVirtualDipoleMagnet()*cnst::u0;

	return m;
}

// operator () ----------------------------------
// 指定位置の磁場ベクトルを計算し、返す。
vector basic_magnet_model::operator()( const vector& pos ) const
{
	return getField(pos);
}

// getFootPrint() -------------------------------
// 開始位置 sp における磁力線をトレースし、
// 母惑星の地表面をfootprint点として返す。
vector basic_magnet_model::getFootPrint(
	const vector&           sp,
	double        trace_factor
) const {

#if defined RTC_BASIC_MAGNET_MODEL_STORE_PAST && RTC_BASIC_MAGNET_MODEL_STORE_PAST > 0
	{
		vector old_result = boost::numeric::ublas::zero_vector<double>(3);
		// 過去の結果を漁り、同じ開始点の結果があればそれを返す。
		if( past_search( m_pastFootPrint, sp, old_result ) ) {
			return old_result;
		}
	}
#endif

	const basic_planet& mother = getMother();
	const double Re = mother.getRadius();

	if( mother.isUnderSoil(sp) ) {
		return sp;
	}
	
	// L値を元に、trace_factorを調整する。
	{
		const vector ptr = convertToPolar(sp);
		const double
			r = std::min(mother.restToSoil(sp),0.5*Re),
			c = std::sin(ptr[1]),
			L = ptr[0]/(c*c * Re);
		
		trace_factor = std::min( r, trace_factor * L );
	}

	register double factor = 0.0;
	{
		// 開始点が北半球か南半球か、及び磁極の方向を見て、
		// トレースする方向を決定する。
		const bool
			is_ptr_north    = (sp[2] >= 0.0),
			is_moment_north = (getMagneticMoment()[2] >= 0.0 );

		// 両方北か南だったら磁場方向に、
		// 片方が北、片方が南だったら磁場と逆方向にトレースする。
		const double trend_factor = 
			(is_ptr_north ^ is_moment_north) ? 1.0 : -1.0;

		factor = trend_factor*trace_factor;
	}
	
	vector v = sp;
	do
	{
		const vector B = (*this)(v);
		assert( 0.0 < norm_1(B) );
		
		const vector ds = factor * B/norm_2(B);
		
		// 情報落ちによる無限ループ回避
		const vector v_next = v+ds;
		if( std::equal( v.begin(), v.end(), v_next.begin() ) ) {
			break;
		}
		v = v_next;
		
	} while( !mother.isUnderSoil(v) );

	// 直前で停止。残りの距離分を計算して、ぴっちり地表に持ってくる。
	// 地表は球面なので、誤差が1.0[m]以内になるまで繰り返す。
	for(
		double lest = mother.restToSoil(v);
		lest > 1.0;
		lest = mother.restToSoil(v)
	){
		const vector B = (*this)(v);
		const vector ds = factor * B/norm_2(B);
		v += -( lest * norm_2(v) / inner_prod(v,ds) )* ds;
	}

#ifdef RTC_RAYTRACE_ENABLE_DETAIL_LOG
	std::clog << "footprint precision:" << getMother().restToSoil(v) << std::endl;;
#endif

#if defined RTC_BASIC_MAGNET_MODEL_STORE_PAST && RTC_BASIC_MAGNET_MODEL_STORE_PAST > 0
	past_newElement(m_pastFootPrint,sp,v);
#endif

	return v;
}

// getEquatorPrint() -------------------------------
// 開始位置 sp における磁力線をトレースし、
// 赤道面(Z=0)に到った点をEquatorPrintとして返す。
vector basic_magnet_model::getEquatorPrint(
	const vector&           sp,
	double        trace_factor
) const {

	const basic_planet& mother = getMother();

	// 開始点が北半球か南半球か、及び磁極の方向を見て、
	// トレースする方向を決定する。
	const bool
		is_ptr_north    = (sp[2] >= 0.0),
		is_moment_north = (getMagneticMoment()[2] >= 0.0 );

	// 両方北か南だったら磁場と逆方向に、
	// 片方が北、片方が南だったら磁場方向にトレースする。
	const double trend_factor = 
		(is_ptr_north ^ is_moment_north) ? -1.0 : 1.0;

	vector v = sp, ds = boost::numeric::ublas::zero_vector<double>(3);
	while( !mother.isUnderSoil(v+ds) )
	{
		v += ds;
		
		const vector B = getField(v);
		assert( 0.0 < norm_1(B) );

		ds = trend_factor * B * (trace_factor/norm_2(B));
	};

	// 直前で停止。残りの距離分を計算して、ぴっちり赤道面に持ってくる。
	vector lest = -v;
	lest[0] = lest[1] = 0.0;

	v += norm_1(lest) / inner_prod(lest,ds) * ds;
#ifdef RTC_RAYTRACE_ENABLE_DETAIL_LOG
	std::clog << "equatorprint precision:" << mother.restToSoil(v) << std::endl;;
#endif
	return v;
}

// plotModel() ----------------------------------
void basic_magnet_model::plotModel(
	cnst::plot_style ps,
	const double     step,  // 一マスの幅
	const double     range, // -range から range にわたってプロットする
	const double other_param 
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

			const vector B = getField(p);
			const double abs = norm_2(B);

			std::cout
				<< a/Re          << " "
				<< b/Re          << " "
				<< norm_2(B)     << " "
				<< B[0] * step/abs << " "
				<< B[1] * step/abs << " "
				<< B[2] * step/abs
			<< "\n";
		}
		std::cout << std::endl;
	}
}


// traceModel() ---------------------------------
void basic_magnet_model::traceModel(
	const double        b_step, /* = 3e-2 */
	const int              div, /* = 9 */
	const double         range  /* = 3  */
) const throw() {

	const double Re = getMother().getRadius();
	const vector m = Re * getMagneticMoment()/norm_2( getMagneticMoment() );

	vector
		n = boost::numeric::ublas::zero_vector<double>(3),
		z = boost::numeric::ublas::zero_vector<double>(3);
	n[1] = 1;
	z[2] = 1;

	std::list<vector> start_ptr;

	// 磁場モーメントをy軸中心に回転して、
	// 適当な間隔をあけてプロットする。
	// 回転の方法は、ray::makeInitialVector()を参照。
	for( double lat = 0; lat <= 90.; lat += 90./div )
	for( double lng = 0; lng < 360.; lng += 360./6 )
	{
		start_ptr.push_back( rotation(
			rotation( m, n, deg2rad(lat) ),
			z, deg2rad(lng)
		) );
	}

	std::list<vector>::const_iterator it;
	for( it = start_ptr.begin(); it != start_ptr.end(); ++it )
	{
		// 開始点が北半球か南半球か、及び磁極の方向を見て、
		// トレースする方向を決定する。
		const bool
			is_ptr_north    = (*it)[2] >= 0.0,
			is_moment_north = getMagneticMoment()[2] >= 0.0;

		// 両方北か南だったら磁場方向に、
		// 片方が北、片方が南だったら磁場と逆方向にトレースする。
		const double trend_factor = 
			(is_ptr_north ^ is_moment_north) ? -1.0 : 1.0;

		// 前方向に進む。
		vector p = *it;
		do
		{
			std::cout
				<< p[0]/Re << " "
				<< p[1]/Re << " "
				<< p[2]/Re << "\n";

			vector B = getField(p);
			p += trend_factor * B * (Re*b_step/norm_2(B));

			if( norm_2(p) > 1.41*range*getMother().getRadius() )
			{
				// xy面に対象の位置にスタート点を追加して、
				// ここでのトレースは終了。
				vector new_ptr = *it;
				if( !(is_ptr_north ^ is_moment_north ) )
				{
					new_ptr[2] *= -1;
					start_ptr.push_back(new_ptr);
				}
				break;
			}
		} while( !getMother().isUnderSoil(p) );

		std::cout << std::endl;
	}
}

// mapSurface() -----------------------------------
void basic_magnet_model::mapSurface() const throw()
{
	const basic_planet& mother = getMother();

	// 回転行列を作成する。
	matrix sm2geo = boost::numeric::ublas::prod(
		mother.getGEI2GEO(),
		makeMatrixInverse( mother.getGEI2GSE() )
	);
	sm2geo = boost::numeric::ublas::prod(
		sm2geo,
		makeMatrixInverse( mother.getGSE2GSM() )
	);
	sm2geo = boost::numeric::ublas::prod(
		sm2geo,
		makeMatrixInverse( mother.getGSM2SM() )
	);
	matrix geo2sm = makeMatrixInverse( sm2geo );

	for( int lng = -180; lng < 180; ++lng )
	{
		for( int lat = 0; lat <= 180; ++lat )
		{
			vector ptr(3);
			ptr[0] = mother.getRadius();
			ptr[1] = deg2rad(lat);
			ptr[2] = deg2rad(lng);
			ptr = convertToCartesian(ptr);
			
			const vector B = rotation_prod(
				sm2geo,
				(*this)(rotation_prod(geo2sm,ptr))
			);
			std::cout << 90-lat << " " << lng << " " << norm_2(B) << "\n";
		}
		std::cout << std::endl;
	}
}


#if defined RTC_BASIC_MAGNET_MODEL_STORE_PAST && RTC_BASIC_MAGNET_MODEL_STORE_PAST > 0

bool basic_magnet_model::past_search(
	const past_results_list& l,
	const vector&       in_pos,
	vector&         out_result
) const {
	// マルチスレッドにされたときのバグを防止するため、
	// 操作中は常にmutexを取得しなければならない。
	// このロックは、スコープを外れると同時に解放される。
	boost::mutex::scoped_lock lock( m_pastGuard );
	
	past_results_list::const_iterator it;
	for( it = l.begin(); it != l.end(); ++it )
	{
		if( std::equal( in_pos.begin(), in_pos.end(), it->first.begin() )){
			out_result = it->second;
			return true;
		}
	}
	return false;
}

const vector& basic_magnet_model::past_newElement(
	past_results_list&     l,
	const vector&     in_pos,
	const vector& in_element
) const {
	boost::mutex::scoped_lock lock( m_pastGuard );

	// 計算結果を追加し、一番古い結果は破棄する。
	l.pop_back();
	l.push_front(
		past_result_element(in_pos, in_element)
	);
	assert( l.size() == RTC_BASIC_MAGNET_MODEL_STORE_PAST );
	
	return in_element;
}

#endif // RTC_BASIC_MAGNET_MODEL_STORE_PAST

