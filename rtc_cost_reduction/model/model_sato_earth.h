////////////////////////////////////////////////////////////////////////
// model_sato_earth.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_MODEL_SATO_EARTH_H
#define RTC_RAYTRACE_MODEL_SATO_EARTH_H

namespace rtc { namespace model { namespace plasma {

	/********************************************
	class sato_earth
	　このモデルは、佐藤さん(2000年卒業,PPARC)によって
	観測された電子密度モデルを元に作成しています。

	*********************************************/
	class sato_earth : public basic_plasma_model
	{
	public:
		void test() const;

	protected:
		double getDensity( const vector& point ) const;

	public:
		struct SATO_PARAM
		{
			// このパラメータは、佐藤(2000)における
			//電子密度の導出式
			//
			//   Ne = N0 * exp( -(z-z0)/(H0 + beta*z) );
			//
			// で利用されるものである。
			// この構造体のメンバ変数名は、上式の変数名に一致する。
			// z は地上からの距離である。
			//
			// この方程式は getDensity()にインプリメントされている。

			double
				MLT,  // 磁気ローカル時刻[h]
				iLAT, // 不変磁気緯度  [deg]
				N0,   // 電子密度基数  [個/cm*3]
				z0,   // 高度基数      [km]
				H0,
				beta;
		};

		class parameter : public SATO_PARAM
		{
		public:
			parameter(){ MLT = iLAT = N0 = z0 = H0 = beta = 0.0; };
			parameter( const SATO_PARAM& p );

			parameter& operator += ( const SATO_PARAM& p );
			parameter& operator =  ( const parameter& p );

			parameter operator +( const SATO_PARAM& p ) const;
			parameter operator *( double f ) const;
		};

	private:
		double getBaseDensity(
			const int MLT,
			const int iLAT,
			const double z
		) const;

		double getMLT ( const vector& point ) const;
		double getILAT( const vector& point ) const;

		parameter getBaseParam( int index ) const;

	private:
		// パラメータの基となる変数セット.
		//9(MLT) x 40(iLAT)の基礎パラメータが格納される。
		//MLT,iLATと配列インデックスの対応は、getTriBaseParamIndices()を参照。
		const static SATO_PARAM m_baseParam[];
		
		// キャビティを生成する補正オブジェクト
		cavity m_cavity;
	};

}}}// namespace rtc

#endif//RTC_RAYTRACE_MODEL_SATO_EARTH_H
