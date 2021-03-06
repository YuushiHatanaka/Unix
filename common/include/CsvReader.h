//==============================================================================
// CsvReaderクラスヘッダ
//==============================================================================
#ifndef _CSV_READER_H_                      // 二重インクルード防止
#define _CSV_READER_H_                      // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "File.h"
#include "String.h"
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <vector>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// CsvReader例外クラス
//------------------------------------------------------------------------------
class CsvReaderException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    CsvReaderException(std::string format, ...)
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
    CsvReaderException(int no): Exception(no)
    {
    };
};

//-----------------------------------------------------------------------------
// CsvReaderクラス
//-----------------------------------------------------------------------------
class CsvReader : public File
{
private :
    //-----------------------------------------------------------------------------
    // 読込
    //-----------------------------------------------------------------------------
    bool Read(std::string& readline)
    {
        // ファイルを1行ずつ読み込む
        while(getline(this->m_stream, readline))
        {
            // 読込行を加算する
            this->m_count += 1;

            // 空行スキップ
            if(readline.empty())
            {
                continue;
            }

            // 改行削除
            String _string(readline);
            readline = _string.DeleteCrlf();

            // 読込成功
            return true;
        }

        // 読込終了
        return false;
    }

    //-----------------------------------------------------------------------------
    // カラム取得
    //-----------------------------------------------------------------------------
    std::vector<std::string> GetColumn(std::string line)
    {
        std::vector<std::string> _columns;  // カラム一覧
        std::string _column;                // カラム
        bool _continue;                     // 継続フラグ
        String _string;                     // 文字列オブジェクト

        // 初期化
        _column = "";
        _continue = false;

        // 1文字づつ処理する
        for(size_t i=0;i<line.size();i++)
        {
            if( line[i] == this->m_field_separator )
            {
                if( !_continue )
                {
                    _string = _column;
                    _string = _string.Trim();
                    _string = _string.Trim("\"");
                    _columns.push_back(_string.str());
                    _column = "";
                }
                else
                {
                    _column += line[i];
                }
            }
            else if( line[i] == '"' )
            {
                _continue = !_continue;
                _column += line[i];
            }
            else
            {
                _column += line[i];
            }
        }

        // 最終カラムを追加
        _string = _column;
        _string = _string.Trim();
        _string = _string.Trim("\"");
        _columns.push_back(_string.str());

        // カラム返却
        return _columns;
    }

protected :
    std::ifstream m_stream;                 // ファイルストリーム
    char m_field_separator;                 // フィールドセパレータ
    uint32_t m_count;                       // 読込行数(空行を含む)

public :
    // アクセスメソッド
    uint32_t Count() { return this->m_count; }
    bool Eof() { return this->m_stream.eof(); }

public:
    //-----------------------------------------------------------------------------
    // コンストラクタ
    //-----------------------------------------------------------------------------
    CsvReader(std::string filename) : File(filename)
    {
        // 初期化
        this->m_field_separator = ',';
        this->m_count = 0;
        this->m_stream.clear();

        // 同期を切る
        std::ios::sync_with_stdio(false);
    }

    //-----------------------------------------------------------------------------
    // コンストラクタ
    //-----------------------------------------------------------------------------
    CsvReader(std::string filename, char field_separator) : File(filename)
    {
        // 初期化
        this->m_field_separator = field_separator;
        this->m_count = 0;
        this->m_stream.clear();

        // 同期を切る
        std::ios::sync_with_stdio(false);
    }

    //-----------------------------------------------------------------------------
    // コンストラクタ
    //-----------------------------------------------------------------------------
    CsvReader(const CsvReader& reader) : File(reader.m_name)
    {
        // 初期化
        this->m_field_separator = reader.m_field_separator;
        this->m_count = reader.m_count;
        this->m_stream.clear();

        // 同期を切る
        std::ios::sync_with_stdio(false);
    }

    //-----------------------------------------------------------------------------
    // デストラクタ
    //-----------------------------------------------------------------------------
    virtual ~CsvReader()
    {
        // ファイルクローズ
        this->Close();

        // 同期を再設定
        std::ios::sync_with_stdio(true);
    }

    //-----------------------------------------------------------------------------
    // ファイルオープン
    //-----------------------------------------------------------------------------
    void Open()
    {
        this->m_stream.open(this->m_name.c_str(), std::ios::in);
    }

    //-----------------------------------------------------------------------------
    // ファイルクローズ
    //-----------------------------------------------------------------------------
    void Close()
    {
        if(!this->m_stream)
        {
            this->m_stream.close();
        }
    }

    //-----------------------------------------------------------------------------
    // 読込
    //-----------------------------------------------------------------------------
    std::vector<std::string> Read()
    {
        std::string _readline;              // 読込行

        // 1行読込
        if(!this->Read(_readline))
        {
            // 読込失敗
            return std::vector<std::string>();
        }

        // 結果返却
        return this->GetColumn(_readline);
    }
};
#endif                                      // 二重インクルード防止
