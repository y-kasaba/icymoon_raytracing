////////////////////////////////////////////////////////////////////////
// basic_model.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_BASIC_MODEL_H
#define RTC_RAYTRACE_BASIC_MODEL_H

namespace rtc {

	// basic_magnet_model ----------------------------------------------
	// 磁場モデルの基本クラス。磁場モデルを記述する際は、
	// basic_magnet_modelから派生しなければならない。
	// basic_magnet_modelをそのまま利用することはできない。
	//
	class basic_magnet_model
	{
		friend class basic_planet;

	public:
		// 構築・消滅 ------------------------------------------------
		basic_magnet_model();
		virtual ~basic_magnet_model();

	protected:
		// 惑星が構築された直後に、basic_planetから呼び出される。
		// 磁場モデルが惑星系に依存する場合、ここをオーバーライドし初期化する。
		// モデルが惑星系に影響を及ぼす場合、この中で惑星系を変化させなければならない。
		// オーバーライドした場合は必ず親クラスのcreate()を呼び出し、
		// その戻り値を return しなければならない。
		virtual int create( basic_planet& mother );

		// 純仮想関数 ------------------------------------------------
		// 指定位置での磁場ベクトルをかえす。
		virtual vector getField      ( const vector& pos ) const = 0;

	public:
		// 指定位置での磁場ベクトルをかえす。
		vector operator()( const vector& pos ) const;

		// 中心となる磁気モーメントベクトルを返す。
		// basic_magnetic_modelでは、惑星磁場の仮想双極子モーメントを
		// -Z軸方向にのびたベクトルとして返す。
		const vector getMagneticMoment() const;


		// 共用のメソッド --------------------------------------------
		// 以下は通常オーバーライドする必要はないが、
		// 数値的に値を導出するため、解析結果が出せるのであれば
		// オーバーライドすることで劇的な高速化が望める。

		// 指定位置から磁力線をトレースし、フットプリント座標を返す。
		// このメソッドでは、開始点から両方向に同時にプロットし、
		// 先に惑星表面にたどり着いた点を返す。
		// 同時にたどり着いた場合、正の方向へのトレース側が優先される。
		virtual vector getFootPrint(
			const vector&           sp,
			double        trace_factor
		) const;

		// 指定位置から磁力線をトレースし、磁気赤道面上にたどり着いた点を返す。
		// このメソッドでは、開始点から両方向に同時にプロットし、
		// 先に磁気赤道を通過した側を返す。
		virtual vector getEquatorPrint(
			const vector&           sp,
			double        trace_factor
		) const;


		// 所属する母天体へのインスタンスを返す。
		basic_planet& getMother()
		{ return *m_mother; }

		const basic_planet& getMother() const
		{ return *m_mother; }


		// モデルによる磁場強度をgnuplot向けに数値プロットする。
		void plotModel(
			cnst::plot_style ps,
			const double     step  = 0.1, // 一マスの幅
			const double     range = 3,   // -range から range にわたってプロットする
			const double other_param = 0.0
		) const throw();

		// traceModelでは、磁力線を惑星表面からトレースする。
		void traceModel(
			const double        b_step  = 3e-2, // 磁場の一ステップ間の距離。母天体の半径単位で指定。
			const int              div  = 9,    // 母天体表面上のスタート地点の分割数
			const double  range_factor  = 3     // トレースする範囲。
		) const throw();

		void mapSurface() const throw();

	private:
		basic_planet* m_mother;

#if defined RTC_BASIC_MAGNET_MODEL_STORE_PAST && RTC_BASIC_MAGNET_MODEL_STORE_PAST > 0
		
		// getFootPrint()の過去の計算結果をキューに保管し高速化を図る。
		
	private:
		typedef std::pair< vector, vector >    past_result_element;
		typedef std::list< past_result_element > past_results_list;
		mutable past_results_list m_pastFootPrint;
		
		bool past_search(
			const past_results_list& l,
			const vector&       in_pos,
			vector&         out_result
		) const;
		const vector& past_newElement(
			past_results_list&     l,
			const vector&     in_pos,
			const vector& in_element
		) const;
		
		// マルチスレッドに対応
		mutable boost::mutex m_pastGuard;
#endif
	};

	// basic_plasma_model ----------------------------------------------
	// プラズマモデルの基本クラス。
	// プラズマモデルを記述する際は、basic_plasma_modelから派生すると便利である。
	// basic_plasma_modelをそのまま利用することはできない。
	class basic_plasma_model  
	{
		friend class basic_planet;

	public:
		// 構築・消滅 ------------------------------------------------
		basic_plasma_model();
		virtual ~basic_plasma_model();

	protected:
		// 惑星が構築された直後に、basic_planetから呼び出される。
		// 磁場モデルが惑星系に依存する場合、ここをオーバーライドし初期化する。
		// モデルが惑星系に影響を及ぼす場合、この中で惑星系を変化させなければならない。
		// オーバーライドした場合は必ず親クラスのcreate()を呼び出し、
		// その戻り値を return しなければならない。
		virtual int create( basic_planet& mother );


	public:
		// 指定位置でのプラズマ密度を返す。単位は（おそらく）[個/m^3]
		double operator()( const vector& point ) const;


		// 共用のメソッド --------------------------------------------
		// 所属する母天体へのインスタンスを返す。
		basic_planet& getMother()
		{ return *m_mother; }

		const basic_planet& getMother() const
		{ return *m_mother; }

		// プラズマにキャビティを設定する ----------------------------
		void addCavity( const cavity& c );
		void clearCavity();

		// モデル値をgnuplot向けに数値プロットする。
		// 数値は母天体の半径単位
		void plotModel(
			cnst::plot_style ps,
			const double     step  = 0.1, // 一マスの幅
			const double     range = 3,   // -range から range にわたってプロットする
			const double other_param = 0.0
		) const throw();

	protected:
		// 純仮想関数 ------------------------------------------------
		// 指定位置に於けるプラズマ密度分布モデルから導出される密度を返す。
		// ユーザーはかならずオーバーライドする必要がある。
		// このメソッドは、operator() から呼び出される。
		virtual double getDensity( const vector& point ) const = 0;

	private:
		basic_planet* m_mother;
		
#	ifdef RTC_ENABLE_PLASMA_CAVITY
		typedef std::list<cavity> cavity_list_t;
		cavity_list_t m_cavity;
#	endif
	};

} // namespace rtc;

#endif// RTC_RAYTRACE_BASIC_MODEL_H
