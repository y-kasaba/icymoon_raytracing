////////////////////////////////////////////////////////////////////////
// parameter_plasma_model.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_PARAMETER_PLASMA_MODEL_H
#define RTC_RAYTRACE_PARAMETER_PLASMA_MODEL_H

namespace rtc { namespace model {

namespace plasma {
	// titan gaussian ---------------------------------------
	// simple を titan_gaussian に変更
	// simple_plasma は　titan_gaussian_plasma に変更 
	// 後々は引数を取りたい
	class titan_gaussian : public basic_plasma_model
	{
	protected:
		double getDensity( const vector& point )   const;
	};

}}}// namespace rtc; ---------------------------------------------------

#endif//RTC_RAYTRACE_PARAMETER_PLASMA_MODEL_H
