//==============================================================================
// IniWriterクラスヘッダ
//==============================================================================
#ifndef _INI_WRITER_H_                      // 二重インクルード防止
#define _INI_WRITER_H_                      // 二重インクルード防止

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
// IniWriter例外クラス
//------------------------------------------------------------------------------
class IniWriterException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IniWriterException(std::string msg) : Exception(msg)
    {
    };

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IniWriterException(int no): Exception(no)
    {
    };
};

//-----------------------------------------------------------------------------
// IniWriterクラス
//-----------------------------------------------------------------------------
class IniWriter
{
private :
    std::string m_filename;                 // ファイル名
    std::ofstream m_stream;                 // ファイルストリーム
                                            // データitems
    std::map<std::string, std::map<std::string, std::vector<std::string> > > m_items;

private :
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
    IniWriter(std::string filename)
    {
        // 初期化
        this->m_filename = filename;
        this->m_stream.clear();
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~IniWriter()
    {
        // ファイルクローズ
        this->Close();
    }

    //--------------------------------------------------------------------------
    // ファイルオープン
    //--------------------------------------------------------------------------
    void Open()
    {
        this->m_stream.open( this->m_filename.c_str(), std::ios::out );
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
    // 設定
    //--------------------------------------------------------------------------
    void Set(std::map<std::string, std::map<std::string, std::vector<std::string> > > items)
    {
        // 元のものは破棄
        this->Destroy();
        this->m_items = items;
    }

    //--------------------------------------------------------------------------
    // 更新
    //--------------------------------------------------------------------------
    void Update(std::string session, std::string key, int index, std::string value)
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
                if(this->m_items[session][key].size() < index)
                {
                    // インデックス以下
                    this->m_items[session][key].push_back(value);
                }
                else
                {
                    // インデックスあり
                    this->m_items[session][key][index] = value;
                }
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
    // 書込み
    //--------------------------------------------------------------------------
    void Execute()
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
                // KEY=VALUE文字列化
                std::vector<std::string> _value = _key_itr->second;
                for(std::vector<std::string>::const_iterator _value_itr = _value.begin(); _value_itr != _value.end(); ++_value_itr)
                {
                    // KEY文字列化
                    _string << _key_itr->first;
                    // VALUE文字列化
                    _string << "=\"" << *_value_itr << "\"\n";
                }
            }
        }

        // 書込み
        this->Open();
        this->m_stream << _string.str();
        this->Close();
    }
};
#endif                                      // 二重インクルード防止
