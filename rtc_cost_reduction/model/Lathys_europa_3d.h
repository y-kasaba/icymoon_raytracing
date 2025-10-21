////////////////////////////////////////////////////////////////////////
// Lathys_europa_3d.h
//	This is a library for software uses RAY-TRACING.
#ifndef RTC_RAYTRACE_LATHYS_EUROPA_3D_H
#define RTC_RAYTRACE_LATHYS_EUROPA_3D_H

namespace rtc { namespace model { namespace plasma {

	class lathys_europa_3d : public basic_plasma_model
	{
	private:
		std::vector<float> density_buffer;
		std::vector<float> s_center_buffer;
		std::vector<float> phys_length_buffer;
		std::vector<float> Gs_buffer;
		std::vector<size_t> grid_len_buffer; // 次元の長さを格納するバッファ
		size_t dimlen[3];
		void loadData(const std::string& filename);

	protected:
		double getDensity( const vector& point ) const;

	public:
		lathys_europa_3d();

	};

}}}// namespace rtc

#endif//RTC_RAYTRACE_MODEL_SATO_EARTH_H
