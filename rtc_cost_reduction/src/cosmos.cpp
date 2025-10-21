//////////////////////////////////////////////////////////////////////
// cosmos.cpp
//
#include "StdAfx.h"
#include "cosmos.h"
using namespace rtc;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

cosmos::cosmos(
	const int year,
	const int month,
	const int mday,
	const int hour,
	const int min,
	const int sec  // 宇宙の時刻を UT で指定する。
	) : m_planet(NULL)
{
	// 環境変数のチェック
#if defined _OPENMP && defined RTC_RAYTRACE_ENABLE_LOG
	if (NULL == std::getenv("OMP_NUM_THREADS"))
	{
		// せっかくのOpenMPが活用されてない
		std::clog << "warning : cosmos::cosmos :\n"
					 "OMP_NUM_THREADS not defined, OpenMP is not used.\n"
					 "Please setenv OMP_NUM_THREADS."
					 "\n";
	}
#endif

	// 時刻を作成し登録
	{
		std::tm t;
		memset(&t, 0, sizeof(t));
		t.tm_year = year;
		t.tm_mon = month;
		t.tm_mday = mday;
		t.tm_hour = hour;
		t.tm_min = min;
		t.tm_sec = sec;

		setUniversalTime(t);
	}

	// グローバルに登録
	if (g_cosmo)
	{
		throw std::runtime_error(
			"cosmos::cosmos : multiple cosmos tried to be created.");
	}
	g_cosmo = this;
}

cosmos::~cosmos()
{
#ifdef RTC_RAYTRACE_ENABLE_LOG
	if (!m_rays.empty())
	{
		std::clog << "debug warning : cosmos::~cosmos :\n"
					 "rtc::ray object not deleted by eraseRay() remains. Erase automatically."
					 "\n";
	}
#endif

	// 光の破棄
	rays_t::iterator it;
	for (it = m_rays.begin(); it != m_rays.end(); ++it)
	{
		delete *it;
	}

	assert(g_cosmo);
	g_cosmo = NULL;
}

// 光線の生成 //////////////////////////////////////////////////////////
ray *cosmos::createRay(const wave_parameter &wparam)
{
	ray *r = new ray(wparam);

	boost::mutex::scoped_lock lock(m_lock);
	m_rays.insert(r);

	return r;
}

ray *cosmos::createRay(
	wave_parameter::wave_mode mode,   // 波動のモード。LO_MODEかRX_MODEを指定する。
	double                    freq,   // 波動の周波数[Hz]を指定する。
	double              prec  /* = 3.74e-4 */, // step前後の比の許容率を指定する。
	double              lstep /* = cnst::c */, // 光線が1stepで進む最大長を指定
	const double timeStep_max /* = 1e0     */, // 1stepで進む時間の最大値を指定
	const double timeStep_min /* = 1e-54   */  // 1stepで進む時間の最小値を指定
)
{
	return createRay(wave_parameter(
		mode,
		freq,
		prec,
		lstep,
		timeStep_max,
		timeStep_min));
}

void cosmos::eraseRay(ray *pray)
{
	boost::mutex::scoped_lock lock(m_lock);

	rays_t::iterator it = std::find(m_rays.begin(), m_rays.end(), pray);
	if (it != m_rays.end())
	{
		delete *it;
		m_rays.erase(it);
	}

	else
		throw std::runtime_error(
			"cosmos::eraseRay : ray object not created with cosmos::createRay() tried to be erased.");
}

// 惑星系の登録 --------------------------------------------------------
bool cosmos::registerPlanet(basic_planet &planet)
{
	const bool result = (m_planet == NULL);
	if (result)
	{
		m_planet = &planet;
		planet.create();
	}

	return result;
}

// 物理的な数値への問い合わせ ------------------------------------------
vector cosmos::getMagnetField(const vector &r) const
{
	return m_planet->getMagnet()(r);
}

// getDerivativeB() -----------------------------
// 派生されたであろう getField()を呼び出し、
// 0.5[m]だけ前後に移動して磁場の差をもとめ、
// 磁場勾配を導出する。

matrix cosmos::getDerivativeB(const vector &r) const
{
	matrix dbdr(3, 3);

	for (int n = 0; n < 3; ++n)
	{
		vector dr = boost::numeric::ublas::zero_vector<double>(3);
		dr(n) = RTC_DERIVATIVE_DISTANCE;

		const vector dB = getMagnetField(r + dr) - getMagnetField(r - dr);

		for (int i = 0; i < 3; ++i) // i = x,y,z
		{
			dbdr(i, n) = dB(i);
		}
	}
	return dbdr;
}

// =====================================================================
vector cosmos::getDerivativeDensity(const vector &r) const
{
	vector result = boost::numeric::ublas::zero_vector<double>(3);

	for (int n = 0; n < 3; ++n)
	{
		vector ra = r, rb = r;
		ra(n) += RTC_DERIVATIVE_DISTANCE / 2.0;
		rb(n) -= RTC_DERIVATIVE_DISTANCE / 2.0;

		result(n) = clearNaN(
			(getCosmos().getPlasmaDensity(ra) - getCosmos().getPlasmaDensity(rb)) / RTC_DERIVATIVE_DISTANCE);
	}
	return result;
}

double cosmos::getPlasmaDensity(const vector &r) const
{
	return m_planet->getPlasma()(r);
}

double cosmos::getHight(const vector &r) const ///////反射用機能　標高を入力（固体・プラズマ境界面からの距離を導出）　単位はm  
{
	///	double h = r(2); 
	/// double h = std::sqrt((pow(r(0), 2.0)) + (pow(r(1), 2.0)) + (pow(r(2) + 2.4103e6, 2.0))) - 2.4103e6; /// カリスト表面（完全球）
	/// double h = std::sqrt((pow(r(0), 2.0)) + (pow(r(1), 2.0)) + (pow(r(2) + 1.5608e6, 2.0))) - 1.5608e6; // エウロパ表面（完全球）
	/// double h = std::sqrt((pow(r(0), 2.0)) + (pow(r(1), 2.0)) + (pow(r(2) + 2.6341e6, 2.0))) - 2.6341e6; // ガニメデ表面（完全球）
	double h = std::sqrt((pow(r(0), 2.0)) + (pow(r(1), 2.0)) + (pow(r(2), 2.0))) - 1.5608e6; // エウロパ表面（3d model）
	return h;
}

// モデルから計算して結果を返す問い合わせ //////////////////////////////
double cosmos::getSquaredPlasmaFreq(const vector &r) const
{
	return getPlasmaDensity(r) * (cnst::e * cnst::e) / (cnst::e0 * cnst::me);
}

double cosmos::getSquaredCycloFreq(const vector &r) const
{
	const vector B = getMagnetField(r);
	const double B2 = inner_prod(B, B);

	return B2 * (cnst::e * cnst::e) / (cnst::me * cnst::me);
}
