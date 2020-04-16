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
};
#endif                                      // 二重インクルード防止