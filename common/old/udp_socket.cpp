//-----------------------------------------------------------------------------
// udp_socketモジュール
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include "udp_socket.hpp"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
udp_socket::udp_socket(std::string address, int port)
{
    // 初期化
    udp_socket::m_capacity = 4096;
    udp_socket::m_sockfd = 0;
    memset( &(udp_socket::m_addrinfo), 0x00, sizeof(udp_socket::m_addrinfo) );

    // 設定
    udp_socket::m_address = address;
    udp_socket::m_port = port;
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
udp_socket::udp_socket(const udp_socket& socket_data)
{
    // 設定
    udp_socket::m_capacity = socket_data.m_capacity;
    udp_socket::m_address = socket_data.m_address;
    udp_socket::m_port = socket_data.m_port;
    udp_socket::m_sockfd = socket_data.m_sockfd;
    memcpy( &(udp_socket::m_addrinfo), &(socket_data.m_addrinfo), sizeof(udp_socket::m_addrinfo) );
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
udp_socket::~udp_socket()
{
}

//-----------------------------------------------------------------------------
// 接続
//-----------------------------------------------------------------------------
bool udp_socket::connect()
{
    // ソケットを開く
    if((udp_socket::m_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        // 異常終了
        perror("socket");
        return false;
    }

    // 正常終了
    return true;
}

//-----------------------------------------------------------------------------
// 切断
//-----------------------------------------------------------------------------
void udp_socket::disconnect()
{
    // ソケットを閉じる
    close(udp_socket::m_sockfd);
}

//-----------------------------------------------------------------------------
// 送信
//-----------------------------------------------------------------------------
bool udp_socket::send(binary send_data)
{
    // 送信先アドレスとポート番号を設定する
    (udp_socket::m_addrinfo).sin_family = AF_INET;
    (udp_socket::m_addrinfo).sin_port = htons(udp_socket::m_port);
    (udp_socket::m_addrinfo).sin_addr.s_addr = inet_addr(udp_socket::m_address.c_str());

    // パケットをUDPで送信
    int _sendto_result = sendto(udp_socket::m_sockfd, send_data.data(), send_data.size(), 0, (struct sockaddr *)&(udp_socket::m_addrinfo), sizeof(udp_socket::m_addrinfo));

    // 送信結果判定
    if( _sendto_result < 0) {
        // 異常終了
        perror("sendto");
        return false;
    }

    // 正常終了
    return true;
}

//-----------------------------------------------------------------------------
// バインド
//-----------------------------------------------------------------------------
bool udp_socket::udp_bind(std::string address, int port)
{
    struct sockaddr_in _addrinfo;   // bindアドレス情報

    // bindアドレス情報生成
    _addrinfo.sin_family = AF_INET;
    _addrinfo.sin_port = htons(port);
    _addrinfo.sin_addr.s_addr = inet_addr(address.c_str());

    // バインドする
    int _bind_result = bind(udp_socket::m_sockfd, (struct sockaddr *)&(_addrinfo), sizeof(_addrinfo));

    // バインド結果判定
    if( _bind_result < 0) {
        // 異常終了
        perror("bind");
        return false;
    }

    // 正常終了
    return true;
}

//-----------------------------------------------------------------------------
// 受信
//-----------------------------------------------------------------------------
bool udp_socket::recv(binary& recv_data)
{
    u_char _recv_buffer[udp_socket::m_capacity];    // 受信バッファ
    struct sockaddr_in _from_addr;                  // 送信元アドレス情報
    socklen_t _sin_size;                            // 送信元アドレス情報サイズ

    // 受信バッファの初期化
    memset(_recv_buffer, 0, sizeof(_recv_buffer));

    // 受信パケットが到着するまでブロック
    int _recvfrom_result = recvfrom(udp_socket::m_sockfd, _recv_buffer, sizeof(_recv_buffer), 0,(struct sockaddr *)&_from_addr, &_sin_size);

    // 受信結果判定
    if( _recvfrom_result < 0) {
        // 異常終了
        perror("recvfrom");
        return false;
    }

    // 受信データ設定
    recv_data.add( _recv_buffer, _recvfrom_result );

    // 正常終了
    return true;
}
