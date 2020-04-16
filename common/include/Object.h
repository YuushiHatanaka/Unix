//==============================================================================
// Objectクラスヘッダ
//==============================================================================
#ifndef _OBJECT_H_                          // 二重インクルード防止
#define _OBJECT_H_                          // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include <stdlib.h>
#include <string>
#include <sstream>
#include <stdint.h>
#include <iomanip>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// Objectクラス
//------------------------------------------------------------------------------
class Object
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    Object()
    {
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~Object()
    {
    }

    //--------------------------------------------------------------------------
    // ハッシュコード取得
    //--------------------------------------------------------------------------
    uint64_t GetHashCode()
    {
        std::stringstream _HashCode;        // ハッシュコード変換用stringstream

        // TODO:自身のアドレスをHASHコードにする(暫定)
        _HashCode << this;

        // ハッシュコードを返却
        return (uint64_t)strtol(_HashCode.str().c_str(), NULL, 16);
    }

    //--------------------------------------------------------------------------
    // 一致判定
    //--------------------------------------------------------------------------
    bool Equals(Object& object)
    {
        // ハッシュコードを比較
        if(this->GetHashCode() == object.GetHashCode())
        {
            // 一致
            return true;
        }

        // 不一致
        return false;
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    virtual std::string ToString()
    {
        std::stringstream _toString;        // 文字列化オブジェクト

        // 自身のアドレスを文字列化
        _toString  << "0x" << std::hex << std::setw(8) << std::setfill('0') << this;

        // 文字列を返却
        return _toString.str();
    }
};
#endif                                      // 二重インクルード防止