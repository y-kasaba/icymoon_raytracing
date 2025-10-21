//////////////////////////////////////////////////
// VIP4_coefficients
// Copyright(C) 2006 Miyamoto Luisch.

namespace rtc { namespace model { namespace magnet {

typedef struct vip4_coeff_element
{
	enum coeffs_mode {g,h} mode;
	int  n,m;
	double data;
} VIP4_COEFFICIENT_ELEMENT;

const static VIP4_COEFFICIENT_ELEMENT g_vip4_coeffs[] = {
	{ vip4_coeff_element::g, 1,0, 420500.e-5 },
	{ vip4_coeff_element::g, 1,1, -65900.e-5 },
	{ vip4_coeff_element::g, 2,0, -  5100.e-5 },
	{ vip4_coeff_element::g, 2,1, - 61900.e-5 },
	{ vip4_coeff_element::g, 2,2,   49700.e-5 },
	{ vip4_coeff_element::g, 3,0, -  1600.e-5 },
	{ vip4_coeff_element::g, 3,1, - 52000.e-5 },
	{ vip4_coeff_element::g, 3,2,   24400.e-5 },
	{ vip4_coeff_element::g, 3,3, - 17600.e-5 },
	{ vip4_coeff_element::g, 4,0, - 16800.e-5 },
	{ vip4_coeff_element::g, 4,1,   22200.e-5 },
	{ vip4_coeff_element::g, 4,2, -  6100.e-5 },
	{ vip4_coeff_element::g, 4,3, - 20200.e-5 },
	{ vip4_coeff_element::g, 4,4,    6600.e-5 },
	{ vip4_coeff_element::h, 1,1,   25000.e-5 },
	{ vip4_coeff_element::h, 2,1, - 36100.e-5 },
	{ vip4_coeff_element::h, 2,2,    5300.e-5 },
	{ vip4_coeff_element::h, 3,1, -  8800.e-5 },
	{ vip4_coeff_element::h, 3,2,   40800.e-5 },
	{ vip4_coeff_element::h, 3,3, - 31600.e-5 },
	{ vip4_coeff_element::h, 4,1,    7600.e-5 },
	{ vip4_coeff_element::h, 4,2,   40400.e-5 },
	{ vip4_coeff_element::h, 4,3, - 16600.e-5 },
	{ vip4_coeff_element::h, 4,4,    3900.e-5 },
}; // IGRF_COEFFICIENT_ELEMENT
const static int g_vip4_coeffs_nums = 24;

}}};// namespace rtc::model::magnet
