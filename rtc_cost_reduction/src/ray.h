// tracer.h: tracer �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACER_H__05BCBAAA_04CB_4D2D_B090_5261F8726BEA__INCLUDED_)
#define AFX_TRACER_H__05BCBAAA_04CB_4D2D_B090_5261F8726BEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wave_param.h"

namespace rtc { // -----------------------------------------------------
	/* *********************************************************************
	+ ray
	+    与えられたモデルを利用して、トレース計算を実行する。
	+   モデルは、basic_plasma_modelとbasic_magnetic_modelの
	+   派生クラスを渡す。rayは、計算時にそれぞれのモデルから
	+   値を参照する。
	+    M には basic_magnet_modelから派生した磁場モデルクラスを、
	+   Pには basic_plasma_modelから派生したプラズマモデルクラスを
	+   それぞれ指定しなければならない。それ以外のクラスを指定すると
	+   コンパイル時にエラーとなる。
	+
	+    インスタンスを生成した後は、take_a_step()メソッドを呼び出すことで
	+   １ステップずつ位置、波数ベクトルを進めることができる。
	+   １ステップで必ず定数時間が進むとは限らないので注意すること。
	+   進んだ時間は take_a_step()の戻り値から知ることができる。
	+
	***********************************************************************/
	class ray  
	{
	public:
		ray( const wave_parameter& wparam );
		virtual ~ray();

		// take_a_step()は位置と波数ベクトルを1ステップ進めるメソッドである。
		// r には現在の波動位置を、k には現在の波数ベクトルを指定する。
		// 呼び出した直後に、rとkが新しい値に更新される。
		// 基本的にコンストラクタに渡した timeStep時間経過後のr,kが
		// 結果として返される。ただし、光線が極めて複雑な動きをする可能性がある場所では
		// さらに細かい時間経過後の結果が返されることもある。
		// 実際に計算された１ステップ間の時間間隔は、戻り値で返される。
		//
		// 二つ目の引数は、指定したvector_pairにrとkベクトルの差分(dr, dk)が
		// 格納されて制御がかえる。省略可。
		inline double take_a_step();

		// 位置情報の取得 ////////////////////////
		// 現在の波動位置を、SM座標系で返す。
		const vector& getR() const
		{ return m_rk.first; }

		// 現在の波数ベクトルを、SM座標系で返す。
		const vector& getK() const
		{ return m_rk.second; }

		// 直前の位置との差を、SM座標系で返す。
		const vector& getDeltaR() const
		{ return m_drk.first; }

		// 直前の波数ベクトルとの差を、SM座標系で返す。
		const vector& getDeltaK() const
		{ return m_drk.second; }


		// 初期の波数ベクトルを生成するための補助関数。
		ray* initialize(
			const vector& r,
			const vector& k
		);
		ray* initialize(
			double rx, double ry, double rz,
			double kx, double ky, double kz
		);
		ray* initialize(
			const vector& r,
			double pitch, double round
		);
		ray* initialize(
			double rx, double ry, double rz,
			double pitch, double round
		);

		// 環境クラスを取得する。
		wave_parameter&       getWaveParam()       { return m_wave; }
		const wave_parameter& getWaveParam() const { return m_wave; }

		// ログメッセージの取得
		const std::ostringstream& getLog() const
		{ return m_log; }
		
	private:
		// 計算補助関数 //
		// ベースとなる式は、referenceディレクトリ以下の画像ファイルを見ること。

		// intermediate構造体には、計算途中でよく利用する値を
		// あらかじめ計算して格納しておく。
		// 
		// m_im は makeInitialVector()内部で初期化され、
		// take_a_step()で利用し、制御を返す前に
		// update_intermediate()を呼び出し、次のステップ用に更新する。
		//
		// 計算式はupdate_intermediate()関数内部をみること。
		//
		struct intermediate
		{
			vector B;
			double
				Bk,
				B2,
				k2,
				w,
				w2,
				wp2,
				wc2,
				s,c,
				s2,c2,
				X2,Y2, X4, Y4,
				iX2, iX22,
				numerator,
				denominator,
				root
			;

			intermediate& operator = ( const intermediate& r );
			bool operator ==( const intermediate& r ) const;

		} m_im;

		void update_intermediate(
			intermediate& i,
			const vector& r,
			const vector& k
		) const ;

		// 1stepの時間量を計算し、移動にかかる時間と
		// 移動後の点における intermediate を返す。
		double calc_dt(
			const vector_pair& rk,
			const vector_pair& drk,
			intermediate&      out_i
		) const;

		// 一次微分式 ///////////////////////////
		double calc_dGdw(
			const intermediate& i,
			const vector&       r,
			const vector&       k
		) const; // dG/dwを計算し、結果を返す。

		vector calc_dGdr(
			const intermediate& i,
			const vector&       r,
			const vector&       k
		) const; // dG/drを計算し、結果を返す。

		vector calc_dGdk(
			const intermediate& i,
			const vector&       r,
			const vector&       k
		) const; // dG/dkを計算し、結果を返す。

		// 二次微分式 ///////////////////////////
		double numerator_G(
			const intermediate& i,
			const vector&       r
		)   const; // 関数Gの第２項の分子の値を返す。
		double denominator_G(
			const intermediate& i,
			const vector&       r
		) const; // 関数Gの第２項の分母の値を返す。

		double reflect_dt(
			const vector_pair rk,
			const vector_pair drk,
			intermediate&      out_i
		) const;

		vector reflect_n(
			const vector_pair rk,
			intermediate&      out_i
		) const;


		// エラーチェック ///////////////////////
		void checkState(
			const intermediate& i,
			const vector_pair& drk,
			const vector& r,
			const vector& k
		) const;

		int checkReflection(
			const vector&       r,
			const vector_pair& drk
		) const;

		// ログメッセージの挿入 /////////////////
		const char* log( const char* msg ) const
		{ m_log << msg << "\n"; return msg; }

	private:
		// 位置 /////////////////////////////////
		vector_pair m_rk, m_drk;

		// パラメータ ///////////////////////////
		wave_parameter  m_wave;
		
		// dt計算に利用するパラメータ
		// mkInitialVector()で最小値に初期化され、
		// calc_dt()が呼び出されるたびに更新する。
		mutable double m_dt_before; 
		
		// トレース・ログ ///////////////////////

		mutable std::ostringstream m_log;
	};
}// namespace rtc; -----------------------------------------------------

#include "tracer.inl"

#endif // !defined(AFX_TRACER_H__05BCBAAA_04CB_4D2D_B090_5261F8726BEA__INCLUDED_)
