#include <iostream>
#include <netcdf.h>
#include <vector>

int main() {
    int ncid, varid, retval;
    nc_type var_type;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];

    // O2イオン密度のNetCDFファイルを開く
    nc_open("O2pl_19_04_23_t00600.nc", NC_NOWRITE, &ncid);

    // "Density"変数のIDを取得
    nc_inq_varid(ncid, "Density", &varid);

    // 変数情報を取得
    nc_inq_var(ncid, varid, nullptr, &var_type, &ndims, dimids, nullptr);

    // 各次元の長さを取得
    size_t dimlen[3];
    for (int i = 0; i < ndims; ++i) {
        nc_inq_dimlen(ncid, dimids[i], &dimlen[i]);
    }

    // 3次元配列を動的に確保
    std::vector<std::vector<std::vector<float>>> data(dimlen[0], std::vector<std::vector<float>>(dimlen[1], std::vector<float>(dimlen[2])));

    // 1次元のバッファにデータを読み込む
    std::vector<float> buffer(dimlen[0] * dimlen[1] * dimlen[2]);
    nc_get_var_float(ncid, varid, buffer.data());

    // 1次元のバッファから3次元配列にデータをコピー
    size_t index = 0;
    for (size_t i = 0; i < dimlen[0]; ++i) {
        for (size_t j = 0; j < dimlen[1]; ++j) {
            for (size_t k = 0; k < dimlen[2]; ++k) {
                data[i][j][k] = buffer[index++];
            }
        }
    }

    // NetCDFファイルを閉じる
    if ((retval = nc_close(ncid))) {
        std::cerr << "ファイルを閉じる際にエラーが発生しました: " << retval << std::endl;
        return retval;
    }
    std::cout << "NetCDFファイルを正常に閉じました。" << std::endl;
    return 0;
}