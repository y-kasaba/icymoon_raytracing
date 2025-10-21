////////////////////////////////////////////////////////////////////////
// cosmos.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_COSMOS_H
#define RTC_RAYTRACE_COSMOS_H

namespace rtc {

	// cosmos ------------------------------------
	// 宇宙を抽象化する。
	// 宇宙の意志によって光が生み出される。
	// 宇宙には惑星が存在する。これらの惑星は
	// 生成された後、宇宙に「登録」する必要がある。
	// 磁場やプラズマ密度などは、宇宙に問い合わせることで取得できる。
	// これによって、不要ではあると思うが、将来的に
	// 複数の惑星、あるいは衛星による複合的なモデルの演算も可能となる。
	// 尚、現在は登録できる惑星は一つのみである。
	//
	// libraytraceではシングルトンとして存在しなければならない。
	//
	class cosmos
	{
	public:
		typedef std::set<ray*>    rays_t;
		
	public:
		cosmos(
			const int year,
			const int month,
			const int mday,
			const int hour,
			const int min,
			const int sec = 0 // 宇宙の時刻を UT で指定する。
		);
		virtual ~cosmos();
		
		// ���� ------------------------------------------------------
		ray* createRay( const wave_parameter& wparam );
		ray* createRay(
			wave_parameter::wave_mode mode,   // 波動のモード。LO_MODEかRX_MODEを指定する。
			double                    freq,   // 波動の周波数[Hz]を指定する。
			double              prec  = 3.74e-4, // step前後の比の許容率を指定する。
			double              lstep = cnst::c, // 光線が1stepで進む最大長を指定
			const double timeStep_max = 1e0,  // 1stepで進む時間の最大値を指定
			const double timeStep_min = 1e-54 // 1stepで進む時間の最小値を指定
		);
		
		// 生成された光線へのアクセス
		const rays_t& getRays() const
		{ return m_rays; }
		
		// 光線の破棄
		void eraseRay( ray* pray );


		// 時刻 ------------------------------------------------------
		const std::tm& getUniversalTime() const
		{ return m_UniversalTime; }

		void setUniversalTime( const std::tm& ut )
		{ std::memcpy( &m_UniversalTime, &ut, sizeof(ut) ); }


		// 惑星 ------------------------------------------------------
		const basic_planet& getPlanet() const
		{ return *m_planet; }
		
		bool registerPlanet( basic_planet& planet );


		// 物理的数値へのアクセス ------------------------------------
		// 位置rでの磁場ベクトルを返す。
		vector getMagnetField( const vector& r ) const;
		
		// 磁場勾配を行列として返す。
		// 値は以下の行列で返すようにしなければならない。
		// | dBx/dx dBx/dy dBx/dz |
		// | dBy/dx dBy/dy dBy/dz |
		// | dBz/dx dBz/dy dBz/dz |
		//
		// cosmosでは、cosmos::getMagnetField()を複数回呼び出し、
		// 磁場の傾きを数値的に計算するメソッドが記述されている。
		// 解析的に求めたものではないので注意すること。
		matrix getDerivativeB( const vector& r ) const;
		
		// プラズマ密度の勾配をベクトルで返す。
		// cosmosでは、cosmos::getPlasmaDensity()を複数回呼び出し、
		// 密度勾配を数値的に計算するメソッドが記述されている。
		// 解析的に求めたものではないので注意すること。
		// 勾配の間隔は、RTC_DERIVATIVE_DISTANCE で定義される。
		vector getDerivativeDensity( const vector& r ) const;
		
		// 位置rでのプラズマ密度を返す。
		double getPlasmaDensity( const vector& r ) const;
		
		// 位置rでの高さhを返す。
		double getHight( const vector& r ) const;
		
		// 位置rでのプラズマ角周波数の二乗値を返す。
		double getSquaredPlasmaFreq( const vector& r ) const;

		// 位置rでのサイクロトロン角周波数の二乗値を返す。
		double getSquaredCycloFreq ( const vector& r ) const;

	private:
		std::tm             m_UniversalTime;

		rays_t              m_rays;
		basic_planet*       m_planet;

		// マルチスレッド対応
		boost::mutex m_lock;
	};

}
#endif // !defined(AFX_ENV_H__933C5528_1711_4028_A437_6E3D8421B3E3__INCLUDED_)
