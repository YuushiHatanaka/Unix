//-----------------------------------------------------------------------------
// fileモジュール
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include "file.hpp"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
file::file(std::string filename)
{
    // 初期化
    file::m_name = filename;
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
file::file(const file& file_data)
{
    // 初期化
    file::m_name = file_data.m_name;
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
file::~file()
{
}

//-------------------------------------------------------------------------
// 情報取得
//-------------------------------------------------------------------------
bool file::get_stat(struct stat& file_stat)
{
    // ファイル情報取得
    if( stat(file::m_name.c_str(),&file_stat) != 0 ) {
        // 異常終了
        return false;
    }

    // 正常終了
    return true;
}

//-----------------------------------------------------------------------------
// サイズ取得
//-----------------------------------------------------------------------------
long file::size()
{
    struct stat _stat;    // ファイル情報

    // ファイル情報取得
    if(file::get_stat(_stat)) {
        // 正常終了
        return _stat.st_size;
    }
    // 異常終了
    return -1;
}

//-------------------------------------------------------------------------
// 存在判定
//-------------------------------------------------------------------------
bool file::exist()
{
    struct stat _stat;    // ファイル情報

    // ファイル存在判定
    if(!file::get_stat(_stat)) {
        // ファイルは存在していない
        return false;
    }

    // ファイルは存在している
    return true;
}

//-------------------------------------------------------------------------
// コピー
//-------------------------------------------------------------------------
bool file::copy(std::string dst)
{
    // 入力ファイルを開く
    std::fstream ifs;
    ifs.open( file::m_name.c_str(), std::ios::in|std::ios::binary);
    if( !ifs.is_open() ) {
        // 異常終了
        return false;
    }

    // 出力ファイルを開く
    std::fstream ofs;
    ofs.open( dst.c_str(), std::ios::out|std::ios::binary);
    if( !ofs.is_open() ) {
        // 異常終了
        return false;
    }

    // 読込バッファ
    char _read_buffer[4096];

    // 入力ファイル読込を繰り返す
    while( !ifs.eof() ) {
        // 入力ファイルを読み込む
        ifs.read(_read_buffer, sizeof(_read_buffer) );
        // 読込失敗したら終了
        if( ifs.fail() && !ifs.eof() ) {
            // ファイルクローズ
            ifs.close();
            ofs.close();
            // 異常終了
            return false;
        }
        // ファイルに書き込む
        ofs.write(_read_buffer, ifs.gcount());
    }

    // ファイルクローズ
    ifs.close();
    ofs.close();

    // 正常終了
    return true;
}

//-----------------------------------------------------------------------------
// 権限付与関数
//-----------------------------------------------------------------------------
bool file::change_mode(mode_t mode)
{
    // 権限付与
    if(chmod(file::m_name.c_str(),mode)  != 0 ) {
        // 異常終了
        return false;
    }

    // 正常終了
    return true;
}