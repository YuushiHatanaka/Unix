//-----------------------------------------------------------------------------
// fileモジュールヘッダ
//-----------------------------------------------------------------------------
#ifndef _FILE_HPP
#define _FILE_HPP

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include <sys/stat.h>

#include <string>
#include<iostream>
#include<fstream>

//-----------------------------------------------------------------------------
// クラス定義
//-----------------------------------------------------------------------------
// fileクラス
class file {
private :
    // ファイル名
    std::string m_name;
public :
    // コンストラクタ
    file(std::string);
    // コンストラクタ
    file(const file&);
    // デストラクタ
    ~file();

    // 情報取得
    bool get_stat(struct stat&);
    // サイズ取得
    long size();
    // 存在判定
    bool exist();

    // コピー
    bool copy(std::string);
    // 権限付与関数
    bool change_mode(mode_t mode);
};
#endif