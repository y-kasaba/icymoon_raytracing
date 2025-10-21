// wave_param.cpp: wave_parameter クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "wave_param.h"

using namespace rtc;
//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

wave_parameter::wave_parameter(
	enum wave_mode mode,
	double freq,
	double precision,
	double lstep,
	double t_max,
	double t_min
) : m_mode (mode ),
    m_freq (freq ),
	m_precision( precision ),
    m_lstep(lstep),
    m_dt( t_max, t_min )
{}

wave_parameter::wave_parameter( const wave_parameter& r )
 : m_mode ( r.m_mode  ),
   m_freq ( r.m_freq  ),
   m_precision( r.m_precision ),
   m_lstep( r.m_lstep ),
   m_dt( r.m_dt )
{}

wave_parameter::~wave_parameter()
{}

// 波動条件の問い合わせ ////////////////////////////////////////////////
int wave_parameter::LO_or_RX() const
{ return static_cast<int>(m_mode); }

wave_parameter::wave_mode wave_parameter::getMode() const
{ return m_mode; }

double wave_parameter::getStepLength() const
{ return m_lstep; }

double wave_parameter::getFreq() const
{ return m_freq; }

double wave_parameter::getPrecision() const
{ return m_precision; }

const std::pair<double,double>&
wave_parameter::getTimeStep() const
{ return m_dt; }

// 波動条件の再設定 ////////////////////////////////////////////////////
void wave_parameter::setPrecision( const double prec )
{ m_precision = prec; }

void wave_parameter::setMode(
	wave_parameter::wave_mode mode
){
	m_mode = mode;
}

void wave_parameter::setStepLength( const double lstep )
{ m_lstep = lstep; }

void wave_parameter::setFreq( const double freq )
{ m_freq = freq; }
