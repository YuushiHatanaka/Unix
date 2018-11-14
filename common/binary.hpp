//-----------------------------------------------------------------------------
// binaryモジュールヘッダ
//-----------------------------------------------------------------------------
#ifndef _BINARY_HPP
#define _BINARY_HPP

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>

#include <cstddef>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <bitset>

//-----------------------------------------------------------------------------
// クラス定義
//-----------------------------------------------------------------------------
// binaryクラス
class binary {
private :
    // データ量
    size_t m_size;
    // データ
    u_char* m_data;
    // 検索済サイズ
    size_t m_find_size;
    // 検索位置
    u_char* m_find_position;
    // 検索残りサイズ
    size_t m_find_remain_size;
public:
    // アクセスメソッド(データ量)
    size_t size() { return binary::m_size; }
    // アクセスメソッド(データ)
    u_char* data() { return binary::m_data; }
    // アクセスメソッド(検索残りサイズ)
    u_char* find_position() { return binary::m_find_position; }
    // アクセスメソッド(検索残りサイズ)
    size_t remain_size() { return binary::m_find_remain_size; }
private :
    // 生成
    void create();
    // 解放
    void destroy();
public:
    // コンストラクタ
    binary();
    // コンストラクタ
    binary(const binary&);
    // コンストラクタ
    binary(u_char*,size_t);
    // デストラクタ
    ~binary();
    // 設定
    void set(u_char*,size_t);
    // 設定
    void set(binary);
    // 追加
    void add(uint16_t);
    // 追加
    void add(binary);
    // 追加
    void add(u_char);
    // 追加
    void add(u_char*,size_t);
    // クリア
    void clear();
    // 代入演算子
    binary& operator = (binary);
    // 代入演算子
    binary& operator = (std::string);
    // 配列添え字参照演算子
    u_char operator [] (uint32_t);
    // 等価演算子
    bool operator == (binary&);
    // 不等価演算子
    bool operator != (binary&);
    // 16進数文字列変換
    std::string to_hexstr();
    // 検索(初回)
    u_char* find_first(u_char*,size_t);
    // 検索(初回以降)
    u_char* find_next(u_char*,size_t);
    // 検索位置移動
    u_char* find_seek(size_t,int);
    // 2進数文字列バイナリ変換
    binary& from_binstr(std::string);
    // 2進数文字列バイナリ変換
    binary& from_binstr(u_char*, size_t);
    // 16進数文字列バイナリ変換
    binary& from_hexstr(std::string);
    // 16進数文字列バイナリ変換
    binary& from_hexstr(u_char*, size_t);
    // 部分取得
    binary sub(int,int);
    // 文字列取得
    std::string to_string();
    // 2進数文字列取得
    std::string to_bin_string();
    // 16進数文字列取得
    std::string to_hex_string();
    // ダンプ文字列取得
    std::string to_dump();

    // reverse
    binary reverse();

    // 数値変換(int)
    int to_int();
    // 数値変換(uint8_t)
    uint8_t to_uint8();
    // 数値変換(uint16_t)
    uint16_t to_uint16();
    // 数値変換(uint32_t)
    uint32_t to_uint32();
    // 数値変換(uint64_t)
    uint64_t to_uint64();
    // 数値変換(int8_t)
    int8_t to_int8();
    // 数値変換(int16_t)
    int16_t to_int16();
    // 数値変換(int32_t)
    int32_t to_int32();
    // 数値変換(int64_t)
    int64_t to_int64();

    // 比較
    bool comp(binary);
};
#endif
