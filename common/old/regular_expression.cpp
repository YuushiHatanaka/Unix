//-----------------------------------------------------------------------------
// regular_expressionモジュール
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include "regular_expression.hpp"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
regular_expression::regular_expression()
{
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
regular_expression::regular_expression(const regular_expression& data)
{
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
regular_expression::~regular_expression()
{
}

//-------------------------------------------------------------------------
// 正規表現検索関数
//-------------------------------------------------------------------------
bool regular_expression::matching(std::string str,std::string regex_str)
{
    // 正規表現検索結果を返却
    return regular_expression::matching(str, regex_str, REG_EXTENDED|REG_NEWLINE);
}

//-------------------------------------------------------------------------
// 正規表現検索関数
//-------------------------------------------------------------------------
bool regular_expression::matching(std::string str,std::string regex_str,int cflg)
{
    regex_t _regex;             // 正規表現オブジェクト
    regmatch_t _regmatch[1];    // 正規表現マッチングオブジェクト
    // 一致数
    size_t _match_count = sizeof(_regmatch)/sizeof(regmatch_t);

    // 正規表現オブジェクトのコンパイル
    if( regcomp( &_regex, regex_str.c_str(), cflg ) != 0 ) {
        throw "regcomp failed.";
    }

    // パターンマッチング
    if( regexec( &_regex, str.c_str(), _match_count, _regmatch, 0 ) != 0 ) {
        // 不一致
        regfree( &_regex );
        return false;
    }

    // 一致
    regfree( &_regex );
    return true;
}

//-------------------------------------------------------------------------
// 正規表現検索(一致リスト取得)関数
//-------------------------------------------------------------------------
std::vector<std::string> regular_expression::matching_list(std::string str,std::string regex_str,uint32_t matchng_max)
{
    regex_t _regex;                         // 正規表現オブジェクト
    regmatch_t _regmatch[matchng_max+1];    // 正規表現マッチングオブジェクト
                                            // 一致数
    size_t _match_count = sizeof(_regmatch)/sizeof(regmatch_t);
    std::vector<std::string> _matching_list;// 一致リスト

    // 正規表現オブジェクトのコンパイル
    if( regcomp( &_regex, regex_str.c_str(), REG_EXTENDED|REG_NEWLINE ) != 0 ) {
        throw "regcomp failed.";
    }

    // パターンマッチング
    if( regexec( &_regex, str.c_str(), _match_count, _regmatch, 0 ) != 0 ) {
        // 不一致
        regfree( &_regex );
        return _matching_list;
    }

    // 一致数分繰り返す
    if( _match_count > 1 ) {
        for( size_t i=1; i<_match_count; i++ ) {
            if( _regmatch[i].rm_so == -1 || _regmatch[i].rm_eo == -1 ) {
                _matching_list.push_back("");
            }
            else {
                _matching_list.push_back(str.substr( _regmatch[i].rm_so, _regmatch[i].rm_eo - _regmatch[i].rm_so ));
            }
        }
    }

    // 後処理
    regfree( &_regex );

    // 一致リストを返却
    return _matching_list;
}

//-------------------------------------------------------------------------
// 正規表現置換関数
// ※置換できなかった場合は元の文字列が返却される
//-------------------------------------------------------------------------
std::string regular_expression::replace(std::string base_str,std::string regex_str,std::string replace_str)
{
    regex_t _regex;            // 正規表現オブジェクト
    regmatch_t _regmatch[1];   // 正規表現マッチングオブジェクト
    // 一致数
    size_t _match_count = sizeof(_regmatch)/sizeof(regmatch_t);
    // 置換文字列
    std::string _result_str = base_str;

    // 正規表現オブジェクトのコンパイル
    if(regcomp( &_regex, regex_str.c_str(), REG_EXTENDED|REG_NEWLINE ) != 0 ) {
        throw "regcomp failed.";
    }

    // パターンマッチング
    if( regexec( &_regex, base_str.c_str(), _match_count, _regmatch, 0 ) != 0 ) {
        // 不一致
        regfree( &_regex );
        return _result_str;
    }

    // 一致
    if(!( _regmatch[0].rm_so == -1 || _regmatch[0].rm_eo == -1 )) {
        _result_str = base_str.replace( _regmatch[0].rm_so, _regmatch[0].rm_eo - _regmatch[0].rm_so, replace_str );
    }

    // 後処理
    regfree( &_regex );

    // 置換文字列を返却
    return _result_str;
}

//-------------------------------------------------------------------------
// 正規表現置換関数
//-------------------------------------------------------------------------
std::string regular_expression::replace(std::string base_str,std::string regex_str,std::string replace_str,int cflg)
{
    regex_t     _regex;                   // 正規表現オブジェクト
    regmatch_t* _regmatch;                // 正規表現マッチングオブジェクト
    size_t      _match_count;             // 正規表現一致数
    std::string _result_str = base_str;   // 置換文字列

    // 正規表現オブジェクトのコンパイル
    if(regcomp( &_regex, regex_str.c_str(), cflg ) != 0 ) {
        throw "regcomp failed.";
    }

    // 正規表現マッチングオブジェクト生成
    _match_count = _regex.re_nsub+1;
    _regmatch = new regmatch_t[_match_count];

    // 一致しなくなるまで実施する
    while(1) {
        // パターンマッチング
        if( regexec( &_regex, _result_str.c_str(), _match_count, _regmatch, 0 ) != 0 ) {
            // 不一致
            break;
        }
        // 置換処理
        for( size_t i=0; i<_match_count; i++ ) {
            if( _regmatch[i].rm_so == -1 || _regmatch[i].rm_eo == -1 ) {
                continue;
            }
            std::string _tmp_str = _result_str.replace( _regmatch[i].rm_so, _regmatch[i].rm_eo - _regmatch[i].rm_so, replace_str );
            _result_str = _tmp_str;
        }
    }

    // 後処理
    regfree( &_regex );
    delete[] _regmatch;

    // 置換文字列を返却
    return _result_str;
}
