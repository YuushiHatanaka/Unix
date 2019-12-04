//==============================================================================
// UNIXソケットクラスヘッダ
//==============================================================================
#ifndef _UnixSocket_H_                      // 二重インクルード防止
#define _UnixSocket_H_                      // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "Socket.h"
#include <sys/un.h>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// UnixSocketException例外クラス
//------------------------------------------------------------------------------
class UnixSocketException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    UnixSocketException(std::string msg) : Exception(msg)
    {
    };
};

//------------------------------------------------------------------------------
// UnixSocketクラス
//------------------------------------------------------------------------------
class UnixSocket : public Socket
{
protected:
    struct sockaddr_un m_pathaddr;          // PATHアドレス
    socklen_t m_pathaddr_length;            // ソケットアドレスサイズ

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    UnixSocket() : Socket()
    {
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    UnixSocket(UnixSocket& socket)
    {
        // コピー
        this->m_socket = socket.m_socket;
        this->m_sockaddr = socket.m_sockaddr;
        this->m_sockaddr_length = socket.m_pathaddr_length;
        this->m_errno = socket.m_errno;;
        this->m_pathaddr = socket.m_pathaddr;
    }

    //--------------------------------------------------------------------------
    // 生成
    //--------------------------------------------------------------------------
    bool Create()
    {
        // 親クラスのCreateメソッドを呼び出し、結果を返却
        return Socket::Create(AF_UNIX, SOCK_STREAM, 0);
    }

    //--------------------------------------------------------------------------
    // PATH設定
    //--------------------------------------------------------------------------
    void SetAddress(std::string path)
    {
        memset(&(this->m_pathaddr), 0x00, sizeof(this->m_pathaddr));
        this->m_pathaddr.sun_family = AF_UNIX;
        strcpy(this->m_pathaddr.sun_path, path.c_str());
    }

    //--------------------------------------------------------------------------
    // 名前付与
    //--------------------------------------------------------------------------
    bool Bind()
    {
        // 名前付与
        if(bind(this->m_socket, (struct sockaddr*)&(this->m_pathaddr), sizeof(this->m_pathaddr)) != 0)
        {
            // エラー番号設定
            this->m_errno = errno;

            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 接続要求受付
    //--------------------------------------------------------------------------
    int Accept()
    {
        // 接続要求受付
        int _socket = accept(this->m_socket, (struct sockaddr*)&(this->m_pathaddr), &(this->m_pathaddr_length));

        // 結果判定
        if(_socket < 0)
        {
            // エラー番号設定
            this->m_errno = errno;

            // 異常終了
            return _socket;
        }

        // 正常終了
        return _socket;
    }

    //--------------------------------------------------------------------------
    // 接続要求受付
    //--------------------------------------------------------------------------
    bool Accept(UnixSocket& socket)
    {
        // 接続要求受付
        this->m_socket = accept(socket.m_socket, (struct sockaddr*)&(this->m_pathaddr), &(this->m_pathaddr_length));

        // 結果判定
        if(this->m_socket < 0)
        {
            // エラー番号設定
            this->m_errno = errno;

            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 接続
    //--------------------------------------------------------------------------
    bool Connect()
    {
        // 接続
        if(connect(this->m_socket, (struct sockaddr*)&(this->m_pathaddr), sizeof(this->m_pathaddr)) != 0)
        {
            // エラー番号設定
            this->m_errno = errno;

            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // ソケットファイル削除
    //--------------------------------------------------------------------------
    bool Unlink()
    {
        // ソケットファイル削除
        if(unlink(this->m_pathaddr.sun_path) != 0)
        {
            // エラー番号設定
            this->m_errno = errno;

            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }
};
#endif                                      // 二重インクルード防止
