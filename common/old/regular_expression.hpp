//-----------------------------------------------------------------------------
// regular_expressionヘッダ
//-----------------------------------------------------------------------------
#ifndef _REGULAR_EXPRESSION_HPP
#define _REGULAR_EXPRESSION_HPP

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include <stdint.h>
#include <regex.h>

#include <string>
#include <vector>

//-----------------------------------------------------------------------------
// クラス定義
//-----------------------------------------------------------------------------
// regular_expressionクラス
class regular_expression  {
private :
public :
    // コンストラクタ
    regular_expression();
    // コンストラクタ
    regular_expression(const regular_expression&);
    // デストラクタ
    ~regular_expression();
    // 正規表現検索
    bool matching(std::string ,std::string);
    // 正規表現検索
    bool matching(std::string ,std::string ,int);
    //  正規表現検索(一致リスト取得)
    std::vector<std::string> matching_list(std::string ,std::string ,uint32_t);
    // 正規表現置換
    std::string replace(std::string ,std::string ,std::string);
    // 正規表現置換
    std::string replace(std::string ,std::string ,std::string ,int);
};

#endif
