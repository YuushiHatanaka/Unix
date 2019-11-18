//-----------------------------------------------------------------------------
// udp_socketモジュールヘッダ
//-----------------------------------------------------------------------------
#ifndef _UDP_SOCKET_HPP
#define _UDP_SOCKET_HPP

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // Cent OS 7対応

#include <string>

#include "binary.hpp"

//-----------------------------------------------------------------------------
// クラス定義
//-----------------------------------------------------------------------------
// udp_socketクラス
class udp_socket {
private :
    // 受信バッファサイズ
    int m_capacity;
    // 通信先IPアドレス
    std::string m_address;
    // 通信先ポート番号
    int m_port;
    // ソケットディスクリプタ
    int m_sockfd;
    // 接続先情報構造体
    struct sockaddr_in m_addrinfo;
public :
    // コンストラクタ
    udp_socket(std::string,int);
    // コンストラクタ
    udp_socket(const udp_socket&);
    // デストラクタ
    ~udp_socket();

    // 接続
    bool connect();
    // バインド
    bool udp_bind(std::string,int);
    // 切断
    void disconnect();

    // 送信
    bool send(binary);
    // 受信
    bool recv(binary&);
};
#endif
