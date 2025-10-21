////////////////////////////////////////////////////////////////////////
// tracer.inl
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
namespace rtc {

// レイトレーシングを実行し、一歩進む //////////////////////////////////
double ray::take_a_step()
{
	// 引数からベクトルを取り出す。
	vector& r  = m_rk.first;
	vector& k  = m_rk.second;

#ifndef NDEBUG
	intermediate i;
	update_intermediate(i,r,k);

	assert( i == m_im );
#endif

	const double dwdg = 1.0/calc_dGdw(m_im,r,k);
	const vector dgdr = calc_dGdr(m_im,r,k);
	const vector dgdk = calc_dGdk(m_im,r,k);

	m_drk.first  = -dgdk*dwdg; // -drdt
	m_drk.second =  dgdr*dwdg; //  dkdt

	// calc_dt() の中で update_intermediate()が呼び出され
	// m_im が更新される。
	const double dt = calc_dt(
		m_rk, m_drk,
		m_im
	);

	// 結果を格納して、制御を返す。
	m_drk.first  *= dt;
	m_drk.second *= dt;
	r += m_drk.first;
	k += m_drk.second;

	// 新しい点での光の状態をチェックする。
	checkState(m_im,m_drk,r,k);

	//反射シミュレーションの場合以下利用 cf)ray.cpp
/*
    if(checkReflection(r,m_drk)==1)
	{
		struct timeval _time;
		gettimeofday(&_time, NULL);
		srand(_time.tv_usec);

		if((rand()%1000)/1000.0 < 0.2) //反射する可能性を記入(not 百分率)
		{
			r -= m_drk.first;  //r,k = m_rk���ɖ߂�
			k -= m_drk.second;
			m_drk.first  /= dt;
			m_drk.second /= dt;

			const double dt = reflect_dt(m_rk,m_drk,m_im);

			m_drk.first  *= dt;
			m_drk.second *= dt;
			r += m_drk.first;
			k += m_drk.second;

			const vector n = reflect_n(m_rk,m_im);
			k = k-2*inner_prod(n,k)*n;
		}
	}
*/
	return dt; 

	
	//mainloopで呼び出されるもの
	//dt=calc(), getDeltaR()=m_dkr.first, getDeltaK()=m_dkr.second
	//getR()=m_rk.first, getK()=m_rk.second ここで参照されているので値の更新が可能に
	//checkstateで固体部の判断をするためこれらの値はdtを返す前に変更する必要あり
}

}// namespace rtc
