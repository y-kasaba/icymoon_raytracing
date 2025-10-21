////////////////////////////////////////////////////////////////////////
// jupiter.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_PLANET_JUPITER_H
#define RTC_RAYTRACE_PLANET_JUPITER_H

namespace rtc { namespace planet {
	
	// �n����\���f���N���X
	class jupiter : public basic_planet
	{
	public:
		jupiter(
			basic_magnet_model&   mag,        // 磁場モデルのインスタンスを指定する。
			basic_plasma_model&  plsm         // プラズマモデルのインスタンスを指定する。
		);

		// ��]�s�� --------------------------------------------------
		//virtual matrix getGEI2GEO() const;
		//virtual matrix getGEI2GSE() const;
		virtual matrix getGSE2GSM() const;
		virtual matrix getGSM2SM () const;
	};
	
}}// namespace

#endif//RTC_RAYTRACE_PLANET_JUPITER_H
