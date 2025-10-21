////////////////////////////////////////////////////////////////////////
// libraytrace raytrace.cpp
//  This program has been written in C++.
//  Copyright (C) 2006 Miyamoto Luisch.
#include "testing.h"
#include "raytrace.h"

// -------------------------------------------------------------------
// raytrace::raytrace()
// 環境値に従って初期値を作成し、operator()に制御を渡して
// トレースを実行する。rtc::rayはコンストラクタで生成する。
//
raytrace::raytrace(
	const testing_env*   env,
	const double       round,
	const double 	   delta_source_x,
	const double	   delta_source_y,
	const double	   delta_source_z
) : m_env( env  ),
    m_ray( NULL ),
	m_round(round),
	m_delta_source_x(delta_source_x),
	m_delta_source_y(delta_source_y),
	m_delta_source_z(delta_source_z),
	m_progress( 0.0 ),
    m_state("init")
{
	// 光を構築するために、rtc::cosmos::createRay()を呼び出し
	// 宇宙に光のインスタンスを生成させる。
	// 生成する光のパラメータは、すべて引数で指定する。
	rtc::ray* ray = rtc::getCosmos().createRay(
		env->mode,                   // LO_MODEかRX_MODEを指定する。
		(2*rtc::cnst::pi)*env->freq, // 波動の角周波数を[Hz]で指定する。
		env->precision,              // 波動step前後での許容率を指定する。省略可。
		env->step_length,            // 1step毎に進める最大長を[m]で指定する。省略可。
		env->time_range.max,         // 1step毎に進める時間の最大値を指定する。省略可。
		env->time_range.min          // 1step毎に進める時間の最小値を指定する。省略可。
	);
	m_ray = ray;
	
	/// レイパスの個数を確保
	m_raypath.reserve( env->ray_segment );
////////////////////////////////////////////////////////step幅確認用
	m_rayvariation.reserve( env->ray_segment );
	
	if( env->is_verbose )
	{
		// 並列計算時用の初期位置表示
		m_output << "# source x,y,z = (" << (m_delta_source_x)/1000 << ", " << (m_delta_source_y)/1000 << ", " << (m_delta_source_z)/1000 << ") [km]" << " \n";
		m_output << " \n" << " \n";	
	}
}


// -------------------------------------------------------------------
// raytrace::~raytrace 
// raytraceクラスのデストラクタ。
// 生成された ray へのポインタを消去する。
//
raytrace::~raytrace()
{
	/// 最後に、作成した光は消去する。
	rtc::getCosmos().eraseRay( m_ray );
}


// -------------------------------------------------------------------
// raytrace::operator ()()
// 初期化した後にメインループを呼び出す。
// 終了後にバックトレースのチェックをし、
// 必要ならば初期化を行った後にバックトレースする。
//
void raytrace::operator ()()
{
	// initialize()は例外エラーを投げる可能性がある。
	// mainloop()内部も例外を投げる可能性があるから、
	// ここから catch 文を定義する。
	try
	{
		// 生成した光の位置ベクトル、波数ベクトルを初期化しなければならない。
		// 初期のベクトルペアは、位置と向きを指定して
		// rtc::ray::initialize()を呼び出すことで
		// 簡単に初期化することができる。
		//
		// initialize()の６引数版では、直交座標系で
		// 発生位置と波数ベクトルの向きを指定する。initialize()は
		// 環境モデルから適切な値に変換し、ベクトルペアをかえす。
		//
		// ３引数版では、ピッチ角とラウンド角を指定することで
		// 磁力線に対する任意の角度をつけることができる。
		// 1レイパスごとの出力データの名前はm_titleで指定する


		m_ray->initialize(
			m_delta_source_x,
			m_delta_source_y,
			m_delta_source_z,
			m_env->pitch_angle,
			m_round
		);
		m_state = "run";
		m_title << "Freq_" << m_env->freq << "Hz-X_" << (m_delta_source_x)/1000 << "km-Y_" << (m_delta_source_y)/1000 << "km-Z_" << (m_delta_source_z)/1000 << "km" <<std::endl;
		mainloop();
		
		// バックトレース ----------------------------
		if( m_env->is_back_trace )
		{
			m_state = "back";

			m_output << "\n";
			if( m_env->is_verbose ) {
				m_output << "# back trace\n";
			}

			//
			// raytrace_proc()の終点位置から、rkベクトルを逆方向に向けて
			// 同じ環境で逆方向にトレースする。
			//
			// 必ず makeInitialVector() を呼び出して初期化すること。
			// そうしなかった場合のトレース結果は未定義となる。
			//
			const rtc::vector_pair rk = m_raypath.back().second;
			m_ray->initialize(
				 rk.first, // r
				-rk.second // k
			);

			m_raypath.clear();
			mainloop();
		}
		m_state = "comp.";
	}
	
	// エラーは実行時の処理エラーの他に、
	// 光消失などの理由でこれ以上トレースできない時に
	// 投げてくることがある。
	// 基本的にエラーメッセージを表示して終了するか
	// 次の光線に処理を移せばよい。
	catch ( std::exception& e )
	{
		if( m_env->is_verbose ) {
			m_output << "## error ## " << e.what() << std::endl;
		}
		m_state = e.what();
	}
	catch( ... )
	{
		m_state = "error";
		throw;
	}
}
// -------------------------------------------------------------------
// raytrace::mainloop()
// 指定された環境、光線、初期位置からトレースを実行する。
// 環境値に指定された終点にたどり着いたとき、制御を返す。
// 基本的にワーキングスレッドで動作させるので、留意すること。
//
void raytrace::mainloop()
{
	// 光の位置をプロットする距離間隔。
	const double ray_segment
		= m_env->ray_segment != 0.0 ?
		m_env->ray_length / m_env->ray_segment : -1.0;

	// 実行した点までのレイパスの長さを格納する。
	double ray_length      = 0.0;

	// 波動発生からの経過時間[s]を格納する。
	double t = 0.0;

	// rtc::ray::take_a_step()を呼び出すことで
	// 1stepずつ進ませることができる。
	// take_a_step()は、ステップ間の経過時間を返す。
	// 波動の現在位置は、ray::getR()の戻り値から得られる。
	// ray::getDeltaR()から前後の差分ベクトルが得られるので、
	// それを足しあわせ、光路長が m_env->ray_length を超えたときに終了する。
	// また、光路長が m_env->ray_length/m_env->ray_segment (== ray_segment)
	// に達する毎にレポート表示を行う。
	
	// rtc::ray::take_a_step()は例外を投げてくることがある。
	// これを捕まえないと「不正な処理が発生」するので、
	// try{} catch(...)で確実に捕まえ、エラー処理を行う。
	// その処理は operator () に記述しているので参照のこと。
	
	// 初めに開始地点を表示する。
	if( m_env->is_plot_startptr )
	{
		report_progress( .0 );
		print_location(raypath_element(
			0.0,
			rtc::vector_pair( m_ray->getR(), m_ray->getK() )
		));
////////////////////////////////////////////////////////////////step間隔確認用
		print_variation(raypath_element(
			0.0,
			rtc::vector_pair( m_ray->getR(), m_ray->getK() )
		));
///////////////////////////////////////////////////////////////
	}

		
		for( unsigned loop = 0;
			loop < m_env->step_count;
			++loop
		){
			try
			{
				
				const double dt = m_ray->take_a_step(); // 波動を1step進める。
				t += dt;
				
				const double dr = rtc::norm_2( m_ray->getDeltaR() );
				ray_length += dr;

				// 以下、光が進んだ距離から終了点を導出。
				if( ray_length < m_env->ray_length )
				{
					
					// プログレス・バーを表示
					report_progress( std::max(
						ray_length / m_env->ray_length,
						static_cast<double>(loop) / m_env->step_count
					));
					
					// 結果をためる
					m_raypath.push_back(raypath_element(
						t,
						rtc::vector_pair( m_ray->getR(), m_ray->getK() )
					));
		//////////////////////////////////////////////////////////////////////step間隔確認用
					m_rayvariation.push_back(raypath_element(
						dt,
						rtc::vector_pair( m_ray->getDeltaR(), m_ray->getDeltaK() )
					));
		///////////////////////////////////////////////////////////////////////
				}
				else /*( ray_length >= m_env->ray_length )*/
				{
					//最終点はray_lengthちょうどに収める。
					rtc::vector r = m_ray->getR()-m_ray->getDeltaR();
					rtc::vector k = m_ray->getK()-m_ray->getDeltaK();
					
					const double lest = m_env->ray_length - (ray_length-dr);
					const double factor = lest/dr;
					r += factor * m_ray->getDeltaR();
					k += factor * m_ray->getDeltaK();
					
					m_raypath.push_back(raypath_element(
						t - dt*(1.0-factor),
						rtc::vector_pair( r, k )
					));
		///////////////////////////////////////////////////////////////////////step間隔確認用
					m_rayvariation.push_back(raypath_element(
						dt * factor,
						rtc::vector_pair( factor * m_ray->getDeltaR(), factor * m_ray->getDeltaK() )
					));
		///////////////////////////////////////////////////////////////////////
					break;
				}
			}
			
			catch(const std::range_error& e)
			{
				break;
			}
		}

	// m_outputに整理して出力
	const double n = m_raypath.size();
	const double t_step = m_raypath[n-1].first / m_env->ray_segment; // 08/06
	if( n < m_env->ray_segment )
	{
		int i = 0;
		for( i = 0; i < n; ++i )
		{
			print_location( m_raypath[i] );
////////////////////////////////////////////////////////step間隔確認用
			print_variation( m_rayvariation[i] );
		}
		/*
		for( ; i < m_env->ray_segment; ++i )
		{
			print_location( m_raypath[ static_cast<int>(n-1) ] );
///////////////////////////////////////////////////////////////////////
			print_variation( m_rayvariation[ static_cast<int>(n-1) ] );
		}
		*/
	}
	/*
	else for( int i = 0; i < m_env->ray_segment; ++i )
	{
		print_location(
			m_raypath[ static_cast<int>(i * n/m_env->ray_segment) ]
		);
		print_variation(
			m_rayvariation[ static_cast<int>(i * n/m_env->ray_segment) ]
		);
	}
	*/
	
	else // 08/06
	{
		double last_t = -100.0;
		for (int i = 0; i < n; ++i)
		{
			if (m_raypath[i].first - last_t > t_step)
			{
				print_location(m_raypath[i]);
				print_variation(m_rayvariation[i]);
				last_t = m_raypath[i].first;
			}
		}	
	}
	
	// 終了地点を確実に表示させる。
	report_progress( 1.0 );
	print_location( m_raypath.back() );
	print_variation( m_rayvariation.back() );
}


// -------------------------------------------------------------------
// raytrace::report_progress()
// トレース中のプログレス・バーを表示する
//
void raytrace::report_progress( const double percent )
{
	//進捗状況を報告
	m_progress = percent;

	if( !m_env->is_parallel && 0 == (static_cast<int>(100*percent) % 3) )
	{
		std::clog << "[";

		int n = 0;
		for( ; n < 24*percent; ++n ) std::clog << "=";
		for( ; n < 24;         ++n ) std::clog << " ";
		std::clog << "] " << static_cast<int>(100*percent) << "%\r";
	}
}


// -------------------------------------------------------------------
// raytrace::print_location()
// 波動の位置、波数ベクトル、および波動が生まれてからの経過時間を出力する。
//
void raytrace::print_location( const raypath_element& ptr )
{
	const double Re = rtc::getCosmos().getPlanet().getRadius();
	const double t  = ptr.first;
	const rtc::vector r = ptr.second.first  / Re;// 位置ベクトル
	const rtc::vector k = ptr.second.second / Re;//波数ベクトル

	m_output
		<< t << " "
		<< r(0) << " " << r(1) << " " << r(2) << " "
		<< k(0) << " " << k(1) << " " << k(2) << "   ";
}

// -------------------------------------------------------------------
// raytrace::print_variation()
// 波動の位置、波数ベクトル、および波動が生まれてからの経過時間を出力する。
//
void raytrace::print_variation( const raypath_element& ptr )
{
	const double Re = rtc::getCosmos().getPlanet().getRadius();
	const double dt  = ptr.first;
	const rtc::vector dr = ptr.second.first  / Re;// 位置ベクトル
	const rtc::vector dk = ptr.second.second / Re;// 波数ベクトル

	m_output
		<< dt << " "
		<< dr(0) << " " << dr(1) << " " << dr(2) << " "
		<< dk(0) << " " << dk(1) << " " << dk(2)
	<< "\n";
}

