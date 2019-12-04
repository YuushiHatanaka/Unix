//------------------------------------------------------------------------------
// Socketクラスヘッダ
//------------------------------------------------------------------------------
#ifndef _Socket_H_                          // 二重インクルード防止
#define _Socket_H_                          // 二重インクルード防止

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>

//------------------------------------------------------------------------------
// クラス定義
//------------------------------------------------------------------------------
class Socket
{
protected :
    // ソケット
    int m_socket;

    // ソケットアドレス
    struct sockaddr_in m_sockaddr;

    // ソケットアドレスサイズ
    socklen_t m_sockaddr_length;

    // エラー番号
    int m_errno;
public :
    // コンストラクタ
    Socket();

    // コンストラクタ
    Socket(Socket& socket);

    // デストラクタ
    ~Socket();

    // 生成
    bool Create(int domain, int type, int protocol);

    // 設定
    void SetSocket(int socket);

    // 破棄
    void Destroy();

    // オプション取得
    bool GetOption(int level, int optname, void* optval, socklen_t* optlen);

    // オプション設定
    bool SetOption(int level, int optname, const void* optval, socklen_t optlen);

    // アドレス設定
    void SetAddress(std::string address, int port, u_char family);

    // 名前付与
    bool Bind();

    // 接続要求待ち
    bool Listen(int backlog);

    // 接続要求受付
    int Accept();

    // 接続要求受付
    bool Accept(Socket& socket);

    // 接続
    bool Connect();

    // 切断
    bool Disconnect();

    // シャットダウン
    bool Shutdown(int how);

    // 読込
    size_t Read(char* buffer, size_t size);

    // 書込
    size_t Write(char* buffer, size_t size);

    // 送信
    size_t Send(const void* buffer, size_t size, int flags);

    // 送信
    size_t SendTo(const void* buffer, size_t size, int flags);

    // 送信
    size_t SendMsg(const struct msghdr* message, int flags);

    // 受信
    size_t Recv(void* buffer, size_t size, int flags);

    // 受信
    size_t RecvFrom(void* buffer, size_t size, int flags);

    // 受信
    size_t RecvMsg(struct msghdr* message, int flags);

    // イベント待ち(select)
    bool Select(short& revent, struct timeval& timeout);

    // TODO:イベント待ち(pselect)

    // イベント待ち(poll)
    bool Poll(short events, short& revent, int timeout);

    // TODO:イベント待ち(ppoll)
};
#endif                                      // 二重インクルード防止
