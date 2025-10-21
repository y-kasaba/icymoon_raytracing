////////////////////////////////////////////////////////////////////////
// model_nsumei_earth.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_MODEL_NSUMEI_EARTH_H
#define RTC_RAYTRACE_MODEL_NSUMEI_EARTH_H

namespace rtc { namespace model { namespace plasma {
	/********************************************
	class nsumei_earth
		JGR, VOL.108, NO.A2, 2003
		P.A.Nsumei et al.

	Nsumeiによるモデルは、
	(1.4Re < r < 5.0Re) && 70 < ILAT の領域において
	ILATを利用した coefficient 0.888 のものがあり
	基本的にこのモデルを利用している。

	また、ILATを利用しないが、全領域のモデルとして
	coefficient 0.878 のものが存在し、上の範囲外においては
	利用しないモデルの結果を返している。

	*********************************************/
	class nsumei_earth : public basic_plasma_model
	{
	public:
		nsumei_earth(
			const double kp
		);

	protected:
		double getDensity( const vector& point ) const;

	private:
		// パラメータ
		const double m_Kp;   // Kp インデックス

	private:
		// デフォルトのコンストラクタは封印する
		nsumei_earth();

		// ILAT を取得。
		double getILAT( const vector& point ) const;
	};
}}}

#endif//RTC_RAYTRACE_MODEL_NSUMEI_EARTH_H
