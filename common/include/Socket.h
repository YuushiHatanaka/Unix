//==============================================================================
// ソケットクラスヘッダ
//==============================================================================
#ifndef _Socket_H_                          // 二重インクルード防止
#define _Socket_H_                          // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Object.h"
#include "Exception.h"
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// Socket例外クラス
//------------------------------------------------------------------------------
class SocketException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    SocketException(std::string format, ...)
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
// Socketクラス
//------------------------------------------------------------------------------
class Socket : public Object
{
protected :
    int m_socket;                           // ソケット
    struct sockaddr_in m_sockaddr;          // ソケットアドレス
    socklen_t m_sockaddr_length;            // ソケットアドレスサイズ
    int m_errno;                            // エラー番号

public :
    int GetSocket() { return this->m_socket; }

public :
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    Socket() : Object()
    {
        // 初期化
        this->m_socket = -1;
        this->m_sockaddr_length = sizeof(this->m_sockaddr);
        memset(&(this->m_sockaddr), 0x0, this->m_sockaddr_length);
        this->m_errno = 0;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    Socket(int socket) : Object()
    {
        // 初期化
        this->m_socket = socket;
        this->m_sockaddr_length = sizeof(this->m_sockaddr);
        memset(&(this->m_sockaddr), 0x0, this->m_sockaddr_length);
        this->m_errno = 0;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    Socket(const Socket& socket) : Object()
    {
        // コピー
        this->m_socket = socket.m_socket;
        this->m_sockaddr = socket.m_sockaddr;
        this->m_sockaddr_length = socket.m_sockaddr_length;
        this->m_errno = socket.m_errno;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~Socket()
    {
        // 破棄
        this->Destroy();
    }

    //--------------------------------------------------------------------------
    // 生成
    //--------------------------------------------------------------------------
    bool Create(int domain, int type, int protocol)
    {
        // ソケット生成
        this->m_socket = socket(domain, type, protocol);

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
    // 設定
    //--------------------------------------------------------------------------
    void SetSocket(int socket)
    {
        // ソケット設定
        this->m_socket = socket;
    }

    //--------------------------------------------------------------------------
    // 破棄
    //--------------------------------------------------------------------------
    void Destroy()
    {
       // 切断
       this->Disconnect();
    }

    //--------------------------------------------------------------------------
    // オプション取得
    //--------------------------------------------------------------------------
    bool GetOption(int level, int optname, void* optval, socklen_t* optlen)
    {
        // ソケットオプション取得
        if(getsockopt(this->m_socket, level, optname, optval, optlen) < 0)
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
    // オプション設定
    //--------------------------------------------------------------------------
    bool SetOption(int level, int optname, const void* optval, socklen_t optlen)
    {
        // ソケットオプション設定
        if(setsockopt(this->m_socket, level, optname, optval, optlen) < 0)
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
    // アドレス設定
    //--------------------------------------------------------------------------
    void SetAddress(std::string address, int port, u_char family)
    {
        // ソケットアドレス生成
        this->m_sockaddr.sin_family = family;
        this->m_sockaddr.sin_addr.s_addr = inet_addr(address.c_str());
        this->m_sockaddr.sin_port = htons(port);
    }

    //--------------------------------------------------------------------------
    // 名前付与
    //--------------------------------------------------------------------------
    bool Bind()
    {
        // 名前付与
        if(bind(this->m_socket, (struct sockaddr*)&(this->m_sockaddr), sizeof(this->m_sockaddr)) != 0)
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
    // 接続要求待ち
    //--------------------------------------------------------------------------
    bool Listen(int backlog)
    {
        // 接続要求待ち
        if(listen(this->m_socket, backlog) != 0)
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
        int _socket = accept(this->m_socket, (struct sockaddr *)&(this->m_sockaddr), &(this->m_sockaddr_length));

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
    // 接続
    //--------------------------------------------------------------------------
    bool Connect()
    {
        // 接続
        if(connect(this->m_socket, (struct sockaddr *)&(this->m_sockaddr), sizeof(this->m_sockaddr)) != 0)
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
    // 切断
    //--------------------------------------------------------------------------
    bool Disconnect()
    {
        // 切断判定
        if(this->m_socket < 0)
        {
            // 正常終了
            return true;
        }

        // 切断
        if(close(this->m_socket) != 0)
        {
            // エラー番号設定
            this->m_errno = errno;

            // 異常終了
            return false;
        }

        // ソケット初期化
        this->m_socket = -1;

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // シャットダウン
    //--------------------------------------------------------------------------
    bool Shutdown(int how)
    {
        // 切断判定
        if(this->m_socket < 0)
        {
            // 正常終了
            return true;
        }

        // シャットダウン
        if(shutdown(this->m_socket, how) != 0)
        {
            // エラー番号設定
            this->m_errno = errno;

            // 異常終了
            return false;
        }

        // ソケット初期化
        this->m_socket = -1;

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 読込
    //--------------------------------------------------------------------------
    size_t Read(char* buffer, size_t size)
    {
        // 読込サイズを返却
        return this->Read(this->m_socket, buffer, size);
    }

    //--------------------------------------------------------------------------
    // 読込
    //--------------------------------------------------------------------------
    size_t Read(int socket, char* buffer, size_t size)
    {
        size_t _readSize = 0;                   // 読込サイズ

        // 読込
        _readSize = read(socket, buffer, size);

        // 読込サイズを返却
        return _readSize;
    }

    //--------------------------------------------------------------------------
    // 書込
    //--------------------------------------------------------------------------
    size_t Write(char* buffer, size_t size)
    {
        // 書込サイズを返却
        return this->Write(this->m_socket, buffer, size);
    }

    //--------------------------------------------------------------------------
    // 書込
    //--------------------------------------------------------------------------
    size_t Write(int socket, char* buffer, size_t size)
    {
        size_t _writeSize = 0;                  // 書込サイズ

        // 書込
        _writeSize = write(socket, buffer, size);

        // 書込サイズを返却
        return _writeSize;
    }

    //--------------------------------------------------------------------------
    // 送信
    //--------------------------------------------------------------------------
    size_t Send(const void* buffer, size_t size, int flags)
    {
        // 送信
        return this->Send(this->m_socket, buffer, size, flags);
    }

    //--------------------------------------------------------------------------
    // 送信
    //--------------------------------------------------------------------------
    size_t Send(int socket, const void* buffer, size_t size, int flags)
    {
        size_t _sendSize = 0;                   // 送信サイズ

        // 送信
        _sendSize = send(socket, buffer, size, flags);

        // 送信サイズを返却
        return _sendSize;
    }

    //--------------------------------------------------------------------------
    // 送信
    //--------------------------------------------------------------------------
    size_t SendTo(const void* buffer, size_t size, int flags)
    {
        size_t _sendSize = 0;                   // 送信サイズ

        // 送信
        _sendSize = sendto(this->m_socket, buffer, size, flags, (struct sockaddr*)&(this->m_sockaddr), sizeof(this->m_sockaddr));

        // 送信サイズを返却
        return _sendSize;
    }

    //--------------------------------------------------------------------------
    // 送信
    //--------------------------------------------------------------------------
    size_t SendMsg(const struct msghdr* message, int flags)
    {
        size_t _sendSize = 0;                   // 送信サイズ

        // 送信
        _sendSize = sendmsg(this->m_socket, message, flags);

        // 送信サイズを返却
        return _sendSize;
    }

    //--------------------------------------------------------------------------
    // 受信
    //--------------------------------------------------------------------------
    size_t Recv(void* buffer, size_t size, int flags)
    {
        // 受信
        return this->Recv(this->m_socket, buffer, size, flags);
    }

    //--------------------------------------------------------------------------
    // 受信
    //--------------------------------------------------------------------------
    size_t Recv(int socket, void* buffer, size_t size, int flags)
    {
        size_t _recvSize = 0;                   // 受信サイズ

        // 受信
        _recvSize = recv(socket, buffer, size, flags);

        // 受信サイズを返却
        return _recvSize;
    }

    //--------------------------------------------------------------------------
    // 受信
    //--------------------------------------------------------------------------
    size_t RecvFrom(void* buffer, size_t size, int flags, Socket& recvSocket)
    {
        size_t _recvSize = 0;                   // 受信サイズ

        // 受信
        _recvSize = recvfrom(this->m_socket, buffer, size, flags, (struct sockaddr*)&(recvSocket.m_sockaddr), &(recvSocket.m_sockaddr_length));

        // 受信サイズを返却
        return _recvSize;
    }

    //--------------------------------------------------------------------------
    // 受信
    //--------------------------------------------------------------------------
    size_t RecvMsg(struct msghdr* message, int flags)
    {
        size_t _recvSize = 0;                   // 受信サイズ

        // 受信
        _recvSize = recvmsg(this->m_socket, message, flags);

        // 受信サイズを返却
        return _recvSize;
    }

    //--------------------------------------------------------------------------
    // イベント待ち(select)
    //--------------------------------------------------------------------------
    bool Select(short& revent, long second)
    {
        // 結果を返却
        return this->Select(revent, second, 0, 0);
    }

    //--------------------------------------------------------------------------
    // イベント待ち(select)
    //--------------------------------------------------------------------------
    bool Select(short& revent, long second, long msec)
    {
        // 結果を返却
        return this->Select(revent, second, msec, 0);
    }

    //--------------------------------------------------------------------------
    // イベント待ち(select)
    //--------------------------------------------------------------------------
    bool Select(short& revent, long second, long msec, long usec)
    {
        // タイムアウト値を設定
        struct timeval _timeout;
        _timeout.tv_sec = second;
        _timeout.tv_usec = (msec * 1000) + usec;

        // 結果を返却
        return this->Select(revent, _timeout);
    }

    //--------------------------------------------------------------------------
    // イベント待ち(select)
    //--------------------------------------------------------------------------
    bool Select(short& revent, struct timeval& timeout)
    {
        fd_set _readfds;                        // fd_set
        fd_set _writefds;                       // fd_set
        fd_set _exceptfds;                      // fd_set
        int _maxfd;                             // ファイルディスクリプタ最大値

        // イベント初期化
        revent = 0x00;

        // fd_set初期化
        FD_ZERO(&_readfds);
        FD_ZERO(&_writefds);
        FD_ZERO(&_exceptfds);

        // selectで待つ読み込みソケットを登録
        FD_SET(this->m_socket, &_readfds);

        // ファイルディスクリプタの最大値設定
        _maxfd = this->m_socket;

        // イベント待ち(select)
        int _result = select(_maxfd+1, &_readfds, &_writefds, &_exceptfds, &timeout);

        // イベント待ち判定
        if(_result < 0)
        {
            // イベント設定
            revent = POLLERR;

            // エラー番号設定
            this->m_errno = errno;

            // 異常終了
            return false;
        }
        // タイムアウト
        else if(_result == 0)
        {
            // イベントなし設定
            revent = 0x00;

            // 正常終了
            return true;
        }

        // 例外イベント判定
        if(FD_ISSET(this->m_socket, &_exceptfds))
        {
            // 例外イベント設定
            revent = POLLHUP;

            // 異常終了
            return false;
        }

        // 読込イベント判定
        if(FD_ISSET(this->m_socket, &_readfds))
        {
            // 読込イベント設定
            revent |= POLLIN;
        }

        // 書込イベント判定
        if(FD_ISSET(this->m_socket, &_writefds))
        {
            // 書込イベント設定
            revent |= POLLOUT;
        }

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // イベント待ち(poll)
    //--------------------------------------------------------------------------
    bool Poll(short events, short& revent, int timeout)
    {
        struct pollfd _fds;                     // pollfd

        // pollfds初期化
        memset(&_fds, 0x00, sizeof(_fds));

        // pollで待つ読み込みソケットを登録
        _fds.fd = this->m_socket;
        _fds.events = events;

        // イベント待ち(poll)
        int _result = poll(&_fds, 1, timeout);

        // イベント待ち判定
        if( _result < 0)
        {
            // エラー番号設定
            this->m_errno = errno;

            // 異常終了
            return false;
        }

        // イベントを返却
        revent = _fds.revents;

        // 正常終了
        return true;
    }

    // TODO:イベント待ち(pselect)

    // TODO:イベント待ち(ppoll)

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString()
    {
        std::stringstream _stringstream;    // 文字列化Stream

        // IPアドレスを文字列化
        _stringstream << inet_ntoa(this->m_sockaddr.sin_addr);

        // ポート番号を文字列化
        if(this->m_sockaddr.sin_port != 0)
        {
            _stringstream << ":" << ntohs(this->m_sockaddr.sin_port);
        }

        // 文字列を返却
        return _stringstream.str();
    }
};
#endif                                      // 二重インクルード防止
