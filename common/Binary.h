//==============================================================================
// Binaryモジュールヘッダ
//==============================================================================
#ifndef _BINARY_H_                          // 二重インクルード防止
#define _BINARY_H_                          // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
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

//==============================================================================
// 定数定義
//==============================================================================
#define DUMP_STRING_WIDHT (16)              // ダンプ文字列長

//==============================================================================
// クラス定義
//==============================================================================
// Binaryクラス
class Binary {
private :
    size_t m_size;                          // データ量
    u_char* m_data;                         // データ
    size_t m_find_size;                     // 検索済サイズ
    u_char* m_find_position;                // 検索位置
    size_t m_find_remain_size;              // 検索残りサイズ

public:
    // アクセスメソッド(データ量)
    size_t Size() { return this->m_size; }
    // アクセスメソッド(データ)
    u_char* Data() { return this->m_data; }
    // アクセスメソッド(検索残りサイズ)
    u_char* FindPosition() { return this->m_find_position; }
    // アクセスメソッド(検索残りサイズ)
    size_t RemainSize() { return this->m_find_remain_size; }

private :
    //-----------------------------------------------------------------------------
    // 生成
    //-----------------------------------------------------------------------------
    void Create()
    {
        // 生成
        this->m_data = new u_char[this->m_size];
        // 初期化
        memset( this->m_data, 0x00, sizeof(this->m_size) );
    }

    //-----------------------------------------------------------------------------
    // 解放
    //-----------------------------------------------------------------------------
    void Destroy()
    {
        // 解放
        if( this->m_data != NULL ) {
            delete[] this->m_data;
            this->m_data = NULL;
            this->m_size = 0;
            this->m_find_size = 0;
            this->m_find_position = this->m_data;
            this->m_find_remain_size = 0;
        }
    }

public:
    //-----------------------------------------------------------------------------
    // コンストラクタ
    //-----------------------------------------------------------------------------
    Binary()
    {
        // 初期化
        this->m_size = 0;
        this->m_data = NULL;
        this->m_find_size = 0;
        this->m_find_position = this->m_data;
        this->m_find_remain_size = 0;
    }

    //-----------------------------------------------------------------------------
    // コンストラクタ
    //-----------------------------------------------------------------------------
    Binary(const Binary& Binary_data)
    {
        // 初期化
        this->m_size = 0;
        this->m_data = NULL;
        this->m_find_size = 0;
        this->m_find_position = this->m_data;
        this->m_find_remain_size = 0;

        // 設定
        this->Set( Binary_data.m_data, Binary_data.m_size );
    }

    //-----------------------------------------------------------------------------
    // コンストラクタ
    //-----------------------------------------------------------------------------
    Binary(u_char* data, size_t size)
    {
        // 初期化
        this->m_size = 0;
        this->m_data = NULL;
        this->m_find_size = 0;
        this->m_find_position = this->m_data;
        this->m_find_remain_size = 0;

        // 設定
        this->Set( data, size );
    }

    //-----------------------------------------------------------------------------
    // デストラクタ
    //-----------------------------------------------------------------------------
    ~Binary()
    {
        // 解放
        this->Destroy();
    }


    //-----------------------------------------------------------------------------
    // 設定
    //-----------------------------------------------------------------------------
    void Set(u_char* data, size_t size)
    {
        // 解放
        this->Destroy();

        // サイズ設定
        this->m_size = size;
        this->m_find_remain_size = size;

        // 生成
        this->Create();

        // データ設定
        memcpy( this->m_data, data, size );
        this->m_find_position = this->m_data;
    }

    //-----------------------------------------------------------------------------
    // 設定
    //-----------------------------------------------------------------------------
    void Set(Binary set_data)
    {
        // 解放
        this->Destroy();

        // サイズ設定
        this->m_size = set_data.Size();
        this->m_find_remain_size = set_data.Size();

        // 生成
        this->Create();

        // データ設定
        memcpy( this->m_data, set_data.Data(), set_data.Size() );
        this->m_find_position = this->m_data;
    }

    //-----------------------------------------------------------------------------
    // 追加
    //-----------------------------------------------------------------------------
    void Add(uint16_t add_data)
    {
        u_char _add_data[sizeof(uint16_t)]; // 追加データ

        // 追加データ設定
        memcpy(_add_data, &add_data, sizeof(_add_data));

        // 追加
        this->Add(_add_data, sizeof(_add_data));
    }

    //-----------------------------------------------------------------------------
    // 追加
    //-----------------------------------------------------------------------------
    void Add(Binary add_data)
    {
        // 追加
        this->Add(add_data.m_data, add_data.m_size);
    }

    //-----------------------------------------------------------------------------
    // 追加
    //-----------------------------------------------------------------------------
    void Add(u_char data)
    {
        u_char _add_data[sizeof(u_char)];  // 追加データ

        // 追加データ設定
        _add_data[0] = data;

        // 追加
        this->Add(_add_data, sizeof(_add_data));
    }

    //-----------------------------------------------------------------------------
    // 追加
    //-----------------------------------------------------------------------------
    void Add(u_char* data, size_t size)
    {
        u_char _save_data[this->m_size];    // 保存データ
        size_t _save_size = this->m_size;   // 保存データサイズ

        // データ保存
        memcpy( _save_data, this->m_data, _save_size );
        // 解放
        this->Destroy();
        // サイズ設定
        this->m_size = _save_size + size;
        this->m_find_remain_size += this->m_size;
        // 生成
        this->Create();
        // データ設定
        memcpy( &(this->m_data[0]), _save_data, _save_size );
        // データ設定
        memcpy( &(this->m_data[_save_size]), data, size );
        this->m_find_position = this->m_data;
    }

    //-----------------------------------------------------------------------------
    // クリア
    //-----------------------------------------------------------------------------
    void Clear()
    {
        // 解放
        this->Destroy();
    }

    //-----------------------------------------------------------------------------
    // 代入演算子
    //-----------------------------------------------------------------------------
    Binary& operator = (Binary data)
    {
        // 設定
        this->Set( data.Data(), data.Size() );

        // 自身の実体を返却
        return *this;
    }

    //-----------------------------------------------------------------------------
    // 代入演算子
    //-----------------------------------------------------------------------------
    Binary& operator = (std::string hexstring)
    {
        // 自身の実体を返却
        return this->FromHexstr( (u_char*)hexstring.data(), hexstring.size() );
    }

    //-----------------------------------------------------------------------------
    // 配列添え字参照演算子
    //-----------------------------------------------------------------------------
    u_char operator [] (uint32_t index)
    {
        // 配列要素を返却
        return this->m_data[index];
    }

    //-----------------------------------------------------------------------------
    // 等価演算子
    //-----------------------------------------------------------------------------
    bool operator == (Binary& data)
    {
        // サイズ判定
        if( this->m_size != data.Size() )
        {
            // 不一致を返却
            return false;
        }

        // 内容判定
        if( memcpy( this->m_data, data.Data(), this->m_size) != 0 )
        {
            // 不一致を返却
            return false;
        }

        // 一致を返却
        return true;
    }

    //-----------------------------------------------------------------------------
    // 不等価演算子
    //-----------------------------------------------------------------------------
    bool operator != (Binary& data)
    {
        // サイズ判定
        if( this->m_size == data.Size() )
        {
            // 内容判定
            if( memcpy( this->m_data, data.Data(), this->m_size) == 0 )
            {
                // 一致を返却
                return false;
            }
            else
            {
                // 不一致を返却
                return true;
            }
        }

        // 不一致を返却
        return true;
    }

    //-----------------------------------------------------------------------------
    // 検索(初回)
    //-----------------------------------------------------------------------------
    u_char* FindFirst(u_char* data,size_t size)
    {
        // 検索位置移動(先頭に設定)
        if(!this->FindSeek(SEEK_SET, 0))
        {
            // 異常終了
            return NULL;
        }

        // データを1バイトずつ検索
        for( size_t i=0; i<this->m_size; i++ )
        {
            // 残りサイズが指定サイズ以下なら検索終了(該当なし)
            if( this->m_find_remain_size < size )
            {
                return NULL;
            }

            // 比較
            if( memcmp(&(this->m_data[i]), data, size) == 0 )
            {
                // 検索位置を返却
                return this->m_find_position;
            }

            // 更新
            this->m_find_size++;
            this->m_find_position++;
            this->m_find_remain_size--;
        }

        // 該当なし
        return NULL;
    }

    //-----------------------------------------------------------------------------
    // 検索(初回以降)
    //-----------------------------------------------------------------------------
    u_char* FindNext(u_char* data,size_t size)
    {
        // 初回検索していない場合
        if( this->m_find_position == NULL )
        {
            // 検索しない
            return NULL;
        }

        // データを1バイトずつ検索
        for( size_t i=this->m_find_size; i<this->m_size; i++ )
        {
            // 残りサイズが指定サイズ以下なら検索終了(該当なし)
            if( this->m_find_remain_size < size )
            {
                return NULL;
            }

            // 比較
            if( memcmp(&(this->m_data[i]), data, size) == 0 )
            {
                // 検索位置を返却
                return this->m_find_position;
            }

            // 更新
            this->m_find_size++;
            this->m_find_position++;
            this->m_find_remain_size--;
        }

        // 該当なし
        return NULL;
    }

    //-----------------------------------------------------------------------------
    // 検索位置移動
    //-----------------------------------------------------------------------------
    u_char* FindSeek(size_t offset,int whence)
    {
        // SEEK_SET, SEEK_CUR, or SEEK_END
        switch(whence)
        {
            case SEEK_SET :
                // offsetの値を判定
                if( offset > this->m_size )
                {
                    // サイズを超えた場合はエラー
                    return NULL;
                }

                // 先頭からオフセット分移動する
                this->m_find_size = offset;
                this->m_find_position = this->m_data + offset;
                this->m_find_remain_size = this->m_size - offset;
                break;
            case SEEK_CUR :
                // offsetの値を判定
                if( ((size_t)this->m_find_position - (size_t)this->m_data) + offset > this->m_size )
                {
                    // サイズを超えた場合はエラー
                    return NULL;
                }

                // 現在位置からオフセット分移動する
                this->m_find_size += offset;
                this->m_find_position = this->m_find_position + offset;
                this->m_find_remain_size = this->m_find_remain_size - offset;
                break;
            case SEEK_END :
                // offsetの値を判定
                if( offset > this->m_size )
                {
                    // サイズを超えた場合はエラー
                    return NULL;
                }

                // 末尾からオフセット分移動する
                this->m_find_size = this->m_size - offset;
                this->m_find_position = (this->m_data + this->m_size) - offset;
                this->m_find_remain_size = this->m_size + offset;
                break;
            default :
                // パラメータエラー
                return NULL;
        }

        // 正常終了
        return this->m_find_position;
    }

    //-----------------------------------------------------------------------------
    // 2進数文字列バイナリ変換
    //-----------------------------------------------------------------------------
    Binary& FromBinstr(std::string binstr)
    {
        // 自身の実体を返却
        return this->FromBinstr((u_char*)binstr.c_str(),binstr.size());
    }

    //-----------------------------------------------------------------------------
    // 2進数文字列バイナリ変換
    //-----------------------------------------------------------------------------
    Binary& FromBinstr(u_char* data, size_t size)
    {
        u_char _save_data[size/8];  // 保存データ
        std::stringstream _tmpdata; // 一時データ
        size_t _save_data_size = 0; // 保存データサイズ

        // 保存データ初期化
        memset( _save_data, 0x00, sizeof(_save_data) );

        // 設定データがNULL以外の場合
        if( data != NULL )
        {
            // 2進数表記文字を取り出す
            for(size_t i=0;  i<strlen((char*)data) && i<size  ; i++ )
            {
                // 2進文字列でないものがあればその時点で変換終了
                if(!( data[i] == '0' || data[i] == '1' ))
                {
                    break;
                }

                // 文字列生成
                _tmpdata << data[i];

                // 8bit(1byte)分を値変換
                if( _tmpdata.str().size() == 8 )
                {
                    // データ設定(2進数変換)
                    _save_data[_save_data_size++] = (u_char)strtol(_tmpdata.str().c_str(), NULL, 2);

                    // 初期化
                    _tmpdata.str("");
                }
            }

            // 残り部分を設定
            if( _tmpdata.str().size() != 0 )
            {
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
            this->Set( _save_data, _save_data_size );
        }

        // 自身の実体を返却
        return *this;
    }

    //-----------------------------------------------------------------------------
    // 16進数文字列バイナリ変換
    //-----------------------------------------------------------------------------
    Binary& FromHexstr(std::string hexstr)
    {
        // 自身の実体を返却
        return this->FromHexstr((u_char*)hexstr.c_str(),hexstr.size());
    }

    //-----------------------------------------------------------------------------
    // 16進数文字列バイナリ変換
    //-----------------------------------------------------------------------------
    Binary& FromHexstr(u_char* data, size_t size)
    {
        u_char _save_data[size/2];  // 保存データ
        std::stringstream _tmpdata; // 一時データ
        size_t _save_data_size = 0; // 保存データサイズ

        // 保存データ初期化
        memset( _save_data, 0x00, sizeof(_save_data) );

        // 設定データがNULL以外の場合
        if( data != NULL )
        {
            // 16進数表記文字を取り出す
            for(size_t i=0;  i<strlen((char*)data) && i<size  ; i++ )
            {
                // 16進文字列でないものがあればその時点で変換終了
                if(!isxdigit(data[i]))
                {
                    break;
                }

                // 文字列生成
                _tmpdata << data[i];

                // 1byte分を値変換
                if( _tmpdata.str().size() == 2 )
                {
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
            if( _tmpdata.str().size() != 0 )
            {
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
            this->Set( _save_data, _save_data_size );
        }

        // 自身の実体を返却
        return *this;
    }

    //-----------------------------------------------------------------------------
    // 部分取得
    //-----------------------------------------------------------------------------
    Binary Sub(int start,int end)
    {
        Binary _result;

        // 文字数分繰り返し
        int _count=0;
        for(size_t i=start; i<this->m_size && _count<end; i++, _count++ )
        {
            _result.Add((this->m_data[i]));
        }

        // 結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 文字列取得
    //-----------------------------------------------------------------------------
    std::string ToString()
    {
        char _buffer[this->m_size+1];   // 文字列バッファ

        // 初期化
        memset(_buffer, 0x00, sizeof(_buffer));

        // 設定
        memcpy( _buffer, this->m_data, this->m_size );

        // 文字列置換
        std::string _result = std::string(_buffer);

        // 文字列返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 2進数文字列取得
    //-----------------------------------------------------------------------------
    std::string ToBinString()
    {
        std::stringstream _result;
        unsigned int _data;

        // サイズ分繰り返す
        for(size_t i=0; i<this->m_size; i++ )
        {
            _data = (unsigned int)(this->m_data[i]);
            _result << static_cast<std::bitset<8> >(_data);
        }

        // 文字列返却
        return _result.str();
    }

    //-----------------------------------------------------------------------------
    // 16進数文字列取得
    //-----------------------------------------------------------------------------
    std::string ToHexString()
    {
        std::stringstream _result;
        unsigned int _data;

        // サイズ分繰り返す
        for(size_t i=0; i<this->m_size; i++ )
        {
            _data = (unsigned int)(this->m_data[i]);
            _result << std::hex << std::setw(2) << std::right << std::setfill('0') << _data;
        }

        // 文字列返却
        return _result.str();
    }

    //-----------------------------------------------------------------------------
    // ダンプ文字列取得
    //-----------------------------------------------------------------------------
    std::string ToDump()
    {
        std::stringstream _dumpmsg; // ダンプメッセージ

        // ログ内容
        const unsigned char* p = (const unsigned char*)(this->m_data);
        size_t bytes = this->m_size;
        char text[DUMP_STRING_WIDHT+1];
        unsigned i = 0;
        unsigned _addrress;
        unsigned int _data;

        while (i < bytes)
        {
            // アドレス出力
            if ((i % DUMP_STRING_WIDHT) == 0)
            {
                _addrress = (uintptr_t)p;
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
            if ((i % DUMP_STRING_WIDHT) == 0)
            {
                _dumpmsg << ": " << text << std::endl;
            }
        }
        if ((i % DUMP_STRING_WIDHT) != 0)
        {
            _dumpmsg << std::setw((DUMP_STRING_WIDHT - (i % DUMP_STRING_WIDHT)) * 3 + 2) << std::setfill(' ');
            _dumpmsg << ": " << text;
        }

        // 文字列返却
        return _dumpmsg.str();
    }

    //-----------------------------------------------------------------------------
    // 反転
    //-----------------------------------------------------------------------------
    Binary Reverse()
    {
        u_char _save_data[this->m_size];  // 保存データ

        // 保存データ初期化
        memset( _save_data, 0x00, sizeof(_save_data) );

        // サイズ分繰り返す
        for(size_t i=0; i<this->m_size; i++ )
        {
            _save_data[i] = this->m_data[this->m_size-i-1];
        }

        // 返却データを設定
        Binary _result;
        _result.Set(_save_data,sizeof(_save_data));

        // 結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 数値変換
    //-----------------------------------------------------------------------------
    int ToInt()
    {
        int _result = 0;                    // 変換結果

        // 格納サイズ分繰り返し
        for(size_t i=0; i<this->m_size; i++ )
        {
            _result = _result << 8;
            _result = _result | this->m_data[i];
        }

        // 変換結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 数値変換(uint8_t)
    //-----------------------------------------------------------------------------
    uint8_t ToUint8()
    {
        uint8_t _result = 0;                // 変換結果

        // 格納サイズ分繰り返し
        for(size_t i=0; i<this->m_size; i++ )
        {
            _result = _result << 8;
            _result = _result | this->m_data[i];
        }

        // 変換結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 数値変換(uint16_t)
    //-----------------------------------------------------------------------------
    uint16_t ToUint16()
    {
        uint16_t _result = 0;               // 変換結果

        // 格納サイズ分繰り返し
        for(size_t i=0; i<this->m_size; i++ )
        {
            _result = _result << 8;
            _result = _result | this->m_data[i];
        }

        // 変換結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 数値変換(uint32_t)
    //-----------------------------------------------------------------------------
    uint32_t ToUint32()
    {
        uint32_t _result = 0;               // 変換結果

        // 格納サイズ分繰り返し
        for(size_t i=0; i<this->m_size; i++ )
        {
            _result = _result << 8;
            _result = _result | this->m_data[i];
        }

        // 変換結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 数値変換(uint64_t)
    //-----------------------------------------------------------------------------
    uint64_t ToUint64()
    {
        uint64_t _result = 0;               // 変換結果

        // 格納サイズ分繰り返し
        for(size_t i=0; i<this->m_size; i++ )
        {
            _result = _result << 8;
            _result = _result | this->m_data[i];
        }

        // 変換結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 数値変換(int8_t)
    //-----------------------------------------------------------------------------
    int8_t ToInt8()
    {
        int8_t _result = 0;                 // 変換結果

        // 格納サイズ分繰り返し
        for(size_t i=0; i<this->m_size; i++ )
        {
            _result = _result << 8;
            _result = _result | this->m_data[i];
        }

        // 変換結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 数値変換(int16_t)
    //-----------------------------------------------------------------------------
    int16_t ToInt16()
    {
        int16_t _result = 0;                // 変換結果

        // 格納サイズ分繰り返し
        for(size_t i=0; i<this->m_size; i++ )
        {
            _result = _result << 8;
            _result = _result | this->m_data[i];
        }

        // 変換結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 数値変換(int32_t)
    //-----------------------------------------------------------------------------
    int32_t ToInt32()
    {
        int32_t _result = 0;                // 変換結果

        // 格納サイズ分繰り返し
        for(size_t i=0; i<this->m_size; i++ )
        {
            _result = _result << 8;
            _result = _result | this->m_data[i];
        }

        // 変換結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 数値変換(int64_t)
    //-----------------------------------------------------------------------------
    int64_t ToInt64()
    {
        int64_t _result = 0;                // 変換結果

        // 格納サイズ分繰り返し
        for(size_t i=0; i<this->m_size; i++ )
        {
            _result = _result << 8;
            _result = _result | this->m_data[i];
        }

        // 変換結果を返却
        return _result;
    }

    //-----------------------------------------------------------------------------
    // 比較
    //-----------------------------------------------------------------------------
    bool Comp(Binary target) const
    {
        // メモリ比較
        if(memcmp(this->m_data,target.Data(),this->m_size) != 0)
        {
            // 不一致
            return false;
        }

        // 一致
        return true;
    }
};
#endif
