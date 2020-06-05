//==============================================================================
// MemoryStreamクラスヘッダ
//==============================================================================
#ifndef _MEMORY_STREAM_H_                   // 二重インクルード防止
#define _MEMORY_STREAM_H_                   // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "Object.h"

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// MemoryStream例外クラス
//------------------------------------------------------------------------------
class MemoryStreamException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MemoryStreamException(std::string format, ...)
        : Exception()
    {
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        this->SetMessage(format, ap);
        va_end(ap);
    };
};

//------------------------------------------------------------------------------
// MemoryStreamクラス
//------------------------------------------------------------------------------
class MemoryStream : public Object
{
protected:
    u_char* m_data;                         // データ
    size_t m_size;                          // データ量
    uint64_t m_capacity;                    // 許容サイズ
    uint64_t m_remaining_size;              // 残りサイズ
    uint64_t m_block_count;                 // ブロック数
    uint64_t m_block_size;                  // ブロックサイズ

private:
    //-----------------------------------------------------------------------------
    // 生成
    //-----------------------------------------------------------------------------
    void Create()
    {
        // 生成
        this->m_data = new u_char[this->m_capacity];

        // 初期化
        memset(this->m_data, 0x00, sizeof(this->m_capacity));

        // サイズクリア
        this->m_size = 0;
    }

    //-----------------------------------------------------------------------------
    // 解放
    //-----------------------------------------------------------------------------
    void Destroy()
    {
        // 解放
        if( this->m_data != NULL )
        {
            delete[] this->m_data;
            this->m_data = NULL;
            this->m_size = 0;
            this->m_remaining_size = this->m_capacity;
        }
    }

    //-----------------------------------------------------------------------------
    // 必要サイズ取得
    //-----------------------------------------------------------------------------
    size_t GetRequiredSize(size_t size)
    {
        // 必要ブロック数取得
        this->m_block_count = size / this->m_block_size;

        // 必要サイズ計算
        size_t _required_size = this->m_block_size * this->m_block_count;

        // 必要サイズ返却
        return _required_size;
    }

    //-----------------------------------------------------------------------------
    // 再生成
    //-----------------------------------------------------------------------------
    bool Regeneration(size_t size)
    {
        // サイズ判定
        if(size <= this->m_capacity)
        {
            // 初期化
            memset(this->m_data, 0x00, sizeof(this->m_capacity));

            // 正常終了[再生成なし]
            return true;
        }

        // 必要サイズ取得
        size_t _required_size = this->GetRequiredSize(size);

        // 設定
        this->m_capacity = _required_size;
        this->m_remaining_size = _required_size;

        // 解放
        this->Destroy();

        // 生成
        this->Create();

        // 正常終了
        return true;
    }

    //-----------------------------------------------------------------------------
    // 再配置
    //-----------------------------------------------------------------------------
    bool Relocation(size_t size)
    {
        // サイズ判定
        if(size <= this->m_capacity)
        {
            // 正常終了[再配置なし]
            return true;
        }

        // 必要サイズ取得
        size_t _required_size = this->GetRequiredSize(size);

        // 設定
        this->m_capacity = _required_size;
        this->m_remaining_size = _required_size;

        // 旧エリア保存
        u_char* _data = this->m_data;
        size_t _size = this->m_size;

        // 生成
        this->Create();

        // 再配置
        memcpy(this->m_data, _data, _size);
        this->m_size = _size;
        this->m_remaining_size -= _size;

        // 旧エリア解放
        delete[] _data;

        // 正常終了
        return true;
    }

public:
    // アクセスメソッド(データ量)
    size_t Size() { return this->m_size; }
    // アクセスメソッド(データ)
    u_char* Data() { return this->m_data; }
    // アクセスメソッド(許容サイズ)
    size_t Capacity() { return this->m_capacity; }
    // アクセスメソッド(残りサイズ)
    size_t RemainingSize() { return this->m_remaining_size; }
    // アクセスメソッド(ブロック数)
    size_t BlockCount() { return this->m_block_count; }
    // アクセスメソッド(ブロックサイズ)
    size_t BlockSize() { return this->m_block_size; }

public:
    //-----------------------------------------------------------------------------
    // コンストラクタ
    //-----------------------------------------------------------------------------
    MemoryStream() : Object()
    {
        // 初期化
        this->m_size = 0;
        this->m_data = NULL;
        this->m_block_size = 8192;
        this->m_block_count = 1;
        this->m_capacity = this->m_block_size * this->m_block_count;
        this->m_remaining_size = this->m_capacity;

        // 生成
        this->Create();
    }

    //-----------------------------------------------------------------------------
    // コンストラクタ
    //-----------------------------------------------------------------------------
    MemoryStream(size_t block_size) : Object()
    {
        // 初期化
        this->m_size = 0;
        this->m_data = NULL;
        this->m_block_size = block_size;
        this->m_block_count = 1;
        this->m_capacity = this->m_block_size * this->m_block_count;
        this->m_remaining_size = this->m_capacity;

        // 生成
        this->Create();
    }

    //-----------------------------------------------------------------------------
    // コンストラクタ
    //-----------------------------------------------------------------------------
    MemoryStream(size_t block_size, size_t block_count) : Object()
    {
        // 初期化
        this->m_size = 0;
        this->m_data = NULL;
        this->m_block_size = block_size;
        this->m_block_count = block_count;
        this->m_capacity = this->m_block_size * this->m_block_count;
        this->m_remaining_size = this->m_capacity;

        // 生成
        this->Create();
    }

    //-----------------------------------------------------------------------------
    // コンストラクタ
    //-----------------------------------------------------------------------------
    MemoryStream(const MemoryStream& stream) : Object(stream)
    {
        // 初期化
        this->m_size = stream.m_size;
        this->m_data = stream.m_data;
        this->m_block_size = stream.m_block_size;
        this->m_block_count = stream.m_block_count;
        this->m_capacity = stream.m_capacity;
        this->m_remaining_size = stream.m_remaining_size;
    }

    //-----------------------------------------------------------------------------
    // デストラクタ
    //-----------------------------------------------------------------------------
    virtual ~MemoryStream()
    {
        // 解放
        this->Destroy();
    }

    //-----------------------------------------------------------------------------
    // クリア
    //-----------------------------------------------------------------------------
    void Clear()
    {
        // 解放
        this->Destroy();

        // 生成
        this->Create();
    }

    //-----------------------------------------------------------------------------
    // 設定
    //-----------------------------------------------------------------------------
    void Set(u_char* data, size_t size)
    {
        // 再生成
        if(!this->Regeneration(size))
        {
            // TODO:例外
        }

        // データ設定
        memcpy(this->m_data, data, size);

        // サイズ設定
        this->m_size = size;

        // 残りサイズ減算
        this->m_remaining_size -= size;
    }

    //-----------------------------------------------------------------------------
    // 設定
    //-----------------------------------------------------------------------------
    void Set(const MemoryStream& stream)
    {
        // 設定
        this->Set(stream.m_data, stream.m_size);
    }

    //-----------------------------------------------------------------------------
    // 追加
    //-----------------------------------------------------------------------------
    void Add(u_char* data, size_t size)
    {
        // 再配置
        if(!this->Relocation(this->m_size + size))
        {
            // TODO:例外
        }

        // データ追加
        memcpy(&(this->m_data[this->m_size]), data, size);

        // サイズ加算
        this->m_size += size;

        // 残りサイズ減算
        this->m_remaining_size -= size;
    }

    //-----------------------------------------------------------------------------
    // 追加
    //-----------------------------------------------------------------------------
    void Add(const MemoryStream& stream)
    {
        // 設定
        this->Add(stream.m_data, stream.m_size);
    }

    //-----------------------------------------------------------------------------
    // ダンプ文字列取得
    //-----------------------------------------------------------------------------
    std::string ToDump()
    {
        // ダンプ文字列返却
        return Object::ToDump(this->m_data, this->m_size);
    }
};
#endif                                      // 二重インクルード防止
