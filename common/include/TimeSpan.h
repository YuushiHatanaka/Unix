//==============================================================================
// TimeSpanクラスヘッダ
//==============================================================================
#ifndef _TIMESPAN_H_                        // 二重インクルード防止
#define _TIMESPAN_H_                        // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "RegularExpression.h"
#include <stdlib.h>
#include <ios>
#include <iomanip>
#include <sstream>
#include <iostream>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// TimeSpan例外クラス
//------------------------------------------------------------------------------
class TimeSpanException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimeSpanException(std::string format, ...)
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
// TimeSpanクラス
//------------------------------------------------------------------------------
class TimeSpan
{
private:
    int64_t m_days;                         // 値(時間間隔の日要素)
    int64_t m_hours;                        // 値(時間間隔の時間)
    int64_t m_minutes;                      // 値(時間間隔の分)
    int64_t m_seconds;                      // 値(時間間隔の秒)
    int64_t m_milliseconds;                 // 値(時間間隔のミリ秒)
    int64_t m_ticks;                        // 値(タイマー刻みの数)
    std::string m_default_format;           // デフォルトフォーマット

private:
    //--------------------------------------------------------------------------
    // 初期化
    //--------------------------------------------------------------------------
    void initializing()
    {
        // 初期化
        this->m_days = 0;
        this->m_hours = 0;
        this->m_minutes = 0;
        this->m_seconds = 0;
        this->m_milliseconds = 0;
        this->m_ticks = 0;
        this->m_default_format = "%d.%h:%m:%s.%fff";
    }

    //--------------------------------------------------------------------------
    // Ticks設定
    //--------------------------------------------------------------------------
    void set_ticks()
    {
        this->set_ticks(this->m_days, this->m_hours, this->m_minutes, this->m_seconds);
    }

    //--------------------------------------------------------------------------
    // Ticks設定
    //--------------------------------------------------------------------------
    void set_ticks(int64_t days,int64_t hours,int64_t minutes,int64_t seconds)
    {
        // 換算処理
        this->m_ticks = 0;
        this->m_ticks += this->m_milliseconds;
        this->m_ticks += this->m_seconds*(1000);
        this->m_ticks += this->m_minutes*(60*1000);
        this->m_ticks += this->m_hours*(60*60*1000);
        this->m_ticks += this->m_days*(24*60*60*1000);
        this->m_ticks *= 10000;
    }

    //--------------------------------------------------------------------------
    // 更新
    //--------------------------------------------------------------------------
    void update(int64_t days,int64_t hours,int64_t minutes,int64_t seconds,int64_t milliseconds)
    {
        // 初期化
        this->m_days = days;
        this->m_hours = hours;
        this->m_minutes = minutes;
        this->m_seconds = seconds;
        this->m_milliseconds = milliseconds;
        this->set_ticks(days, hours, minutes, seconds);
    }

    //--------------------------------------------------------------------------
    // 更新
    //--------------------------------------------------------------------------
    void update(int64_t ticks)
    {
        // 更新
        this->update(0, 0, 0, 0, 0);

        // 値を設定
        this->m_ticks = ticks;

        // 値を加算
        this->AddMilliseconds(ticks);
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string tostring_right(std::string& format, uint64_t& index, const char type, int64_t value)
    {
        std::stringstream _stringstream;    // 文字列化Stream

        // フォーマット文字列長を取得
        uint64_t _length = this->tostring_getformatlength(format, index, type);

        // 文字列作成
        _stringstream << std::right << std::setw(_length) << value;

        // 文字列を返却
        return _stringstream.str();
    }

    //--------------------------------------------------------------------------
    // 文字列化(前ゼロあり、右詰)
    //--------------------------------------------------------------------------
    std::string tostring_zero_right(std::string& format, uint64_t& index, const char type, int64_t value)
    {
        std::stringstream _stringstream;    // 文字列化Stream

        // フォーマット文字列長を取得
        uint64_t _length = this->tostring_getformatlength(format, index, type);

        // 文字列作成
        _stringstream << std::right << std::setw(_length) << std::setfill('0') << value;

        // 文字列を返却
        return _stringstream.str();
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string tostring_left(std::string& format, uint64_t& index, const char type, int64_t value)
    {
        std::stringstream _stringstream;    // 文字列化Stream

        // フォーマット文字列長を取得
        uint64_t _length = this->tostring_getformatlength(format, index, type);

        // 文字列作成
        _stringstream << std::left << std::setw(_length) << value;

        // 文字列を返却
        return _stringstream.str();
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string tostring_zero_left(std::string& format, uint64_t& index, const char type, int64_t value)
    {
        std::stringstream _stringstream;    // 文字列化Stream

        // フォーマット文字列長を取得
        uint64_t _length = this->tostring_getformatlength(format, index, type);

        // 文字列作成
        _stringstream << std::left << std::setw(_length) << std::setfill('0') << value;

        // 文字列を返却
        return _stringstream.str();
    }

    //--------------------------------------------------------------------------
    // 文字列化フォーマット長取得
    //--------------------------------------------------------------------------
    uint64_t tostring_getformatlength(std::string& format, uint64_t& index, const char type)
    {
        uint64_t _length = 0;               // フォーマット文字列長

        // フォーマット文字分繰り返す
        for(; index<format.length(); index++)
        {
            // フォーマットが変化したら
            if(format[index] != type)
            {
                // 処理終了
                --index;
                break;
            }
            // フォーマット長をインクリメント
            _length++;
        }

        // フォーマット文字列長を返却
        return _length;
    }

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimeSpan()
    {
        // 初期化
        this->initializing();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimeSpan(int64_t ticks)
    {
        // 初期化
        this->initializing();

        // 更新
        this->update(ticks);
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimeSpan(int64_t hours,int64_t minutes,int64_t seconds)
    {
        // 初期化
        this->initializing();

        // 更新
        this->update(0, hours, minutes, seconds, 0);
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimeSpan(int64_t days,int64_t hours,int64_t minutes,int64_t seconds)
    {
        // 初期化
        this->initializing();

        // 更新
        this->update(days, hours, minutes, seconds, 0);
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimeSpan(int64_t days,int64_t hours,int64_t minutes,int64_t seconds,int64_t milliseconds)
    {
        // 初期化
        this->initializing();

        // 更新
        this->update(days, hours, minutes, seconds, milliseconds);
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimeSpan(const TimeSpan& timespan)
    {
        // コピー
        this->m_days = timespan.m_days;
        this->m_hours = timespan.m_hours;
        this->m_minutes = timespan.m_minutes;
        this->m_seconds = timespan.m_seconds;
        this->m_milliseconds = timespan.m_milliseconds;
        this->m_ticks = timespan.m_ticks;
        this->m_default_format = timespan.m_default_format;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~TimeSpan()
    {
    }

    //--------------------------------------------------------------------------
    // 加算
    //--------------------------------------------------------------------------
    TimeSpan& Add(const TimeSpan& value)
    {
        // 加算
        this->AddMilliseconds(value.m_milliseconds);
        this->AddSeconds(value.m_seconds);
        this->AddMinutes(value.m_minutes);
        this->AddHours(value.m_hours);
        this->AddDays(value.m_days);
        this->set_ticks();

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 減算
    //--------------------------------------------------------------------------
    TimeSpan& Subtract(const TimeSpan& value)
    {
        // 減算
        this->AddDays(-1*value.m_days);
        this->AddHours(-1*value.m_hours);
        this->AddMinutes(-1*value.m_minutes);
        this->AddSeconds(-1*value.m_seconds);
        this->AddMilliseconds(-1*value.m_milliseconds);
        this->set_ticks();

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(日)
    //--------------------------------------------------------------------------
    TimeSpan& AddDays(int64_t value)
    {
        // 加算
        this->m_days += value;

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(時)
    //--------------------------------------------------------------------------
    TimeSpan& AddHours(int64_t value)
    {
        // 加算値計算
        int64_t _days = value/24;
        int64_t _hours = value-(_days*24);

        // 加算(時)
        this->m_hours += _hours;

        // 加算値再計算
        if(this->m_hours < 0)
        {
            this->m_hours += 24;
            _days -= 1;
        }
        else if(this->m_hours > 24)
        {
            this->m_hours -= 24;
            _days += 1;
        }

        // 加算(日)
        if(_days != 0)
        {
            this->AddDays(_days);
        }

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(分)
    //--------------------------------------------------------------------------
    TimeSpan& AddMinutes(int64_t value)
    {
        // 加算値計算
        int64_t _hours = value/60;
        int64_t _minutes = value-(_hours*60);

        // 加算(分)
        this->m_minutes += _minutes;

        // 加算値再計算
        if(this->m_minutes < 0)
        {
            this->m_minutes += 60;
            _hours -= 1;
        }
        else if(this->m_minutes > 60)
        {
            this->m_minutes -= 60;
            _hours += 1;
        }

        // 加算(時)
        if(_hours != 0)
        {
            this->AddHours(_hours);
        }

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(秒)
    //--------------------------------------------------------------------------
    TimeSpan& AddSeconds(int64_t value)
    {
        // 加算値計算
        int64_t _minutes = value/60;
        int64_t _seconds = value-(_minutes*60);

        // 加算(秒)
        this->m_seconds += _seconds;

        // 加算値再計算
        if(this->m_seconds < 0)
        {
            this->m_seconds += 60;
            _minutes -= 1;
        }
        else if(this->m_seconds > 60)
        {
            this->m_seconds -= 60;
            _minutes += 1;
        }

        // 加算(分)
        if(_minutes != 0)
        {
            this->AddMinutes(_minutes);
        }

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 加算(ミリ秒)
    //--------------------------------------------------------------------------
    TimeSpan& AddMilliseconds(int64_t value)
    {
        // 加算値計算
        int64_t _seconds = value/1000;
        int64_t _milliseconds = value-(_seconds*1000);

        // 加算(ミリ秒)
        this->m_milliseconds += _milliseconds;

        // 加算値再計算
        if(this->m_milliseconds < 0)
        {
            this->m_milliseconds += 1000;
            _seconds -= 1;
        }
        else if(this->m_milliseconds > 1000)
        {
            this->m_milliseconds -= 1000;
            _seconds += 1;
        }

        // 加算(秒)
        if(_seconds != 0)
        {
            this->AddSeconds(_seconds);
        }

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // 解析
    //--------------------------------------------------------------------------
    bool Parse(std::string str)
    {
        // 解析
        return this->Parse(this->m_default_format, str);
    }

    //--------------------------------------------------------------------------
    // 解析
    //--------------------------------------------------------------------------
    bool Parse(std::string format, std::string str)
    {
        // TOOD:未実装
        // フォーマット文字列長繰り返し
        //for(std::string::iterator _itr=format.begin(); _itr!=format.end(); ++_itr)
        bool _find = false;
        uint64_t _str_index = 0;
        for(uint64_t _format_index=0; _format_index<format.length(); _format_index++)
        {
            // '%'を検出した場合
            if(_find == false && format[_format_index] == '%')
            {
                // 一旦出力せずに処理継続
                _find = true;
                continue;
            }
            // '%'を検出している場合
            else if(_find == true && format[_format_index] == '%')
            {
                // 文字一致しているか？
                if(str[_str_index] != '%')
                {
                    // 解析エラー
                    return false;
                }

                // 直前に%がある場合は、文字列を進める
                _find = false;
                _str_index += 1;
                continue;
            }
            // '%'を検出していない場合
            else if(_find == false && format[_format_index] != '%')
            {
                // 文字一致しているか？
                if(format[_format_index] != str[_str_index])
                {
                    // 解析エラー
                    return false;
                }

                // '%'検出前の文字は、文字列を進める
                _str_index += 1;
                continue;
            }

            // TODO:%で終端している場合エラーにする

            // フォーマット文字列長を取得
            uint64_t _start_index = _format_index;
            uint64_t _format_length = this->tostring_getformatlength(format, _format_index, format[_start_index]);

            // 数値検索用正規表現決定
            std::string _regular_expression_string;
            // 文字毎に分岐処理する
            switch(format[_start_index])
            {
            case 'd' :
            case 'h' :
            case 'm' :
            case 's' :
            case 'f' : 
            case 'F' : 
                _regular_expression_string = "(^[0-9]*)";
                break;
            case 'D' :
            case 'H' :
            case 'M' :
            case 'S' :
                _regular_expression_string = "^[ |\t]*([0-9]*)";
                break;
            // 上記以外
            default:
                // 解析エラー
                return false;
            }

            // 文字列から数値を検出
            RegularExpression _regular_expression;
            std::vector<std::string> _regular_expression_result = _regular_expression.ExtractionList(str.substr(_str_index), _regular_expression_string.c_str());
            if(_regular_expression_result.size() == 1)
            {
                // 文字列を進める
                _str_index += _regular_expression_result[0].length();
            }
            else
            {
                // 解析エラー
                return false;
            }

            // 桁数判定
            if( _format_length != _regular_expression_result[0].length())
            {
                // TODO:エラーにするか要検討
            }

            // 数値化
            uint64_t _value = atoi(_regular_expression_result[0].c_str());

            // 文字毎に分岐処理する
            switch(format[_start_index])
            {
            case 'd' :
            case 'D' :
                // 加算
                this->AddDays(_value);
                this->set_ticks();
                break;
            case 'h' :
            case 'H' :
                // 加算
                this->AddHours(_value);
                this->set_ticks();
                break;
            case 'm' :
            case 'M' :
                // 加算
                this->AddMinutes(_value);
                this->set_ticks();
                break;
            case 's' :
            case 'S' :
                // 加算
                this->AddSeconds(_value);
                this->set_ticks();
                break;
            case 'f' : 
            case 'F' : 
                // 加算
                this->AddMilliseconds(_value);
                this->set_ticks();
                break;
            // 上記以外
            default:
                // 解析エラー
                return false;
            }

            // 検出フラグリセット
            _find = false;
        }

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString()
    {
        // 文字列を返却
        return this->ToString(this->m_default_format);
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString(std::string format)
    {
        std::stringstream _nowstring;       // 文字列化Stream

        // フォーマット文字分繰り返す
        bool _find = false;
        for(uint64_t i=0; i<format.length(); i++)
        {
            // '%'を検出した場合
            if(_find == false && format[i] == '%')
            {
                // 一旦出力せずに処理継続
                _find = true;
                continue;
            }
            // '%'を検出している場合
            else if(_find == true && format[i] == '%')
            {
                // 直前に%がある場合は、'%'を出力
                _nowstring << "%";
                _find = false;
                continue;
            }
            // '%'を検出していない場合
            else if(_find == false && format[i] != '%')
            {
                // '%'検出前の文字はそのまま
                _nowstring << format[i];
                continue;
            }

            // 文字毎に分岐処理する
            switch(format[i])
            {
            case 'd' :
                _nowstring << this->tostring_zero_right(format, i, format[i], this->m_days);
                break;
            case 'D' :
                _nowstring << this->tostring_right(format, i, format[i], this->m_days);
                break;
            case 'h' :
                _nowstring << this->tostring_zero_right(format, i, format[i], this->m_hours);
                break;
            case 'H' :
                _nowstring << this->tostring_right(format, i, format[i], this->m_hours);
                break;
            case 'm' :
                _nowstring << this->tostring_zero_right(format, i, format[i], this->m_minutes);
                break;
            case 'M' :
                _nowstring << this->tostring_right(format, i, format[i], this->m_minutes);
                break;
            case 's' :
                _nowstring << this->tostring_zero_right(format, i, format[i], this->m_seconds);
                break;
            case 'S' :
                _nowstring << this->tostring_right(format, i, format[i], this->m_seconds);
                break;
            case 'f' : 
                _nowstring << this->tostring_zero_right(format, i, format[i], this->m_milliseconds);
                break;
            case 'F' : 
                _nowstring << this->tostring_zero_right(format, i, format[i], this->m_milliseconds);
                break;
            // 上記以外
            default:
                // %を含めて、そのまま出力する
                _nowstring << '%' << format[i];
                break;
            }

            // 検出フラグリセット
            _find = false;
        }

        // 文字列を返却
        return _nowstring.str();
    }

    //--------------------------------------------------------------------------
    // getter
    //--------------------------------------------------------------------------
    int64_t Days() const { return this->m_days;}
    int64_t Hours() const { return this->m_hours;}
    int64_t Minutes() const { return this->m_minutes;}
    int64_t Seconds() const { return this->m_seconds;}
    int64_t Milliseconds() const { return this->m_milliseconds;}
    int64_t Ticks() const { return this->m_ticks;}

    //--------------------------------------------------------------------------
    // 換算時間(日)
    //--------------------------------------------------------------------------
    double TotalDays()
    {
        double _total_value = 0;            // 換算時間

        // 換算処理
        _total_value += (double)((double)this->m_milliseconds/(double)(24*60*60*1000));
        _total_value += (double)((double)this->m_seconds/(double)(24*60*60));
        _total_value += (double)((double)this->m_minutes/(double)(24*60));
        _total_value += (double)((double)this->m_hours/(double)(24));
        _total_value += (double)(this->m_days);

        // 換算時間を返却する
        return _total_value;
    }

    //--------------------------------------------------------------------------
    // 換算時間(時間)
    //--------------------------------------------------------------------------
    double TotalHours()
    {
        double _total_value = 0;            // 換算時間

        // 換算処理
        _total_value += (double)((double)this->m_milliseconds/(double)(60*60*1000));
        _total_value += (double)((double)this->m_seconds/(double)(60*60));
        _total_value += (double)((double)this->m_minutes/(double)(60));
        _total_value += (double)(this->m_hours);
        _total_value += (double)(this->m_days*(24));

        // 換算時間を返却する
        return _total_value;
    }

    //--------------------------------------------------------------------------
    // 換算時間(分)
    //--------------------------------------------------------------------------
    double TotalMinutes()
    {
        double _total_value = 0;            // 換算時間

        // 換算処理
        _total_value += (double)((double)this->m_milliseconds/(double)(60*1000));
        _total_value += (double)((double)this->m_seconds/(double)60);
        _total_value += (double)(this->m_minutes);
        _total_value += (double)(this->m_hours*(60));
        _total_value += (double)(this->m_days*(24*60));

        // 換算時間を返却する
        return _total_value;
    }

    //--------------------------------------------------------------------------
    // 換算時間(秒)
    //--------------------------------------------------------------------------
    double TotalSeconds()
    {
        double _total_value = 0;            // 換算時間

        // 換算処理
        _total_value += (double)((double)this->m_milliseconds/(double)1000);
        _total_value += (double)(this->m_seconds);
        _total_value += (double)(this->m_minutes*(60));
        _total_value += (double)(this->m_hours*(60*60));
        _total_value += (double)(this->m_days*(24*60*60));

        // 換算時間を返却する
        return _total_value;
    }

    //--------------------------------------------------------------------------
    // 換算時間(ミリ秒)
    //--------------------------------------------------------------------------
    double TotalMilliseconds()
    {
        double _total_value = 0;            // 換算時間

        // 換算処理
        _total_value += (double)(this->m_milliseconds);
        _total_value += (double)(this->m_seconds*(1000));
        _total_value += (double)(this->m_minutes*(60*1000));
        _total_value += (double)(this->m_hours*(60*60*1000));
        _total_value += (double)(this->m_days*(24*60*60*1000));

        // 換算時間を返却する
        return _total_value;
    }
};
#endif                                      // 二重インクルード防止
