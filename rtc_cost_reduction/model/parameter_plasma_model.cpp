// basic_plasma_model.cpp: basic_plasma_model クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "parameter_plasma_model.h"

using namespace rtc;
using namespace model;

// titan gaussian model //////////////////////////////////////////////////////
double plasma::titan_gaussian::getDensity( const vector& point ) const
{
	// 原点が衛星の表面におく
	// xとyは原点におけるあ接線方向にとる
	// zは原点における法線方向にとる
	//ガウシアンのような密度分布をもつ電離圏モデル

	// 天体の半径[m]
	const double 
	Rp = 2574.7 * 1.0e3;

	// バックグランドのプラズマ密度[m-3]
	const double 	
	Nbg = 10.0 * 1.0e6;

	// 電離圏におけるプラズマの最大密度（正確にはNbgを加える前の密度）[m-3]
	const double 
	Npeak = 2000.0 * 1.0e6 - Nbg;

	// 電離圏の密度のピーク高度[m]
	const double
	h = 1200.0 * 1.0e3; 

	// 電離層におけるプラズマ密度の高度分布の標準偏差
	const double
	c = 200.0 * 1.0e3;

	const double
	r = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)) + (pow(point(2) + Rp, 2.0)));

	const double
	z = r - Rp;

	const double
	n = Nbg + Npeak * exp(-pow(z - h, 2.0) / (2.0 * pow(c, 2.0)));	

	return n;
}

