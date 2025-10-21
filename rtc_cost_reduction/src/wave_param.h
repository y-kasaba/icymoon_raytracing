// wave_param.h: wave_parameter クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVE_PARAM_H__C6B69AE0_831E_48CB_9621_49637B2FB295__INCLUDED_)
#define AFX_WAVE_PARAM_H__C6B69AE0_831E_48CB_9621_49637B2FB295__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace rtc { // -----------------------------------------------------

	// wave_parameter --------------------------------------------------
	// 計算する波動の特性を管理するクラス。
	// コンストラクタの引数に、波動のパラメータを指定して生成する。
	class wave_parameter  
	{
	public:
		// 定数
		enum wave_mode{
			LO_MODE = +1,
			RX_MODE = -1
		};

	public:
		wave_parameter(){};
		wave_parameter(
			enum wave_mode mode, // 波動のモード。LO_MODEかRX_MODEを指定する。
			double         freq, // 波動の周波数[Hz]を指定する。
			double         precision,
			double         lstep,
			double         t_max,
			double         t_min
		);
		wave_parameter( const wave_parameter& r );

		virtual ~wave_parameter();

	public:
		//値の参照
		int       LO_or_RX()      const; // LOモードならば+1、RXモードなら-1を返す。
		wave_mode getMode()       const; // enum wave_mode値を返す。
		double    getStepLength() const; // 一ステップで進む距離の＊参考値＊を返す。
		double    getFreq()       const; // 周波数を返す。
		double    getPrecision()  const; // 精度を返す。
		
		// 時間精度の範囲を返す。
		const std::pair<double,double>& getTimeStep() const;

		// 値の設定
		void setPrecision( const double prec );
		void setMode( wave_mode mode );
		void setStepLength( const double lstep );
		void setFreq( const double freq );

	private:
		wave_mode m_mode;
		double    m_freq;
		double    m_lstep;
		double    m_precision;
		std::pair<double,double> m_dt;
	};
}// namespace rtc; -----------------------------------------------------
#endif // !defined(AFX_WAVE_PARAM_H__C6B69AE0_831E_48CB_9621_49637B2FB295__INCLUDED_)
