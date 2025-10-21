////////////////////////////////////////////////////////////////////////
// test_model.h
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#ifndef RTC_RAYTRACE_TEST_MODEL_H
#define RTC_RAYTRACE_TEST_MODEL_H

namespace rtc
{
	namespace model
	{

		namespace magnet
		{

			// test_null model -----------------------------------------
			// test_nullモデルでは、磁場は存在しない。
			// 常に 0 を返す。
			class test_null_magnet : public basic_magnet_model
			{
			protected:
				vector getField(const vector &pos) const;

			public:
				matrix getDerivativeB(const vector &pos) const;
			};

			// test_simpte model --------------------------------------////////練習用（z軸方向に一定の磁場を作る）

			class test_simple : public basic_magnet_model
			{
			public:
				test_simple();

				vector getFootPrint(
					const vector &sp,
					double) const;

				vector getEquatorPrint(
					const vector &sp,
					double) const;

			protected:
				vector getField(const vector &pos) const;
			};

		}
		namespace plasma
		{

			// test_null model -----------------------------------------
			// test_nullモデルでは、プラズマは存在しない。
			// 常に 0 を返す。
			class test_null_plasma : public basic_plasma_model
			{
			protected:
				double getDensity(const vector &point) const;
			};

			// test_simple model ---------------------------------------
			class test_simple : public basic_plasma_model
			{
			protected:
				double getDensity(const vector &point) const;
			};

			// europa_plume model ---------------------------------------
			// Europaの静水圧平衡プラズマとプルームモデルを組み合わせたモデル。
			class europa_plume : public basic_plasma_model
			{
			protected:
				double getDensity(const vector &point) const;
			};

			// europa_nonplume model ---------------------------------------
			// Europaの静水圧平衡プラズマのみのモデル。
			class europa_nonplume : public basic_plasma_model
			{
			protected:
				double getDensity(const vector &point) const;
			};

			// ganymede_nonplume model ---------------------------------------
			// Ganymedeの静水圧平衡プラズマのみのモデル。
			class ganymede_nonplume : public basic_plasma_model
			{
			protected:
				double getDensity(const vector &point) const;
			};

			// callisto_nonplume model ---------------------------------------
			// Callistoの静水圧平衡プラズマのみのモデル。
			class callisto_nonplume : public basic_plasma_model
			{
			protected:
				double getDensity(const vector &point) const;
			};

		}
	}
} // namespace rtc; ---------------------------------------------------

#endif // RTC_RAYTRACE_TEST_MODEL_H
