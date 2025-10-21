////////////////////////////////////////////////////////////////////////
// plasma_cavity.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_PLASMA_CAVITY_H
#define RTC_RAYTRACE_PLASMA_CAVITY_H

namespace rtc {
	
	// プラズマキャビティを作成し、設置する。
	// 中央ILAT上のfp/fcが最低値をとるように
	// プラズマ密度を減少させる。
	class cavity
	{
		friend class basic_plasma_model;

	public:
		cavity();
		cavity(
			double min_fpfc,    // cavity中心での、大体のfp/fc値を指定する。
			double ilat_center, // cavity中心を通るILATを指定する。
			double ilat_range,  // cavityの緯度方向の半値幅を角度で指定する。
			double mlt_center,  // cavity中心を通るMLTを指定する。
			double mlt_range,   // cavityのLT方向の半値幅を時刻で指定する。
			double max_height,  // cavityが影響を及ぼす最高高度[km]を指定する。
			double bottom_height// cavity中心の高度[km]を指定する。
		);
		
		cavity& operator =( const cavity& c );
		double factor( const vector& ptr ) const;

		double getMinFpFc()                    const { return m_min_fpfc; }
		double getInvariantLatitudeCenterPtr() const { return m_ilat_center_ptr; }
		double getMagnetLocalTimeCenterPtr()   const { return m_mlt_center_ptr;  }
		double getInvariantLatitudeRange()     const { return m_ilat_range;      }
		double getMagnetLocalTimeRange()       const { return m_mlt_range;       }
		double getMaxHeight()                  const { return m_max_height;      }
		double getBottomHeight()               const { return m_bottomHeight;    }
		
		// �ȉ������I�Ɏg�p
	private:
		void create(
			const basic_planet& mother
		);
		
	private:
		double m_min_fpfc;
		double m_ilat_center_ptr;
		double m_mlt_center_ptr;
		double m_ilat_range;
		double m_mlt_range;
		double m_max_height;
		
		double m_bottomHeight;
		double m_scale;
		
		const basic_planet* m_mother;
	};
}// namespace rtc

#endif//RTC_RAYTRACE_PLASMA_CAVITY_H
