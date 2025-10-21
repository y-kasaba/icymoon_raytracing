////////////////////////////////////////////////////////////////////////
// libraytrace testing
//  This program has been written in C++.
//  Copyright (C) 2005 Miyamoto Luisch.
#ifndef RTC_RAYTRACE_TESTING_H
#define RTC_RAYTRACE_TESTING_H

// 標準で使用するライブラリへの include はここに書く。
#include <iostream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <boost/format.hpp>
#include <netcdf.h>
#include <string>
#include <fstream>
#include <sstream>
#include "../raytrace.h"

class testing_env
{
public:
	double source_x, source_y, source_z;  //（2024/07~）初期位置のテキストファイルから読み込む箇所、env_x, y, zは使用されない
	double freq;
	rtc::wave_parameter::wave_mode mode;

	double step_length, ray_length;
	double pitch_angle;
	double thread;
	double ray_segment;
	double precision;

	unsigned step_count;

	bool is_plot_startptr;
	bool is_back_trace;
	bool is_parallel;

	int is_verbose;

	std::list<rtc::cavity> cavity;

	struct __env_trange
	{
		double max;
		double min;
	} time_range;

	enum testing_execmode
	{
		plot_none,
		plot_raypath,
		plot_plasma,
		plot_magnet,
		trace_magnet,
		map_magnet,
		plot_error,
	} exec_mode;

	enum testing_coord
	{
		source_coord_none,
		source_coord_euclid,
		source_coord_polar,
		source_coord_error,
	} source_coord;

	enum model
	{
		model_null,
		model_test_null,
		model_simple,
		model_titan_gaussian,
		model_test_simple,
		model_europa_nonplume,
		model_europa_plume,
		model_ganymede_nonplume,
		model_callisto_nonplume,
		model_sato_earth,
		model_nsumei_earth,
		model_lathys_europa_3d,
		model_devine_garrett,
		model_igrf,
		model_igrf4,
		model_vip4,
		model_error
	} plasma_model,
		magnet_model;

	enum planet_tag
	{
		planet_earth,
		planet_jupiter,
		planet_benchmark,
		planet_error
	} planet;

	struct date_time
	{
		unsigned
			year,
			month, day,
			hour, minute, sec;
	} date_time;

	// utility ------------------------------------------------------
	const char *getModelName(enum model m) const
	{
		switch (m)
		{
		case model_simple:
			return "simple";

		case model_test_simple:
			return "test_simple";

		case model_titan_gaussian:
			return "titan_gaussian";

		case model_europa_plume:
			return "europa_plume";

		case model_europa_nonplume:
			return "europa_nonplume";

		case model_ganymede_nonplume:
			return "ganymede_nonplume";

		case model_callisto_nonplume:
			return "callisto_nonplume";

		case model_sato_earth:
			return "sato";

		case model_nsumei_earth:
			return "nsumei";
		
		case model_lathys_europa_3d:
			return "lathys_europa_3d";

		case model_devine_garrett:
			return "dg";

		case model_igrf:
			return "igrf";

		case model_igrf4:
			return "igrf4";

		case model_vip4:
			return "vip4";

		default:
			return "unknown";
		}
	}
	const char *getPlanetName(enum planet_tag p) const
	{
		switch (p)
		{
		case planet_earth:
			return "earth";

		case planet_jupiter:
			return "jupiter";

		case planet_benchmark:
			return "benchmark";

		default:
			return "unknown";
		}
	}
};

// 関数テンプレート /////////////////////////////
testing_env *parseCmdline(int argc, char *argv[]);
void printHelp();

#endif // RTC_RAYTRACE_TESTING_H
