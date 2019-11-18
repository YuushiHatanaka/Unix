//-----------------------------------------------------------------------------
// binaryモジュール
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include "binary.hpp"

//-----------------------------------------------------------------------------
// 定数定義
//-----------------------------------------------------------------------------
#define DUMP_STRING_WIDHT (16)      // ダンプ文字列長

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
binary::binary()
{
    // 初期化
    binary::m_size = 0;
    binary::m_data = NULL;
    binary::m_find_size = 0;
    binary::m_find_position = binary::m_data;
    binary::m_find_remain_size = 0;
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
binary::binary(const binary& binary_data)
{
    // 初期化
    binary::m_size = 0;
    binary::m_data = NULL;
    binary::m_find_size = 0;
    binary::m_find_position = binary::m_data;
    binary::m_find_remain_size = 0;

    // 設定
    binary::set( binary_data.m_data, binary_data.m_size );
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
binary::binary(u_char* data, size_t size)
{
    // 初期化
    binary::m_size = 0;
    binary::m_data = NULL;
    binary::m_find_size = 0;
    binary::m_find_position = binary::m_data;
    binary::m_find_remain_size = 0;

    // 設定
    binary::set( data, size );
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
binary::~binary()
{
    // 解放
    binary::destroy();
}

//-----------------------------------------------------------------------------
// 生成
//-----------------------------------------------------------------------------
void binary::create()
{
    // 生成
    binary::m_data = new u_char[binary::m_size];
    // 初期化
    memset( binary::m_data, 0x00, sizeof(binary::m_size) );
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void binary::destroy()
{
    // 解放
    if( binary::m_data != NULL ) {
        delete[] binary::m_data;
        binary::m_data = NULL;
        binary::m_size = 0;
        binary::m_find_size = 0;
        binary::m_find_position = binary::m_data;
        binary::m_find_remain_size = 0;
    }
}

//-----------------------------------------------------------------------------
// 設定
//-----------------------------------------------------------------------------
void binary::set(u_char* data, size_t size)
{
    // 解放
    binary::destroy();
    // サイズ設定
    binary::m_size = size;
    binary::m_find_remain_size = size;
    // 生成
    binary::create();
    // データ設定
    memcpy( binary::m_data, data, size );
    binary::m_find_position = binary::m_data;
}

//-----------------------------------------------------------------------------
// 設定
//-----------------------------------------------------------------------------
void binary::set(binary set_data)
{
    // 解放
    binary::destroy();
    // サイズ設定
    binary::m_size = set_data.size();
    binary::m_find_remain_size = set_data.size();
    // 生成
    binary::create();
    // データ設定
    memcpy( binary::m_data, set_data.data(), set_data.size() );
    binary::m_find_position = binary::m_data;
}

//-----------------------------------------------------------------------------
// 追加
//-----------------------------------------------------------------------------
void binary::add(uint16_t add_data)
{
    u_char _add_data[sizeof(uint16_t)];  // 追加データ

    // 追加データ設定
    memcpy(_add_data, &add_data, sizeof(_add_data));

    // 追加
    binary::add(_add_data, sizeof(_add_data));
}

//-----------------------------------------------------------------------------
// 追加
//-----------------------------------------------------------------------------
void binary::add(binary add_data)
{
    // 追加
    binary::add(add_data.m_data, add_data.m_size);
}

//-----------------------------------------------------------------------------
// 追加
//-----------------------------------------------------------------------------
void binary::add(u_char data)
{
    u_char _add_data[sizeof(u_char)];  // 追加データ

    // 追加データ設定
    _add_data[0] = data;

    // 追加
    binary::add(_add_data, sizeof(_add_data));
}

//-----------------------------------------------------------------------------
// 追加
//-----------------------------------------------------------------------------
void binary::add(u_char* data, size_t size)
{
    u_char _save_data[binary::m_size];    // 保存データ
    size_t _save_size = binary::m_size;   // 保存データサイズ

    // データ保存
    memcpy( _save_data, binary::m_data, _save_size );
    // 解放
    binary::destroy();
    // サイズ設定
    binary::m_size = _save_size + size;
    binary::m_find_remain_size += binary::m_size;
    // 生成
    binary::create();
    // データ設定
    memcpy( &(binary::m_data[0]), _save_data, _save_size );
    // データ設定
    memcpy( &(binary::m_data[_save_size]), data, size );
    binary::m_find_position = binary::m_data;
}

//-----------------------------------------------------------------------------
// クリア
//-----------------------------------------------------------------------------
void binary::clear()
{
    // 解放
    binary::destroy();
}

//-----------------------------------------------------------------------------
// 代入演算子
//-----------------------------------------------------------------------------
binary& binary::operator = (binary binary_data)
{
    // 設定
    binary::set( binary_data.data(), binary_data.size() );
    // 自身の実体を返却
    return *this;
}

//-----------------------------------------------------------------------------
// 代入演算子
//-----------------------------------------------------------------------------
binary& binary::operator = (std::string hexstring)
{
    // 自身の実体を返却
    return binary::from_hexstr( (u_char*)hexstring.data(), hexstring.size() );
}

//-----------------------------------------------------------------------------
// 配列添え字参照演算子
//-----------------------------------------------------------------------------
u_char binary::operator [] (uint32_t index)
{
    // 配列要素を返却
    return binary::m_data[index];
}

//-----------------------------------------------------------------------------
// 等価演算子
//-----------------------------------------------------------------------------
bool binary::operator == (binary& binary_data)
{
    // サイズ判定
    if( binary::m_size != binary_data.size() ) {
        // 不一致を返却
        return false;
    }
    // 内容判定
    if( memcpy( binary::m_data, binary_data.data(), binary::m_size) != 0 ) {
        // 不一致を返却
        return false;
    }
    // 一致を返却
    return true;
}

//-----------------------------------------------------------------------------
// 不等価演算子
//-----------------------------------------------------------------------------
bool binary::operator != (binary& binary_data)
{
    // サイズ判定
    if( binary::m_size == binary_data.size() ) {
        // 内容判定
        if( memcpy( binary::m_data, binary_data.data(), binary::m_size) == 0 ) {
            // 一致を返却
            return false;
        }
        else {
            // 不一致を返却
            return true;
        }
    }
    // 不一致を返却
    return true;
}

//-----------------------------------------------------------------------------
// 16進数文字列変換
//-----------------------------------------------------------------------------
std::string binary::to_hexstr()
{
    std::stringstream _to_hexstr;   // 16進数文字列

    // サイズ分(1byeずつ)繰り返す
    for(size_t i=0; i<binary::m_size; i++ ) {
        // 1バイト分を16進文字列に変換
        int _tmp_binary = binary::m_data[i];
        _to_hexstr << std::hex << std::setw(2) << std::setfill('0') << _tmp_binary;
    }

    // 16進数文字列返却
    return _to_hexstr.str();
}

//-----------------------------------------------------------------------------
// 検索(初回)
//-----------------------------------------------------------------------------
u_char* binary::find_first(u_char* data,size_t size)
{
    // 検索位置移動(先頭に設定)
    if( !binary::find_seek(SEEK_SET, 0) ) {
        // 異常終了
        return NULL;
    }

    // データを1バイトずつ検索
    for( size_t i=0; i<binary::m_size; i++ ) {
        // 残りサイズが指定サイズ以下なら検索終了(該当なし)
        if( binary::m_find_remain_size < size ) {
            return NULL;
        }
        // 比較
        if( memcmp(&(binary::m_data[i]), data, size) == 0 ){
            // 検索位置を返却
            return binary::m_find_position;
        }
        // 更新
        binary::m_find_size++;
        binary::m_find_position++;
        binary::m_find_remain_size--;
    }

    // 該当なし
    return NULL;
}

//-----------------------------------------------------------------------------
// 検索(初回以降)
//-----------------------------------------------------------------------------
u_char* binary::find_next(u_char* data,size_t size)
{
    // 初回検索していない場合
    if( binary::m_find_position == NULL ) {
        // 検索しない
        return NULL;
    }

    // データを1バイトずつ検索
    for( size_t i=binary::m_find_size; i<binary::m_size; i++ ) {
        // 残りサイズが指定サイズ以下なら検索終了(該当なし)
        if( binary::m_find_remain_size < size ) {
            return NULL;
        }
        // 比較
        if( memcmp(&(binary::m_data[i]), data, size) == 0 ){
            // 検索位置を返却
            return binary::m_find_position;
        }
        // 更新
        binary::m_find_size++;
        binary::m_find_position++;
        binary::m_find_remain_size--;
    }

    // 該当なし
    return NULL;
}

//-----------------------------------------------------------------------------
// 検索位置移動
//-----------------------------------------------------------------------------
u_char* binary::find_seek(size_t offset,int whence)
{
    // SEEK_SET, SEEK_CUR, or SEEK_END
    switch(whence) {
        case SEEK_SET :
            // offsetの値を判定
            if( offset > binary::m_size ) {
                // サイズを超えた場合はエラー
                return NULL;
            }
            // 先頭からオフセット分移動する
            binary::m_find_size = offset;
            binary::m_find_position = binary::m_data + offset;
            binary::m_find_remain_size = binary::m_size - offset;
            break;
        case SEEK_CUR :
            // offsetの値を判定
            if( ((size_t)binary::m_find_position - (size_t)binary::m_data) + offset > binary::m_size ) {
                // サイズを超えた場合はエラー
                return NULL;
            }
            // 現在位置からオフセット分移動する
            binary::m_find_size += offset;
            binary::m_find_position = binary::m_find_position + offset;
            binary::m_find_remain_size = binary::m_find_remain_size - offset;
            break;
        case SEEK_END :
            // offsetの値を判定
            if( offset > binary::m_size ) {
                // サイズを超えた場合はエラー
                return NULL;
            }
            // 末尾からオフセット分移動する
            binary::m_find_size = binary::m_size - offset;
            binary::m_find_position = (binary::m_data + binary::m_size) - offset;
            binary::m_find_remain_size = binary::m_size + offset;
            break;
        default :
            // パラメータエラー
            return NULL;
    }
    // 正常終了
    return binary::m_find_position;
}

//-----------------------------------------------------------------------------
// 2進数文字列バイナリ変換
//-----------------------------------------------------------------------------
binary& binary::from_binstr(std::string binstr)
{
    // 自身の実体を返却
    return binary::from_binstr((u_char*)binstr.c_str(),binstr.size());
}

//-----------------------------------------------------------------------------
// 2進数文字列バイナリ変換
//-----------------------------------------------------------------------------
binary& binary::from_binstr(u_char* data, size_t size)
{
    u_char _save_data[size/8];  // 保存データ
    std::stringstream _tmpdata; // 一時データ
    size_t _save_data_size = 0; // 保存データサイズ

    // 保存データ初期化
    memset( _save_data, 0x00, sizeof(_save_data) );

    // 設定データがNULL以外の場合
    if( data != NULL ) {

        // 2進数表記文字を取り出す
        for(size_t i=0;  i<strlen((char*)data) && i<size  ; i++ ) {

            // 2進文字列でないものがあればその時点で変換終了
            if(!( data[i] == '0' || data[i] == '1' )) {
                break;
            }

            // 文字列生成
            _tmpdata << data[i];

            // 8bit(1byte)分を値変換
            if( _tmpdata.str().size() == 8 ) {
                // データ設定(2進数変換)
                _save_data[_save_data_size++] = (u_char)strtol(_tmpdata.str().c_str(), NULL, 2);

                // 初期化
                _tmpdata.str("");
            }
        }

        // 残り部分を設定
        if( _tmpdata.str().size() != 0 ) {
            // 0埋め込み数を算出
            std::stringstream _remaining;
            _remaining << std::setfill('0') << std::left << std::setw(8) << _tmpdata.str();

            // データ設定(2進数変換)
            _save_data[_save_data_size++] = (u_char)strtol(_remaining.str().c_str(), NULL, 2);

            // 初期化
            _tmpdata.str("");
            _remaining.str("");
        }

        // 設定
        binary::set( _save_data, _save_data_size );
    }

    // 自身の実体を返却
    return *this;
}

//-----------------------------------------------------------------------------
// 16進数文字列バイナリ変換
//-----------------------------------------------------------------------------
binary& binary::from_hexstr(std::string hexstr)
{
    // 自身の実体を返却
    return binary::from_hexstr((u_char*)hexstr.c_str(),hexstr.size());
}

//-----------------------------------------------------------------------------
// 16進数文字列バイナリ変換
//-----------------------------------------------------------------------------
binary& binary::from_hexstr(u_char* data, size_t size)
{
    u_char _save_data[size/2];  // 保存データ
    std::stringstream _tmpdata; // 一時データ
    size_t _save_data_size = 0; // 保存データサイズ


    // 保存データ初期化
    memset( _save_data, 0x00, sizeof(_save_data) );

    // 設定データがNULL以外の場合
    if( data != NULL ) {

        // 16進数表記文字を取り出す
        for(size_t i=0;  i<strlen((char*)data) && i<size  ; i++ ) {

            // 16進文字列でないものがあればその時点で変換終了
            if(!isxdigit(data[i])) {
                break;
            }

            // 文字列生成
            _tmpdata << data[i];

            // 1byte分を値変換
            if( _tmpdata.str().size() == 2 ) {
                // データ設定(2進数変換)
                std::stringstream _hexstring;
                _hexstring << "0x" << _tmpdata.str();
                _save_data[_save_data_size++] = (u_char)std::atof(_hexstring.str().c_str());

                // 初期化
                _tmpdata.str("");
                _hexstring.str("");
            }
        }

        // 残り部分を設定
        if( _tmpdata.str().size() != 0 ) {
            // 0埋め込み数を算出
            std::stringstream _remaining;
            _remaining << std::setfill('0') << std::left << std::setw(2) << _tmpdata.str();

            // データ設定(2進数変換)
            std::stringstream _hexstring;
            _hexstring << "0x" << _remaining.str();
            _save_data[_save_data_size++] = (u_char)std::atof(_hexstring.str().c_str());

            // 初期化
            _tmpdata.str("");
            _hexstring.str("");
            _remaining.str("");
        }

        // 設定
        binary::set( _save_data, _save_data_size );
    }

    // 自身の実体を返却
    return *this;
}

//-----------------------------------------------------------------------------
// 部分取得
//-----------------------------------------------------------------------------
binary binary::sub(int start,int end)
{
    binary _result;

    // 文字数分繰り返し
    int _count=0;
    for(size_t i=start; i<binary::m_size && _count<end; i++, _count++ ) {
        _result.add((binary::m_data[i]));
    }

    // 結果を返却
    return _result;
}

//-----------------------------------------------------------------------------
// 文字列取得
//-----------------------------------------------------------------------------
std::string binary::to_string()
{
    char _buffer[binary::m_size+1];   // 文字列バッファ

    // 初期化
    memset(_buffer, 0x00, sizeof(_buffer));

    // 設定
    memcpy( _buffer, binary::m_data, binary::m_size );

    // 文字列置換
    std::string _result = std::string(_buffer);

    // 文字列返却
    return _result;
}

//-----------------------------------------------------------------------------
// 2進数文字列取得
//-----------------------------------------------------------------------------
std::string binary::to_bin_string()
{
    std::stringstream _result;
    unsigned int _data;

    // サイズ分繰り返す
    for(size_t i=0; i<binary::m_size; i++ ){
        _data = (unsigned int)(binary::m_data[i]);
        _result << static_cast<std::bitset<8> >(_data);
    }

    // 文字列返却
    return _result.str();
}

//-----------------------------------------------------------------------------
// 16進数文字列取得
//-----------------------------------------------------------------------------
std::string binary::to_hex_string()
{
    std::stringstream _result;
    unsigned int _data;

    // サイズ分繰り返す
    for(size_t i=0; i<binary::m_size; i++ ){
        _data = (unsigned int)(binary::m_data[i]);
        _result << std::hex << std::setw(2) << std::right << std::setfill('0') << _data;
    }

    // 文字列返却
    return _result.str();
}

//-----------------------------------------------------------------------------
// ダンプ文字列取得
//-----------------------------------------------------------------------------
std::string binary::to_dump()
{
    std::stringstream _dumpmsg; // ダンプメッセージ

    // ログ内容
    const unsigned char* p = (const unsigned char*)(binary::m_data);
    size_t bytes = binary::m_size;
    char text[DUMP_STRING_WIDHT+1];
    unsigned i = 0;
    unsigned _addrress;
    unsigned int _data;

    while (i < bytes) {
        // アドレス出力
        if ((i % DUMP_STRING_WIDHT) == 0) {
            _addrress = (unsigned int)p;
            _dumpmsg << "0x" << std::hex << std::setw(8) << std::right << std::setfill('0') <<  _addrress << ": ";
            memset(text, '\0', sizeof(text));
        }
        _data = (unsigned int)*p;
        _dumpmsg << std::hex << std::setw(2) << std::right << std::setfill('0') << _data << " ";
        // テキスト部分出力
        text[i % DUMP_STRING_WIDHT] = isprint(*p) ? *p : '.';
        p++;
        i++;
        // テキスト部分出力
        if ((i % DUMP_STRING_WIDHT) == 0) {
            _dumpmsg << ": " << text << std::endl;
        }
    }
    if ((i % DUMP_STRING_WIDHT) != 0){
        _dumpmsg << std::setw((DUMP_STRING_WIDHT - (i % DUMP_STRING_WIDHT)) * 3 + 2) << std::setfill(' ');
        _dumpmsg << ": " << text /*<< std::endl*/;
    }

    // 文字列返却
    return _dumpmsg.str();
}

//-----------------------------------------------------------------------------
// reverse
//-----------------------------------------------------------------------------
binary binary::reverse()
{
    u_char _save_data[binary::m_size];  // 保存データ

    // 保存データ初期化
    memset( _save_data, 0x00, sizeof(_save_data) );

    // サイズ分繰り返す
    for(size_t i=0; i<binary::m_size; i++ ) {
        _save_data[i] = binary::m_data[binary::m_size-i-1];
    }

    // 返却データを設定
    binary _result;
    _result.set(_save_data,sizeof(_save_data));

    // 結果を返却
    return _result;
}

//-----------------------------------------------------------------------------
// 数値変換
//-----------------------------------------------------------------------------
int binary::to_int()
{
    int _result = 0;

    // 格納サイズ分繰り返し
    for(size_t i=0; i<binary::m_size; i++ ) {
        _result = _result << 8;
        _result = _result | binary::m_data[i];
    }

    return _result;
}

//-----------------------------------------------------------------------------
// 数値変換(uint8_t)
//-----------------------------------------------------------------------------
uint8_t binary::to_uint8()
{
    uint8_t _result = 0;

    // 格納サイズ分繰り返し
    for(size_t i=0; i<binary::m_size; i++ ) {
        _result = _result << 8;
        _result = _result | binary::m_data[i];
    }

    return _result;
}

//-----------------------------------------------------------------------------
// 数値変換(uint16_t)
//-----------------------------------------------------------------------------
uint16_t binary::to_uint16()
{
    uint16_t _result = 0;

    // 格納サイズ分繰り返し
    for(size_t i=0; i<binary::m_size; i++ ) {
        _result = _result << 8;
        _result = _result | binary::m_data[i];
    }

    return _result;
}

//-----------------------------------------------------------------------------
// 数値変換(uint32_t)
//-----------------------------------------------------------------------------
uint32_t binary::to_uint32()
{
    uint32_t _result = 0;

    // 格納サイズ分繰り返し
    for(size_t i=0; i<binary::m_size; i++ ) {
        _result = _result << 8;
        _result = _result | binary::m_data[i];
    }

    return _result;
}

//-----------------------------------------------------------------------------
// 数値変換(uint64_t)
//-----------------------------------------------------------------------------
uint64_t binary::to_uint64()
{
    uint64_t _result = 0;

    // 格納サイズ分繰り返し
    for(size_t i=0; i<binary::m_size; i++ ) {
        _result = _result << 8;
        _result = _result | binary::m_data[i];
    }

    return _result;
}

//-----------------------------------------------------------------------------
// 数値変換(int8_t)
//-----------------------------------------------------------------------------
int8_t binary::to_int8()
{
    int8_t _result = 0;

    // 格納サイズ分繰り返し
    for(size_t i=0; i<binary::m_size; i++ ) {
        _result = _result << 8;
        _result = _result | binary::m_data[i];
    }

    return _result;
}

//-----------------------------------------------------------------------------
// 数値変換(int16_t)
//-----------------------------------------------------------------------------
int16_t binary::to_int16()
{
    int16_t _result = 0;

    // 格納サイズ分繰り返し
    for(size_t i=0; i<binary::m_size; i++ ) {
        _result = _result << 8;
        _result = _result | binary::m_data[i];
    }

    return _result;
}

//-----------------------------------------------------------------------------
// 数値変換(int32_t)
//-----------------------------------------------------------------------------
int32_t binary::to_int32()
{
    int32_t _result = 0;

    // 格納サイズ分繰り返し
    for(size_t i=0; i<binary::m_size; i++ ) {
        _result = _result << 8;
        _result = _result | binary::m_data[i];
    }

    return _result;
}

//-----------------------------------------------------------------------------
// 数値変換(int64_t)
//-----------------------------------------------------------------------------
int64_t binary::to_int64()
{
    int64_t _result = 0;

    // 格納サイズ分繰り返し
    for(size_t i=0; i<binary::m_size; i++ ) {
        _result = _result << 8;
        _result = _result | binary::m_data[i];
    }

    return _result;
}

//-----------------------------------------------------------------------------
// 比較
//-----------------------------------------------------------------------------
bool binary::comp(binary target)
{
    // メモリ比較
    if(memcmp(binary::m_data,target.data(),binary::m_size) != 0) {
        // 不一致
        return false;
    }

    // 一致
    return true;
}
