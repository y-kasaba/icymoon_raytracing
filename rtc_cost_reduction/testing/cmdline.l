/*********************************************************************
	コマンドライン解析専用 lex - ただしflex専用。
	コンパイルするときは -Pcmd するべし。
	
	yylex()はコマンドラインにエラーがあったら0以外の値を返す。
**********************************************************************/
/*定義部*/
space	[\t ]+
digit	[+-]?([0-9]+(\.[0-9]*)?|\.[0-9]+)([eE][+-]?[0-9]+)?
string  [A-Za-z_][A-Za-z0-9_]*

%{
// for VC ///////////////////////////////////////
#ifdef _MSC_VER
#	include <io.h>
#	define isatty _isatty
#endif

#	include "testing.h"
	using namespace std;
	using namespace rtc;

#	undef YY_INPUT
#	define YY_INPUT(b, r, ms) (r=cmdinput_local(b,ms))
	int cmdinput_local( char* buf, int max_size );

	////////////////////////////////////////////////
	//ここで使う変数。
	static char* pszInput;
	static testing_env g_env;
%}

%s SOURCE_COORD
%s SOURCE_POS_X
%s SOURCE_POS_Y
%s SOURCE_POS_Z
%s FREQENCY
%s RAY_MODE
%s RAY_LENGTH
%s STEP_LENGTH
%s STEP_TIME
%s STEP_COUNT
%s PRECISION
%s PITCH_ANGLE
%s THREAD
%s PLOT_MODE
%s PLASMA_MODEL
%s MAGNET_MODEL
%s PLANET_OBJ
%s RAYPATH_SEGMENT
%s TIME_RANGE
%s DATE
%s TIME
%s CAVITY

/*以下にコマンドライン処理を記述。*/
%%

[ ]+ /*ignore blanks*/;

--source-coord {
    BEGIN SOURCE_COORD;
}
--source-x {
	BEGIN SOURCE_POS_X;
}
--source-y {
	BEGIN SOURCE_POS_Y;
}
--source-z {
	BEGIN SOURCE_POS_Z;
}

--freq     {
	BEGIN FREQENCY;
}

--ray-mode {
	BEGIN RAY_MODE;
}

--ray-length  {
	BEGIN RAY_LENGTH;
}

--step-length {
	BEGIN STEP_LENGTH;
}

--step-count {
	BEGIN STEP_COUNT;
}

--pitch    {
	BEGIN PITCH_ANGLE;
}

--thread {
	BEGIN THREAD;
}

--precision {
	BEGIN PRECISION;
}

--plot {
	BEGIN PLOT_MODE;
}

--plasma-model {
    BEGIN PLASMA_MODEL;
}

--magnet-model {
    BEGIN MAGNET_MODEL;
}

--planet {
	BEGIN PLANET_OBJ;
}

--ray-path-segment {
	BEGIN RAYPATH_SEGMENT;
}

--time-range {
	BEGIN TIME_RANGE;
}

--date {
	BEGIN DATE;
}

--time {
	BEGIN TIME;
}

--cavity {
	BEGIN CAVITY;
}

--without-plot-startptr {
	g_env.is_plot_startptr = false;
}

--back-trace {
    g_env.is_back_trace = true;
}

--verbose {
	++ g_env.is_verbose;
}

--parallel {
	g_env.is_parallel = true;
}


<SOURCE_COORD>(euclid)|(polar) {
	const std::string str(yytext);
    if( g_env.source_coord != testing_env::source_coord_none )
	{
		/* 複数のモードが指定されている */
		g_env.source_coord = testing_env::source_coord_error;
	}
	else if( str == "euclid" )
	{
	    g_env.source_coord = testing_env::source_coord_euclid;
	}
	else if( str == "polar" )
	{
	    g_env.source_coord = testing_env::source_coord_polar;
	}
	else
	{
	    g_env.source_coord = testing_env::source_coord_error;
	}
	BEGIN 0;
}
<SOURCE_POS_X>{digit} {
	g_env.source_x = strtod( yytext, NULL );
	BEGIN 0;
}
<SOURCE_POS_Y>{digit} {
	g_env.source_y = strtod( yytext, NULL );
	BEGIN 0;
}
<SOURCE_POS_Z>{digit} {
	g_env.source_z = strtod( yytext, NULL );
	BEGIN 0;
}

<FREQENCY>{digit}  {
	g_env.freq        = strtod( yytext, NULL );
	BEGIN 0;
}

<RAY_MODE>([Ll][Oo])|([Rr][Xx]) {
	std::string str;

	// 大文字に変換
	{
		std::string tmp( yytext );
		std::string::iterator it;

		for( it = tmp.begin(); it != tmp.end(); ++it ) {
			str.push_back( std::toupper(*it, std::locale() ) );
		}
	}
	if( "LO" == str ){
		g_env.mode = rtc::wave_parameter::LO_MODE;
	}
	else if( "RX" == str ){
		g_env.mode = rtc::wave_parameter::RX_MODE;
	}
	else throw;

	BEGIN 0;
}

<RAY_LENGTH>{digit} {
	g_env.ray_length = strtod( yytext, NULL );
	BEGIN 0;
};

<STEP_LENGTH>{digit} {
	g_env.step_length = strtod( yytext, NULL );
	BEGIN 0;
};

<STEP_COUNT>{digit} {
	g_env.step_count = static_cast<unsigned>( strtod( yytext, NULL ) );
	BEGIN 0;
}

<PRECISION>{digit} {
	g_env.precision = strtod( yytext, NULL );
	BEGIN 0;
}

<PITCH_ANGLE>{digit} {
	g_env.pitch_angle = strtod( yytext, NULL ) *(rtc::cnst::pi/180.0);
	BEGIN 0;
};

<THREAD>{digit} {
	g_env.thread = strtod( yytext, NULL );
	BEGIN 0;
}

<RAYPATH_SEGMENT>{digit} {
	g_env.ray_segment = strtod( yytext, NULL );
	BEGIN 0;
}

<TIME_RANGE>{digit}:{digit} {
	const std::string str(yytext);
	const std::string::size_type pos = str.find(":");
	const std::string
		tmax = str.substr( 0, pos ),
		tmin = str.substr( pos+1 );

	g_env.time_range.max = strtod( tmax.c_str(), NULL );
	g_env.time_range.min = strtod( tmin.c_str(), NULL );

	BEGIN 0;
}

<DATE>{digit}\/{digit}\/{digit} {
	const std::string str(yytext);

	const std::string::size_type
		p1 = str.find("/"),
		p2 = str.find("/",p1+1);

	g_env.date_time.year  = strtol( str.substr( 0, p1    ).c_str(), NULL, 10 );
	g_env.date_time.month = strtol( str.substr( p1+1, p2 ).c_str(), NULL, 10 );
	g_env.date_time.day   = strtol( str.substr( p2+1 ).c_str()    , NULL, 10 );

	BEGIN 0;
}

<TIME>{digit}:{digit}.{digit} {
	const std::string str(yytext);

	const std::string::size_type
		p1 = str.find(":"),
		p2 = str.find(".",p1+1);

	g_env.date_time.hour   = strtol( str.substr( 0, p1    ).c_str(), NULL, 10 );
	g_env.date_time.minute = strtol( str.substr( p1+1, p2 ).c_str(), NULL, 10 );
	g_env.date_time.sec    = strtol( str.substr( p2+1 ).c_str()    , NULL, 10 );

	BEGIN 0;
}

<CAVITY>{digit}\/{digit}:{digit}\/{digit}:{digit}\/{digit}:{digit} {
	const std::string str(yytext);

	const std::string::size_type
		p1 = str.find("/"),     /* fp/fc */
		p2 = str.find(":",p1+1),/* ilat center */
		p3 = str.find("/",p2+1),/* ilat range  */
		p4 = str.find(":",p3+1),/* mlt  cenver */
		p5 = str.find("/",p4+1),/* mlt  range  */
		p6 = str.find(":",p5+1);/* height upper*/
		/* p6-end : height bottom*/
	
	g_env.cavity.push_back( rtc::cavity(
		strtod( str.substr(   0,p1).c_str(), NULL ),
		strtod( str.substr(p1+1,p2).c_str(), NULL ),
		strtod( str.substr(p2+1,p3).c_str(), NULL ),
		strtod( str.substr(p3+1,p4).c_str(), NULL ),
		strtod( str.substr(p4+1,p5).c_str(), NULL ),
		strtod( str.substr(p5+1,p6).c_str(), NULL ),
		strtod( str.substr(p6+1   ).c_str(), NULL )
	));
	
	BEGIN 0;
}

<PLOT_MODE>{string} {
	const std::string str(yytext);
	if( g_env.exec_mode != testing_env::plot_none )
	{
		/* 複数のモードが指定されている */
		g_env.exec_mode = testing_env::plot_error;
	}
	if( str == "plasma" )
	{
		/* プラズマモデルをプロットする。*/
		g_env.exec_mode = testing_env::plot_plasma;
	}
	else if( str == "magnet" )
	{
		/* 磁場モデルをプロットする。*/
		g_env.exec_mode = testing_env::plot_magnet;
	}
	else if( str == "magnet-trace" )
	{
		/* 磁場モデルをトレースする。*/
		g_env.exec_mode = testing_env::trace_magnet;
	}
	else if( str == "magnet-surface" )
	{
		g_env.exec_mode = testing_env::map_magnet;
	}
	else
	{
		/* レイトレースを行う */
		g_env.exec_mode = testing_env::plot_raypath;
	}
	BEGIN 0;
}

<PLASMA_MODEL>{string} {
	const std::string str(yytext);

	if( g_env.plasma_model != testing_env::model_null )
	{
		/* 複数のモードが指定されている */
		g_env.plasma_model = testing_env::model_error;
	}
	else if( str == "test_null" )
	{
		g_env.plasma_model = testing_env::model_error;
	}
	else if( str == "simple" )
	{
		g_env.plasma_model = testing_env::model_simple;
	}
	else if( str == "titan_gaussian" )
	{
		g_env.plasma_model = testing_env::model_titan_gaussian;
	}	
	else if( str == "test_simple" )
	{
		g_env.plasma_model = testing_env::model_test_simple;
	}
	else if( str == "europa_plume" )
	{
		g_env.plasma_model = testing_env::model_europa_plume;
	}
	else if( str == "europa_nonplume" )
	{
		g_env.plasma_model = testing_env::model_europa_nonplume;
	}
	else if( str == "ganymede_nonplume" )
	{
		g_env.plasma_model = testing_env::model_ganymede_nonplume;
	}

	else if( str == "sato" )
	{
		g_env.plasma_model = testing_env::model_sato_earth;
	}
	else if( str == "lathys_europa_3d" )
	{
		g_env.plasma_model = testing_env::model_lathys_europa_3d;
	}	
	else if( str == "devine_garrett" )
	{
		g_env.plasma_model = testing_env::model_devine_garrett;
	}
	else if( str != "null" )
	{
		g_env.plasma_model = testing_env::model_error;
	}
	BEGIN 0;
}

<MAGNET_MODEL>{string} {
	const std::string str(yytext);

	if( g_env.magnet_model != testing_env::model_null )
	{
		/* 複数のモードが指定されている */
		g_env.magnet_model = testing_env::model_error;
	}
    else if( str == "simple" )
	{
		g_env.magnet_model = testing_env::model_simple;
	}
    else if( str == "test_simple" )
	{
		g_env.magnet_model = testing_env::model_test_simple;
	}
	else if( str == "igrf" )
	{
		g_env.magnet_model = testing_env::model_igrf;
	}
	else if( str == "igrf4" )
	{
		g_env.magnet_model = testing_env::model_igrf4;
	}
	else if( str == "vip4" )
	{
		g_env.magnet_model = testing_env::model_vip4;
	}
	else if( str != "null" )
	{
		g_env.magnet_model = testing_env::model_error;
	}
	else if( str != "test_null" )
	{
		g_env.magnet_model = testing_env::model_error;
	}
	BEGIN 0;
}

<PLANET_OBJ>{string} {
	const std::string str(yytext);

	if( str == "earth" ){
		g_env.planet = testing_env::planet_earth;
	}
	else if( str == "jupiter" ){
		g_env.planet = testing_env::planet_jupiter;
	}
	else if( str == "benchmark" ){
		g_env.planet = testing_env::planet_benchmark;
	}
	BEGIN 0;
}

%%
int cmdwrap(){ return 1; }
int cmdinput_local( char* buf, int max_size )
{
	const int nInputLen = strlen( pszInput );
	const int n = max_size > nInputLen ? nInputLen : max_size;
	if( n > 0 )
	{
		memcpy( buf, pszInput, n );
		pszInput += n;
	}
	return n;
}

/*以下にユーザー定義関数を記述。*/

testing_env* parseCmdline( int argc, char* argv[] )
{
	// g_env を初期化。
	g_env.source_x =
	g_env.source_y =
	g_env.source_z = 0.0;

	g_env.freq        = 0;
	g_env.mode        = rtc::wave_parameter::LO_MODE;
	g_env.step_length = rtc::cnst::c;
	g_env.ray_length  = rtc::cnst::c;
	g_env.step_count  = ~0;
	g_env.precision   = 3.74e-4;
	g_env.pitch_angle = 90 * (rtc::cnst::pi/180.0);
	g_env.thread   = 1;
	g_env.ray_segment = 100;

	g_env.is_plot_startptr = true;
	g_env.is_back_trace    = false;
	g_env.is_parallel      = false;
	g_env.is_verbose       = 0;

	g_env.time_range.max   = 1;
	g_env.time_range.min   = 1e-12;

	g_env.date_time.year   = 2000;
	g_env.date_time.month  = 1;
	g_env.date_time.day    = 1;
	g_env.date_time.hour   = 0;
	g_env.date_time.minute = 0;
	g_env.date_time.sec    = 0;

	g_env.exec_mode   = testing_env::plot_none;
	g_env.source_coord = testing_env::source_coord_none;
	g_env.magnet_model = testing_env::model_null;
	g_env.plasma_model = testing_env::model_null;
	g_env.planet       = testing_env::planet_earth;
	
	for( int n = 1; n < argc; n++ )
	{
		pszInput = argv[n];
		if( cmdlex() )
			break;
	}
	return &g_env;
}

void printHelp()
{
	// コマンドラインの使い方を表示。
	std::clog <<
		"libraytrace testing."                                                            "\n"
		"This program has been written in C++, STLport and boost."                        "\n"
		"Copyright(C) 2005-2006 Miyamoto Luisch, PPARC."                                  "\n"
		                                                                                  "\n"
		"Usage: testing [OPTIONS]..."                                                     "\n"
		                                                                                  "\n"
		"--plot    [plot-mode]    "                                                       "\n"
		"--plasma-model [name]    "                                                       "\n"
		"--magnet-model [name]    "                                                       "\n"
		"--source-pos-(x|y|z)     "                                                       "\n"
		"--source-coord [coord]   "                                                       "\n"
		"--freq [frequency]       "                                                       "\n"
		"--ray-mode    (LO|RX)    "                                                       "\n"
		"--ray-length  [length]   "                                                       "\n"
		"--step-length [length]   "                                                       "\n"
		"--step-count  [count]    "                                                       "\n"
		"--pitch       [angle]    "                                                       "\n"
		"--thread      [division] "                                                       "\n"
		"--precision   [prec]     "                                                       "\n"
		"--ray-path-segment [n]   "                                                       "\n"
		"--time-range [max]-[min] "                                                       "\n"
		"--date [year]/[mon]/[day]"                                                       "\n"
		"--time [hour]:[min].[sec]"                                                       "\n"
		"--cavity [cavity params] "                                                       "\n"
		"--verbose                "                                                       "\n"
		"--without-plot-startptr  "                                                       "\n"
		"--back-trace             "                                                       "\n"
		"--parallel               "                                                       "\n"
		                                                                                  "\n"
		"NOTE:"                                                                           "\n"
		"The calculation result is output to stdout, otherwise is output to stderr."      "\n"
		"To file the calculation results and to log the errors (on UNIX)"                 "\n"
		"\t$ ./testing 1> data.dat 2> err.log"                                            "\n"
	<< std::endl;
}
