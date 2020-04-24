//==============================================================================
// Diagnosticsクラスヘッダ
//==============================================================================
#ifndef _DIAGNOSTICS_                       // 二重インクルード防止
#define _DIAGNOSTICS_                       // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include <sys/time.h>
#include <fstream>
#include <iomanip>

//==============================================================================
// 定数定義
//==============================================================================
#define DUMP_STRING_WIDHT (16)              // ダンプ文字列長
#define DUMP_MESSAGE_MAX  (8192)            // メッセージ文字列長

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// Diagnostics例外クラス
//------------------------------------------------------------------------------
class DiagnosticsException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    DiagnosticsException(std::string format, ...)
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
// Diagnosticsクラス
//------------------------------------------------------------------------------
class Diagnostics
{
public:
    //--------------------------------------------------------------------------
    // 出力用Stream
    //--------------------------------------------------------------------------
    static std::ofstream DiagnosticsStream;

    //--------------------------------------------------------------------------
    // AutoFlush
    //--------------------------------------------------------------------------
    static bool AutoFlush;

public:
    //--------------------------------------------------------------------------
    // Diagnostics - Open
    //--------------------------------------------------------------------------
    static void Open(std::string logName)
    {
#if _DEBUG_
        // open
        Diagnostics::DiagnosticsStream.open(logName);

        // エラー発生か？
        if(Diagnostics::DiagnosticsStream.fail())
        {
            // 例外
            throw DiagnosticsException("%s:[%s]\n",strerror(errno), logName.c_str());
        }
#endif
    }

    //--------------------------------------------------------------------------
    // Diagnostics - Close
    // 　出力バッファーをフラッシュし、Diagnostics.Trace.Listeners を閉じます。
    //--------------------------------------------------------------------------
    static void Close()
    {
#if _DEBUG_
        // Flush
        Diagnostics::DiagnosticsStream.flush();

        // Close
        Diagnostics::DiagnosticsStream.close();
#endif
    }
    //--------------------------------------------------------------------------
    // Diagnostics - Flush
    // 　出力バッファーをフラッシュします。
    //--------------------------------------------------------------------------
    static void Flush()
    {
#if _DEBUG_
        // Flush
        Diagnostics::Flush(Diagnostics::AutoFlush);
#endif
    }
    //--------------------------------------------------------------------------
    // Diagnostics - Flush
    // 　出力バッファーをフラッシュします。
    //--------------------------------------------------------------------------
    static void Flush(bool autoFlush)
    {
#if _DEBUG_
        // Flush
        if(autoFlush)
        {
            Diagnostics::DiagnosticsStream.flush();
        }
#endif
    }
#if _DEBUG_
    //-------------------------------------------------------------------------
    // 現在日時文字列取得
    //-------------------------------------------------------------------------
    static std::string get_datetime_string()
    {
        std::string _now_datetime = ""; // 現在日時文字列
        struct timeval _timeval;        // 現在日時
        struct tm _tm;                  // 現在日時
        char _now_datetime_str[32+1];   // 現在日時文字列

        // 初期化
        memset(_now_datetime_str, 0x00, sizeof(_now_datetime_str));
        memset(&_timeval, 0x00, sizeof(_timeval));

        // 現在日時取得
        gettimeofday( &_timeval, NULL );
        localtime_r( &_timeval.tv_sec , &_tm);

        // 現在日時を文字列に変換
        strftime( _now_datetime_str, sizeof(_now_datetime_str), "%Y/%m/%d %H:%M:%S", &_tm );
        _now_datetime += std::string(_now_datetime_str);
        memset(_now_datetime_str, 0x00, sizeof(_now_datetime_str));
        snprintf( _now_datetime_str, sizeof(_now_datetime_str), ".%03d", (int)(_timeval.tv_usec/1000) );
        _now_datetime += std::string(_now_datetime_str);

        // 現在日時文字列を返却
        return _now_datetime;
    }

    //--------------------------------------------------------------------------
    // メッセージ出力
    //--------------------------------------------------------------------------
    static void Write(const char* tag, const char* format, va_list arg)
    {
        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << Diagnostics::get_datetime_string();

        // タグ設定
        Diagnostics::DiagnosticsStream << " " << tag << " ";

        // メッセージ生成
        char _buffer[DUMP_MESSAGE_MAX+1];
        memset( _buffer, 0x00, sizeof(_buffer) );
        vsnprintf(_buffer, sizeof(_buffer), format, arg);

        // バッファ書込み
        Diagnostics::DiagnosticsStream << _buffer;

        // バッファフラッシュ
        Diagnostics::Flush();
    }

    //--------------------------------------------------------------------------
    // メッセージ出力
    //--------------------------------------------------------------------------
    static void WriteLine(const char* tag, const char* format, va_list arg)
    {
        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << Diagnostics::get_datetime_string();

        // タグ設定
        Diagnostics::DiagnosticsStream << " " << tag << " ";

        // メッセージ生成
        char _buffer[DUMP_MESSAGE_MAX+1];
        memset( _buffer, 0x00, sizeof(_buffer) );
        vsnprintf(_buffer, sizeof(_buffer), format, arg);

        // バッファ書込み
        Diagnostics::DiagnosticsStream << _buffer << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();
    }
#endif
};

//------------------------------------------------------------------------------
// Traceクラス
//------------------------------------------------------------------------------
class Trace
{
public:
    //--------------------------------------------------------------------------
    // Trace - Assert
    // 　条件をチェックします。
    //--------------------------------------------------------------------------
    static void Assert(bool condition)
    {
#if _DEBUG_
        // 条件判定
        if(condition)
        {
            return;
        }

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << Diagnostics::get_datetime_string() << " [Trace][Assert]" << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Assert
    // 　条件をチェックします。
    //--------------------------------------------------------------------------
    static void Assert(bool condition, std::string message)
    {
#if _DEBUG_
        // 条件判定
        if(condition)
        {
            return;
        }

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << Diagnostics::get_datetime_string() << " [Trace][Assert] " << message << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Assert
    // 　条件をチェックします。
    //--------------------------------------------------------------------------
    static void Assert(bool condition, std::string message, std::string detailMessage)
    {
#if _DEBUG_
        // 条件判定
        if(condition)
        {
            return;
        }

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << Diagnostics::get_datetime_string() << " [Trace][Assert] " << message << "\n" << detailMessage << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Fail
    // 　指定されたエラー メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Fail(std::string format, ...)
    {
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace][Fail]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Error
    // 　エラーメッセージを出力します。
    //--------------------------------------------------------------------------
    static void Error(std::string format, ...)
    {
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace][Error]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Information
    // 　情報メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Information(std::string format, ...)
    {
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace][Information]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Warning
    // 　警告メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Warning(std::string format, ...)
    {
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace][Warning]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Write
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Write(std::string format, ...)
    {
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::Write("[Trace]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - WriteIf
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void WriteIf(bool condition, std::string format, ...)
    {
#if _DEBUG_
        // 条件判定
        if(!condition)
        {
            return;
        }

        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::Write("[Trace]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - WriteLine
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void WriteLine(std::string format, ...)
    {
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - WriteLineIf
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void WriteLineIf(bool condition, std::string format, ...)
    {
#if _DEBUG_
        // 条件判定
        if(!condition)
        {
            return;
        }

        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace]", format.c_str(), ap);
        va_end(ap);
#endif
    }

    //-------------------------------------------------------------------------
    // Trace - Dump
    //-------------------------------------------------------------------------
    static void Dump(const char* message, const void *addr, size_t bytes)
    {
#if _DEBUG_
        std::stringstream _logmsg;    // ログメッセージ
        // ログヘッダ
        _logmsg << message << " - size:" << std::dec << bytes << " -" << "\n";

        // ログ内容
        const unsigned char* p = (const unsigned char*)addr;
        char text[DUMP_STRING_WIDHT+1];
        unsigned i = 0;
        unsigned _addrress;
        unsigned int _data;
        while (i < bytes)
        {
            // アドレス出力
            if ((i % DUMP_STRING_WIDHT) == 0)
            {
                _addrress = (unsigned int)p;
                _logmsg << "0x" << std::hex << std::setw(8) << std::right << std::setfill('0') <<  _addrress << ": ";
                memset(text, '\0', sizeof(text));
            }
            _data = (unsigned int)*p;
            _logmsg << std::hex << std::setw(2) << std::right << std::setfill('0') << _data << " ";
            text[i % DUMP_STRING_WIDHT] = isprint(*p) ? *p : '.';
            p++;
            i++;
            // テキスト部分出力
            if ((i % DUMP_STRING_WIDHT) == 0)
            {
                _logmsg << ": " << text << "\n";
            }
        }
        if ((i % DUMP_STRING_WIDHT) != 0)
        {
            _logmsg << std::setw((DUMP_STRING_WIDHT - (i % DUMP_STRING_WIDHT)) * 3 + 2) << std::setfill(' ');
            _logmsg << ": " << text << "\n";
        }

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << Diagnostics::get_datetime_string() << " [Trace][Dump] " << _logmsg.str();
#endif
    }
};

//------------------------------------------------------------------------------
// Debugクラス
//------------------------------------------------------------------------------
class Debug
{
public:
    //--------------------------------------------------------------------------
    // Trace - Assert
    // 　条件をチェックします。
    //--------------------------------------------------------------------------
    static void Assert(bool condition)
    {
#if _DEBUG_
        // 条件判定
        if(condition)
        {
            return;
        }

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << Diagnostics::get_datetime_string() << " [Debug][Assert]" << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Assert
    // 　条件をチェックします。
    //--------------------------------------------------------------------------
    static void Assert(bool condition, std::string message)
    {
#if _DEBUG_
        // 条件判定
        if(condition)
        {
            return;
        }

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << Diagnostics::get_datetime_string() << " [Debug][Assert] " << message << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Assert
    // 　条件をチェックします。
    //--------------------------------------------------------------------------
    static void Assert(bool condition, std::string message, std::string detailMessage)
    {
#if _DEBUG_
        // 条件判定
        if(condition)
        {
            return;
        }

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << Diagnostics::get_datetime_string() << " [Debug][Assert] " << message << "\n" << detailMessage << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();
#endif
    }
    //--------------------------------------------------------------------------
    // Debug - Fail
    // 　指定されたエラー メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Fail(std::string format, ...)
    {
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Debug][Fail]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Debug - Print
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Print(std::string format, ...)
    {
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Debug][Print]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Debug - Write
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Write(std::string format, ...)
    {
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::Write("[Debug]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Debug - WriteIf
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void WriteIf(bool condition, std::string format, ...)
    {
#if _DEBUG_
        // 条件判定
        if(!condition)
        {
            return;
        }

        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::Write("[Debug]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Debug - WriteLine
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void WriteLine(std::string format, ...)
    {
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Debug]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Debug - WriteLineIf
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void WriteLineIf(bool condition, std::string format, ...)
    {
#if _DEBUG_
        // 条件判定
        if(!condition)
        {
            return;
        }

        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Debug]", format.c_str(), ap);
        va_end(ap);
#endif
    }
};

//------------------------------------------------------------------------------
// 実体定義
//------------------------------------------------------------------------------
bool Diagnostics::AutoFlush = false;
std::ofstream Diagnostics::DiagnosticsStream;
#endif                                      // 二重インクルード防止
