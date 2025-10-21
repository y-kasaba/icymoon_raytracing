// tracer.cpp: tracer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ray.h"

using namespace rtc;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
ray::ray( const wave_parameter& wparam )
 : m_dt_before(0.0),
   m_wave( wparam ),
   m_rk (
      boost::numeric::ublas::zero_vector<double>(3),
	  boost::numeric::ublas::zero_vector<double>(3)
   ),
   m_drk(
      boost::numeric::ublas::zero_vector<double>(3),
	  boost::numeric::ublas::zero_vector<double>(3)
   )
{}

ray::~ray()
{}

// ray::intermediate ///////////////////////////////////////////////////
ray::intermediate& ray::intermediate::operator =(
	const ray::intermediate& r
){
	B    = r.B;
	Bk   = r.Bk;
	B2   = r.B2;
	k2   = r.k2;
	w    = r.w;
	w2   = r.w2;
	wp2  = r.wp2;
	wc2  = r.wc2;
	s    = r.s;
	c    = r.c;
	s2   = r.s2;
	c2   = r.c2;
	X2   = r.X2;
	Y2   = r.Y2;
	X4   = r.X4;
	Y4   = r.Y4;
	iX2  = r.iX2;
	iX22 = r.iX22;
	numerator   = r.numerator;
	denominator = r.denominator;
	root = r.root;

	return *this;
}
bool ray::intermediate::operator ==(
	const ray::intermediate& r
) const {
	// 高速化のため、&& ではなく'&'を使う。
	// 全部 bool で １つでも false があればいい訳だから
	// これでいい・・・はず
	assert( B.size() == r.B.size() );
	return (
		( B(0) == r.B(0) )&
		( B(1) == r.B(1) )&
		( B(2) == r.B(2) )&
		( Bk  == r.Bk    )&
		( B2  == r.B2    )&
		( k2  == r.k2    )&
		( w   == r.w     )&
		( w2  == r.w2    )&
		( wp2 == r.wp2   )&
		( wc2 == r.wc2   )&
		( s   == r.s     )&
		( c   == r.c     )&
		( s2  == r.s2    )&
		( c2  == r.c2    )&
		( X2  == r.X2    )&
		( Y2  == r.Y2    )&
		( X4  == r.X4    )&
		( Y4  == r.Y4    )&
		( iX2 == r.iX2   )&
		( iX22 == r.iX22 )&
		( numerator   == r.numerator   )&
		( denominator == r.denominator )&
		( root == r.root )
	);
}

// 途中で利用する計算値の更新 //////////////////////////////////////////

void ray::update_intermediate(
	ray::intermediate& i,
	const vector&      r,
	const vector&      k
) const {
	const cosmos& c = getCosmos();

	i.B      = c.getMagnetField(r);
	i.Bk     = clearNaN( inner_prod(i.B,k)             );
	i.B2     = clearNaN( inner_prod(i.B,i.B)           );
	i.k2     = clearNaN( inner_prod(k,k)               );
	i.wp2    = clearNaN( c.getSquaredPlasmaFreq(r) );
	i.wc2    = clearNaN( c.getSquaredCycloFreq(r)  );

	i.w      =  m_wave.getFreq()             ;
	i.w2     =  i.w*i.w                      ;

	const double theta = std::acos(
		inner_prod( i.B/norm_2(i.B), k/norm_2(k) )
	); // theta = 磁場と成す角
	i.s      =  std::sin(theta)              ;
	i.c      =  std::cos(theta)              ;
	i.s2     =  i.s*i.s                      ;
	i.c2     =  i.c*i.c                      ;

	i.X2     = clearNaN( i.wp2/i.w2          );
	i.Y2     = clearNaN( i.wc2/i.w2          );

	i.X4     =  i.X2*i.X2                    ;
	i.Y4     =  i.Y2*i.Y2                    ;
	i.iX2    =  1-i.X2                       ;
	i.iX22   =  i.iX2*i.iX2                  ;

	i.root        = std::sqrt(
		  i.Y4 * i.s2*i.s2
		+ 4.0  * i.Y2*i.c2*i.iX22
	);

	i.numerator   = numerator_G  (i,r)       ;
	i.denominator = denominator_G(i,r)       ;
}

// 光線の状態チェック //////////////////////////////////////////////////
void ray::checkState(
	const ray::intermediate& i,
	const vector_pair& drk,
	const vector& r,
	const vector& k
) const {

	// 光が消えてないかどうかチェックする。
	if( i.numerator >= i.denominator )
		throw std::runtime_error(log("the ray was evanesced."));


	// 不正なNaN値は例外エラーを投げる。
	const bool isNaN_r = 
		(isnan(r[0]) != 0) |
		(isnan(r[1]) != 0) |
		(isnan(r[2]) != 0) ;

	if( isNaN_r )
		throw std::runtime_error(
			log("r vector reached an invalid value.")
	);

	const bool isNaN_k =
		 (isnan(k[0]) != 0) |
		 (isnan(k[1]) != 0) |
		 (isnan(k[2]) != 0) ;

	if( isNaN_k )
		throw std::runtime_error(
			log("k vector reached an invalid value.")
	);

	// 屈折率が 0 < n <= 1 にあることを確認する。
	// ちなみにこの範囲を逸脱したらモード変換が起こるらしい。
	const double n = cnst::c * norm_2(k)/( 2*cnst::pi*getWaveParam().getFreq() );
	
	if( n <= 0.0 || 1.0 < n )
		throw std::runtime_error(
			log("core::ray : The refractive index reached outside the range.")
	);
	
	//�����V�~�����[�V�����̏ꍇ�ȉ����p cf)tracer.inl

	if( ray::checkReflection(r,drk) == 1 )
		throw std::range_error(
			log("enter solid part.")
	);

}

// �����̔��˂̗L�����`�F�b�N //////////////////////////////////////////////////
int ray::checkReflection (
	const vector&        r,
	const vector_pair& drk	
) const{
	const cosmos& c = getCosmos();
	double Ref1 = c.getHight(r);
	double Ref2 = c.getHight(r-drk.first);
	double Ref = Ref1 *Ref2;

	if (Ref > 0)
		{
		return 0;
		}
	else
		{
		return 1;
		}
}

// 波数ベクトルの初期値 ////////////////////////////////////////////////
ray* ray::initialize(
	const vector& r,
	const vector& k
) {
	// r(x,y,z)からk(x,y,z)方向を向いている
	// 波数ベクトルを生成して返す。
	intermediate i;
	update_intermediate(i,r,k);
	const vector k_new = k
		* std::sqrt( 1 - i.numerator/i.denominator )
		* m_wave.getFreq()
		/( cnst::c * norm_2(k) )
	;

	// initializeで m_im も初期化しなければならない。
	update_intermediate(m_im,r,k_new);
	checkState(m_im,m_drk,r,k_new);
	
	// initializeで、m_dt_beforeも初期化しなければならない。
	m_dt_before = getWaveParam().getTimeStep().second;

	// kベクトルの絶対値は、関数Gを変形することで出すことができる。
	m_rk = vector_pair( r, k_new );
	
	return this;
}

ray* ray::initialize(
	double rx, double ry, double rz,
	double kx, double ky, double kz
) {
	assert( kx*kx + ky*ky + kz*kz != 0 );

	vector
		r = boost::numeric::ublas::zero_vector<double>(3),
		k = boost::numeric::ublas::zero_vector<double>(3);
	r(0) = rx; r(1) = ry; r(2) = rz;
	k(0) = kx; k(1) = ky; k(2) = kz;

	return initialize(r,k);
}

ray* ray::initialize(
	const vector& r,
	double pitch, double round
) {
	// r(rx,ry,rz)における磁場Bについて、磁気モーメントベクトルをNとすると
	// ベクトルBを(N x B)を軸としてpitch角回転し、次にBを軸にroundだけ回転した
	// 方向を波数ベクトルとして生成し、初期ベクトルペアを返す。
	vector
		m = getCosmos().getPlanet().getMagnet().getMagneticMoment(),
		B = getCosmos().getMagnetField(r);

	// 磁場がないとまずいのでチェック。
	double norm_m = norm_2(m), norm_B = norm_2(B);

	if( norm_m == 0.0 )
	{
		// 磁場モーメントは、とりあえずZ軸方向に伸びるものを使う。
		log("libraytrace: ray<>::initialize: "
			"magnetic moment not exists, instead we use vector(0,0,1).");
		m(2) = 1.0;
		norm_m = norm_2(m);
	}
	if( norm_B == 0.0 )
	{
		// 磁場は、とりあえず-mをつかう。
		B = -m;
		norm_B = norm_2(B);
	}

    // 回転で利用するために、単位ベクトル化する。
    m /= norm_m;
    B /= norm_B;

	// 第一段の回転。この回転は、N×Bを軸として、pitch[rad]だけ回転する。
	// outer_prod()はテンソル積なので使えなかったりする。
	vector n = boost::numeric::ublas::zero_vector<double>(3);
	n(0) = m(1)*B(2)-m(2)*B(1),
	n(1) = m(2)*B(0)-m(0)*B(2),
	n(2) = m(0)*B(1)-m(1)*B(0);

	vector vk = rotation( B, n, pitch );
	
	// 第２段の回転。この回転は、Bを軸としてround[rad]だけ回転する。
	vk = rotation( vk, B, round );

	// 回転した結果が、波数ベクトルの方向である。
	return initialize(r,vk);
}

ray* ray::initialize(
	double rx, double ry, double rz,
	double pitch, double round
) {

	vector r = boost::numeric::ublas::zero_vector<double>(3);
	r(0) = rx; r(1) = ry; r(2) = rz;

	return initialize( r, pitch, round );
}

// 1stepの時間量を計算 /////////////////////////////////////////////////
double ray::calc_dt(
	const vector_pair&      rk,
	const vector_pair&     drk,
	ray::intermediate&   out_i
) const {
	assert( m_dt_before > 0.0 );

	// 前後ベクトル長の許容範囲
	const double precision = m_wave.getPrecision();

	// step.first == max, step.second == min;
	const std::pair<double,double>& step = getWaveParam().getTimeStep();
	double        dt = m_dt_before;
	const  double abs_drdt = norm_2(drk.first); // dr/dtの長さ

	// dt = m_dt_before 時の前後ベクトルを比較し、
	// 許容範囲内なら、許容範囲ぎりぎりまでdtをのばし
	// 許容範囲外なら、許容範囲までdtをけずる。
	vector
		r = rk.first  + dt*drk.first, // Yasuda Master thesis 2022 (2.13)
		k = rk.second + dt*drk.second; // Yasuda Master thesis 2022 (2.14)

	intermediate i;
	update_intermediate(i,r,k);

	double ratio = norm_2(
		calc_dGdk(i,r,k)/calc_dGdw(i,r,k)
	) / abs_drdt; // dr/dt nextとdr/dtの比率


	// 時間ステップの調整
	if(
	   1.0 - precision < ratio && ratio < 1.0 + precision &&
	   abs_drdt*dt < m_wave.getStepLength()               &&
	   dt < step.first                                    &&
	   i.numerator < i.denominator
	){
		// 誤差範囲内であり、かつ
		// ２倍の時間にしても、時間制限内＆距離制限内である
		// 誤差範囲内であっても、時間と距離制限にかかるのであれば
		// これ以上時間を増やすことはできない。
		do
		{
			out_i = i;
			dt *= 2.0;

			r = rk.first  + dt*drk.first,
			k = rk.second + dt*drk.second;
			update_intermediate(i,r,k);

			ratio = norm_2(
				calc_dGdk(i,r,k)/calc_dGdw(i,r,k)
			) / abs_drdt;

		} while (
			1.0 - precision < ratio && ratio < 1.0 + precision &&
			abs_drdt*dt < m_wave.getStepLength()               && 
			dt < step.first /* 時間、距離が範囲内である*/      &&
			i.numerator < i.denominator
		);

#ifndef RTC_RAYTRACE_RAY_LOGS_OUT_OF_TIME_RANGE
		if( dt >= step.first )
		{ log("ray::calc_dt : out of dt range, over."); }
#endif//RTC_RAYTRACE_RAY_LOGS_OUT_OF_TIME_RANGE

		// dt が制限を超えたところで帰ってくるので
		// dt/2にして制限内に戻す。
		dt *= 0.5;
	}

	else
	{
		assert( dt >= step.second );

		// 距離制限を超えている可能性があるから
		// 距離制限を満たす所まで時間を減らす。
		while(
			dt          > step.second           &&
			abs_drdt*dt > m_wave.getStepLength()
		){  dt *= 0.5; };

		// 誤差範囲外であり、かつ
		// 1/2倍の時間にしても、時間制限内である。
		// 誤差範囲内であっても、時間制限にかかるのであれば
		// これ以上時間を減らすことはできない。
		// 時間を減らすメソッドなので、距離制限にかかることはあり得ない。
		out_i = i;
		do
		{
			dt *= 0.5;

			r = rk.first  + dt*drk.first,
			k = rk.second + dt*drk.second;
			update_intermediate(i,r,k);

			ratio = norm_2(
				calc_dGdk(i,r,k)/calc_dGdw(i,r,k)
			) / abs_drdt;

			if( 
			   step.second > dt               ||
			   i.numerator >= i.denominator
			){
#ifndef RTC_RAYTRACE_RAY_LOGS_OUT_OF_TIME_RANGE
				log("ray::calc_dt() : out of dt range, under.");
#endif//RTC_RAYTRACE_RAY_LOGS_OUT_OF_TIME_RANGE
				
#ifdef RTC_RAYTRACE_ENABLE_EXCEPTION_WHEN_TIMESTEP_UNDERFLOW
				throw std::runtime_error(
					"ray::calc_dt : dt range under flow."
				);
#endif//RTC_RAYTRACE_ENABLE_EXCEPTION_WHEN_TIMESTEP_UNDERFLOW
				
				// 時間制限から外れたので、2倍にして返す。
				dt *= 2.0;
				break;
			}

			out_i = i;
		} while (
			ratio < 1.0 - precision || 1.0 + precision < ratio
		);

		// dt がレート制限に入ったか、時間制限から外れたところで帰ってくる
		// dt を 0.5倍にすると、レート制限から外れる可能性がある。
	}

	assert( dt == std::min( step.first, std::max( step.second, dt ) ) );
	return m_dt_before = dt;
}


// 求める微分式 ////////////////////////////////////////////////////////
double ray::calc_dGdw(
	const ray::intermediate& i,
	const vector&            r,
	const vector&            k
) const {

	const int    ox         = m_wave.LO_or_RX();
	const double first_term = -2*i.k2*(cnst::c*cnst::c)/(i.w2*i.w);

	const double
		// numerator_G()をwで微分した結果値
		dnG_dw  = clearNaN( -4.0*(i.X2 - 2.0*i.X4)/i.w ),

		// denominator_G()をwで微分した結果値
		ddG_dw  = clearNaN(
		          4*(i.X2/i.w)
		        + 2*(i.Y2/i.w)*i.s2
				+ ox*2*(
				  -(i.Y4/i.w) * (i.s2*i.s2)
				  -2*(i.Y2/i.w) * i.c2*(i.iX22)
				  +4*(i.X2*i.Y2/i.w) * i.c2 * i.iX2
				)/i.root
		);

	return clearNaN(
		first_term + (
		 i.denominator*dnG_dw - i.numerator*ddG_dw
		) / (i.denominator * i.denominator)
	);
}

vector ray::calc_dGdr(
	const ray::intermediate& i,
	const vector&            r,
	const vector&            k
) const {
	vector result = boost::numeric::ublas::zero_vector<double>(3);
	const cosmos& c = getCosmos();
	
	const matrix dBdr = c.getDerivativeB(r);
	const int    ox   = m_wave.LO_or_RX();
	const vector dndx = c.getDerivativeDensity(r);

	// ベクトル微分なので、各成分毎に微分した結果を格納する。
	for( int n=0; n<3; ++n )
	{
		const double
			kdBdr =(   k(0)*dBdr(0,n) +   k(1)*dBdr(1,n) +   k(2)*dBdr(2,n) ),
			BdBdr =( i.B(0)*dBdr(0,n) + i.B(1)*dBdr(1,n) + i.B(2)*dBdr(2,n) );

		const double
			dc2 = clearNaN(
				  2.*i.Bk / (i.B2*i.B2*i.k2)
				  * (i.B2*kdBdr - i.Bk*BdBdr)
				),
			ds2  = -dc2,
			ds4  = clearNaN( -2.0 * (1.0-i.c2) * dc2 );


		const double
			dwp2w2 = clearNaN(
				(cnst::e*cnst::e) * dndx(n)/(i.w2 * cnst::e0*cnst::me)
			),
			dwc2w2 = clearNaN(
				2*(cnst::e*cnst::e)/(i.w2 * cnst::me*cnst::me) * BdBdr
			);

		const double
			first_term = clearNaN(
				(2-4*(i.X2))*(dwp2w2)
			),
			second_term = clearNaN(
				-2*(dwp2w2) - i.s2*(dwc2w2) - (i.Y2) * ds2
				  + ox* ( 
					   2*(i.s2*i.s2)*(i.Y2) * dwc2w2
					  + (i.Y4) * ds4 
					  + 4*i.c2 * i.iX22 * dwc2w2
					  + 4*(i.Y2)*(i.iX22)*dc2
					  - 8*(i.Y2)*(i.c2)*(i.iX2)*(dwp2w2)
				  ) / (2*i.root)
			);

		result(n) = clearNaN(
			( i.denominator * first_term - i.numerator * second_term )
		    / ( i.denominator * i.denominator )
		);
	}
	return result;
}

vector ray::calc_dGdk(
	const ray::intermediate& i,
	const vector&            r,
	const vector&            k
) const {
	vector result = boost::numeric::ublas::zero_vector<double>(3);

	const int    ox = m_wave.LO_or_RX();
	const double sterm_factor = clearNaN(
		i.numerator/(i.denominator*i.denominator)
	);

	for( int n=0; n<3; ++n )
	{
		const double
			dc2dk = clearNaN(
			      2*(i.Bk)/(i.B2*i.k2*i.k2)
			      *( i.B(n)*i.k2 - i.Bk*k(n) )
				),
			ds2dk = -dc2dk,
			ds4dk = -2*(1-i.c2)*dc2dk,

			first_term  = clearNaN(
				2*cnst::c*cnst::c*k(n)/i.w2
			),

			second_term = clearNaN(
				-(i.Y2)*ds2dk
				+ ox*(i.Y4*ds4dk + 4*i.Y2*i.iX22*dc2dk)/(2*i.root)
			)
		;

		result(n) = first_term - sterm_factor*second_term;
	}
	return result;
}

// =====================================================================
double ray::numerator_G(        // 関数Gの第２項の分子の値を返す。
	const ray::intermediate& i,
	const vector&            r
) const {
	return clearNaN( 2 * i.X2 * i.iX2 );
}

double ray::denominator_G(      // 関数Gの第２項の分母の値を返す。
	const ray::intermediate& i,
	const vector&            r
) const {
	const int ox = m_wave.LO_or_RX();

	return clearNaN(
		2*i.iX2
		- i.Y2*i.s2
		+ ox*i.root
	);
}

double ray::reflect_dt (
	const vector_pair      rk,
	const vector_pair     drk,
	ray::intermediate&   out_i
) const{
	const cosmos& c = getCosmos();
	double        dt1 = 0.0;
	double        dt2 = m_dt_before;
	double Hight1 = std::fabs(c.getHight(rk.first));

	while (Hight1 > 0.001)
	{
		double ddt  = (dt1+dt2)/2;
		vector point1 = rk.first + drk.first * dt1;
		vector point2 = rk.first + drk.first * dt2;
		vector point3 = rk.first + drk.first * ddt;

		if (c.getHight(point1) * c.getHight(point3) > 0)
			{
			dt1 = ddt;
			}
		if (c.getHight(point2) * c.getHight(point3) > 0)
			{
			dt2 = ddt;
			}
		point1 = rk.first + drk.first * dt1;
		Hight1 = std::fabs(c.getHight(point1));
	}
		return dt1;
}


vector ray::reflect_n (
	const vector_pair      rk,
	ray::intermediate&   out_i
) const{
	const cosmos& c = getCosmos();
	vector refpoint = rk.first;
	vector dx(3);
	dx[0] = 0.5;
	dx[1] = 0.0;
	dx[2] = 0.0;
	
	vector dy(3);
	dy[0] = 0.0;
	dy[1] = 0.5;
	dy[2] = 0.0;	

	vector dz(3);
	dz[0] = 0.0;
	dz[1] = 0.0;
	dz[2] = 0.5;

	double dfdx = c.getHight(refpoint + dx) - c.getHight(refpoint - dx);
	double dfdy = c.getHight(refpoint + dy) - c.getHight(refpoint - dy); 
	double dfdz = c.getHight(refpoint + dz) - c.getHight(refpoint - dz);

	vector n(3);
	n[0] = dfdx;
	n[1] = dfdy;
	n[2] = dfdz;

    double length = norm_2(n);
	n = n / length;
	return n;
}
