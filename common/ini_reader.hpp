//-----------------------------------------------------------------------------
// ini_readerヘッダ
//-----------------------------------------------------------------------------
#ifndef _INI_READER_HPP
#define _INI_READER_HPP

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>

#include "regular_expression.hpp"

//-----------------------------------------------------------------------------
// クラス定義
//-----------------------------------------------------------------------------
// ini_readerクラス
class ini_reader  {
private :
    // ファイル名
    std::string m_filename;
    // ファイルストリーム
    std::ifstream m_stream;
    // データitems
    std::map<std::string, std::map<std::string,std::string> > m_items;
private :
    // 登録
    void regston(std::string,std::string,std::string);
    // 破棄
    void destroy();
    // 文字列改行削除
    std::string string_delete_crlf(const std::string);
public :
    // コンストラクタ
    ini_reader(std::string);
    // デストラクタ
    ~ini_reader();
    // ファイルオープン
    void open();
    // ファイルクローズ
    void close();
    // 解析
    void parse();
    // セッション存在判定
    bool find_session(std::string);
    // 要素取得
    std::map<std::string, std::map<std::string,std::string> > items();
    // 要素取得
    std::map<std::string, std::string> items(std::string);
    // 要素取得
    std::string items(std::string,std::string);
};

#endif
