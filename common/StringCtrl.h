//==============================================================================
// StringCtrlクラスヘッダ
//==============================================================================
#ifndef _STRING_CTRL_H_                     // 二重インクルード防止
#define _STRING_CTRL_H_                     // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include <stdlib.h>
#include <regex.h>
#include <string>
#include <sstream>
#include <vector>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// StringCtrlクラス
//------------------------------------------------------------------------------
class StringCtrl
{
public:
    //-----------------------------------------------------------------------------
    // 文字列分割関数
    //-----------------------------------------------------------------------------
    std::vector<std::string> Split(const std::string str, char sep)
    {
        std::vector<std::string> v;
        std::stringstream ss(str);
        std::string buffer;
        while( std::getline(ss, buffer, sep) ){
            v.push_back(buffer);
        }
        return v;
    }

    //-----------------------------------------------------------------------------
    // 文字列改行削除関数
    //-----------------------------------------------------------------------------
    std::string DeleteCrlf(const std::string str)
    {
        const char CR = '\r';
        const char LF = '\n';
        std::string destStr;

        for(std::string::const_iterator it=str.begin(); it!=str.end(); ++it)
        {
            if( *it != CR && *it != LF )
            {
                destStr += *it;
            }
        }
        return destStr;
    }

    //-----------------------------------------------------------------------------
    // 空白削除(前後)関数
    //-----------------------------------------------------------------------------
    std::string Trim(const std::string str)
    {
        std::string _trim_string = str;     // 空白削除文字列

        // 空白削除(先頭)
        _trim_string = this->TrimLeft(_trim_string);

        // 空白削除(末尾)
        _trim_string = this->TrimRight(_trim_string);

        // 空白削除文字列返却
        return _trim_string;
    }

    //-----------------------------------------------------------------------------
    // 文字列削除(前後)関数
    //-----------------------------------------------------------------------------
    std::string Trim(const std::string str, const std::string trim)
    {
        std::string _trim_string = str;     // 空白削除文字列

        // 空白削除(先頭)
        _trim_string = this->TrimLeft(_trim_string, trim);

        // 空白削除(末尾)
        _trim_string = this->TrimRight(_trim_string, trim);

        // 空白削除文字列返却
        return _trim_string;
    }

    //-----------------------------------------------------------------------------
    // 空白削除(先頭)関数
    //-----------------------------------------------------------------------------
    std::string TrimLeft(const std::string str)
    {
        // 空白削除文字列返却
        return this->TrimLeft( str, "[ |\t]" );;
    }

    //-----------------------------------------------------------------------------
    // 文字列削除(先頭)関数
    //-----------------------------------------------------------------------------
    std::string TrimLeft(const std::string str, const std::string trim)
    {
        std::string _trim_string = str;     // 空白削除文字列
        regex_t _regex;                     // 正規表現オブジェクト
        regmatch_t _regmatch[1];            // 正規表現マッチングオブジェクト
                                            // 一致数
        size_t _match_count = sizeof(_regmatch)/sizeof(regmatch_t);
                                            // trim用正規表現文字列
        std::string _trim_regex_str = "^"+trim+"+";

        // 正規表現オブジェクトのコンパイル
        if( regcomp( &_regex, _trim_regex_str.c_str(), REG_EXTENDED|REG_NEWLINE ) != 0 )
        {
            throw "regcomp failed.";
        }

        // パターンマッチング
        if( regexec( &_regex, _trim_string.c_str(), _match_count, _regmatch, 0 ) != 0 )
        {
            // 不一致
            regfree( &_regex );
            return _trim_string;
        }

        // 一致数分繰り返す
        for( size_t i=0; i<_match_count; i++ )
        {
            if( _regmatch[i].rm_so == -1 || _regmatch[i].rm_eo == -1 )
            {
                continue;
            }
            _trim_string = _trim_string.substr( _regmatch[i].rm_eo );
        }

        // 後処理
        regfree( &_regex );

        // 空白削除文字列返却
        return _trim_string;
    }

    //-----------------------------------------------------------------------------
    // 空白削除(末尾)関数
    //-----------------------------------------------------------------------------
    std::string TrimRight(const std::string str)
    {
        // 空白削除文字列返却
        return this->TrimRight( str, "[ |\t]" );
    }

    //-----------------------------------------------------------------------------
    // 文字列削除(末尾)関数
    //-----------------------------------------------------------------------------
    std::string TrimRight(const std::string str, const std::string trim)
    {
        std::string _trim_string = str;     // 空白削除文字列
        regex_t _regex;                     // 正規表現オブジェクト
        regmatch_t _regmatch[1];            // 正規表現マッチングオブジェクト
                                            // 一致数
        size_t _match_count = sizeof(_regmatch)/sizeof(regmatch_t);
                                            // trim用正規表現文字列
        std::string _trim_regex_str = trim+"+$";

        // 正規表現オブジェクトのコンパイル
        if( regcomp( &_regex, _trim_regex_str.c_str(), REG_EXTENDED|REG_NEWLINE ) != 0 )
        {
            throw "regcomp failed.";
        }

        // パターンマッチング
        if( regexec( &_regex, _trim_string.c_str(), _match_count, _regmatch, 0 ) != 0 )
        {
            // 不一致
            regfree( &_regex );
            return _trim_string;
        }

        // 一致数分繰り返す
        for( size_t i=0; i<_match_count; i++ )
        {
            if( _regmatch[i].rm_so == -1 || _regmatch[i].rm_eo == -1 )
            {
                continue;
            }
            _trim_string = _trim_string.substr( 0, _regmatch[i].rm_so );
        }

        // 後処理
        regfree( &_regex );

        // 空白削除文字列返却
        return _trim_string;
    }

    //-----------------------------------------------------------------------------
    // 文字前ゼロ削除関数
    //-----------------------------------------------------------------------------
    std::string DeleteFrontZeros(const std::string str)
    {
        std::string _trim_string = str;     // 削除文字列

        // 先頭に'0'が付与されているか？
        if( _trim_string[0] == '0' )
        {
            // 先頭の'0'以外を返却
            return _trim_string.substr(1);
        }

        // 元の文字列を返却
        return str;
    }

    //-----------------------------------------------------------------------------
    // ランダム文字列作成関数
    //-----------------------------------------------------------------------------
    std::string MakeRandomStrings(int length)
    {
        std::stringstream _random;          // ランダム文字列
        char material[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

        // 取得文字列数文繰り返す
        for(int i=0;i<length;i++)
        {
            // ランダムな値を1文字設定
            _random << material[rand()%(sizeof(material)-1)];
        }

        // ランダム文字列を返却
        return _random.str();
    }
};
#endif                                      // 二重インクルード防止
