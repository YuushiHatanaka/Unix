//------------------------------------------------------------------------------
// HTTPサーバクラスヘッダ
//------------------------------------------------------------------------------
#ifndef _HTTP_SERVER_H_                     // 二重インクルード防止
#define _HTTP_SERVER_H_                     // 二重インクルード防止
//------------------------------------------------------------------------------
// インクルード
//------------------------------------------------------------------------------
#include "TcpServer.hpp"
//------------------------------------------------------------------------------
// クラス定義
//------------------------------------------------------------------------------
class HttpServer : public TcpServer
{
};
#endif                                      // 二重インクルード防止
