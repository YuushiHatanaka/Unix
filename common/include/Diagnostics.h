//==============================================================================
// Diagnosticsクラスヘッダ
//==============================================================================
#ifndef _DIAGNOSTICS_                       // 二重インクルード防止
#define _DIAGNOSTICS_                       // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "DateTime.h"
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
private:
    //--------------------------------------------------------------------------
    // 排他制御用オブジェクト
    //--------------------------------------------------------------------------
    static pthread_mutex_t m_mutex;

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
        // 初期化
        pthread_mutex_init(&(Diagnostics::m_mutex), NULL);

        // 排他設定
        Diagnostics::Lock();

        // open
        Diagnostics::DiagnosticsStream.open(logName);

        // 排他解除
        Diagnostics::Unlock();

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
        // 排他設定
        Diagnostics::Lock();

        // Flush
        Diagnostics::DiagnosticsStream.flush();

        // Close
        Diagnostics::DiagnosticsStream.close();

        // 排他解除
        Diagnostics::Unlock();
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
        // 自動Flush判定
        if(autoFlush)
        {
            // 排他設定
            Diagnostics::Lock();

            // Flush
            Diagnostics::DiagnosticsStream.flush();

            // 排他解除
            Diagnostics::Unlock();
        }
#endif
    }

    //--------------------------------------------------------------------------
    // 排他設定
    //--------------------------------------------------------------------------
    static bool Lock()
    {
        // mutex設定
        if(pthread_mutex_lock(&(Diagnostics::m_mutex)) != 0)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 排他解除
    //--------------------------------------------------------------------------
    static bool Unlock()
    {
        // mutex解除
        if(pthread_mutex_unlock(&(Diagnostics::m_mutex)) != 0)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

#if _DEBUG_
    //--------------------------------------------------------------------------
    // メッセージ出力
    //--------------------------------------------------------------------------
    static void Write(const char* tag, const char* format, va_list arg)
    {
        // 排他設定
        Diagnostics::Lock();

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << DateTime().Now().ToString("%Y/%m/%d %H:%M:%S.%L");

        // タグ設定
        if(tag != NULL)
        {
            Diagnostics::DiagnosticsStream << " " << tag << " ";
        }

        // メッセージ生成
        char _buffer[DUMP_MESSAGE_MAX+1];
        memset( _buffer, 0x00, sizeof(_buffer) );
        vsnprintf(_buffer, sizeof(_buffer), format, arg);

        // バッファ書込み
        Diagnostics::DiagnosticsStream << _buffer;

        // バッファフラッシュ
        Diagnostics::Flush();

        // 排他解除
        Diagnostics::Unlock();
    }

    //--------------------------------------------------------------------------
    // メッセージ出力
    //--------------------------------------------------------------------------
    static void WriteLine(const char* tag, const char* format, va_list arg)
    {
        // 排他設定
        Diagnostics::Lock();

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << DateTime().Now().ToString("%Y/%m/%d %H:%M:%S.%L");

        // タグ設定
        Diagnostics::DiagnosticsStream << " ";
        if(tag != NULL)
        {
            Diagnostics::DiagnosticsStream << tag << " ";
        }

        // メッセージ生成
        char _buffer[DUMP_MESSAGE_MAX+1];
        memset( _buffer, 0x00, sizeof(_buffer) );
        vsnprintf(_buffer, sizeof(_buffer), format, arg);

        // バッファ書込み
        Diagnostics::DiagnosticsStream << _buffer << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();

        // 排他解除
        Diagnostics::Unlock();
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
#if _TRACE_
        // 条件判定
        if(condition)
        {
            return;
        }

        // 排他設定
        Diagnostics::Lock();

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << DateTime().Now().ToString("%Y/%m/%d %H:%M:%S.%L") << " [Trace][Assert]" << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();

        // 排他解除
        Diagnostics::Unlock();
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Assert
    // 　条件をチェックします。
    //--------------------------------------------------------------------------
    static void Assert(bool condition, std::string message)
    {
#if _TRACE_
        // 条件判定
        if(condition)
        {
            return;
        }

        // 排他設定
        Diagnostics::Lock();

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << DateTime().Now().ToString("%Y/%m/%d %H:%M:%S.%L") << " [Trace][Assert] " << message << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();

        // 排他解除
        Diagnostics::Unlock();
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Assert
    // 　条件をチェックします。
    //--------------------------------------------------------------------------
    static void Assert(bool condition, std::string message, std::string detailMessage)
    {
#if _TRACE_
        // 条件判定
        if(condition)
        {
            return;
        }

        // 排他設定
        Diagnostics::Lock();

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << DateTime().Now().ToString("%Y/%m/%d %H:%M:%S.%L") << " [Trace][Assert] " << message << "\n" << detailMessage << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();

        // 排他解除
        Diagnostics::Unlock();
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Abort
    // 　指定されたエラー メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Abort(std::string format, ...)
    {
#if _TRACE_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace][Abort]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Panic
    // 　指定されたエラー メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Panic(std::string format, ...)
    {
#if _TRACE_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace][Panic]", format.c_str(), ap);
        va_end(ap);
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Fail
    // 　指定されたエラー メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Fail(std::string format, ...)
    {
#if _TRACE_
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
#if _TRACE_
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
#if _TRACE_
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace][Information]", format.c_str(), ap);
        va_end(ap);
#endif
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Warning
    // 　警告メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Warning(std::string format, ...)
    {
#if _TRACE_
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace][Warning]", format.c_str(), ap);
        va_end(ap);
#endif
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - Write
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void Write(std::string format, ...)
    {
#if _TRACE_
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::Write("[Trace]", format.c_str(), ap);
        va_end(ap);
#endif
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - WriteIf
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void WriteIf(bool condition, std::string format, ...)
    {
#if _TRACE_
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
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - WriteLine
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void WriteLine(std::string format, ...)
    {
#if _TRACE_
#if _DEBUG_
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        Diagnostics::WriteLine("[Trace]", format.c_str(), ap);
        va_end(ap);
#endif
#endif
    }
    //--------------------------------------------------------------------------
    // Trace - WriteLineIf
    // 　メッセージを出力します。
    //--------------------------------------------------------------------------
    static void WriteLineIf(bool condition, std::string format, ...)
    {
#if _TRACE_
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
#endif
    }

    //-------------------------------------------------------------------------
    // Trace - Dump
    //-------------------------------------------------------------------------
    static void Dump(const char* message, const void *addr, size_t bytes)
    {
#if _TRACE_
#if _DEBUG_
        std::stringstream _logmsg;    // ログメッセージ

        // ログ内容
        const unsigned char* p = (const unsigned char*)addr;
        char text[DUMP_STRING_WIDHT+1];
        unsigned i = 0;
        unsigned _top_addrress = (uintptr_t)p;
        unsigned _current_addrress;
        unsigned int _current_data[DUMP_STRING_WIDHT+1];
        unsigned int _old_data[DUMP_STRING_WIDHT+1];
        bool _display_flag = false;

        // 初期化
        memset(text, 0x00, sizeof(text));
        memset(_current_data, 0x00, sizeof(_current_data));
        memset(_old_data, 0x00, sizeof(_old_data));

        // ログヘッダ
        _logmsg << message;
        _logmsg << " 《adderss:0x" << std::hex << std::setw(8) << std::right << std::setfill('0') << _top_addrress << " - ";
        _logmsg << "size:" << std::dec << bytes << "》" << "\n";
        _logmsg << "Address    : +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F : ASCII\n";

        while (i < bytes)
        {
            // アドレス設定
            if ((i % DUMP_STRING_WIDHT) == 0)
            {
                _current_addrress = i;
            }

            // データ設定
            _current_data[i % DUMP_STRING_WIDHT] = (unsigned int)*p;
            text[i % DUMP_STRING_WIDHT] = isprint(*p) ? *p : '.';
            p++;
            i++;

            // 出力
            if ((i % DUMP_STRING_WIDHT) == 0)
            {
                // 先頭は無条件出力
                if(i==DUMP_STRING_WIDHT)
                {
                    _logmsg << "0x" << std::hex << std::setw(8) << std::right << std::setfill('0') << _current_addrress << " : ";
                    for(unsigned j=0; j<DUMP_STRING_WIDHT; j++)
                    {
                        _logmsg << std::hex << std::setw(2) << std::right << std::setfill('0') << _current_data[j] << " ";
                    }
                    _logmsg << ": " << text << "\n";
                }
                else
                {
                    // 新旧データを比較
                    if(memcmp(_current_data, _old_data, sizeof(_current_data)) != 0)
                    {
                        // 違いがある場合
                        _logmsg << "0x" << std::hex << std::setw(8) << std::right << std::setfill('0') << _current_addrress << " : ";
                        for(unsigned j=0; j<DUMP_STRING_WIDHT; j++)
                        {
                            _logmsg << std::hex << std::setw(2) << std::right << std::setfill('0') << _current_data[j] << " ";
                        }
                        _logmsg << ": " << text << "\n";
                        _display_flag = false;
                    }
                    else
                    {
                        // 違いがない場合
                        if(!_display_flag)
                        {
                            _logmsg << "0x" << std::hex << std::setw(8) << std::right << std::setfill('0') << _current_addrress << "*: ";
                            for(unsigned j=0; j<DUMP_STRING_WIDHT; j++)
                            {
                                _logmsg << std::hex << std::setw(2) << std::right << std::setfill('0') << _current_data[j] << " ";
                            }
                            _logmsg << ": " << text << "\n";
                            _display_flag = true;
                        }
                    }
                }

                // バッファクリア
                memset(text, '\0', sizeof(text));
                memcpy(_old_data, _current_data, sizeof(_old_data));
                memset(_current_data, 0x00, sizeof(_current_data));
            }
        }

        // 出力(残り)
        if ((i % DUMP_STRING_WIDHT) != 0)
        {
            _logmsg << "0x" << std::hex << std::setw(8) << std::right << std::setfill('0') << _current_addrress << " : ";
            for(unsigned j=0; j<(i % DUMP_STRING_WIDHT); j++)
            {
                _logmsg << std::hex << std::setw(2) << std::right << std::setfill('0') << _current_data[j] << " ";
            }
            _logmsg << std::setw((DUMP_STRING_WIDHT - (i % DUMP_STRING_WIDHT)) * 3 + 2) << std::setfill(' ');
            _logmsg << ": " << text << "\n";
        }

        // 排他設定
        Diagnostics::Lock();

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << DateTime().Now().ToString("%Y/%m/%d %H:%M:%S.%L") << " [Trace][Dump] " << _logmsg.str();

        // 排他解除
        Diagnostics::Unlock();
#endif
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

        // 排他設定
        Diagnostics::Lock();

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << DateTime().Now().ToString("%Y/%m/%d %H:%M:%S.%L") << " [Debug][Assert]" << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();

        // 排他解除
        Diagnostics::Unlock();
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

        // 排他設定
        Diagnostics::Lock();

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << DateTime().Now().ToString("%Y/%m/%d %H:%M:%S.%L") << " [Debug][Assert] " << message << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();

        // 排他解除
        Diagnostics::Unlock();
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

        // 排他設定
        Diagnostics::Lock();

        // 現在日時文字列取得
        Diagnostics::DiagnosticsStream << DateTime().Now().ToString("%Y/%m/%d %H:%M:%S.%L") << " [Debug][Assert] " << message << "\n" << detailMessage << "\n";

        // バッファフラッシュ
        Diagnostics::Flush();

        // 排他解除
        Diagnostics::Unlock();
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
pthread_mutex_t Diagnostics::m_mutex;
#endif                                      // 二重インクルード防止
