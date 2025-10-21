////////////////////////////////////////////////////////////////////////
// Lathys_europa_3d.cpp
//	This is a library for software uses RAY-TRACING.
//	Copyright (C) 2005 Miyamoto Luisch
#include "StdAfx.h"
#include "lathys_europa_3d.h"
#include <iostream>
#include <netcdf.h>
#include <vector>
#include <algorithm>

using namespace rtc;
using namespace model::plasma;


void handle_error(int status) {
    if (status != NC_NOERR) {
        std::cerr << "Error: " << nc_strerror(status) << std::endl;
        exit(1);
    }
}

lathys_europa_3d::lathys_europa_3d() {
	const std::string& filename = "/home/parallels/Desktop/Parallels Shared Folders/Home/research/icymoon_raytracing/src_venv/rtc_cost_reduction/model/3d_model/summed_density.nc";
	loadData(filename);
}

void lathys_europa_3d::loadData(const std::string& filename) {
    int ncid, varid_density, varid_s_centr, varid_phys_length, varid_Gs, retval;
    nc_type var_type;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];

    // NetCDFファイルを開く
    handle_error(nc_open(filename.c_str(), NC_NOWRITE, &ncid));

    // 変数のIDを取得
    handle_error(nc_inq_varid(ncid, "Density", &varid_density));
    handle_error(nc_inq_varid(ncid, "s_centr", &varid_s_centr));
    handle_error(nc_inq_varid(ncid, "phys_length", &varid_phys_length));
    handle_error(nc_inq_varid(ncid, "gstep", &varid_Gs));

    // 密度データの次元数を取得
    handle_error(nc_inq_var(ncid, varid_density, nullptr, &var_type, &ndims, dimids, nullptr));
    for (int i = 0; i < ndims; ++i) {
        handle_error(nc_inq_dimlen(ncid, dimids[i], &dimlen[i]));
    }

    // バッファをリサイズ
    grid_len_buffer.resize(ndims);
    density_buffer.resize(dimlen[0] * dimlen[1] * dimlen[2]);
    s_center_buffer.resize(3);
    phys_length_buffer.resize(1);
    Gs_buffer.resize(3);

    for (int i = 0; i < ndims; ++i) {
    grid_len_buffer[i] = dimlen[i];
    }

    // grid_len_bufferの要素を反転
    std::reverse(grid_len_buffer.begin(), grid_len_buffer.end());

    // バッファにデータを読み込む
    handle_error(nc_get_var_float(ncid, varid_density, density_buffer.data()));
    handle_error(nc_get_var_float(ncid, varid_s_centr, s_center_buffer.data()));
    handle_error(nc_get_var_float(ncid, varid_phys_length, phys_length_buffer.data()));
    handle_error(nc_get_var_float(ncid, varid_Gs, Gs_buffer.data()));

    // std::cerr << "Finish reading data from NetCDF file!" << std::endl;

    /*
    // phys_length_bufferの要素を1000倍して単位を[m]に変換
    for (auto& val : phys_length_buffer) {
        val *= 1000;
    }


    // s_centr_bufferの出力
    std::cerr << "s_center: ";
    for (const auto& val : s_center_buffer) {
        std::cerr << val << " ";
    }
    std::cerr << std::endl;

    // phys_length_bufferの出力
    std::cerr << "phys_length: ";
    for (const auto& val : phys_length_buffer) {
        std::cerr << val << " ";
    }
    std::cerr << std::endl;

    // Gs_bufferの出力
    std::cerr << "Gs_buffer: ";
    for (const auto& val : Gs_buffer) {
        std::cerr << val << " ";
    }
    std::cerr << std::endl;

    // grid_len_bufferの出力
    std::cerr << "grid_len_buffer: ";
    for (const auto& val : grid_len_buffer) {
        std::cerr << val << " ";
    }
    std::cerr << std::endl;
    */

    // NetCDFファイルを閉じる
    if ((retval = nc_close(ncid))) {
        std::cerr << "ファイルを閉じる際にエラーが発生しました: " << retval << std::endl;
        exit(retval);
    }
    std::cout << "NetCDFファイルを正常に閉じました。" << std::endl;
}



double lathys_europa_3d::getDensity( const vector& point ) const
{
	/*    
	int ncid, varid_density, retval;
    nc_type var_type;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];

    // O2イオン密度のNetCDFファイルを開く
    nc_open("O2pl_19_04_23_t00600.nc", NC_NOWRITE, &ncid);

    // "Density"変数のIDを取得
    nc_inq_varid_density(ncid, "Density", &varid_density);

    // 変数情報を取得
    nc_inq_var(ncid, varid_density, nullptr, &var_type, &ndims, dimids, nullptr);

    // 各次元の長さを取得
    size_t dimlen[3];
    for (int i = 0; i < ndims; ++i) {
        nc_inq_dimlen(ncid, dimids[i], &dimlen[i]);
    }
    // 1次元のバッファにデータを読み込む
    std::vector<float> density_buffer(dimlen[0] * dimlen[1] * dimlen[2]);
    nc_get_var_float(ncid, varid_density, density_buffer.data());
	*/


	const double initial_pos[] = {-1.0*s_center_buffer[0]*phys_length_buffer[0], -1.0*s_center_buffer[1]*phys_length_buffer[0], -1.0*s_center_buffer[2]*phys_length_buffer[0]}; // [m]
	//const int grid_numbser[] = {dimlen[2], dimlen[1], dimlen[0]}; 
	const int grid_numbser[] = {grid_len_buffer[0], grid_len_buffer[1], grid_len_buffer[2]}; 
	const double grid_size[] = {Gs_buffer[0]*phys_length_buffer[0], Gs_buffer[1]*phys_length_buffer[0], Gs_buffer[2]*phys_length_buffer[0]}; // [m]

	double x = point[0]; // [m]
	double y = point[1]; // [m]
	double z = point[2]; // [m]

	double x2i = x-initial_pos[0];
	double y2i = y-initial_pos[1];
	double z2i = z-initial_pos[2];

	int x_ind = static_cast<int>(x2i/grid_size[0]);
	int y_ind = static_cast<int>(y2i/grid_size[1]);
	int z_ind = static_cast<int>(z2i/grid_size[2]);

	double x2l = x2i - x_ind*grid_size[0];
	double y2l = y2i - y_ind*grid_size[1];
	double z2l = z2i - z_ind*grid_size[2];

	double x2h = grid_size[0] - x2l;
	double y2h = grid_size[1] - y2l;
	double z2h = grid_size[2] - z2l;

	int index_lll = x_ind + grid_numbser[0]*y_ind + grid_numbser[0]*grid_numbser[1]*z_ind;
	int index_llh = x_ind + grid_numbser[0]*y_ind + grid_numbser[0]*grid_numbser[1]*(z_ind+1);
	int index_lhl = x_ind + grid_numbser[0]*(y_ind+1) + grid_numbser[0]*grid_numbser[1]*z_ind;
	int index_lhh = x_ind + grid_numbser[0]*(y_ind+1) + grid_numbser[0]*grid_numbser[1]*(z_ind+1);
	int index_hll = (x_ind + 1) + grid_numbser[0]*y_ind + grid_numbser[0]*grid_numbser[1]*z_ind;
	int index_hlh = (x_ind + 1) + grid_numbser[0]*y_ind + grid_numbser[0]*grid_numbser[1]*(z_ind+1);
	int index_hhl = (x_ind + 1) + grid_numbser[0]*(y_ind+1) + grid_numbser[0]*grid_numbser[1]*z_ind;
	int index_hhh = (x_ind + 1) + grid_numbser[0]*(y_ind+1) + grid_numbser[0]*grid_numbser[1]*(z_ind+1);

	double density = (x2h*y2h*z2h*density_buffer[index_lll] + x2h*y2h*z2l*density_buffer[index_llh] + x2h*y2l*z2h*density_buffer[index_lhl] + x2h*y2l*z2l*density_buffer[index_lhh] + x2l*y2h*z2h*density_buffer[index_hll] + x2l*y2h*z2l*density_buffer[index_hlh] + x2l*y2l*z2h*density_buffer[index_hhl] + x2l*y2l*z2l*density_buffer[index_hhh])/grid_size[0]/grid_size[1]/grid_size[2];

	return density;
}

