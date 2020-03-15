//==============================================================================
// 文字列型クラスヘッダ
//==============================================================================
#ifndef _STRING_H_                          // 二重インクルード防止
#define _STRING_H_                          // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Object.h"
#include "Exception.h"
#include "RegularExpression.h"

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// String例外クラス
//------------------------------------------------------------------------------
class StringException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    StringException(std::string msg) : Exception(msg)
    {
    };
};

//------------------------------------------------------------------------------
// Stringクラス
//------------------------------------------------------------------------------
class String : public Object
{
protected :
    // 文字列データ
    std::stringstream m_Data;

private :
    //-----------------------------------------------------------------------------
    // 文字列削除(先頭)関数
    //-----------------------------------------------------------------------------
    std::string TrimLeft(const std::string str, const std::string trim)
    {
                                            // trim用正規表現文字列
        std::string _trim_regex_str = "^"+trim+"+";
        RegularExpression _re;              // 正規表現オブジェクト

        // 正規表現で置換（結果を返却）
        return _re.Replace(str, _trim_regex_str, "");
    }

    //-----------------------------------------------------------------------------
    // 文字列削除(末尾)関数
    //-----------------------------------------------------------------------------
    std::string TrimRight(const std::string str, const std::string trim)
    {
                                            // trim用正規表現文字列
        std::string _trim_regex_str = trim+"+$";
        RegularExpression _re;              // 正規表現オブジェクト

        // 正規表現で置換（結果を返却）
        return _re.Replace(str, _trim_regex_str, "");
    }

public :
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    String() : Object()
    {
std::cout << "0\n";
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    String(const char*& value) : Object()
    {
        this->m_Data << value;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    String(const std::string& value) : Object()
    {
        this->m_Data << value;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    String(const std::stringstream& value) : Object()
    {
        this->m_Data << value.str();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    String(const String& value) : Object()
    {
        this->m_Data << value.m_Data.str();
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    ~String()
    {
        // クリア
        this->clear();
    }

    //--------------------------------------------------------------------------
    // operator =
    //--------------------------------------------------------------------------
    String& operator = (const char*& value)
    {
        // クリア
        this->clear();

        // 設定
        this->m_Data << value;

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator =
    //--------------------------------------------------------------------------
    String& operator = (const std::stringstream& value)
    {
        // クリア
        this->clear();

        // 設定
        this->m_Data << value.str();

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator =
    //--------------------------------------------------------------------------
    String& operator = (const std::string& value)
    {
        // クリア
        this->clear();

        // 設定
        this->m_Data << value;

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator =
    //--------------------------------------------------------------------------
    String& operator = (const String& value)
    {
        // クリア
        this->clear();

        // 設定
        this->m_Data << value.m_Data.str();

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator +=
    //--------------------------------------------------------------------------
    String& operator += (const char*& value)
    {
        // 設定
        this->m_Data << value;

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator +=
    //--------------------------------------------------------------------------
    String& operator += (const std::stringstream& value)
    {
        // 設定
        this->m_Data << value.str();

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator +=
    //--------------------------------------------------------------------------
    String& operator += (const std::string& value)
    {
        // 設定
        this->m_Data << value;

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator +=
    //--------------------------------------------------------------------------
    String& operator += (const String& value)
    {
        // 設定
        this->m_Data << value.m_Data.str();

        // 自身を返却
        return *this;
    }

    //--------------------------------------------------------------------------
    // operator <<
    //--------------------------------------------------------------------------
    std::ostream& operator << (const char*& value)
    {
        // 追加
        return this->append(value);
    }

    //--------------------------------------------------------------------------
    // operator <<
    //--------------------------------------------------------------------------
    std::ostream& operator << (const std::stringstream& value)
    {
        // 追加
        return this->append(value);
    }

    //--------------------------------------------------------------------------
    // operator <<
    //--------------------------------------------------------------------------
    std::ostream& operator << (const std::string& value)
    {
        // 追加
        return this->append(value);
    }

    //--------------------------------------------------------------------------
    // operator <<
    //--------------------------------------------------------------------------
    std::ostream& operator << (const String& value)
    {
        // 追加
        return this->append(value);
    }

    //--------------------------------------------------------------------------
    // operator >>
    //--------------------------------------------------------------------------
    std::istream& operator >> (std::stringstream& value)
    {
        // 追加
        value <<  this->m_Data.str();

        // オブジェクト返却
        return value;
    }

    //--------------------------------------------------------------------------
    // operator >>
    //--------------------------------------------------------------------------
    std::string& operator >> (std::string& value)
    {
        // 追加
        value += this->m_Data.str();

        // オブジェクト返却
        return value;
    }

    //--------------------------------------------------------------------------
    // operator >>
    //--------------------------------------------------------------------------
    String& operator >> (String& value)
    {
        // 追加
        value.m_Data << this->m_Data.str();

        // オブジェクト返却
        return value;
    }

    //--------------------------------------------------------------------------
    // operator []
    //--------------------------------------------------------------------------
    const char operator [] (size_t pos)
    {
        return this->m_Data.str()[pos];
    }

    //--------------------------------------------------------------------------
    // 先頭の要素を指すイテレータを取得する
    //--------------------------------------------------------------------------
    std::string::iterator begin()
    {
        return this->m_Data.str().begin();
    }

    //--------------------------------------------------------------------------
    // 末尾の次を指すイテレータを取得する
    //--------------------------------------------------------------------------
    std::string::iterator end()
    {
        return this->m_Data.str().end();
    }

    //--------------------------------------------------------------------------
    // 文字／文字列を追加する
    //--------------------------------------------------------------------------
    std::ostream& append(const char*& value)
    {
        // 追加
        this->m_Data << value;

        // std::stringstreamを返却
        return this->m_Data;
    }

    //--------------------------------------------------------------------------
    // 文字／文字列を追加する
    //--------------------------------------------------------------------------
    std::ostream& append(const std::stringstream& value)
    {
        // 追加
        this->m_Data << value;

        // std::stringstreamを返却
        return this->m_Data;
    }

    //--------------------------------------------------------------------------
    // 文字／文字列を追加する
    //--------------------------------------------------------------------------
    std::ostream& append(const std::string& value)
    {
        // 追加
        this->m_Data << value;

        // std::stringstreamを返却
        return this->m_Data;
    }

    //--------------------------------------------------------------------------
    // 文字／文字列を追加する
    //--------------------------------------------------------------------------
    std::ostream& append(const String& value)
    {
        // 追加
        this->m_Data << value.m_Data.str();

        // std::stringstreamを返却
        return this->m_Data;
    }

    //--------------------------------------------------------------------------
    // クリア
    //--------------------------------------------------------------------------
    void clear()
    {
        // バッファをクリアします。
        this->m_Data.str("");

        // 状態をクリアします。
        this->m_Data.clear(std::stringstream::goodbit);
    }

    //--------------------------------------------------------------------------
    // サイズ取得
    //--------------------------------------------------------------------------
    size_t size()
    {
        return this->m_Data.str().size();
    }

    //--------------------------------------------------------------------------
    // サイズ取得
    //--------------------------------------------------------------------------
    size_t length()
    {
        return this->m_Data.str().length();
    }

    //--------------------------------------------------------------------------
    // 文字列が空かどうかを判定する
    //--------------------------------------------------------------------------
    bool empty()
    {
        return this->m_Data.str().empty();
    }

    //--------------------------------------------------------------------------
    // 文字列返却
    //--------------------------------------------------------------------------
    std::string str()
    {
        return this->m_Data.str();
    }

    //--------------------------------------------------------------------------
    // ポインタ返却
    //--------------------------------------------------------------------------
    const char* c_str()
    {
        return this->m_Data.str().c_str();
    }

    //--------------------------------------------------------------------------
    // 部分文字列返却
    //--------------------------------------------------------------------------
    std::string substr(size_t pos)
    {
        return this->m_Data.str().substr(pos);
    }

    //--------------------------------------------------------------------------
    // 部分文字列返却
    //--------------------------------------------------------------------------
    std::string substr(size_t pos, size_t size)
    {
        return this->m_Data.str().substr(pos, size);
    }

    //--------------------------------------------------------------------------
    // 数値化(int)
    //--------------------------------------------------------------------------
    int ToInt()
    {
        // 数値化用オブジェクト生成
        std::istringstream _istringstream(this->m_Data.str());
        int _ToIntValue;
        _istringstream >> _ToIntValue;

        // 値を返却
        return _ToIntValue;
    }

    //--------------------------------------------------------------------------
    // 文字列分割関数
    //--------------------------------------------------------------------------
    std::vector<std::string> Split(char sep)
    {
        std::vector<std::string> v;
        std::stringstream ss(this->m_Data.str());
        std::string buffer;
        while(std::getline(ss, buffer, sep))
        {
            v.push_back(buffer);
        }
        return v;
    }

    //--------------------------------------------------------------------------
    // 文字列改行削除関数
    //--------------------------------------------------------------------------
    std::string DeleteCrlf()
    {
        // 自身をクリア
        this->clear();

        // 改行を削除
        const char CR = '\r';
        const char LF = '\n';
        std::string destStr;
        for(std::string::const_iterator it=this->m_Data.str().begin(); it!=this->m_Data.str().end(); ++it)
        {
            if( *it != CR && *it != LF )
            {
                destStr += *it;
            }
        }

        // 文字列を返却
        return destStr;
    }

    //--------------------------------------------------------------------------
    // 空白削除(前後)関数
    //--------------------------------------------------------------------------
    std::string Trim()
    {
        // 空白削除文字列返却
        return this->Trim("[ |\t]");
    }

    //--------------------------------------------------------------------------
    // 文字列削除(前後)関数
    //--------------------------------------------------------------------------
    std::string Trim(const std::string trim)
    {
        std::string _trim_string;           // 空白削除文字列


        // 先頭空白削除
        _trim_string = this->TrimLeft(this->m_Data.str(), trim);

        // 後方空白削除
        _trim_string = this->TrimRight(_trim_string, trim);

        // 空白削除文字列返却
        return _trim_string;
    }

    //-----------------------------------------------------------------------------
    // 空白削除(先頭)関数
    //-----------------------------------------------------------------------------
    std::string TrimLeft()
    {
        // 空白削除文字列返却
        return this->TrimLeft("[ |\t]");
    }

    //-----------------------------------------------------------------------------
    // 文字列削除(先頭)関数
    //-----------------------------------------------------------------------------
    std::string TrimLeft(const std::string trim)
    {
        // 削除文字列返却
        return this->TrimLeft(this->m_Data.str(), trim);
    }

    //-----------------------------------------------------------------------------
    // 空白削除(末尾)関数
    //-----------------------------------------------------------------------------
    std::string TrimRight()
    {
        // 空白削除文字列返却
        return this->TrimRight("[ |\t]");
    }

    //-----------------------------------------------------------------------------
    // 文字列削除(末尾)関数
    //-----------------------------------------------------------------------------
    std::string TrimRight(const std::string trim)
    {
        // 削除文字列返却
        return this->TrimRight(this->m_Data.str(), trim);
    }

    //-------------------------------------------------------------------------
    // 正規表現検索
    //-------------------------------------------------------------------------
    bool Matching(std::string regex_str)
    {
        // 正規表現検索結果を返却
        return this->Matching(regex_str, REG_EXTENDED|REG_NEWLINE);
    }

    //-------------------------------------------------------------------------
    // 正規表現検索
    //-------------------------------------------------------------------------
    bool Matching(std::string regex_str, int cflg)
    {
        RegularExpression _re;              // 正規表現オブジェクト

        // 正規表現検索結果を返却
        return _re.Matching(this->m_Data.str(), regex_str, cflg);
    }

    //-----------------------------------------------------------------------------
    // 正規表現置換
    //-----------------------------------------------------------------------------
    std::string Replace(std::string regex_str,std::string replace_str)
    {
        // 置換文字列返却
        return this->Replace(regex_str, replace_str, REG_EXTENDED|REG_NEWLINE);
    }

    //-----------------------------------------------------------------------------
    // 正規表現置換
    //-----------------------------------------------------------------------------
    std::string Replace(std::string regex_str,std::string replace_str, int cflg)
    {
        RegularExpression _re;              // 正規表現オブジェクト

        // 置換文字列返却
        return _re.Replace(this->m_Data.str(), regex_str, replace_str, cflg);
    }
};
#endif                                      // 二重インクルード防止
