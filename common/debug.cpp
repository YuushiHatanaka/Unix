//-----------------------------------------------------------------------------
// debugモジュール
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include "debug.hpp"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
debug::debug()
{
}
    
//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
debug::~debug()
{
}

//-------------------------------------------------------------------------
// 出力
//-------------------------------------------------------------------------
void debug::Print(const char* format,...)
{
    std::stringstream _message; // ログメッセージ

    // 現在日時文字列取得
    _message << debug::get_datetime_string() << " ";

    // メッセージ生成
    va_list ap;
    char _buffer[DUMP_MESSAGE_MAX+1];
    va_start( ap, format );
    memset( _buffer, 0x00, sizeof(_buffer) );
    vsnprintf( _buffer, sizeof(_buffer), format, ap );

    // メッセージ出力
    std::cout << _message.str();
}

//-------------------------------------------------------------------------
// 出力
//-------------------------------------------------------------------------
void debug::PrintLine(const char* format,...)
{
    std::stringstream _message; // ログメッセージ

    // 現在日時文字列取得
    _message << debug::get_datetime_string() << " ";

    // メッセージ生成
    va_list ap;
    char _buffer[DUMP_MESSAGE_MAX+1];
    va_start( ap, format );
    memset( _buffer, 0x00, sizeof(_buffer) );
    vsnprintf( _buffer, sizeof(_buffer), format, ap );

    // メッセージ追加(改行を含む)
    _message << _buffer << std::endl;

    // メッセージ出力
    std::cout << _message.str();
}

//-------------------------------------------------------------------------
// デバッグ出力(日付／改行なし)
//-------------------------------------------------------------------------
void debug::Write(const char* message)
{
#ifdef _DEBUG
    std::stringstream _message; // ログメッセージ

    // メッセージ追加
    _message << message;

    // メッセージ出力
    std::cout << _message.str();
#endif
}

//-------------------------------------------------------------------------
// デバッグ出力
//-------------------------------------------------------------------------
void debug::WriteLine(const char* format,...)
{
#ifdef _DEBUG
    std::stringstream _message; // ログメッセージ

    // 現在日時文字列取得
    _message << debug::get_datetime_string() << " ";

    // メッセージ生成
    va_list ap;
    char _buffer[DUMP_MESSAGE_MAX+1];
    va_start( ap, format );
    memset( _buffer, 0x00, sizeof(_buffer) );
    vsnprintf( _buffer, sizeof(_buffer), format, ap );

    // メッセージ追加(改行を含む)
    _message << _buffer << std::endl;

    // メッセージ出力
    std::cout << _message.str();
#endif
}

//-------------------------------------------------------------------------
// エラー出力
//-------------------------------------------------------------------------
void debug::Fail(const char* format,...)
{
    std::stringstream _message; // ログメッセージ

    // 現在日時文字列取得
    _message << debug::get_datetime_string() << " [ERR] ";

    // メッセージ生成
    va_list ap;
    char _buffer[DUMP_MESSAGE_MAX+1];
    va_start( ap, format );
    memset( _buffer, 0x00, sizeof(_buffer) );
    vsnprintf( _buffer, sizeof(_buffer), format, ap );

    // メッセージ追加(改行を含む)
    _message << _buffer << std::endl;

    // メッセージ出力
    std::cerr << _message.str();
}

//-------------------------------------------------------------------------
// メモリダンプ
//-------------------------------------------------------------------------
void* debug::Dump(const char* message, const void *addr, size_t bytes)
{
#ifdef _DEBUG
    std::stringstream _message; // ログメッセージ

    // 現在日時文字列取得
    _message << debug::get_datetime_string() << " ";

    // ログヘッダ
    _message << "[DUMP] " << message << " - size:" << std::dec << bytes << " -" << std::endl;

    // ログ内容
    const unsigned char* p = (const unsigned char*)addr;
    char text[DUMP_STRING_WIDHT+1];
    unsigned i = 0;
    unsigned _addrress;
    unsigned int _data;
    while (i < bytes) {
        // アドレス出力
        if ((i % DUMP_STRING_WIDHT) == 0) {
            _addrress = (unsigned int)p;
            _message << "0x" << std::hex << std::setw(8) << std::right << std::setfill('0') <<  _addrress << ": ";
            memset(text, '\0', sizeof(text));
        }
        _data = (unsigned int)*p;
        _message << std::hex << std::setw(2) << std::right << std::setfill('0') << _data << " ";
        text[i % DUMP_STRING_WIDHT] = isprint(*p) ? *p : '.';
        p++;
        i++;
        // テキスト部分出力
        if ((i % DUMP_STRING_WIDHT) == 0) {
            _message << ": " << text << std::endl;
        }
    }
    if ((i % DUMP_STRING_WIDHT) != 0){
        _message << std::setw((DUMP_STRING_WIDHT - (i % DUMP_STRING_WIDHT)) * 3 + 2) << std::setfill(' ');
        _message << ": " << text << std::endl;
    }

    // メッセージ出力
    std::cout << _message.str();
#endif
    return (void*)addr;
}

//-------------------------------------------------------------------------
// 現在日時文字列取得
//-------------------------------------------------------------------------
std::string debug::get_datetime_string()
{
    std::string _now_datetime = ""; // 現在日時文字列
    struct timeval _timeval;        // 現在日時
    struct tm _tm;                  // 現在日時
    char _now_datetime_str[32+1];   // 現在日時文字列

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
