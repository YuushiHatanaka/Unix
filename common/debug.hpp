//-----------------------------------------------------------------------------
// debugヘッダ
//-----------------------------------------------------------------------------
#ifndef _DEBUG_HPP
#define _DEBUG_HPP

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

//-----------------------------------------------------------------------------
// 定数定義
//-----------------------------------------------------------------------------
#define DUMP_STRING_WIDHT (16)      // ダンプ文字列長
#define DUMP_MESSAGE_MAX  (8192)    // メッセージ文字列長

//-----------------------------------------------------------------------------
// クラス定義
//-----------------------------------------------------------------------------
// debugクラス
class debug {
private :
    // 現在日時文字列取得
    std::string get_datetime_string();
public :
    // デバッグ出力
    void Print(const char* ,...);
    // デバッグ出力
    void PrintLine(const char* ,...);
    // デバッグ出力
    void Write(const char*);
    // デバッグ出力(改行付与)
    void WriteLine(const char* ,...);
    // エラー出力
    void Fail(const char* ,...);
    // メモリダンプ
    void* Dump(const char*,const void*,size_t);
public :
    // コンストラクタ
    debug();
    // デストラクタ
    ~debug();
};
#endif
