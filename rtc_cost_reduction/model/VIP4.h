////////////////////////////////////////////////////////////////////////
// VIP4.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_MODEL_MAGNET_VIP4_H
#define RTC_RAYTRACE_MODEL_MAGNET_VIP4_H

namespace rtc { namespace model { namespace magnet {
	
	/******************************************
	class VIP4
	
	******************************************/
	
	class VIP4 : public basic_magnet_model
	{
	public:
		VIP4( const int dimension = 4 );

	protected:
		int create( basic_planet& mother );
		
	private:
		// 計算時に使用する項を計算し、あるいは保持し返す。
		// IAGAによるガウス係数のデータ保持
		class coefficient
		{
		public:
			// �K�E�X�W���f�[�^�̕ێ�
			struct coefficient_element {
				coefficient_element()
				: g(0.0), G(0.0),
				  h(0.0), H(0.0)
				{};
				double g, h; // データの生の値
				double G, H; // (g|h) * ( em * (n-m)! / (n+m)! )
			};
			typedef boost::multi_array< coefficient_element, 2 > coeff_array_t;
			
		public:
			coefficient( const int n_max );
			
			// cosmosの時刻データを用いて、
			//元データから日時補正を行った結果を返す。
			double g( int n, int m ) const;
			double h( int n, int m ) const;

			// 係数に必要なファクターをあらかじめ掛け合わせた数値を返す。
			// 磁場の計算時には通常こちらを使う。
			double G( int n, int m ) const;
			double H( int n, int m ) const;

			// 保持しているデータの最大次数N+1を返す。
			int getDimensionEnd() const
			{ return m_elements.shape()[0] + m_elements.index_bases()[0]; }
			
		private:
			// ガウス係数データを読み出す。コンストラクタからのみ呼び出される。
			void load();

			// 二次元配列。
			// m_elements[n][m]でアクセスする事ができる。
			// アクセス範囲は、n = [1,4], m = [0,4]。
			coeff_array_t m_elements;
			
		} m_coefficients;
		

		// SMとGeodetic系の変換行列
		matrix
			m_sm2geo,
			m_geo2sm;
		

	protected: // 外部へのインターフェイス

		vector getField( const vector& pos ) const;

		const coefficient& getCoeff() const
		{ return m_coefficients; }

	};// class VIP4
	
}}} // namespace rtc::model::magnet;

#endif// RTC_RAYTRACE_MODEL_MAGNET_VIP4_H
