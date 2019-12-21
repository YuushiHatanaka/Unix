//==============================================================================
// IniReaderクラスヘッダ
//==============================================================================
#ifndef _INI_READER_H_                      // 二重インクルード防止
#define _INI_READER_H_                      // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "RegularExpression.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// IniReader例外クラス
//------------------------------------------------------------------------------
class IniReaderException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IniReaderException(std::string msg) : Exception(msg)
    {
    };

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IniReaderException(int no): Exception(no)
    {
    };
};

//-----------------------------------------------------------------------------
// IniReaderクラス
//-----------------------------------------------------------------------------
class IniReader
{
private :
    std::string m_filename;                 // ファイル名
    std::ifstream m_stream;                 // ファイルストリーム
                                            // データitems
    std::map<std::string, std::map<std::string,std::string> > m_items;
private :
    //--------------------------------------------------------------------------
    // 登録
    //--------------------------------------------------------------------------
    void Regston(std::string session,std::string key,std::string value)
    {
                                            // セッション内リスト
        std::map<std::string,std::string> _section_list;

        // セッション登録判定
        std::map<std::string, std::map<std::string,std::string> >::const_iterator _section_itr = this->m_items.find(session);
        if( _section_itr != this->m_items.end() )
        {
            // セッション登録あり
            std::map<std::string,std::string> _map = this->m_items[session];
            _map[key] = value;
            this->m_items[session] = _map;
        }
        else
        {
            // セッション登録なし
            std::map<std::string,std::string> _map;
            _map[key] = value;
            this->m_items[session] = _map;
        }
    }

    //--------------------------------------------------------------------------
    // 破棄
    //--------------------------------------------------------------------------
    void Destroy()
    {
        // セッション毎に繰り返し
        for(std::map<std::string, std::map<std::string,std::string> >::const_iterator section_itr = this->m_items.begin(); section_itr != this->m_items.end(); ++section_itr)
        {
            // キー、値取り出し
            std::string _section_key = section_itr->first;
            std::map<std::string,std::string> _section_list = section_itr->second;

            // クリア
            _section_list.clear();
        }

        // 全体クリア
        this->m_items.clear();
    }

public :
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IniReader(std::string filename)
    {
        // 初期化
        this->m_filename = filename;
        this->m_stream.clear();
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~IniReader()
    {
        // ファイルクローズ
        this->Close();
    }

    //--------------------------------------------------------------------------
    // ファイルオープン
    //--------------------------------------------------------------------------
    void Open()
    {
        this->m_stream.open( this->m_filename.c_str(), std::ios::in );
    }

    //--------------------------------------------------------------------------
    // ファイルクローズ
    //--------------------------------------------------------------------------
    void Close()
    {
        if(!this->m_stream)
        {
            this->m_stream.close();
        }
    }

    //--------------------------------------------------------------------------
    // 解析
    //--------------------------------------------------------------------------
    void Parse()
    {
        std::string _readline;              // 読込行
        std::string _session = "";          // セッション名
        std::string _key;                   // キー
        std::string _value;                 // 値
        uint32_t _line_no = 0;              // 行数
        RegularExpression _regex;           // 正規表現オブジェクト

        // ファイルの開始位置を先頭にする
        this->m_stream.seekg(0, std::ios_base::beg);

        // 連想配列クリア
        this->Destroy();

        // ファイルを1行ずつ読み込む
        while(getline( this->m_stream, _readline ))
        {
            // 行数加算
            _line_no += 1;

            // 先頭からの空白を削除
            _readline = _regex.Replace(_readline, "^[ |\t]*", "");

            // 先頭からの空白を削除
            _readline = _regex.Replace(_readline, "\"[ |\t]*#.*$", "\"");

            // 空行スキップ
            if(_readline.empty())
            {
                continue;
            }

            // コメント行スキップ
            if( _readline.c_str()[0] == '#' )
            {
                continue;
            }

            // セッション設定
            if( _readline.c_str()[0] == '[' )
            {
                _session = _readline;
                // 最初の'['を削除
                _session = _session.replace( _session.find("["), 1, "" );
                // 後方の']'を削除
                _session = _session.replace( _session.rfind("]"), 1, "" );
                continue;
            }

            // セッション判定
            if ( _session.empty() )
            {
                std::stringstream _error_msg;
                std::string _throw_msg;
                _error_msg << "外部定義ファイルフォーマット異常:[";
                _error_msg << "セッション未定義";
                _error_msg << "]";
                _throw_msg = _error_msg.str();
                throw IniReaderException(_error_msg.str().c_str());
            }

            // 空行以外で'='が存在しない場合は、例外(フォーマットエラー)
            if( _readline.find("=") == std::string::npos )
            {
                std::stringstream _error_msg;
                std::string _throw_msg;
                _error_msg << "外部定義ファイルフォーマット異常:[";
                _error_msg << _readline.c_str();
                _error_msg << "]";
                _throw_msg = _error_msg.str();
                throw IniReaderException(_error_msg.str().c_str());
            }

            // 正規表現検索(一致リスト取得)関数
            std::vector<std::string> _key_value = _regex.MatchingList(_readline, "^([^ |^\t|.]+)[ |\t]*=[ |\t]*\"(.*)\"$", 2 );

            // キー登録
            if( _key_value.size() == 2 )
            {
                this->Regston( _session, _key_value[0], _key_value[1] );
            }
        }
    }

    //--------------------------------------------------------------------------
    // セッション存在判定
    //--------------------------------------------------------------------------
    bool Find(std::string session)
    {
                                            // セッション内リスト
        std::map<std::string,std::string> _section_list;

        // セッション登録判定
        std::map<std::string, std::map<std::string,std::string> >::const_iterator _section_itr = this->m_items.find(session);
        if( _section_itr != this->m_items.end() )
        {
            // セッション登録あり
            return true;
        }
        else {
            // セッション登録なし
            return false;
        }
    }

    //--------------------------------------------------------------------------
    // 要素取得
    //--------------------------------------------------------------------------
    std::map<std::string, std::map<std::string,std::string> > Items()
    {
        // 要素を返却
        return this->m_items;
    }

    //--------------------------------------------------------------------------
    // 要素取得
    //--------------------------------------------------------------------------
    std::map<std::string, std::string> Items(std::string session)
    {
        // セッション登録判定
        std::map<std::string, std::map<std::string,std::string> >::const_iterator section_itr = this->m_items.find(session);

        // 要素を返却
        return section_itr->second;
    }

    //--------------------------------------------------------------------------
    // 要素取得
    //--------------------------------------------------------------------------
    std::string Items(std::string session,std::string key)
    {
        // セッション内要素を取得
        std::map<std::string, std::string> _session_item = this->Items(session);

        // キー登録判定
        std::map<std::string,std::string>::const_iterator _itr = _session_item.find(key);
        if( _itr != _session_item.end() )
        {
            // キー登録あり
            return _itr->second;
        }
        else
        {
            // キー登録なし
            return "";
        }
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString()
    {
        std::stringstream _string;          // 返却文字列

        // セッション分繰り返し
        for(std::map<std::string, std::map<std::string,std::string> >::const_iterator _section_itr = this->m_items.begin(); _section_itr != this->m_items.end(); ++_section_itr)
        {
            // セッション文字列化
            _string << "[" << _section_itr->first << "]\n";

            // KEY分繰り返し
            std::map<std::string,std::string> _keys = _section_itr->second;
            for(std::map<std::string,std::string>::const_iterator _key_itr = _keys.begin(); _key_itr != _keys.end(); ++_key_itr)
            {
                // KEY文字列化
                _string << " " << _key_itr->first << " = ";

                // VALUE文字列化
                _string << " " << _key_itr->second << "\n";
            }

            // 改行追加
            _string << "\n";
        }

        // 文字列を返却する
        return _string.str();
    }
};
#endif                                      // 二重インクルード防止
