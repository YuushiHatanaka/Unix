//==============================================================================
// SQLite3クラスヘッダ
//==============================================================================
#ifndef _SQLITE3_CLASS_H_                   // 二重インクルード防止
#define _SQLITE3_CLASS_H_                   // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Object.h"
#include "Exception.h"
#include "RegularExpression.h"
#include <list>
#include <sqlite3.h>

//------------------------------------------------------------------------------
// コールバック関数
//------------------------------------------------------------------------------
                                            // SELECTコールバック関数
typedef int (*SelectCallbak)(void*,int,char**,char**);

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// SQLite3例外クラス
//------------------------------------------------------------------------------
class SQLite3Exception : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    SQLite3Exception(std::string format, ...)
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
// SQLite3クラス
//------------------------------------------------------------------------------
class SQLite3 : public Object
{
private:
    sqlite3* m_connection;                  // SQLite3接続子
    sqlite3_stmt* m_stmt;                   // ステートメントオブジェクト
    char* m_err_msg;                        // エラーメッセージ
    std::list<std::string> m_err_msg_list;  // エラーメッセージリスト

public:
    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    SQLite3() : Object()
    {
        // 初期化
        this->m_connection = NULL;
        this->m_stmt = NULL;
        this->m_err_msg = NULL;
    }

    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    SQLite3(const SQLite3& obj) : Object(obj)
    {
        // コピー
        this->m_connection = obj.m_connection;
        this->m_stmt = obj.m_stmt;
        this->m_err_msg = obj.m_err_msg;
    }

    //-------------------------------------------------------------------------
    // デストラクタ
    //-------------------------------------------------------------------------
    ~SQLite3()
    {
        // データベースClose
        this->close();
    }

    //-------------------------------------------------------------------------
    // open
    //-------------------------------------------------------------------------
    bool open(std::string fileName)
    {
        // sqlite3_open
        if(sqlite3_open(fileName.c_str(), &(this->m_connection)) != SQLITE_OK)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //-------------------------------------------------------------------------
    // close
    //-------------------------------------------------------------------------
    bool close()
    {
        // データベース接続子判定
        if(this->m_connection == NULL)
        {
            // 正常終了
            return true;
        }

        // sqlite3_close
        if(sqlite3_close(this->m_connection) != SQLITE_OK)
        {
            // 異常終了
            return false;
        }

        // データベース接続子初期化
        this->m_connection = NULL;

        // 正常終了
        return true;
    }

    //-------------------------------------------------------------------------
    // config
    //-------------------------------------------------------------------------
    bool config(int config)
    {
        // sqlite3_config
        if(sqlite3_config(config) != SQLITE_OK)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //-------------------------------------------------------------------------
    // errmsg
    //-------------------------------------------------------------------------
    std::string errmsg()
    {
        std::stringstream _toErrMsg;        // 文字列化オブジェクト

        // sqlite3_errmsg
        _toErrMsg << sqlite3_errmsg(this->m_connection);

        // 文字列を返却
        return _toErrMsg.str();
    }

    //-------------------------------------------------------------------------
    // free
    //-------------------------------------------------------------------------
    void free()
    {
        // 解放判定
        if(this->m_err_msg != NULL)
        {
            // エラーメッセージ取得
            std::stringstream _errmsg(this->m_err_msg);

            // エラーリスト追加
            this->m_err_msg_list.push_back(_errmsg.str());

            // sqlite3_free
            sqlite3_free(this->m_err_msg);
            this->m_err_msg = NULL;
        }
    }

    //-------------------------------------------------------------------------
    // finalize
    //-------------------------------------------------------------------------
    void finalize()
    {
        // 解放判定
        if(this->m_stmt != NULL)
        {
            // sqlite3_finalize
            sqlite3_finalize(this->m_stmt);
            this->m_stmt = NULL;
        }
    }

    //-------------------------------------------------------------------------
    // exec
    //-------------------------------------------------------------------------
    bool exec(std::string query)
    {
        // exec
        return this->exec(query, NULL, NULL);
    }

    //-------------------------------------------------------------------------
    // exec
    //-------------------------------------------------------------------------
    bool exec(std::string query, SelectCallbak callback, void* arg)
    {
        // sqlite3_exec
        if(sqlite3_exec(this->m_connection, query.c_str(), callback, arg, &(this->m_err_msg)) != SQLITE_OK)
        {
            // エラーメッセージ解放
            this->free();

            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //-------------------------------------------------------------------------
    // prepare
    //-------------------------------------------------------------------------
    bool prepare(std::string query)
    {
        // sqlite3_prepare
        if(sqlite3_prepare(this->m_connection, query.c_str(), -1, &(this->m_stmt), NULL) != SQLITE_OK)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //-------------------------------------------------------------------------
    // prepare_v2
    //-------------------------------------------------------------------------
    bool prepare_v2(std::string query)
    {
        // sqlite3_prepare_v2
        if(sqlite3_prepare_v2(this->m_connection, query.c_str(), -1, &(this->m_stmt), NULL) != SQLITE_OK)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //-------------------------------------------------------------------------
    // column_int
    //-------------------------------------------------------------------------
    int column_int(int pos)
    {
        // sqlite3_column_int
        return sqlite3_column_int(this->m_stmt ,pos);
    }

    //-------------------------------------------------------------------------
    // column_text
    //-------------------------------------------------------------------------
    const u_char* column_text(int pos)
    {
        // sqlite3_column_text
        return sqlite3_column_text(this->m_stmt ,pos);
    }

    //-------------------------------------------------------------------------
    // bind_double
    //-------------------------------------------------------------------------
    bool bind_double(int pos, double value)
    {
        // sqlite3_bind_double
        if(sqlite3_bind_double(this->m_stmt, pos, value) != SQLITE_OK)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //-------------------------------------------------------------------------
    // bind_int
    //-------------------------------------------------------------------------
    bool bind_int(int pos, int value)
    {
        // sqlite3_bind_int
        if(sqlite3_bind_int(this->m_stmt, pos, value) != SQLITE_OK)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //-------------------------------------------------------------------------
    // bind_int64
    //-------------------------------------------------------------------------
    bool bind_int64(int pos, sqlite3_int64 value)
    {
        // sqlite3_bind_int64
        if(sqlite3_bind_int64(this->m_stmt, pos, value) != SQLITE_OK)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //-------------------------------------------------------------------------
    // step
    //-------------------------------------------------------------------------
    bool step()
    {
        // sqlite3_step
        if(sqlite3_step(this->m_stmt) != SQLITE_ROW)
        {
            // 終了
            return false;
        }

        // 継続
        return true;
    }
};
#endif                                      // 二重インクルード防止
