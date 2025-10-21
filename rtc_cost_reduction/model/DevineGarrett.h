////////////////////////////////////////////////////////////////////////
// DevineGarrett.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2006 Miyamoto Luisch
#ifndef RTC_RAYTRACE_MODEL_DEVINE_AND_GARRETT_H
#define RTC_RAYTRACE_MODEL_DEVINE_AND_GARRETT_H

namespace rtc { namespace model { namespace plasma {
	/***********************************
	class DevineGarrett
	　このモデルは、Devine and Garrettによる
	木星電子密度分布を再現したモデルである。
	 ***********************************/

	class DevineGarrett : public rtc::basic_plasma_model 
	{
	protected:
		int create( basic_planet& mother );
		double getDensity( const vector& point ) const;

	private:
		matrix m_sm2geo;
	};

}}}

#endif//RTC_RAYTRACE_MODEL_DEVINE_AND_GARRETT_H
