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
    IniReaderException(std::string format, ...)
        : Exception()
    {
        // メッセージ生成
        va_list ap;
        va_start(ap, format);
        this->SetMessage(format, ap);
        va_end(ap);
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
    std::map<std::string, std::map<std::string, std::vector<std::string> > > m_items;
private :
    //--------------------------------------------------------------------------
    // 登録
    //--------------------------------------------------------------------------
    void Regston(std::string session, std::string key, std::string value)
    {
        // セッション登録判定
        std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator _section_itr = this->m_items.find(session);
        if(_section_itr != this->m_items.end())
        {
            // セッション登録あり
            std::map<std::string, std::vector<std::string> > _map = this->m_items[session];

            // キー登録判定
            std::map<std::string, std::vector<std::string> >::iterator _key_itr = this->m_items[session].find(key);
            if(_key_itr != this->m_items[session].end())
            {
                // キー登録あり
                this->m_items[session][key].push_back(value);
            }
            else
            {
                // キー登録なし
                std::vector<std::string> _values;
                _values.push_back(value);
                this->m_items[session][key] = _values;
            }
        }
        else
        {
            // セッション登録なし(セッション登録)
            std::vector<std::string> _values;
            std::map<std::string, std::vector<std::string> > _map;
            _values.push_back(value);
            _map[key] = _values;
            this->m_items[session] = _map;
        }
    }

    //--------------------------------------------------------------------------
    // 破棄
    //--------------------------------------------------------------------------
    void Destroy()
    {
        // セッション分繰り返し
        for(std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator _section_itr = this->m_items.begin(); _section_itr != this->m_items.end(); ++_section_itr)
        {
            // キー、値取り出し
            std::string _section_key = _section_itr->first;
            std::map<std::string, std::vector<std::string> > _keys = _section_itr->second;
            for(std::map<std::string, std::vector<std::string> >::iterator _key_itr = _keys.begin(); _key_itr != _keys.end(); ++_key_itr)
            {
                // クリア
                std::vector<std::string> _values = _key_itr->second;
                std::vector<std::string>().swap(_values);
            }

            // クリア
            _keys.clear();
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
    // コンストラクタ
    //--------------------------------------------------------------------------
    IniReader(const IniReader& reader)
    {
        // コピー
        this->m_filename = reader.m_filename;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~IniReader()
    {
        // 連想配列クリア
        this->Destroy();

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
            _readline = _regex.Replace(_readline, "^[ |\t]+", "");

            // 先頭からの空白を削除
            _readline = _regex.Replace(_readline, "\"[ |\t]+#.*$", "\"");

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
                // 例外
                std::stringstream _error_msg;
                _error_msg << "外部定義ファイルフォーマット異常:[";
                _error_msg << "セッション未定義";
                _error_msg << "]";
                throw IniReaderException(_error_msg.str().c_str());
            }

            // 空行以外で'='が存在しない場合は、例外(フォーマットエラー)
            if( _readline.find("=") == std::string::npos )
            {
                // 例外
                std::stringstream _error_msg;
                _error_msg << "外部定義ファイルフォーマット異常('='記述なし):[";
                _error_msg << _readline.c_str();
                _error_msg << "]";
                throw IniReaderException(_error_msg.str().c_str());
            }

            // 正規表現検索(一致リスト取得)関数
            std::vector<std::string> _key_value = _regex.MatchingList(_readline, "^([^ |^\t|.]+)[ |\t]*=[ |\t]*\"(.*)\"$", 2 );

            // キー登録
            if( _key_value.size() == 2 )
            {
                this->Regston( _session, _key_value[0], _key_value[1] );
            }
            else
            {
                // 例外
                std::stringstream _error_msg;
                _error_msg << "外部定義ファイルフォーマット異常(キー、値分割不可):[";
                _error_msg << _readline.c_str();
                _error_msg << "]";
                throw IniReaderException(_error_msg.str().c_str());
            }
        }
    }

    //--------------------------------------------------------------------------
    // セッション存在判定
    //--------------------------------------------------------------------------
    bool Find(std::string session)
    {
        // 要素を取得
        std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator _section_itr = this->m_items.find(session);

        // セッション登録判定
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
    std::map<std::string, std::map<std::string, std::vector<std::string> > > Items()
    {
        // 要素を返却
        return this->m_items;
    }

    //--------------------------------------------------------------------------
    // 要素取得
    //--------------------------------------------------------------------------
    std::map<std::string, std::vector<std::string> > Items(std::string session)
    {
        // 要素を取得
        std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator section_itr = this->m_items.find(session);

        // 取得判定
        if(section_itr != this->m_items.end())
        {
            // セッション登録あり
            return section_itr->second;
        }
        else
        {
            // セッション登録なし
            return std::map<std::string, std::vector<std::string> >();
        }
    }

    //--------------------------------------------------------------------------
    // 要素取得
    //--------------------------------------------------------------------------
    std::vector<std::string> Items(std::string session, std::string key)
    {
        // セッション内要素を取得
        std::map<std::string, std::vector<std::string> > _session_item = this->Items(session);

        // キー登録判定
        std::map<std::string, std::vector<std::string> >::const_iterator _itr = _session_item.find(key);
        if( _itr != _session_item.end() )
        {
            // キー登録あり
            return _itr->second;
        }
        else
        {
            // キー登録なし
            return std::vector<std::string>();
        }
    }

    //--------------------------------------------------------------------------
    // 要素取得
    //--------------------------------------------------------------------------
    std::string Item(std::string session, std::string key)
    {
        // 要素取得
        std::vector<std::string> _Items = this->Items(session, key);

        // サイズ判定
        if(_Items.size() <= 0)
        {
            // 例外
            std::stringstream _error_msg;
            _error_msg << "Sesstion、Key指定異常:[";
            _error_msg << "「" << session << "」と「" << key <<"」のデータ定義はありません";
            _error_msg << "]";
            throw IniReaderException(_error_msg.str().c_str());
        }

        // 先頭を返却
        return _Items[0];
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString()
    {
        std::stringstream _string;          // 返却文字列

        // セッション分繰り返し
        for(std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator _section_itr = this->m_items.begin(); _section_itr != this->m_items.end(); ++_section_itr)
        {
            // セッション文字列化
            _string << "[" << _section_itr->first << "]\n";

            // KEY分繰り返し
            std::map<std::string, std::vector<std::string> > _keys = _section_itr->second;
            for(std::map<std::string, std::vector<std::string> >::const_iterator _key_itr = _keys.begin(); _key_itr != _keys.end(); ++_key_itr)
            {
                // KEY文字列化
                _string << " " << _key_itr->first << "\n";

                // VALUE文字列化
                std::vector<std::string> _value = _key_itr->second;

                for(std::vector<std::string>::const_iterator _value_itr = _value.begin(); _value_itr != _value.end(); ++_value_itr)
                {
                    // VALUE文字列化
                    _string << " └ " << *_value_itr << "\n";
                }
            }

            // 改行追加
            _string << "\n";
        }

        // 文字列を返却する
        return _string.str();
    }
};
#endif                                      // 二重インクルード防止
