//-----------------------------------------------------------------------------
// dirモジュールヘッダ
//-----------------------------------------------------------------------------
#ifndef _DIR_HPP
#define _DIR_HPP

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include <string>
#include <vector>

#include "regular_expression.hpp"

//-----------------------------------------------------------------------------
// クラス定義
//-----------------------------------------------------------------------------
// dirクラス
class dir {
private :
    // パス名
    std::string m_path;
public :
    // コンストラクタ
    dir(std::string);
    // コンストラクタ
    dir(const dir&);
    // デストラクタ
    ~dir();

    // 情報取得
    bool get_stat(struct stat&);
    // 情報取得
    bool get_stat(std::string,struct stat&);
    // 存在判定
    bool exist();
    // ディレクトリ判定
    bool is_dir();
    // ディレクトリ判定
    bool is_dir(std::string);
    // ディレクトリ作成
    bool mk_dir();

    // ファイル一覧取得
    std::vector<std::string> get_file_list(bool);
    // ファイル一覧取得
    std::vector<std::string> get_file_list(std::string,bool);
    // ファイル一覧取得
    std::vector<std::string> get_file_list(std::string,std::string,bool);
    // ディスク空き容量取得
    uint64_t get_diskfree();
};
#endif
