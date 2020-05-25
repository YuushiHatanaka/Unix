//==============================================================================
// EmptyBlock(空塞表)モジュールヘッダ
//==============================================================================
#ifndef _EMPTY_BLOCK_H_                     // 二重インクルード防止
#define _EMPTY_BLOCK_H_                     // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "Mutex.h"
#include <stdint.h>
#include <sys/types.h>
#include <iomanip>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// EmptyBlock例外クラス
//------------------------------------------------------------------------------
class EmptyBlockException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    EmptyBlockException(std::string format, ...)
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
// EmptyBlockクラス
//------------------------------------------------------------------------------
class EmptyBlock {
private:
    Mutex m_Mutex;                          // 排他制御
    uint64_t m_Capacity;                    // 空塞表最大数
    uint64_t m_BitmapSize;                  // 空塞表マップサイズ
    uint64_t m_BitmapBlockSize;             // 空塞表マップブロックサイズ
    uint64_t m_BlockCount;                  // 閉塞数
    u_char* m_Bitmap;                       // 空塞表
    static const int DUMP_STRING_WIDHT = 16;// ダンプ文字列長

private:
    //--------------------------------------------------------------------------
    // 初期化
    //--------------------------------------------------------------------------
    void Initialization()
    {
        // 最大数を8の倍数に変更する
        this->m_BitmapSize = (this->m_Capacity + 7) / 8 * 8;
        this->m_BitmapBlockSize = this->m_BitmapSize / 8;
        this->m_Bitmap = new u_char[this->m_BitmapBlockSize];
        memset(this->m_Bitmap, 0x00, this->m_BitmapBlockSize);
    }

    //--------------------------------------------------------------------------
    // ダンプイメージ取得
    //--------------------------------------------------------------------------
    std::string Dump(int indent)
    {
        std::stringstream _image;           // ダンプイメージ

        // ログ内容
        const unsigned char* p = (const unsigned char*)this->m_Bitmap;
        char text[EmptyBlock::DUMP_STRING_WIDHT+1];
        unsigned i = 0;

        // サイズ分繰り返す
        while (i < this->m_BitmapBlockSize)
        {
            // アドレス部取得
            if ((i % EmptyBlock::DUMP_STRING_WIDHT) == 0)
            {
                _image << std::string(indent, ' ') << "0x" << std::hex << std::setw(8) << std::right << std::setfill('0') << i << ": ";
                memset(text, '\0', sizeof(text));
            }

            // ダンプデータ設定
            unsigned int d = (unsigned int)(*p);
            _image << std::hex << std::setw(2) << std::right << std::setfill('0') << d << " ";
            text[i % EmptyBlock::DUMP_STRING_WIDHT] = isprint(*p) ? *p : '.';
            p++; i++;

            // テキスト部出力
            if ((i % EmptyBlock::DUMP_STRING_WIDHT) == 0)
            {
                _image << ": " << text << std::endl;
            }
        }

        // 残り部分出力
        if ((i % EmptyBlock::DUMP_STRING_WIDHT) != 0)
        {
            _image << std::setw((DUMP_STRING_WIDHT - (i % DUMP_STRING_WIDHT)) * 3 + 2) << std::setfill(' ');
            _image << ": " << text << std::endl;
        }

        // ダンプイメージを返却
        return _image.str();
    }

    //--------------------------------------------------------------------------
    // 位置取得
    //--------------------------------------------------------------------------
    void GetPosition(uint64_t index, uint64_t& row, uint64_t& col)
    {
        row = index / 8;
        col = index % 8;
    }

    //--------------------------------------------------------------------------
    // 値取得
    //--------------------------------------------------------------------------
    u_char GetValue(uint64_t index)
    {
        uint64_t _row = 0;                  // 位置(縦)
        uint64_t _col = 0;                  // 位置(横)

        // 位置取得
        this->GetPosition(index, _row, _col);

        // ビット bit に _col 番目のフラグが立っているかどうか
        if(this->m_Bitmap[_row] & (1<<_col))
        {
            // 塞がりを返却
            return 1;
        }
        else
        {
            // 空きを返却
            return 0;
        }
    }

    //--------------------------------------------------------------------------
    // 値設定
    //--------------------------------------------------------------------------
    void SetValue(uint64_t index)
    {
        uint64_t _row = 0;                  // 位置(縦)
        uint64_t _col = 0;                  // 位置(横)

        // 位置取得
        this->GetPosition(index, _row, _col);

        // ビット bit に _col 番目のフラグを立てる
        this->m_Bitmap[_row] |= (0x01 << _col);
    }

    //--------------------------------------------------------------------------
    // 値設定
    //--------------------------------------------------------------------------
    void FreeValue(uint64_t index)
    {
        uint64_t _row = 0;                  // 位置(縦)
        uint64_t _col = 0;                  // 位置(横)

        // 位置取得
        this->GetPosition(index, _row, _col);

        // ビット bit に _col 番目のフラグを消す
        this->m_Bitmap[_row] &= ~(0x01 << _col);
    }

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    EmptyBlock(uint64_t capacity)
    {
        // 排他
        this->m_Mutex.Lock();

        // 設定
        this->m_Capacity = capacity;
        this->m_BitmapSize = 0;
        this->m_BitmapBlockSize = 0;
        this->m_BlockCount = 0;
        this->m_Bitmap = NULL;

        // 初期化
        this->Initialization();

        // 排他解除
        this->m_Mutex.Unlock();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    EmptyBlock(const EmptyBlock& emptyblock)
    {
        // 排他
        this->m_Mutex.Lock();

        // コピー
        this->m_Capacity = emptyblock.m_Capacity;
        this->m_BitmapSize = emptyblock.m_BitmapSize;
        this->m_BitmapBlockSize = emptyblock.m_BitmapBlockSize;
        this->m_BlockCount = emptyblock.m_BlockCount;
        this->m_Bitmap = new u_char[this->m_BitmapBlockSize];
        memcpy(this->m_Bitmap, emptyblock.m_Bitmap, this->m_BitmapBlockSize);

        // 排他解除
        this->m_Mutex.Unlock();
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~EmptyBlock()
    {
        // 破棄
        if(this->m_Bitmap != NULL)
        {
            delete[] this->m_Bitmap;
        }
    }

    //--------------------------------------------------------------------------
    // 空き判定
    //--------------------------------------------------------------------------
    bool IsEmpty(uint64_t index)
    {
        // 範囲判定
        if(index > this->m_Capacity - 1)
        {
            // 例外
            std::stringstream _indexstream;
            _indexstream << index;
            throw EmptyBlockException("IndexOutOfBounds:["+_indexstream.str()+"]");
        }

        // 値取得
        if(this->GetValue(index))
        {
            // 閉塞
            return false;
        }

        // 空き
        return true;
    }

    //--------------------------------------------------------------------------
    // 閉塞
    //--------------------------------------------------------------------------
    char Occlusion(uint64_t index)
    {
        // 範囲判定
        if(index > this->m_Capacity - 1)
        {
            // 異常終了
            return -1;
        }

        // 排他
        this->m_Mutex.Lock();

        // 空判定
        if(!this->IsEmpty(index))
        {
            // 排他解除
            this->m_Mutex.Unlock();

            // 塞がり
            return 0;
        }

        // 閉塞設定
        this->SetValue(index);

        // 閉塞数を加算
        this->m_BlockCount++;

        // 排他解除
        this->m_Mutex.Unlock();

        // 正常終了
        return 1;
    }

    //--------------------------------------------------------------------------
    // 解放
    //--------------------------------------------------------------------------
    char Free(uint64_t index)
    {
        // 範囲判定
        if(index > this->m_Capacity - 1)
        {
            // 異常終了
            return -1;
        }

        // 排他
        this->m_Mutex.Lock();

        // 塞判定
        if(this->IsEmpty(index))
        {
            // 排他解除
            this->m_Mutex.Unlock();

            // 既に空き
            return 0;
        }

        // 空き設定
        this->FreeValue(index);

        // 閉塞数を減算
        this->m_BlockCount--;

        // 排他解除
        this->m_Mutex.Unlock();

        // 正常終了
        return 1;
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString()
    {
        // 文字列を返却
        return this->ToString(0);
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString(int indent)
    {
        std::stringstream _toString;        // 文字列化オブジェクト

        // 情報設定
        _toString << std::string(indent, ' ') << "空塞表最大数              ：" << this->m_Capacity << "\n";
        _toString << std::string(indent, ' ') << "空塞表マップサイズ        ：" << this->m_BitmapSize << "\n";
        _toString << std::string(indent, ' ') << "空塞表マップブロックサイズ：" << this->m_BitmapBlockSize << "\n";
        _toString << std::string(indent, ' ') << "閉塞数                    ：" << this->m_BlockCount << "\n";

        // ダンプイメージ取得
        _toString << this->Dump(indent);

        // 文字列を返却
        return _toString.str();
    }
};
#endif                                      // 二重インクルード防止
