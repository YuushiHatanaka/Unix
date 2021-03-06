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
    struct sockaddr_in m_addr;              // アドレス情報

public:
    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    IPv4() : Object()
    {
        // 初期化
        memset(&(this->m_addr), 0x00, sizeof(this->m_addr));
    }

    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    IPv4(std::string ip, int port) : Object()
    {
        // アドレス情報設定
        if(!this->Set(ip,port))
        {
            // 例外
            throw IPv4Exception("IPアドレス形式不正:[%s,%s]", ip.c_str(), this->ToErrMsg().c_str());
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

    //--------------------------------------------------------------------------
    // アドレス情報設定
    //--------------------------------------------------------------------------
    bool Set(std::string ip, int port)
    {
        // 初期化
        memset(&(this->m_addr), 0x00, sizeof(this->m_addr));

        // ASCII⇒アドレス情報
        if(inet_aton(ip.c_str(), &(this->m_addr.sin_addr)) == 0)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // アドレスファミリ設定
        this->m_addr.sin_family = AF_INET;

        // ポート番号変換
        this->m_addr.sin_port = htons(port);
        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // アドレス情報取得
    //--------------------------------------------------------------------------
    struct sockaddr_in& Get()
    {
        // アドレス情報返却
        return this->m_addr;
    }

    //--------------------------------------------------------------------------
    // アドレス取得
    //--------------------------------------------------------------------------
    std::string GetAddr()
    {
        std::stringstream _toString;        // 文字列化オブジェクト

        // アドレスを文字列化
        _toString << inet_ntoa(this->m_addr.sin_addr);

        // アドレスを返却
        return _toString.str();
    }

    //--------------------------------------------------------------------------
    // ポート番号取得
    //--------------------------------------------------------------------------
    uint16_t GetPort()
    {
        // ポート番号返却
        return ntohs(this->m_addr.sin_port);
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString()
    {
        std::stringstream _toString;        // 文字列化オブジェクト

        // IPアドレスを文字列化
        _toString << this->GetAddr();

        // ポート番号を文字列化
        if(this->m_addr.sin_port)
        {
            _toString << ":" << this->GetPort();
        }

        // 文字列を返却
        return _toString.str();
    }
};
#endif                                      // 二重インクルード防止
