////////////////////////////////////////////////////////////////////////
// basic_planet.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_BASIC_PLANET_H
#define RTC_RAYTRACE_BASIC_PLANET_H

namespace rtc {
	
	// basic_planet ------------------------------
	// 惑星の基本クラス。
	// 惑星は磁場とプラズマの各モデルに対し
	// has a の関係にあり、どのモデルを使用するかは
	// コンストラクタで指定しなければならない。
	//
	class basic_planet
	{
		friend class cosmos;

	public:
		// 地軸、および磁軸の情報を格納する。
		class axis_info
		{
		public:
			axis_info(
				const double magnet_latitude, // 磁軸の緯度 [deg]
				const double magnet_longitude // 磁軸の経度 [deg]
			);
			axis_info( const axis_info& r );

			// 軸の再設定
			// 磁軸の緯度経度は、双極子モーメントが正の方向となる半球側の点を記述する。
			// 例えば、地球の場合は南半球上の磁極を渡さなければならない。
			void setAxis(
				const double magnet_latitude,
				const double magnet_longitude // �����̌o�x [deg]
			);

			// 軸の北半球側の位置を、緯度・経度系で表した三次元直交座標で返す。
			const vector& getGeometricRotationalAxis() const
			{ return m_rotAxis; }

			const vector& getGeometricMagneticalAxis() const
			{ return m_magAxis; }


		private:
			vector m_rotAxis;
			vector m_magAxis;
		};

	protected:
		// コンストラクタ・デストラクタ
		basic_planet(
			const double         radius,        // この惑星の半径を指定する。
			const double            VDM,        // この惑星の仮想磁気モーメントを指定する。
			const axis_info&       axis,        // 惑星の自転・磁軸情報を渡す。
			basic_magnet_model&     mag,        // 磁場モデルのインスタンスを指定する。
			basic_plasma_model&    plsm         // プラズマモデルのインスタンスを指定する。
		);

	private:
		void create();

	public:
		virtual ~basic_planet(){};

		
	public:

		// モデルへのアクセス ----------------------------------------
		basic_magnet_model& getMagnet() { return m_magnet; }
		const basic_magnet_model& getMagnet() const { return m_magnet; }

		basic_plasma_model& getPlasma() { return m_plasma; }
		const basic_plasma_model& getPlasma() const { return m_plasma; }


		// 座標変換系 ------------------------------------------------
		// 回転行列 --------------------------------------------------
		virtual matrix getGEI2GEO() const;
		virtual matrix getGEI2GSE() const;
		virtual matrix getGSE2GSM() const;
		virtual matrix getGSM2SM () const;


		// 地中かどうかの判定 ----------------------------------------
		double getRadius () const
		{ return m_radius; }

		bool isUnderSoil ( const vector& p ) const
		{ return restToSoil(p) <= 0.0; }

		virtual double restToSoil( const vector& p ) const
		{ return norm_2(p) - m_radius; }


		// �f������
		void setMagneticalAxis(
			const double magnet_latitude, // 磁軸の緯度 [deg]
			const double magnet_longitude // 磁軸の経度 [deg]
		);
		double getVirtualDipoleMagnet() const
		{ return m_vdm; }


		// 惑星に対する位置座標系 ------------------------------------
		
		// 指定位置から磁力線をトレースし、フットプリント座標を返す。
		// このメソッドは、磁場モデルの getFootPrint()を呼び出し
		// その結果を返す。
		vector getFootPrint(
			const vector& source_ptr,
			double      trace_factor
		) const;

		// 指定位置から磁力線をトレースし、磁気赤道面(Z=0)上の点を返す。
		// このメソッドは、磁場モデルの getEquatorPrint()を呼び出し
		// その結果を返す。
		vector getEquatorPrint(
			const vector& source_ptr,
			double      trace_factor
		) const;
		
		// getFootPrint()の結果から、フットプリント緯度(FLAT)を返す。
		// 結果として、不変磁気緯度(ILAT)を返す。
		double getFLAT(
			const vector&  point,
			double  trace_factor
		) const;

		// getEquatorPrint()の結果から、L値を導出し、
		// 双極子磁場を仮定した上で磁気緯度を返す。
		// 結果として、不変磁気緯度(ILAT)を返す。
		double getEqLAT(
			const vector&  point,
			double  trace_factor
		) const;

		// 双極子磁場を仮定した上で、磁気経度を 0から24の数値に直してかえす。
		// 結果として磁気ローカル時刻(MLT)をかえす。
		double getMLT(
			const vector& point
		) const;
		
		// 磁気緯度・経度と高度情報から磁力線をトレースし、目的地点の座標を返す。
		// このメソッドでは、渡された緯度・経度を満たす地表面の位置から
		// 磁場モデルに沿った方向に磁力線をトレースし、指定の高度にたどり着いた点を
		// ユークリッド空間座標系で返す。
		vector getLocation(
			const double MLAT,      // MLAT値を指定[degree] -90 <= MLAT <= 90
			const double MLT,       // MLT値を指定 [h]        0 <= MLT  <  24
			const double altitude,  // 惑星表面からの高度を指定
			const double trace_factor = 1e3, // 磁場トレースの精度
			std::vector<vector>* const out_trace_line = NULL// トレース経路を格納
		) const;


	protected:
		// 太陽方向、自転軸、磁軸 -------------------------------------
		// これらのメソッドに限り、GSE座標系で返す。
		const vector  getRotationalAxisInGSE() const;
		const vector  getMagneticalAxisInGSE() const;


	protected:
		basic_magnet_model& m_magnet;
		basic_plasma_model& m_plasma;

	private:
		const double m_radius;        // 惑星の半径
		const double m_vdm;           // 惑星の仮想磁気モーメント

		axis_info m_axisInfo; // 軸の位置

#	ifndef NDEBUG
		// テスト用のメソッド
		void test() const;
#	endif
	};
}

#endif//RTC_RAYTRACE_BASIC_PLANET_H

