//==============================================================================
// IPv4クラスヘッダ
//==============================================================================
#ifndef _IPV4_H_                            // 二重インクルード防止
#define _IPV4_H_                            // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Object.h"
#include "Exception.h"
#include "RegularExpression.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// IPv4例外クラス
//------------------------------------------------------------------------------
class IPv4Exception : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IPv4Exception(std::string format, ...)
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
// IPv4クラス
//------------------------------------------------------------------------------
class IPv4 : public Object
{
private:
    struct in_addr m_addr;                  // アドレス情報

public:
    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    IPv4(std::string ip) : Object()
    {
        // 文字列アドレス変換
        if(!this->ToAddr(ip))
        {
            // 例外
            throw IPv4Exception("IPアドレス形式不正:[%s]", ip.c_str());
        }
    }

    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    IPv4(const IPv4& obj) : Object(obj)
    {
        // コピー
        this->m_addr = obj.m_addr;
    }

    //-------------------------------------------------------------------------
    // 文字列アドレス変換
    //-------------------------------------------------------------------------
    bool ToAddr(std::string ip)
    {
        // ASCII⇒アドレス情報
        if(inet_aton(ip.c_str(), &(this->m_addr)) == 0)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString()
    {
        std::stringstream _toString;        // 文字列化オブジェクト

        // IPアドレスを文字列化
        _toString << inet_ntoa(this->m_addr);

        // 文字列を返却
        return _toString.str();
    }
};
#endif                                      // 二重インクルード防止
