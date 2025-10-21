////////////////////////////////////////////////////////////////////////
// IGRF.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_MODEL_MAGNET_IGRF_H
#define RTC_RAYTRACE_MODEL_MAGNET_IGRF_H

namespace rtc { namespace model { namespace magnet {
	
	/******************************************
	class IGRF
		http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html, IAGA
		
		地質調査総合センター研究資料集 no.402
		Calculation of the Internatinal Geomagnetic Reference Field(3),
		Tadashi Nakatsuka et al.
	
		IAGAによる国際標準地球磁場モデルの実装。
		IAGAが５年おきにリリースするガウス係数
		を基に、特定日時の磁場を計算し、返す。
		cosmosで定義された日時が範囲外の場合、
		getField()が out_of_range例外を投げる。

		データは、IGRFがリリースするテキストファイルを
		コンストラクタで指定し、食べさせることができる。
		指定しなかった場合、デフォルトで組み込まれている
		IGRF10(2005)を使用する。
		
	******************************************/
	
	class IGRF : public basic_magnet_model
	{
	public:
		IGRF( const int dimension = 8 );
		IGRF( const char* data_file, const int dimension = 8 );

	protected:
		int create( basic_planet& mother );
		
	private:
		// 計算時に使用する項を計算し、あるいは保持し返す。
		// IAGAによるガウス係数のデータ保持
		class coefficient
		{
		public:
			// ガウス係数データの保持
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
			coefficient( const char* data_file, const int n_max );
			
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
			// igrf2c.plによって変換された配列から読み出す。
			void load();

			// こちらはIGRFで公開されているtextタイプのものを読み出す。
			void load( const char* data_file );
			
			// 9x10の二次元配列。
			// m_elements[n][m]でアクセスする事ができる。
			// アクセス範囲は、n = [1,9], m = [0,9]。
			// n = 0でのアクセスを不可能にするために、
			//  m_elements( boost::extents[ multi_array<>::extent_range(1,9)][10] )
			// で初期化する。
			// m_elementsは、構築時のUTを参照して時刻補正を行い保持する。
			// 構築後にUTを変更した場合の結果は未定義である。
			// 経年変化率は、未来のデータに関してはSV値をそのまま、
			// そうでない場合は前後年の差分から線形的に導き出す。
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

	};// class IGRF
	
}}} // namespace rtc::model::magnet;

#endif// RTC_RAYTRACE_MODEL_MAGNET_IGRF_H
