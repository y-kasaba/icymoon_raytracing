////////////////////////////////////////////////////////////////////////
// sample_model.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_SAMPLE_MODEL_H
#define RTC_RAYTRACE_SAMPLE_MODEL_H

namespace rtc { namespace model {

namespace magnet {

	// null model -----------------------------------------
	// nullモデルでは、磁場は存在しない。
	// 常に 0 を返す。
	class null_magnet : public basic_magnet_model
	{
	protected: vector getField      ( const vector& pos ) const;
	public:    matrix getDerivativeB( const vector& pos ) const;
	};

	// simple model ---------------------------------------
	// 単純な磁場モデルを記述。
	// このモデルでは、磁場は単純な双極子磁場である。
	class simple : public basic_magnet_model
	{
	public:
		simple();

		// simple_magnetは単純な双極子磁場であるから、
		// フットプリントは解析的に導くことができる。
		// そのため、basic_magnet_model::getFootPrint()を
		// オーバーライドして高速化することができる。
		vector getFootPrint(
			const vector& sp,
			double
		) const;

		vector getEquatorPrint(
			const vector& sp,
			double
		) const;

	protected:
		vector getField( const vector& pos ) const;
	};

} namespace plasma {

	// null model -----------------------------------------
	// nullモデルでは、プラズマは存在しない。
	// 常に 0 を返す。
	class null_plasma : public basic_plasma_model
	{
	protected:
		double getDensity( const vector& point )   const;
	};

	// simple model ---------------------------------------
	// 単純なプラズマ圏モデル。
	// このモデルでは、地球周辺に強力なプラズマの膜があり、
	// x軸方向に ±２Re 離れた位置を中心に、強力なプラズマの塊が存在する。
	// 現実のプラズマ圏にかなりのデフォルメをかけたものであると考えて差し支えない。
	class simple : public basic_plasma_model
	{
	protected:
		double getDensity( const vector& point )   const;
	};

}}}// namespace rtc; ---------------------------------------------------

#endif//RTC_RAYTRACE_SAMPLE_MODEL_H
