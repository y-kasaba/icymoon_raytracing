////////////////////////////////////////////////////////////////////////
// libraytrace testing
//  This program has been written in C++.
//  Copyright (C) 2005 Miyamoto Luisch.
#include "testing.h"
#include "raytrace.h"

#include "../model.h"
#include "../planet.h"

#if defined(_MSC_VER) && _MSC_VER < 1300
namespace std
{
	using ::ctime;
	using ::time;
}
#endif

// -------------------------------------------------------------------
// getMagnetModel()
// 磁場モデルで利用可能なインスタンスをここで実体化し、
// 環境値に従って、しかるべきモデルへの参照を返す。
//
rtc::basic_magnet_model &getMagnetModel(const testing_env *env)
{
	// 磁場モデルのインスタンスはここで実体化する
	static rtc::model::magnet::null_magnet null;
	static rtc::model::magnet::test_null_magnet test_null;
	static rtc::model::magnet::simple simple;
	static rtc::model::magnet::test_simple test_simple;

	switch (env->magnet_model)
	{
	case testing_env::model_null:
		return null;

	case testing_env::model_test_null:
		return test_null;

	case testing_env::model_simple:
		return simple;

	case testing_env::model_test_simple:
		return test_simple;

	case testing_env::model_igrf:
	{
		static rtc::model::magnet::IGRF igrf;
		return igrf;
	}

	case testing_env::model_igrf4:
	{
		static rtc::model::magnet::IGRF igrf4(4);
		return igrf4;
	}

	case testing_env::model_vip4:
	{
		static rtc::model::magnet::VIP4 vip4;
		return vip4;
	}

	default:
		// error
		throw std::runtime_error(
			"an invalid magnetic-model was selected.");
	}
}

// -------------------------------------------------------------------
// getPlasmaModel()
// プラズマモデルで利用可能なインスタンスをここで実体化し、
// 環境値に従って、しかるべきモデルへの参照を返す。
//
rtc::basic_plasma_model &getPlasmaModel(const testing_env *env)
{
	// プラズマモデルのインスタンスはここで実体化する
	static rtc::model::plasma::null_plasma null;
	static rtc::model::plasma::test_null_plasma test_null;
	static rtc::model::plasma::simple simple;
	static rtc::model::plasma::titan_gaussian titan_gaussian;	
	static rtc::model::plasma::test_simple test_simple;
	static rtc::model::plasma::europa_plume europa_plume;
	static rtc::model::plasma::europa_nonplume europa_nonplume;
	static rtc::model::plasma::ganymede_nonplume ganymede_nonplume;
	static rtc::model::plasma::callisto_nonplume callisto_nonplume;
	static rtc::model::plasma::sato_earth sato;
	static rtc::model::plasma::nsumei_earth nsumei(2.0 /*Kp*/);
	static rtc::model::plasma::lathys_europa_3d lathys_europa_3d;
	static rtc::model::plasma::DevineGarrett dg;

	switch (env->plasma_model)
	{
	case testing_env::model_null:
		return null;

	case testing_env::model_test_null:
		return test_null;

	case testing_env::model_simple:
		return simple;
	
	case testing_env::model_titan_gaussian:
		return titan_gaussian;

	case testing_env::model_test_simple:
		return test_simple;

	case testing_env::model_europa_plume:
		return europa_plume;

	case testing_env::model_europa_nonplume:
		return europa_nonplume;

	case testing_env::model_ganymede_nonplume:
		return ganymede_nonplume;

	case testing_env::model_callisto_nonplume:
		return callisto_nonplume;

	case testing_env::model_sato_earth:
		return sato;

	case testing_env::model_nsumei_earth:
		return nsumei;

	case testing_env::model_lathys_europa_3d:
		return lathys_europa_3d;

	case testing_env::model_devine_garrett:
		return dg;

	default:
		// error
		throw std::runtime_error(
			"an invalid plasma-model was selected.");
	}
}

// -------------------------------------------------------------------
// getPlanet()
// 指定した惑星へのインスタンスを作成し、返す。
rtc::basic_planet &getPlanet(
	const testing_env *env,
	rtc::basic_magnet_model &m,
	rtc::basic_plasma_model &p)
{
	switch (env->planet)
	{
	case testing_env::planet_earth:
	{
		static rtc::planet::earth e(m, p);
		return e;
	}
	case testing_env::planet_jupiter:
	{
		static rtc::planet::jupiter e(m, p);
		return e;
	}
	case testing_env::planet_benchmark:
	{
		static rtc::planet::benchmark e(m, p);
		return e;
	}
	default:
		// error
		throw std::runtime_error(
			"an invalid planet was selected.");
	}
}

// -------------------------------------------------------------------
// RX_cutoff()
// 指定された位置のRXカットオフ周波数を、角周波数で返す。
//
double RX_cutoff(const rtc::vector &r)
{
	const double fp = rtc::getCosmos().getSquaredPlasmaFreq(r);
	const double fc = rtc::getCosmos().getSquaredCycloFreq(r);

	return 0.5 * std::sqrt(fc) + std::sqrt(fp + fc / 4);
}

// -------------------------------------------------------------------
// LO_cutoff()
// 指定された位置のLOカットオフ周波数を、角周波数で返す。
//
double LO_cutoff(const rtc::vector &r)
{
	const double fp = rtc::getCosmos().getSquaredPlasmaFreq(r);
	const double fc = rtc::getCosmos().getSquaredCycloFreq(r);

	const double flo = -0.5 * std::sqrt(fc) + std::sqrt(fp + fc / 4);

	return std::max(std::max(flo, std::sqrt(fp)), std::sqrt(fc));
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> Read_position()
{
    std::ifstream ifs("/media/psf/Home/research/icymoon_raytracing/src_venv/rtc_cost_reduction/testing/position.txt");
    if (ifs.fail()) {
       std::cerr << "Cannot open file\n";
       exit(0);
    }
    std::string str;
    std::vector<double> x, y, z;
    double x_temp, y_temp, z_temp;
    while (getline(ifs, str)) {
          std::stringstream ss(str);
          ss >> x_temp >> y_temp >> z_temp;
          x.push_back(x_temp);
          y.push_back(y_temp);
          z.push_back(z_temp);
    }	    		

    return std::make_tuple(x, y, z);
}


class watch
{
public:
	typedef std::list<raytrace *> rays_list;

	watch(const rays_list &r)
		: m_rays(r), m_isExit(false){};

	void operator()()
	{
		const char *head =
			"-------------------------------------------------------------------------------\n"
			" thr | proc[%]| status | thr | proc[%]| status | thr | proc[%]| status | all[%]\n";

		const int thr_size = m_rays.size();

		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC_);

		// m_rays の状況をレポートし続ける。
		int n = 0;
		rays_list::const_iterator it = m_rays.begin();

		while (!m_isExit)
		{
			// 今から1秒後の時刻を記録
			boost::xtime_get(&xt, boost::TIME_UTC_);
			xt.sec += 1;

			// スレッド一覧を表示
			if (0 == n % 60)
			{
				std::clog << head;
			}

			for (const int end_n = n + 3; n < end_n; ++n)
			{
				const double prog = (*it)->getProgress();

				std::clog << " "
						  << std::setfill(' ') << std::setw(3) << std::right
						  << n << " | "
						  << std::setfill(' ') << std::setw(5) << std::right
						  << static_cast<int>(100 * prog) << "% | "
						  << std::setfill(' ') << std::setw(6) << std::right
						  << (*it)->getState() << " |";

				if (++it == m_rays.end())
				{
					n = 0;
					it = m_rays.begin();
					break;
				}
			}

			// 全スレッドの進捗状況を報告し、改行
			double progress_all = 0.0;
			for (rays_list::const_iterator i = m_rays.begin(); i != m_rays.end(); ++i)
			{
				progress_all += (*i)->getProgress() / thr_size;
			}
			std::clog << "| "
					  << std::setfill(' ') << std::setw(4) << std::right
					  << std::setprecision(4)
					  << 100 * progress_all << "%"
					  << std::endl;

			// しばらく待って次へ。
			boost::thread::sleep(xt);
		}
	};

	void exit()
	{
		m_isExit = true;
	}

private:
	const rays_list &m_rays;
	volatile bool m_isExit;
};

// -------------------------------------------------------------------
// raytrace_start()
// 環境値に従って初期値を作成し、raytrace()に制御を渡してトレースを実行する。
// この関数を呼び出すまでに宇宙環境は完備すること。
//
int raytrace_start(testing_env *env, const std::string &header_str)
{
	// コマンドライン引数が極座標系で指定されている場合は
	// あらかじめそれを直交座標に変換しておかなければならない。
	// （2024/07）この操作は後で初期位置をテキストファイルから指定するため計算上意味をなさない
	rtc::vector source = boost::numeric::ublas::zero_vector<double>(3);
	if (env->source_coord == testing_env::source_coord_polar)
	{
		source = rtc::getCosmos().getPlanet().getLocation(
			env->source_x, // MLAT[rad]
			env->source_y, // MLT [h]
			env->source_z  // Altitude [km]
		);

		if (env->is_verbose)
		{
			std::cout << "# start ptr (FLAT, MLT, Altitude) : ( "
					  << env->source_x << ", "
					  << env->source_y << ", "
					  << env->source_z << ") : is "
					  << source / rtc::getCosmos().getPlanet().getRadius() << "[Re]"
					  << std::endl;
		}

		env->source_x = source[0];
		env->source_y = source[1];
		env->source_z = source[2];
	}
	// （2024/07）この操作も後で初期位置をテキストファイルから指定するため計算上意味をなさない
	else
	{
		const double Re = rtc::getCosmos().getPlanet().getRadius();
		source[0] = (env->source_x *= Re);
		source[1] = (env->source_y *= Re);
		source[2] = (env->source_z *= Re);
	}

	// もし env->freq == 0.0 なら、初期位置での
	// カットオフ周波数の 1.05倍値を使用する
	// （2024/07）この操作は後も初期位置をテキストファイルから指定するため計算上意味をなさない
	if (env->freq == 0.0)
	{
		double fc = 0;
		switch (env->mode)
		{
		case rtc::wave_parameter::LO_MODE:
			fc = LO_cutoff(source);
			break;

		case rtc::wave_parameter::RX_MODE:
			fc = RX_cutoff(source);
			break;
		}
		env->freq = 1.05 * fc;

		if (env->is_verbose)
		{
			std::cout << "# ray frequency is set in 0.0[Hz]. "
						 "it is set to 1.05*cutoff-frequency ("
					  << 1.05 * fc / (2 * rtc::cnst::pi) << "[Hz])"
					  << std::endl;
		}
	}

	// 初期位置のテキストファイルから読み込む箇所、env_x, y, zは使用されない
	std::vector<double> x_list, y_list, z_list;
    std::tie(x_list, y_list, z_list) = Read_position();
	int x_list_size = x_list.size(); // 配列の要素数を計算
	std::cerr << "x_list_size = " << x_list_size << std::endl;
	int loop_times = (x_list_size/env->thread) + 1;

	// 並列化しない場合
	if (!env->is_parallel)
	{
		std::cerr << "no parallel";		
		// ここでは初期位置とピッチ角を固定し、
		// 磁力線周りに３６０度回転させた波動を生成する。
		for (int round = 0; round < static_cast<int>(x_list_size); ++round)
		{
			// raytraceクラスを構築し、operator ()を呼び出してループ処理を行う。
			// 光が終着点に到達したとき、operator ()から制御が返る。
			//raytrace rtrc(env, (2 * rtc::cnst::pi / env->thread) * round, 0.0);
			raytrace rtrc(env, 0.0, x_list[round], y_list[round], z_list[round]);	
			rtrc(); // operator()を実行してる
			std::cout << rtrc.getResult() << std::endl;

			// std::stringを使用してファイル名を生成
			std::string fileName = "Pla_" + std::string(env->getModelName(env->plasma_model)) + "-Mag_" + std::string(env->getModelName(env->magnet_model)) + "-Mode_" +  std::string(env->mode == rtc::wave_parameter::LO_MODE ? "LO" : "RX")  + "-" + rtrc.getTitle();
			std::ofstream outFile(fileName);
			if (!outFile) {
				std::cerr << "Failed to open " << fileName << " for writing" << std::endl;
				continue;
			}
			outFile << header_str;
			outFile << rtrc.getResult() << std::endl;	
			outFile.close();

		}


	}

	// 並列化する場合
	else
	{
		std::cerr << "parallel";

		for (int loop = 0; loop < loop_times; ++loop)
		{
			// 並列計算用のスレッドを構築
			watch::rays_list rays;

			// 光の数だけスレッドを構築
			boost::thread_group threads;

			for (int round = loop * env->thread; round < std::min(static_cast<int>((loop+1) * env->thread), static_cast<int>(x_list_size)) ; ++round)
			{
				std::cerr << "round = " << round;
				// raytrace *r = new raytrace(env, (2 * rtc::cnst::pi / env->thread) * round, 0.0);
				raytrace *r = new raytrace(env, 0.0, x_list[round], y_list[round], z_list[round]);	
				rays.push_back(r);
				threads.create_thread(boost::ref(*r));
			}

			// 経過状態をレポートしつつ、
			watch w(rays);
			boost::thread w_thread(boost::ref(w));

			// 全トレースが終了するまで待つ。
			threads.join_all();

			w.exit();
			w_thread.join();
			
			// 結果を順に出力
			watch::rays_list::iterator it;
			for (it = rays.begin(); it != rays.end(); ++it)
			{
				///std::ofstream outFile((*it)->getTitle());
				// std::stringを使用してファイル名を生成
				std::string fileName = "Pla_" + std::string(env->getModelName(env->plasma_model)) + "-Mag_" + std::string(env->getModelName(env->magnet_model)) + "-Mode_" +  std::string(env->mode == rtc::wave_parameter::LO_MODE ? "LO" : "RX")  + "-" + (*it)->getTitle();
				std::ofstream outFile(fileName);
				if (!outFile) {
					std::cerr << "Failed to open " << fileName << " for writing" << std::endl;
					continue;
				}
				outFile << header_str;
				outFile << (*it)->getResult() << std::endl;
				std::cout << (*it)->getResult() << std::endl;			
				delete (*it);
				outFile.close();	
			}

		}

	}

	return 0;
}

// -------------------------------------------------------------------
// main()
// 書くまでもない。
//
int main(int argc, char *argv[])
{
	// 開始時刻を表示
	const std::time_t start_at = std::time(NULL);
	std::cerr << "testing start at : "
			  << std::ctime(&start_at)
			  << std::endl;

	// コマンドライン引数を解析する。
	testing_env *env = parseCmdline(argc, argv);

	std::string header = boost::str(boost::format(
		"# frequency    = %5%[Hz]"
		"\n"
		"# wave mode    = %6%"
		"\n"
		"# step length  = %7%[m]"
		"\n"
		"# ray length   = %8%[m]"
		"\n"
		"# pitch angle  = %9%[rad]"
		"\n"
		"# round div    = %26%"
		"\n"
		"# segment      = %10%"
		"\n"
		"# precision    = %23%"
		"\n"
		"# step count   = %11%"
		"\n"
		"# back trace   = %12%"
		"\n"
		"# parallel     = %25%"
		"\n"
		"# time range   = [%13%]-[%14%]"
		"\n"
		"# plasma model = %15%"
		"\n"
		"# magnet model = %16%"
		"\n"
		"# planet       = %27%"
		"\n"
		"# date         = %17%/%18%/%19% %20%:%21%.%22%"
		"\n"
		"# testing start at : %24%") %
		(env->source_coord == testing_env::source_coord_polar
			? "(mlat,mlt,h)"
			: "(x,y,z)     ") %
		env->source_x % env->source_y % env->source_z % env->freq % (env->mode == rtc::wave_parameter::LO_MODE ? "LO" : "RX") % env->step_length % env->ray_length % env->pitch_angle % env->ray_segment % env->step_count % (env->is_back_trace ? "true" : "false") % env->time_range.max % env->time_range.min % env->getModelName(env->plasma_model) % env->getModelName(env->magnet_model) % env->date_time.year % env->date_time.month % env->date_time.day % env->date_time.hour % env->date_time.minute % env->date_time.sec % env->precision % std::ctime(&start_at) % (env->is_parallel ? "true" : "false") % env->thread % env->getPlanetName(env->planet));


	if (env->is_verbose)
	{
		// プロットする条件の詳細を出力する。
		// 書式フォーマットは POSIX版printf
		std::cout << boost::format(
						 "# %1% = ( %2%, %3%, %4% )"
						 "\n"
						 "# frequency    = %5%[Hz]"
						 "\n"
						 "# wave mode    = %6%"
						 "\n"
						 "# step length  = %7%[m]"
						 "\n"
						 "# ray length   = %8%[m]"
						 "\n"
						 "# pitch angle  = %9%[rad]"
						 "\n"
						 "# round div    = %26%"
						 "\n"
						 "# segment      = %10%"
						 "\n"
						 "# precision    = %23%"
						 "\n"
						 "# step count   = %11%"
						 "\n"
						 "# back trace   = %12%"
						 "\n"
						 "# parallel     = %25%"
						 "\n"
						 "# time range   = [%13%]-[%14%]"
						 "\n"
						 "# plasma model = %15%"
						 "\n"
						 "# magnet model = %16%"
						 "\n"
						 "# planet       = %27%"
						 "\n"
						 "# date         = %17%/%18%/%19% %20%:%21%.%22%"
						 "\n"
						 "# testing start at : %24%"
						 "\n") %
						 (env->source_coord == testing_env::source_coord_polar
							  ? "(mlat,mlt,h)"
							  : "(x,y,z)     ") %
						 env->source_x % env->source_y % env->source_z % env->freq % (env->mode == rtc::wave_parameter::LO_MODE ? "LO" : "RX") % env->step_length % env->ray_length % env->pitch_angle % env->ray_segment % env->step_count % (env->is_back_trace ? "true" : "false") % env->time_range.max % env->time_range.min % env->getModelName(env->plasma_model) % env->getModelName(env->magnet_model) % env->date_time.year % env->date_time.month % env->date_time.day % env->date_time.hour % env->date_time.minute % env->date_time.sec % env->precision % std::ctime(&start_at) % (env->is_parallel ? "true" : "false") % env->thread % env->getPlanetName(env->planet) //%27%
				  << std::endl;

		int n = 0;
		std::list<rtc::cavity>::const_iterator it;
		for (it = env->cavity.begin(); it != env->cavity.end(); ++it, ++n)
		{
			std::cout << "# cavity no." << n << "#####\n"
												"# fp/fc min     : "
					  << it->getMinFpFc() << "\n"
											 "# ilat center   : "
					  << it->getInvariantLatitudeCenterPtr() << "\n"
																"# mlt  cenver   : "
					  << it->getMagnetLocalTimeCenterPtr() << "\n"
															  "# ilat range    : "
					  << it->getInvariantLatitudeRange() << "\n"
															"# mlt  range    : "
					  << it->getMagnetLocalTimeRange() << "\n"
														  "# max height    : "
					  << it->getMaxHeight() << "\n"
											   "# bottom height : "
					  << it->getBottomHeight() << "\n";
		}
	}

	try
	{

		// 宇宙を構築する。
		rtc::cosmos cosmo(
			env->date_time.year,
			env->date_time.month,
			env->date_time.day,
			env->date_time.hour,
			env->date_time.minute,
			env->date_time.sec
			// 宇宙の時刻を指定
		);
		// モデルを構築する。
		rtc::basic_magnet_model &m = getMagnetModel(env);
		rtc::basic_plasma_model &p = getPlasmaModel(env);

		// 惑星を構築する。
		rtc::basic_planet &e = getPlanet(
			env,
			m, // 磁場モデルのインスタンスを指定。
			p  // プラズマモデルのインスタンスを指定。
		);

		// プラズマ・キャビティをつくり、プラズマモデルにセットする。
		std::list<rtc::cavity>::const_iterator it;
		for (it = env->cavity.begin(); it != env->cavity.end(); ++it)
		{
			e.getPlasma().addCavity(*it);
		}

		// 完成した地球を宇宙に登録。
		cosmo.registerPlanet(e);

		// コマンドラインの結果から、実行する内容を決定。
		switch (env->exec_mode)
		{
		case testing_env::plot_raypath:
			raytrace_start(env, header);
			break;

		case testing_env::plot_plasma:
			rtc::getCosmos().getPlanet().getPlasma().plotModel(
				rtc::cnst::plot_xz,
				7e-2,
				30,
				0);
			break;

		case testing_env::plot_magnet:
			rtc::getCosmos().getPlanet().getMagnet().plotModel(
				rtc::cnst::plot_xz);
			break;

		case testing_env::trace_magnet:
			rtc::getCosmos().getPlanet().getMagnet().traceModel();
			break;

		case testing_env::map_magnet:
			rtc::getCosmos().getPlanet().getMagnet().mapSurface();
			break;

		default:
			throw std::runtime_error(
				"--plot is not specified.");
		}
	}

	catch (std::runtime_error &e)
	{
		if (env->is_verbose)
		{
			std::cout << "## error ## " << e.what() << std::endl;
		}
		std::cerr << e.what() << std::endl;
		return 0;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		printHelp();
		return -1;
	}

	// 終わった時刻を表示
	const std::time_t end_at = std::time(NULL);
	std::cerr
		<< "testing is normal end at : "
		<< std::ctime(&end_at)
		<< std::endl;

	if (env->is_verbose)
		std::cout
			<< "# testing is normal end at : "
			<< std::ctime(&end_at)
			<< std::endl;

	return 0;
}
