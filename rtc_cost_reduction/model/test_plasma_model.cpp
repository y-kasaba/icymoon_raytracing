#include "StdAfx.h"
#include "test_model.h"

using namespace rtc;
using namespace model;

// test_null model ////////////////////////////////////////////////////////
double plasma::test_null_plasma::getDensity(const vector &point) const
{
	return 0;
}

double plasma::test_simple::getDensity(const vector &point) const ///////////////新しいプラズマモデル
{
	const double
		z = 2 * 6.4e6; // 単位m
	const double
		n = 1.15e8; // 単�? / m3
	const double
		h = std::fabs(n * pow((point(2) / z), -3));

	return h;
	/*
	return 1.0e3;
	*/
}

double plasma::europa_plume::getDensity(const vector &point) const ///////////////エウロパプリュームモデル＋エウロパ拡散並行モデル
{
	const double
		r = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)) + (pow(point(2) + 1.5608e6, 2.0)));
	const double
		rxy = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)));
	const double
		plume = std::fabs(1.0e12 * exp(-(r - 1.5608e6) / 1.5e5) * exp(-((atan2(rxy, point(2))) / 0.261799) * ((atan2(rxy, point(2))) / 0.261799)));
	const double
		t = std::fabs(9e9 * exp(-(r - 1.5608e6) / 2.4e5)); 
	const double
		d = t + plume;
	;

	return d;
}

double plasma::europa_nonplume::getDensity(const vector &point) const //////////////エウロパ拡散並行モデル
{
	const double
		r = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)) + (pow(point(2) + 1.5608e6, 2.0)));
	const double
		rxy = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)));
	const double
		t = std::fabs(4.5e8 * exp(-(r - 1.5608e6) / 6e5)); 
	;

	return t;
}
/*
double plasma::ganymede_nonplume::getDensity(const vector &point) const ///////////////ガニメデ拡散並行モデル
{
	const double
		r = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)) + (pow(point(2) + 2.6341e6, 2.0)));
	const double
		rxy = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)));
	const double
		t = std::fabs(1e8 * exp(-(r - 2.6341e6) / 10e5)); 
	;

	return t;
}
*/

double plasma::ganymede_nonplume::getDensity(const vector &point) const ///////////////ガニメデ拡散並行モデル
{
	const double
		r = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)) + (pow(point(2) + 2.6341e6, 2.0)));
	const double
		rxy = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)));
	const double
		t = std::fabs(1e8 * exp(-(r - 2.6341e6) / 10e5)); 
	;

	return t;
}

double plasma::callisto_nonplume::getDensity(const vector &point) const ///////////////カリスト拡散並行モデル
{
	const double
		r = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)) + (pow(point(2) + 2.4103e6, 2.0)));
	const double
		rxy = std::sqrt((pow(point(0), 2.0)) + (pow(point(1), 2.0)));
	const double
		t = std::fabs(1.5e8 * exp(-(r - 2.4103e6) / 4e5)); 
	;
	return t;
}