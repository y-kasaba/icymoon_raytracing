////////////////////////////////////////////////////////////////////////
// libraytrace raytrace.h
//  This program has been written in C++.
//  Copyright (C) 2006 Miyamoto Luisch.
#ifndef RTC_RAYTRACE_TESTING_RAYTRACE_H
#define RTC_RAYTRACE_TESTING_RAYTRACE_H

class raytrace
{
	typedef std::pair< double, rtc::vector_pair > raypath_element;

public:
	raytrace(
		const testing_env*   env,
		const double       round,
		const double 	   delta_source_x,
		const double	   delta_source_y,
		const double	   delta_source_z
	);
	~raytrace();
	
	void operator ()(); // mainloop
	
	
	std::string getResult() const
	{ return m_output.str(); }

	std::string getTitle() const
	{ return m_title.str(); }
	
	const std::string& getState() const
	{ return m_state; }

	const double getProgress() const
	{ return m_progress; }
	

private:

	void mainloop();
	void report_progress( const double percent );
	void print_location( const raypath_element& ptr );
	void print_variation( const raypath_element& ptr );
	
	std::ostringstream    m_output;
	std::ostringstream    m_title;
	std::string            m_state;
	rtc::ray*                m_ray;
	const testing_env* const m_env;
	const double           m_round;
	const double           m_delta_source_x;
	const double           m_delta_source_y;
	const double           m_delta_source_z;
	volatile double     m_progress;

	std::vector< raypath_element > m_raypath;
	std::vector< raypath_element > m_rayvariation;

private:
	//危険なコピーコンストラクタを封印
	raytrace( const raytrace& );
};

#endif//RTC_RAYTRACE_TESTING_RAYTRACE_H
