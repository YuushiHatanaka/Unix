//==============================================================================
// DateTimeクラスヘッダ
//==============================================================================
#ifndef _DATETIME_H_                        // 二重インクルード防止
#define _DATETIME_H_                        // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "Object.h"
#include "Binary.h"
#include "TimeSpan.h"
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <iomanip>
#include <sstream>

//==============================================================================
// 定数定義
//==============================================================================
//------------------------------------------------------------------------------
// 日時種別
//------------------------------------------------------------------------------
typedef enum _DateTimeType
{
    DateTimeType_Unknown = 0,               // 不明
    DateTimeType_Timespec,                  // struct timespec
    DateTimeType_Tm,                        // struct tm
    DateTimeType_Timet,                     // time_t
} DateTimeType;

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// DateTime例外クラス
//------------------------------------------------------------------------------
class DateTimeException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    DateTimeException(std::string format, ...)
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
// DateTimeクラス
//------------------------------------------------------------------------------
class DateTime : public Object
{
private:
    struct timespec m_timespec;             // 値(struct timespec)
    struct tm m_tmval;                      // 値(struct tm)
    time_t m_value;                         // 値(time_t)
    std::string m_format;                   // フォーマット(Default)

public:
    struct timespec GetTimesec() const { return this->m_timespec; }
    struct tm GetTmval() const { return this->m_tmval; }
    time_t GetValue() const { return this->m_value; }

private:
    //--------------------------------------------------------------------------
    // 現在時刻設定
    //--------------------------------------------------------------------------
    void SetNow()
    {
        // 現在時刻取得
        clock_gettime(CLOCK_REALTIME, &(this->m_timespec));
/*
        timespec_get(&(this->m_timespec), TIME_UTC);
*/
        // 時刻変換
        this->ConvertTimeVal();
    }

    //--------------------------------------------------------------------------
    // 時刻変換
    //--------------------------------------------------------------------------
    void ConvertTimeVal()
    {
        // 時刻変換
        this->ConvertTimeVal(this->m_timespec);
    }

    //--------------------------------------------------------------------------
    // 時刻変換
    //--------------------------------------------------------------------------
    void ConvertTimeVal(const struct timespec& value)
    {
        // 時刻変換(struct timeval⇒struct tm)
        localtime_r(&(value.tv_sec), &(this->m_tmval));

        // 時刻変換(struct tm⇒time_t)
        this->m_value = mktime(&(this->m_tmval));
    }

    //--------------------------------------------------------------------------
    // 時刻変換
    //--------------------------------------------------------------------------
    void ConvertTimeVal(const struct tm& tmval, int64_t msec)
    {
        // 時刻変換(struct tm⇒time_t)
        struct tm _value = tmval;
        this->m_value = mktime(&_value);

        // 時刻変換(time_t⇒struct timeval)
        this->m_timespec.tv_sec = this->m_value;
        this->m_timespec.tv_nsec = msec * 1000000;
    }

    //--------------------------------------------------------------------------
    // 時刻変換
    //--------------------------------------------------------------------------
    void ConvertTimeVal(const time_t& value, int64_t msec)
    {
        // 時刻変換(time_t⇒struct tm)
        localtime_r(&value, &(this->m_tmval));

        // 時刻変換(time_t⇒struct timeval)
        this->m_timespec.tv_sec = this->m_value;
        this->m_timespec.tv_nsec = msec * 1000000;
    }

    //--------------------------------------------------------------------------
    // 時刻変換
    //--------------------------------------------------------------------------
    void ConvertTimeVal(const TimeSpan& timeSpan)
    {
        // 時刻形式変換
        this->m_timespec.tv_sec = (timeSpan.Days()*24*60*60)+
                                  (timeSpan.Hours()*60*60)+
                                  (timeSpan.Minutes()*60)+
                                   timeSpan.Seconds();
        this->m_timespec.tv_nsec = timeSpan.Milliseconds()*1000000;

        // 時刻変換
        this->ConvertTimeVal();
    }

    //--------------------------------------------------------------------------
    // 加算
    //--------------------------------------------------------------------------
    void Add(const struct timespec& A, const struct timespec& B, struct timespec& C) const
    {
        C.tv_sec  = A.tv_sec  + B.tv_sec;
        C.tv_nsec = A.tv_nsec + B.tv_nsec;
        if(C.tv_nsec>=1000000000){
            C.tv_sec++;
            C.tv_nsec -= 1000000000;
        }
    }

    //--------------------------------------------------------------------------
    // 減算
    //--------------------------------------------------------------------------
    void Sub(const struct timespec& A, const struct timespec& B, struct timespec& C) const
    {
        C.tv_sec  = A.tv_sec  - B.tv_sec;
        C.tv_nsec = A.tv_nsec - B.tv_nsec;
        if(C.tv_nsec<0){
            C.tv_sec--;
            C.tv_nsec += 1000000000;
        }
    }

    //--------------------------------------------------------------------------
    // 文字列取得
    //--------------------------------------------------------------------------
    std::string tostring(std::string fm, const struct timespec& value)
    {
        std::stringstream _stringstream;    // 文字列化Stream
        char _buffer[512+1];                // 文字列変換バッファ
        struct tm _tm;                      // 日時構造体

        // 日時型変換
        localtime_r(&(value.tv_sec), &_tm);

        // 文字列変換
        memset(_buffer, 0x00, sizeof(_buffer));
        strftime(_buffer, sizeof(_buffer), fm.c_str(), &(_tm));
        _stringstream << _buffer;

        // 文字列を返却
        return _stringstream.str();
    }

    //--------------------------------------------------------------------------
    // 文字列取得(%f)
    //--------------------------------------------------------------------------
    std::string ToString_f(const struct timespec& value)
    {
        std::stringstream _stringstream;    // 文字列化Stream

        // 日数計算
        uint64_t _days = value.tv_sec/(24*60*60);

        // 文字列作成
        _stringstream << _days;

        // 文字列を返却する
        return _stringstream.str();
    }

    //--------------------------------------------------------------------------
    // 文字列取得(%L)
    //--------------------------------------------------------------------------
    std::string ToString_L(const struct timespec& value)
    {
        std::stringstream _stringstream;    // 文字列化Stream

        // 文字列作成
        _stringstream << std::setw(3) << std::setfill('0') << (int)(value.tv_nsec/1000000);

        // 文字列を返却する
        return _stringstream.str();
    }

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    DateTime() : Object()
    {
        // 初期設定
        this->m_format = "%Y/%m/%d %H:%M:%S";

        // 現在時刻設定
        this->SetNow();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    DateTime(const struct timespec& value) : Object()
    {
        // 初期設定
        this->m_format = "%Y/%m/%d %H:%M:%S";
        this->m_timespec = value;

        // 時刻変換
        this->ConvertTimeVal();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    DateTime(const struct tm& value) : Object()
    {
        // 初期設定
        this->m_format = "%Y/%m/%d %H:%M:%S";
        this->m_tmval = value;

        // 時刻変換
        this->ConvertTimeVal(value, 0);
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    DateTime(const time_t& value) : Object()
    {
        // 初期設定
        this->m_format = "%Y/%m/%d %H:%M:%S";
        this->m_value = value;

        // 時刻変換
        this->ConvertTimeVal(value);
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    DateTime(const DateTime& datetime) : Object(datetime)
    {
        // コピー
        this->Copy(datetime);
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~DateTime()
    {
    }

    //--------------------------------------------------------------------------
    // getter
    //--------------------------------------------------------------------------
    struct timespec TimeSpec(){ return this->m_timespec;}
    struct tm Tmval(){ return this->m_tmval;}
    time_t Value(){ return this->m_value;}

    //--------------------------------------------------------------------------
    // フォーマット設定
    //--------------------------------------------------------------------------
    void SetFormat(std::string format)
    {
        this->m_format = format;
    }

    //--------------------------------------------------------------------------
    // フォーマット取得
    //--------------------------------------------------------------------------
    std::string GetFormat()
    {
        return this->m_format;
    }

    //--------------------------------------------------------------------------
    // 現在時刻取得
    //--------------------------------------------------------------------------
    DateTime Now()
    {
        DateTime _Now;                      // 現在時刻

        // 現在時刻を返却
        return _Now;
    }

    //--------------------------------------------------------------------------
    // 解析
    //--------------------------------------------------------------------------
    DateTime& Parse(std::string s)
    {
        // 解析時刻を返却
        return this->Parse(this->m_format, s);
    }

    //--------------------------------------------------------------------------
    // 解析
    //--------------------------------------------------------------------------
    DateTime& Parse(std::string format, std::string s)
    {
        struct tm _parse;                   // 解析時刻

        // 文字列解析
        if(strptime(s.c_str(), format.c_str(), &_parse) == NULL)
        {
            // 例外
            throw DateTimeException("DateTime(文字列)の解析に失敗しました：[" + format + "]["+ s +"]");
        }

        // 時刻変換
        this->ConvertTimeVal(_parse, 0);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // コピー
    //--------------------------------------------------------------------------
    void Copy(const DateTime& datetime)
    {
        // コピー
        this->m_timespec = datetime.m_timespec;
        this->m_tmval = datetime.m_tmval;
        this->m_value = datetime.m_value;
        this->m_format = datetime.m_format;
    }

    //--------------------------------------------------------------------------
    // 時刻更新
    //--------------------------------------------------------------------------
    DateTime& Update()
    {
        // 現在時刻設定
        this->SetNow();

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 時刻取得
    //--------------------------------------------------------------------------
    time_t GetGMT()
    {
        // struct tm⇒time_t変換(GMT)
        time_t _time_value = timegm(&(this->m_tmval));

        // 結果を返却
        return _time_value;
    }

    //--------------------------------------------------------------------------
    // operator =
    //--------------------------------------------------------------------------
    DateTime& operator = (const DateTime& value)
    {
        // コピー
        this->Copy(value);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator =
    //--------------------------------------------------------------------------
    DateTime& operator = (const struct timespec& value)
    {
        // 時刻設定
        this->m_timespec = value;

        // 時刻変換
        this->ConvertTimeVal(value);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator =
    //--------------------------------------------------------------------------
    DateTime& operator = (const struct tm& value)
    {
        // 時刻設定
        this->m_tmval = value;

        // 時刻変換
        this->ConvertTimeVal(value, this->m_timespec.tv_nsec/1000000);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator =
    //--------------------------------------------------------------------------
    DateTime& operator = (const time_t& value)
    {
        // 時刻設定
        this->m_value = value;

        // 時刻変換
        this->ConvertTimeVal(value);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator +
    //--------------------------------------------------------------------------
    const DateTime operator + (const TimeSpan& value) const
    {
        DateTime _result;                   // 結果

        // 結果を返却
        return _result;
    }

    //--------------------------------------------------------------------------
    // operator -
    //--------------------------------------------------------------------------
    const TimeSpan operator - (const DateTime& value) const
    {
        TimeSpan _result;

        // 自身の値を追加
        _result.AddMilliseconds(this->m_timespec.tv_nsec/1000000);
        _result.AddSeconds(this->m_timespec.tv_sec);

        // value値を減算
        _result.AddMilliseconds((value.m_timespec.tv_nsec/1000000*-1));
        _result.AddSeconds(value.m_timespec.tv_sec*-1);

        // 計算結果を返却
        return _result;
    }

    //--------------------------------------------------------------------------
    // operator ==
    //--------------------------------------------------------------------------
    bool operator == (const DateTime& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec == value.m_timespec.tv_sec && this->m_timespec.tv_nsec == value.m_timespec.tv_nsec)
        {
            // 一致
            return true;
        }

        // 不一致
        return false;
    }

    //--------------------------------------------------------------------------
    // operator ==
    //--------------------------------------------------------------------------
    bool operator == (const struct timespec& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec == value.tv_sec && this->m_timespec.tv_nsec == value.tv_nsec)
        {
            // 一致
            return true;
        }

        // 不一致
        return false;
    }

    //--------------------------------------------------------------------------
    // operator ==
    //--------------------------------------------------------------------------
    bool operator == (const struct tm& value) const
    {
        struct tm _from = this->m_tmval;    // 比較値
        struct tm _to = value;              // 比較値

        // 比較
        if(mktime(&(_from)) == mktime(&_to))
        {
            // 一致
            return true;
        }

        // 不一致
        return false;
    }

    //--------------------------------------------------------------------------
    // operator ==
    //--------------------------------------------------------------------------
    bool operator == (const time_t& value) const
    {
        // 比較
        if(this->m_value == value)
        {
            // 一致
            return true;
        }

        // 不一致
        return false;
    }

    //--------------------------------------------------------------------------
    // operator !=
    //--------------------------------------------------------------------------
    bool operator != (const DateTime& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec == value.m_timespec.tv_sec && this->m_timespec.tv_nsec == value.m_timespec.tv_nsec)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator !=
    //--------------------------------------------------------------------------
    bool operator != (const struct timespec& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec == value.tv_sec && this->m_timespec.tv_nsec == value.tv_nsec)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator !=
    //--------------------------------------------------------------------------
    bool operator != (const struct tm& value) const
    {
        struct tm _from = this->m_tmval;    // 比較値
        struct tm _to = value;              // 比較値

        // 比較
        if(mktime(&(_from)) == mktime(&_to))
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator !=
    //--------------------------------------------------------------------------
    bool operator != (const time_t& value) const
    {
        // 比較
        if(this->m_value == value)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator <
    //--------------------------------------------------------------------------
    bool operator < (const DateTime& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec < value.m_timespec.tv_sec && this->m_timespec.tv_nsec < value.m_timespec.tv_nsec)
        {
            // 条件成立
            return true;
        }

        // 条件不成立
        return false;
    }

    //--------------------------------------------------------------------------
    // operator <
    //--------------------------------------------------------------------------
    bool operator < (const struct timespec& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec < value.tv_sec && this->m_timespec.tv_nsec < value.tv_nsec)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator <
    //--------------------------------------------------------------------------
    bool operator < (const struct tm& value) const
    {
        struct tm _from = this->m_tmval;    // 比較値
        struct tm _to = value;              // 比較値

        // 比較
        if(mktime(&(_from)) < mktime(&_to))
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator <
    //--------------------------------------------------------------------------
    bool operator < (const time_t& value) const
    {
        // 比較
        if(this->m_value < value)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator >
    //--------------------------------------------------------------------------
    bool operator > (const DateTime& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec > value.m_timespec.tv_sec && this->m_timespec.tv_nsec > value.m_timespec.tv_nsec)
        {
            // 条件成立
            return true;
        }

        // 条件不成立
        return false;
    }

    //--------------------------------------------------------------------------
    // operator >
    //--------------------------------------------------------------------------
    bool operator > (const struct timespec& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec > value.tv_sec && this->m_timespec.tv_nsec > value.tv_nsec)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator >
    //--------------------------------------------------------------------------
    bool operator > (const struct tm& value) const
    {
        struct tm _from = this->m_tmval;    // 比較値
        struct tm _to = value;              // 比較値

        // 比較
        if(mktime(&(_from)) > mktime(&_to))
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator >
    //--------------------------------------------------------------------------
    bool operator > (const time_t& value) const
    {
        // 比較
        if(this->m_value > value)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator <=
    //--------------------------------------------------------------------------
    bool operator <= (const DateTime& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec <= value.m_timespec.tv_sec && this->m_timespec.tv_nsec <= value.m_timespec.tv_nsec)
        {
            // 条件成立
            return true;
        }

        // 条件不成立
        return false;
    }

    //--------------------------------------------------------------------------
    // operator <=
    //--------------------------------------------------------------------------
    bool operator <= (const struct timespec& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec <= value.tv_sec && this->m_timespec.tv_nsec <= value.tv_nsec)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator <=
    //--------------------------------------------------------------------------
    bool operator <= (const struct tm& value) const
    {
        struct tm _from = this->m_tmval;    // 比較値
        struct tm _to = value;              // 比較値

        // 比較
        if(mktime(&(_from)) <= mktime(&_to))
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator <=
    //--------------------------------------------------------------------------
    bool operator <= (const time_t& value) const
    {
        // 比較
        if(this->m_value <= value)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator >=
    //--------------------------------------------------------------------------
    bool operator >= (const DateTime& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec >= value.m_timespec.tv_sec && this->m_timespec.tv_nsec >= value.m_timespec.tv_nsec)
        {
            // 条件成立
            return true;
        }

        // 条件不成立
        return false;
    }

    //--------------------------------------------------------------------------
    // operator >=
    //--------------------------------------------------------------------------
    bool operator >= (const struct timespec& value) const
    {
        // 比較
        if(this->m_timespec.tv_sec >= value.tv_sec && this->m_timespec.tv_nsec >= value.tv_nsec)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator >=
    //--------------------------------------------------------------------------
    bool operator >= (const struct tm& value) const
    {
        struct tm _from = this->m_tmval;    // 比較値
        struct tm _to = value;              // 比較値

        // 比較
        if(mktime(&(_from)) >= mktime(&_to))
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // operator >=
    //--------------------------------------------------------------------------
    bool operator >= (const time_t& value) const
    {
        // 比較
        if(this->m_value >= value)
        {
            // 一致
            return false;
        }

        // 不一致
        return true;
    }

    //--------------------------------------------------------------------------
    // 取得(年)
    //--------------------------------------------------------------------------
    int64_t Year()
    {
        // 年を返却
        return this->m_tmval.tm_year + 1900;
    }

    //--------------------------------------------------------------------------
    // 取得(月)
    //--------------------------------------------------------------------------
    int64_t Month()
    {
        // 月を返却
        return this->m_tmval.tm_mon + 1;
    }

    //--------------------------------------------------------------------------
    // 取得(日)
    //--------------------------------------------------------------------------
    int64_t Day()
    {
        // 日を返却
        return this->m_tmval.tm_mday;
    }

    //--------------------------------------------------------------------------
    // 取得(時)
    //--------------------------------------------------------------------------
    int64_t Hour()
    {
        // 時を返却
        return this->m_tmval.tm_hour;
    }

    //--------------------------------------------------------------------------
    // 取得(分)
    //--------------------------------------------------------------------------
    int64_t Minute()
    {
        // 分を返却
        return this->m_tmval.tm_min;
    }

    //--------------------------------------------------------------------------
    // 取得(秒)
    //--------------------------------------------------------------------------
    int64_t Second()
    {
        // 秒を返却
        return this->m_tmval.tm_sec;
    }

    //--------------------------------------------------------------------------
    // 取得(ミリ秒)
    //--------------------------------------------------------------------------
    int64_t Millisecond()
    {
        // ミリ秒を返却
        return this->m_timespec.tv_nsec/1000000;
    }

    //--------------------------------------------------------------------------
    // 加算(年)
    //--------------------------------------------------------------------------
    DateTime& AddYears(int64_t value)
    {
        // 年を加算
        this->m_tmval.tm_year += value;

        // 変換
        this->ConvertTimeVal(this->m_tmval, this->m_timespec.tv_nsec/1000000);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(月)
    //--------------------------------------------------------------------------
    DateTime& AddMonths(int64_t value)
    {
        // 追加月／年の計算
        int64_t _addyear = value/12;
        int64_t _addmonth = value - (_addyear*12);

        // 月加算
        if(_addmonth != 0)
        {
            this->m_tmval.tm_mon += _addmonth;
        }

        // 年加算
        if(_addyear != 0)
        {
            this->AddYears(_addyear);
        }
        else
        {
            // 変換
            this->ConvertTimeVal(this->m_tmval, this->m_timespec.tv_nsec/1000000);
        }

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(日)
    //--------------------------------------------------------------------------
    DateTime& AddDays(int64_t value)
    {
        // 自身のTimeSpanオブジェクトを取得
        TimeSpan _timeSpan = this->ToTimeSpan();

        // 加算
        _timeSpan.AddDays(value);

        // 変換
        this->ConvertTimeVal(_timeSpan);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(時)
    //--------------------------------------------------------------------------
    DateTime& AddHours(int64_t value)
    {
        // 自身のTimeSpanオブジェクトを取得
        TimeSpan _timeSpan = this->ToTimeSpan();

        // 加算
        _timeSpan.AddHours(value);

        // 変換
        this->ConvertTimeVal(_timeSpan);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(分)
    //--------------------------------------------------------------------------
    DateTime& AddMinutes(int64_t value)
    {
        // 自身のTimeSpanオブジェクトを取得
        TimeSpan _timeSpan = this->ToTimeSpan();

        // 加算
        _timeSpan.AddMinutes(value);

        // 変換
        this->ConvertTimeVal(_timeSpan);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(秒)
    //--------------------------------------------------------------------------
    DateTime& AddSeconds(int64_t value)
    {
        // 自身のTimeSpanオブジェクトを取得
        TimeSpan _timeSpan = this->ToTimeSpan();

        // 加算
        _timeSpan.AddSeconds(value);

        // 変換
        this->ConvertTimeVal(_timeSpan);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(ミリ秒)
    //--------------------------------------------------------------------------
    DateTime& AddMilliseconds(int64_t value)
    {
        // 自身のTimeSpanオブジェクトを取得
        TimeSpan _timeSpan = this->ToTimeSpan();

        // 加算
        _timeSpan.AddMilliseconds(value);

        // 変換
        this->ConvertTimeVal(_timeSpan);

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // Binary化
    //--------------------------------------------------------------------------
    Binary ToBinary(DateTimeType type)
    {
        Binary _Binary;                     // Binaryオブジェクト

        // 日時種別で分岐
        switch(type)
        {
            case DateTimeType_Timespec :
                // データ設定
                _Binary.Set((u_char*)&(this->m_timespec), sizeof(this->m_timespec));
                break;
            case DateTimeType_Tm :
                // データ設定
                _Binary.Set((u_char*)&(this->m_tmval), sizeof(this->m_tmval));
                break;
            case DateTimeType_Timet :
                // データ設定
                _Binary.Set((u_char*)&(this->m_value), sizeof(this->m_value));
                break;
            default :
                // 例外
                throw DateTimeException("Binar化解析に失敗しました:[%d]", type);
        }

        // Binaryオブジェクトを返却
        return _Binary;
    }

    //--------------------------------------------------------------------------
    // TimeSpan取得
    //--------------------------------------------------------------------------
    TimeSpan ToTimeSpan()
    {
        TimeSpan _result;

        // 自身の値を追加
        _result.AddMilliseconds(this->m_timespec.tv_nsec/1000000);
        _result.AddSeconds(this->m_timespec.tv_sec);

        // 計算結果を返却
        return _result;
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString()
    {
        // 文字列を返却する
        return this->ToString(this->m_format, this->m_timespec);
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString(std::string format)
    {
        // 文字列を返却する
        return this->ToString(format, this->m_timespec);
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString(std::string format, const time_t& value)
    {
        struct timespec _timespec;          // 経過時間構造体

        // 初期化
        _timespec.tv_sec = value;
        _timespec.tv_nsec = 0;

        // 文字列を返却
        return this->ToString(format, _timespec);
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString(std::string format, const struct timespec& value)
    {
        std::stringstream _nowstring;       // 文字列化Stream

        // フォーマット文字分繰り返す
        bool _find = false;
        for(std::string::const_iterator _itr = format.begin(); _itr != format.end(); ++_itr)
        {
            // '%'を検出した場合
            if(_find == false && *_itr == '%')
            {
                // 一旦出力せずに処理継続
                _find = true;
                continue;
            }
            else if(_find == true && *_itr == '%')
            {
                // 直前に%がある場合は、'%'を出力
                _nowstring << "%";
                _find = false;
                continue;
            }
            // '%'を検出していない場合
            else if(_find == false && *_itr != '%')
            {
                // '%'検出前の文字はそのまま
                _nowstring << *_itr;
                continue;
            }

            // 検出後の文字毎に処理する
            switch(*_itr)
            {
            case 'a' :  // %a : 現在のロケールにおける曜日の省略名。
            case 'A' :  // %A : 現在のロケールにおける曜日の完全な名前。
            case 'b' :  // %b : 現在のロケールにおける月の省略名。
            case 'B' :  // %B : 現在のロケールにおける月の完全な名前。
            case 'c' :  // %c : 現在のロケールにおいて一般的な日付・時刻の表記。
            case 'C' :  // %C : 世紀 (西暦年の上 2 桁)。 (SU)
            case 'd' :  // %d : 月内通算日 (10 進数表記) (01-31)。
            case 'D' :  // %D : %m/%d/%y と等価。(うえっ、アメリカ専用だ。アメリカ以外の国では %d/%m/%y の方が一般的だ。紛らわしいので、使用すべきではない。) (SU)
            case 'e' :  // %e : %d と同様に月内通算日を 10 進数で表現するが、 1 桁の場合 10 の位にゼロを置かずスペースを置く。(SU)
            case 'E' :  // %E : 別形式を使用する際の修飾子。下記参照。 (SU)
            case 'F' :  // %F : %Y-%m-%d と等価 (ISO 8601 形式の日付フォーマット)。 (C99)
            case 'G' :  // %G : ISO 8601 週単位表記の年 (week-based year; 「注意」の節を参照)。 世紀も 10 進数で表す。 ISO 週番号 (%V を参照) に対応した 4 桁の西暦年。 これは基本的には %Y と同じ形式だが、ISO 週数が前年や翌年になる 場合にはその年が使用される点が異なる。(TZ)
            case 'g' :  // %g : %G と同様。但し、世紀を含まず下 2 桁のみを表示 (00-99)。 (TZ)
            case 'h' :  // %h : %b と等価 (SU)
            case 'H' :  // %H : 24 時間表記での時 (hour)。 (00-23)
            case 'I' :  // %I : 12 時間表記での時 (hour)。 (01-12)
            case 'j' :  // %j : 年の初めから通算の日数。 (001-366)
            case 'k' :  // %k : 24 時間表記での時 (0-23)。 1 桁の場合には前にゼロでなくスペースが置かれる。 (%H も参照) (TZ)
            case 'l' :  // %l : 12 時間表記での時 (0-12)。 1 桁の場合には前にゼロでなくスペースが置かれる。 (%I も参照) (TZ)
            case 'm' :  // %m : 月 (10 進数表記)。 (01-12)
            case 'M' :  // %M : 分 (10 進数表記) (00-59)
            case 'n' :  // %n : 改行。 (SU)
            case 'O' :  // %O : 別形式を使用する際の修飾子。下記参照。 (SU)
            case 'p' :  // %p : 現在のロケールにおける「午前」「午後」に相当する文字列。 英語の場合には "AM" または "PM" となる。 正午は「午後」、真夜中は「午前」として扱われる。
            case 'P' :  // %P : %p と同様であるが小文字が使用される。 英語の場合には "am" や "pm" となる。(GNU)
            case 'r' :  // %r : 午前・午後形式での時刻。 POSIX ロケールでは %I:%M:%S %p と等価である。(SU)
            case 'R' :  // %R : 24 時間表記での時刻、秒は表示しない (%H:%M)。 秒を含んだものは以下の %T を参照すること。(SU)
            case 's' :  // %s : 紀元 (Epoch; 1970-01-01 00:00:00 +0000 (UTC)) からの秒数。 (TZ)
            case 'S' :  // %S : 秒 (10 進数表記) (00-60) (時々ある閏秒に対応するため、値の範囲は 60 までとなっている)
            case 't' :  // %t : タブ文字 (SU)
            case 'T' :  // %T : 24 時間表記の時間 (%H:%M:%S) (SU)
            case 'u' :  // %u : 週の何番目の日 (10 進数表記) か。月曜日を 1 とする (1-7)。 %w も参照。(SU)
            case 'U' :  // %U : 年の初めからの通算の週番号 (10 進数表記) (00-53)。 その年の最初の日曜日を、第 1 週の始まりとして計算する。 %V と %W も参照すること。
            case 'V' :  // %V : ISO 8601 形式での年の始めからの週番号 (「注意」の節を参照)。 10 進数表記で、01 から 53 の値となる。週番号は、 新しい年が少なくとも 4 日以上含まれる最初の週を 1 として計算する。 %U と %W も参照のこと。(SU)
            case 'w' :  // %w : 週の何番目の日 (10 進数表記) か。日曜日を 0 とする。(0-6)。 %u も参照。(SU)
            case 'W' :  // %W : 年の初めからの通算の週番号 (10 進数表記) (00-53)。 その年の最初の月曜日を、第 1 週の始まりとして計算する。
            case 'x' :  // %x : 現在のロケールで一般的な日付表記。時刻は含まない。
            case 'X' :  // %X : 現在のロケールで一般的な時刻表記。日付は含まない。
            case 'y' :  // %y : 西暦の下2桁 (世紀部分を含まない年) (00-99)。
            case 'Y' :  // %Y : 世紀部分を含めた ( 4 桁の) 西暦年。
            case 'z' :  // %z : +hhmm や -hhmm の形式のタイムゾーン (UTC へのオフセット時間)。(SU)
            case 'Z' :  // %Z : タイムゾーン名または省略名。
            case '+' :  // %+ :  date(1) 形式での日時。(TZ) (glibc2 ではサポートされていない)
            {
                // フォーマット作成
                std::stringstream _fmt;
                _fmt << "%" << *_itr;

                // 変換文字列を格納
                _nowstring << this->tostring(_fmt.str(), value);
            }
            break;
            // 独自フォーマット
            case 'f':   // %f : 経過日数
                // 変換文字列を格納
                _nowstring << this->ToString_f(value);
                break;
            case 'L':   // %L : ミリ秒(3桁、前0あり)
                // 変換文字列を格納
                _nowstring << this->ToString_L(value);
                break;
            // 上記以外
            default:
                // %を含めて、そのまま出力する
                _nowstring << '%' << *_itr;
                break;
            }

            // 検出フラグリセット
            _find = false;
        }

        // 文字列を返却
        return _nowstring.str();
    }
};
#endif                                      // 二重インクルード防止
