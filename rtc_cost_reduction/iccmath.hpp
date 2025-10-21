// iccmath.hpp
// Intel Compiler で cmath を includeするとバグるためのパッチ
//
// RTC - raytrace classes library
// copyright(C) 2006 Miyamoto Luisch, PPARC
//

// for cmath --------------------------------------

namespace std {
	double __builtin_powi( double x, int i );
	double __builtin_powif( float x, int i );
	double __builtin_powil( long double x, int i );
}
#include <cmath>
namespace std {
	inline double __buildin_powi( double x, int i )
	{ return std::pow(x,i); }
	inline double __buildin_powif( float x, int i )
	{ return std::pow(x,i); }
	inline double __buildin_powil( long double x, int i )
	{ return std::pow(x,i); }
}

// for complex -----------------------------------
namespace std {
	template<typename _Tp> class complex;
	
	float __builtin_cabsf( __complex__ float __z);
	long double __builtin_cabsl( __complex__ long double __z);

	float  __builtin_cargf( __complex__ float __z);
	double __builtin_carg( __complex__ double __z);
	long double   __builtin_cargl( __complex__ long double __z);

	__complex__ long double __builtin_ccosl( __complex__ long double __z);
	__complex__ float __builtin_ccoshf( __complex__ float __z);
	__complex__ double __builtin_ccosh(__complex__ double __z);
	__complex__ long double __builtin_ccoshl(__complex__ long double __z);

	__complex__ long double __builtin_cexpl(__complex__ long double __z);

	__complex__ long double __builtin_csinl(__complex__ long double __z);
	__complex__ float __builtin_csinhf(__complex__ float __z);
	__complex__ double __builtin_csinh(__complex__ double __z);
	__complex__ long double __builtin_csinhl(__complex__ long double __z);

	__complex__ long double __builtin_csqrtl(__complex__ long double __z);

	__complex__ long double __builtin_ctanl(__complex__ long double __z);
	__complex__ float __builtin_ctanhf(__complex__ float __z);
	__complex__ double __builtin_ctanh(__complex__ double __z);
	__complex__ long double __builtin_ctanhl(__complex__ long double __z);

	__complex__ long double __builtin_cpowl(__complex__ long double __x, __complex__ long double __y);
}
#include <complex>
/*
namespace std {
	inline float __builtin_cabsf( __complex__ float __z)
	{ return cabsf(__z); }
	inline long double __builtin_cabsl( __complex__ long double __z)
	{ return cabsl(__z);}

	inline float  __builtin_cargf( __complex__ float __z)
	{ return cargf(__z);}
	inline double __builtin_carg( __complex__ double __z)
	{ return carg(__z);}
	inline long double   __builtin_cargl( __complex__ long double __z)
	{ return cargl(__z);}

	inline __complex__ long double __builtin_ccosl( __complex__ long double __z)
	{ return ccosl(__z);}
	inline __complex__ float __builtin_ccoshf( __complex__ float __z)
	{ return ccoshf(__z);}
	inline __complex__ double __builtin_ccosh(__complex__ double __z)
	{ return ccosh(__z);}
	inline __complex__ long double __builtin_ccoshl(__complex__ long double __z)
	{ return ccoshl(__z);}

	inline __complex__ long double __builtin_cexpl(__complex__ long double __z)
	{ return cexpl(__z);}

	inline __complex__ long double __builtin_csinl(__complex__ long double __z)
	{ return csinl(__z);}
	inline __complex__ float __builtin_csinhf(__complex__ float __z)
	{ return csinhf(__z);}
	inline __complex__ double __builtin_csinh(__complex__ double __z)
	{ return csinh(__z);}
	inline __complex__ long double __builtin_csinhl(__complex__ long double __z)
	{ return csinhl(__z);}

	inline __complex__ long double __builtin_csqrtl(__complex__ long double __z)
	{ return csqrtl(__z);}

	inline __complex__ long double __builtin_ctanl(__complex__ long double __z)
	{ return ctanl(__z);}
	inline __complex__ float __builtin_ctanhf(__complex__ float __z)
	{ return ctanhf(__z);}
	inline __complex__ double __builtin_ctanh(__complex__ double __z)
	{ return ctanh(__z);}
	inline __complex__ long double __builtin_ctanhl(__complex__ long double __z)
	{ return ctanhl(__z);}

	inline __complex__ long double __builtin_cpowl(__complex__ long double __x, __complex__ long double __y)
	{ return cpowl(__x,__y);}
}*/
