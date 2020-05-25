//==============================================================================
// Loggingクラスヘッダ
//==============================================================================
#ifndef _LOGGING_H_                         // 二重インクルード防止
#define _LOGGING_H_                         // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "DateTime.h"
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stdarg.h>

//------------------------------------------------------------------------------
// マクロ定義
//------------------------------------------------------------------------------
#define LoggingInfo(obj, fmt, ...)        (*obj).Info(__FILE__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LoggingPanic(obj, fmt, ...)       (*obj).Panic(__FILE__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LoggingAbort(obj, fmt, ...)       (*obj).Abort(__FILE__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LoggingWarning(obj, fmt, ...)     (*obj).Warning(__FILE__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LoggingError(obj, fmt, ...)       (*obj).Error(__FILE__, __LINE__, __func__, fmt, ## __VA_ARGS__)
#define LoggingDump(obj, msg, addr, size) (*obj).Dump(__FILE__, __LINE__, __func__, msg, addr, size)
#define TraceStart(obj, fmt, ...)         (*obj).Trace(__FILE__, __LINE__, __func__, "=>>>>", fmt, ## __VA_ARGS__)
#define TraceEnd(obj, fmt, ...)           (*obj).Trace(__FILE__, __LINE__, __func__, "<<<<=", fmt, ## __VA_ARGS__)

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// Logging例外クラス
//------------------------------------------------------------------------------
class LoggingException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LoggingException(std::string format, ...)
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
// Loggingクラス
//------------------------------------------------------------------------------
class Logging
{
private :
    std::string m_name;                     // ログファイル名
    int m_dump_width;                       // ダンプ最大文字列長
    int m_message_max;                      // メッセージ最大長
    std::ofstream* m_log_stream;            // ログストリームオブジェクト
    bool m_dump;                            // DUMP出力可否
    bool m_info;                            // INFO出力可否
    bool m_panic;                           // PANIC出力可否
    bool m_abort;                           // ABORT出力可否
    bool m_warning;                         // WARNING出力可否
    bool m_error;                           // ERROR出力可否
    bool m_trace;                           // TRACE出力可否

private :
    //-------------------------------------------------------------------------
    // ログレコード情報取得
    //-------------------------------------------------------------------------
    std::string get_log_topinfo(std::string filename, int fileline, std::string type, std::string sted, std::string funcname)
    {
        DateTime _dateTime;           // 時刻取得用
        std::stringstream _logmsg;    // ログメッセージ
        int _thread_id;               // スレッドID

        // ログレコードを作成
        _thread_id = (int)pthread_self();
        _logmsg << _dateTime.Now().ToString("%Y/%m/%d %H:%M:%S.%L") << " ";
        _logmsg << std::setw(32) << std::left << filename.substr(0,32) << ":";
        _logmsg << "(" << std::dec << std::setw(8) << std::right << fileline << ")";
        _logmsg << "[0x" << std::hex << std::setw(8) << std::right << _thread_id << "] ";
        _logmsg << "[" << std::left << std::setw(8) << type << "] ";
        _logmsg << " " << sted << " " << funcname << " ";

        // ログレコードを返却する
        return _logmsg.str();
    }

    //-------------------------------------------------------------------------
    // ログレコード情報取得
    //-------------------------------------------------------------------------
    std::string get_log_topinfo(std::string filename, int fileline, std::string type, std::string funcname)
    {
        DateTime _dateTime;           // 時刻取得用
        std::stringstream _logmsg;    // ログメッセージ
        int _thread_id;               // スレッドID

        // ログレコードを作成
        _thread_id = (int)pthread_self();
        _logmsg << _dateTime.Now().ToString("%Y/%m/%d %H:%M:%S.%L") << " ";
        _logmsg << std::setw(32) << std::left << filename.substr(0,32) << ":";
        _logmsg << "(" << std::dec << std::setw(8) << std::right << fileline << ")";
        _logmsg << "[0x" << std::hex << std::setw(8) << std::right << _thread_id << "] ";
        _logmsg << "[" << std::left << std::setw(8) << type << "] ";
        _logmsg << "[" << funcname << "] ";

        // ログレコードを返却する
        return _logmsg.str();
    }

public:
    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    Logging(std::string name)
    {
        // 初期化
        this->m_name = name;
        this->m_dump_width = 16;
        this->m_message_max = 8192;
        this->m_log_stream = new std::ofstream();
        this->m_log_stream->open(name, std::ios::out);
        this->m_dump = true;
        this->m_info = true;
        this->m_panic = true;
        this->m_abort = true;
        this->m_warning = true;
        this->m_error = true;
        this->m_trace = true;
    }

    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    Logging(const Logging& logging)
    {
        // 初期化
        this->m_name = logging.m_name;
        this->m_dump_width = logging.m_dump_width;
        this->m_message_max = logging.m_message_max;
        this->m_log_stream = logging.m_log_stream;
        this->m_dump = logging.m_dump;
        this->m_info = logging.m_info;
        this->m_panic = logging.m_panic;
        this->m_abort = logging.m_abort;
        this->m_warning = logging.m_warning;
        this->m_error = logging.m_error;
        this->m_trace = logging.m_trace;
    }

    //-------------------------------------------------------------------------
    // デストラクタ
    //-------------------------------------------------------------------------
    virtual ~Logging()
    {
        // 破棄
        this->m_log_stream->close();
        delete this->m_log_stream;
    }

    //-------------------------------------------------------------------------
    // 出力オプション設定
    //-------------------------------------------------------------------------
    void SetOption(bool dump, bool info, bool panic, bool abort, bool warning, bool error, bool trace)
    {
        // 設定
        this->m_dump = dump;
        this->m_info = info;
        this->m_panic = panic;
        this->m_abort = abort;
        this->m_warning = warning;
        this->m_error = error;
        this->m_trace = trace;
    }

    //-------------------------------------------------------------------------
    // メモリダンプ[DUMP]
    //-------------------------------------------------------------------------
    void Dump(std::string filename,int fileline,std::string funcname, const char* message, const void *addr, size_t bytes)
    {
        // 出力判定
        if(!this->m_dump)
        {
            // 何もしない
            return;
        }

        std::stringstream _logmsg;    // ログメッセージ

        // ログメッセージ
        _logmsg << this->get_log_topinfo(filename, fileline, "DUMP", funcname);

        // ログヘッダ
        _logmsg << message << " - size:" << std::dec << bytes << " -" << std::endl;

        // ログ内容
        const unsigned char* p = (const unsigned char*)addr;
        char text[this->m_dump_width+1];
        unsigned i = 0;
        unsigned _addrress;
        unsigned int _data;
        while (i < bytes)
        {
            // アドレス出力
            if ((i % this->m_dump_width) == 0)
            {
                _addrress = (unsigned int)p;
                _logmsg << "0x" << std::hex << std::setw(8) << std::right << std::setfill('0') <<  _addrress << ": ";
                memset(text, '\0', sizeof(text));
            }
            _data = (unsigned int)*p;
            _logmsg << std::hex << std::setw(2) << std::right << std::setfill('0') << _data << " ";
            text[i % this->m_dump_width] = isprint(*p) ? *p : '.';
            p++;
            i++;
            // テキスト部分出力
            if ((i % this->m_dump_width) == 0)
            {
                _logmsg << ": " << text << std::endl;
            }
        }
        if ((i % this->m_dump_width) != 0)
        {
            _logmsg << std::setw((this->m_dump_width - (i % this->m_dump_width)) * 3 + 2) << std::setfill(' ');
            _logmsg << ": " << text << std::endl;
        }

        // 改行付与
        _logmsg << "\n";

        // 出力
        *(this->m_log_stream) << _logmsg.str();
        this->m_log_stream->flush();
    }

    //-------------------------------------------------------------------------
    // デバッグ出力[INFO]
    //-------------------------------------------------------------------------
    void Info(std::string filename,int fileline,std::string funcname, const char* format,...)
    {
        // 出力判定
        if(!this->m_info)
        {
            // 何もしない
            return;
        }

        std::stringstream _logmsg;    // ログメッセージ

        // ログメッセージ
        _logmsg << this->get_log_topinfo(filename, fileline, "INFO", funcname);

        // メッセージを付与
        va_list ap;
        char _buffer[this->m_message_max+1];
        va_start( ap, format );
        memset( _buffer, 0x00, sizeof(_buffer) );
        vsnprintf( _buffer, sizeof(_buffer), format, ap );
        _logmsg << _buffer;

        // 改行付与
        _logmsg << "\n";

        // 出力
        *(this->m_log_stream) << _logmsg.str();
        this->m_log_stream->flush();
    }

    //-------------------------------------------------------------------------
    // デバッグエラー出力[PANIC]
    //-------------------------------------------------------------------------
    void Panic(std::string filename,int fileline,std::string funcname, const char* format,...)
    {
        // 出力判定
        if(!this->m_panic)
        {
            // 何もしない
            return;
        }

        std::stringstream _logmsg;    // ログメッセージ

        // ログメッセージ
        _logmsg << this->get_log_topinfo(filename, fileline, "PANIC", funcname);

        // メッセージを付与
        va_list ap;
        char _buffer[this->m_message_max+1];
        va_start( ap, format );
        memset( _buffer, 0x00, sizeof(_buffer) );
        vsnprintf( _buffer, sizeof(_buffer), format, ap );
        _logmsg << _buffer;

        // 改行付与
        _logmsg << "\n";

        // 出力
        *(this->m_log_stream) << _logmsg.str();
        this->m_log_stream->flush();
    }

    //-------------------------------------------------------------------------
    // デバッグエラー出力[ABORT]
    //-------------------------------------------------------------------------
    void Abort(std::string filename,int fileline,std::string funcname, const char* format,...)
    {
        // 出力判定
        if(!this->m_abort)
        {
            // 何もしない
            return;
        }

        std::stringstream _logmsg;    // ログメッセージ

        // ログメッセージ
        _logmsg << this->get_log_topinfo(filename, fileline, "ABORT", funcname);

        // メッセージを付与
        va_list ap;
        char _buffer[this->m_message_max+1];
        va_start( ap, format );
        memset( _buffer, 0x00, sizeof(_buffer) );
        vsnprintf( _buffer, sizeof(_buffer), format, ap );
        _logmsg << _buffer;

        // 改行付与
        _logmsg << "\n";

        // 出力
        *(this->m_log_stream) << _logmsg.str();
        this->m_log_stream->flush();
    }

    //-------------------------------------------------------------------------
    // デバッグエラー出力[WARNING]
    //-------------------------------------------------------------------------
    void Warning(std::string filename,int fileline,std::string funcname, const char* format,...)
    {
        // 出力判定
        if(!this->m_warning)
        {
            // 何もしない
            return;
        }

        std::stringstream _logmsg;    // ログメッセージ

        // ログメッセージ
        _logmsg << this->get_log_topinfo(filename, fileline, "WARNING", funcname);

        // メッセージを付与
        va_list ap;
        char _buffer[this->m_message_max+1];
        va_start( ap, format );
        memset( _buffer, 0x00, sizeof(_buffer) );
        vsnprintf( _buffer, sizeof(_buffer), format, ap );
        _logmsg << _buffer;

        // 改行付与
        _logmsg << "\n";

        // 出力
        *(this->m_log_stream) << _logmsg.str();
        this->m_log_stream->flush();
    }

    //-------------------------------------------------------------------------
    // デバッグエラー出力[ERROR]
    //-------------------------------------------------------------------------
    void Error(std::string filename,int fileline,std::string funcname, const char* format,...)
    {
        // 出力判定
        if(!this->m_error)
        {
            // 何もしない
            return;
        }

        std::stringstream _logmsg;    // ログメッセージ

        // ログメッセージ
        _logmsg << this->get_log_topinfo(filename, fileline, "ERROR", funcname);

        // メッセージを付与
        va_list ap;
        char _buffer[this->m_message_max+1];
        va_start( ap, format );
        memset( _buffer, 0x00, sizeof(_buffer) );
        vsnprintf( _buffer, sizeof(_buffer), format, ap );
        _logmsg << _buffer;

        // 改行付与
        _logmsg << "\n";

        // 出力
        *(this->m_log_stream) << _logmsg.str();
        this->m_log_stream->flush();
    }

    //-------------------------------------------------------------------------
    // トレース出力[TRACE]
    //-------------------------------------------------------------------------
    void Trace(std::string filename,int fileline,std::string funcname, const char* type, const char* format,...)
    {
        // 出力判定
        if(!this->m_trace)
        {
            // 何もしない
            return;
        }

        std::stringstream _logmsg;    // ログメッセージ

        // ログメッセージ
        _logmsg << this->get_log_topinfo(filename, fileline, "TRACE", type, funcname);

        // メッセージを付与
        va_list ap;
        char _buffer[this->m_message_max+1];
        va_start( ap, format );
        memset( _buffer, 0x00, sizeof(_buffer) );
        vsnprintf( _buffer, sizeof(_buffer), format, ap );
        _logmsg << _buffer;

        // 改行付与
        _logmsg << "\n";

        // 出力
        *(this->m_log_stream) << _logmsg.str();
        this->m_log_stream->flush();
    }
};
#endif                                      // 二重インクルード防止
