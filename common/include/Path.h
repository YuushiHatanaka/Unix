//==============================================================================
// Pathクラスヘッダ
//==============================================================================
#ifndef _PATH_H_                            // 二重インクルード防止
#define _PATH_H_                            // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Object.h"
#include "Exception.h"
#include "RegularExpression.h"

#include <dirent.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include <vector>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// Path例外クラス
//------------------------------------------------------------------------------
class PathException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    PathException(std::string format, ...)
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
// Pathクラス
//------------------------------------------------------------------------------
class Path : public Object
{
protected :
    std::string m_name;                     // パス名

public:
    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    Path(std::string name) : Object()
    {
        // 初期化
        this->m_name = name;
    }

    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    Path(const Path& path) : Object(path)
    {
        // 初期化
        this->m_name = path.m_name;
    }

    //-------------------------------------------------------------------------
    // パス名取得
    //-------------------------------------------------------------------------
    std::string Name()
    {
        // パス名返却
        return this->m_name;
    }

    //-------------------------------------------------------------------------
    // 情報取得
    //-------------------------------------------------------------------------
    bool Stat(struct stat& dir_stat)
    {
        // 情報取得
        return this->Stat(this->m_name, dir_stat);
    }

    //-------------------------------------------------------------------------
    // 情報取得
    //-------------------------------------------------------------------------
    bool Stat(std::string path, struct stat& dir_stat)
    {
        // ファイル情報取得
        if(stat(path.c_str(),&dir_stat) != 0)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //-------------------------------------------------------------------------
    // 存在判定
    //-------------------------------------------------------------------------
    bool Exist()
    {
        struct stat _stat;    // ファイル情報

        // ファイル存在判定
        if(!this->Stat(_stat))
        {
            // ファイルは存在していない
            return false;
        }

        // ファイルは存在している
        return true;
    }

    //-------------------------------------------------------------------------
    // ディレクトリ名取得
    //-------------------------------------------------------------------------
    std::string GetDirectoryName()
    {
        // 終端から文字列を検索
        size_t _pos = this->m_name.size();
        for(std::string::const_iterator itr = this->m_name.end(); itr != this->m_name.begin(); --itr)
        {
            // 文字判定
            if(*itr == '/')
            {
                // ベースファイル名返却
                return this->m_name.substr(0,_pos);
            }

            // ポジション減算
            _pos--;
        }

        // 全体を返却
        return this->m_name;
    }

    //-------------------------------------------------------------------------
    // ファイル名取得
    //-------------------------------------------------------------------------
    std::string GetFileName()
    {

        // 終端から文字列を検索
        size_t _pos = this->m_name.size();
        for(std::string::const_iterator itr = this->m_name.end(); itr != this->m_name.begin(); --itr)
        {
            // 文字判定
            if(*itr == '/')
            {
                // ベースファイル名返却
                return this->m_name.substr(_pos+1);
            }

            // ポジション減算
            _pos--;
        }

        // 全体を返却
        return this->m_name;
    }

    //-------------------------------------------------------------------------
    // 拡張子取得
    //-------------------------------------------------------------------------
    std::string GetExtension()
    {
        // 終端から文字列を検索
        size_t _pos = this->m_name.size();
        for(std::string::const_iterator itr = this->m_name.end(); itr != this->m_name.begin(); --itr)
        {
            // 文字判定
            if(*itr == '.')
            {
                // 拡張子返却
                return this->m_name.substr(_pos+1);
            }

            // ポジション減算
            _pos--;
        }

        // 全体を返却(拡張子なし)
        return this->m_name;
    }

    //-------------------------------------------------------------------------
    // ベース名取得
    //-------------------------------------------------------------------------
    std::string GetFileNameWithoutExtension()
    {
        // 終端から文字列を検索
        size_t _pos = this->m_name.size();
        for(std::string::const_iterator itr = this->m_name.end(); itr != this->m_name.begin(); --itr)
        {
            // 文字判定
            if(*itr == '.')
            {
                // ベースファイル名返却
                return this->m_name.substr(0,_pos);
            }

            // ポジション減算
            _pos--;
        }

        // 全体を返却
        return this->m_name;
    }

    //-------------------------------------------------------------------------
    // ディレクトリ判定
    //-------------------------------------------------------------------------
    bool IsDirectory()
    {
        // ディレクトリ判定
        return this->IsDirectory(this->m_name);
    }

    //-------------------------------------------------------------------------
    // ディレクトリ判定
    //-------------------------------------------------------------------------
    bool IsDirectory(std::string path)
    {
        // ファイル情報取得
        struct stat _stat;
        if(!this->Stat(path,_stat))
        {
            // 異常終了
            return false;
        }

        // ディレクトリとそれ以外で処理を分ける
        if( ( _stat.st_mode & S_IFMT ) == S_IFDIR )
        {
            // ディレクトリの場合
            return true;
        }

        // ディレクトリでない場合
        return false;
    }

    //-------------------------------------------------------------------------
    // ディレクトリ作成
    //-------------------------------------------------------------------------
    bool MkDir()
    {
        // ディレクトリが存在しているか判定
        if(!this->Exist())
        {
            // 存在していない場合作成
            if( mkdir( this->m_name.c_str(), 0777 ) != 0 )
            {
                // 作成失敗
                perror("mkdir");
                return false;
            }
        }

        // 作成成功(または既に存在している)
        return true;
    }

    //-------------------------------------------------------------------------
    // ファイル一覧取得
    //-------------------------------------------------------------------------
    std::vector<std::string> FileList(bool recursion)
    {
        // ファイルリストを返却
        return this->FileList("", recursion);
    }

    //-------------------------------------------------------------------------
    // ファイル一覧取得
    //-------------------------------------------------------------------------
    std::vector<std::string> FileList(std::string extend,bool recursion)
    {
        DIR* _dir_hander;               // ディレクトリハンドラ
        std::vector<std::string> _list; // ファイルリスト

        // ディレクトリ存在判定
        if(!this->Exist())
        {
            throw new PathException(this->m_name+" : No such file or directory");
        }

        // ディレクトリオープン
        _dir_hander = opendir(this->m_name.c_str());
        if( _dir_hander == NULL )
        {
            // ファイルリストを返却
            throw new PathException(this->m_name+" : opendir failed.");
        }

        do {
            // ディレクトリを読み込む
            struct dirent* _dirent = readdir(_dir_hander);
            // 読込結果を判定
            if( _dirent == NULL )
            {
                // 読込が終わったら終了
                break;
            }

            // 特殊ディレクトリはスキップ
            if( _dirent->d_name[0] == '.' )
            {
                continue;
            }

            // ディレクトリとそれ以外で処理を分ける
            std::string _filename = this->m_name + "/" + std::string(_dirent->d_name);
            if( this->IsDirectory( _filename ) )
            {
                // ディレクトリである場合再帰読出し
                if( recursion )
                {
                    std::vector<std::string> _tmp_list = this->FileList(extend, recursion);
                    for(std::vector<std::string>::const_iterator _itr = _tmp_list.begin(); _itr != _tmp_list.end(); ++_itr)
                    {
                        _list.push_back(*_itr);
                    }
                }
            }
            else
            {
                // ディレクトリでない場合は拡張子判定
                if(!extend.empty())
                {
                    // 拡張子が指定されている場合のみ
                    RegularExpression _regex;
                    if( _regex.Matching( _filename,  "\\."+extend+"$" ) )
                    {
                        // 一致する場合追加
                        _list.push_back(_filename);
                    }
                }
                else
                {
                    // 拡張子が未指定の場合は、無条件追加
                    _list.push_back(_filename);
                }
            }
            
        } while(1);

        // ディレクトリクローズ
        closedir(_dir_hander);

        // ファイルリストを返却
        return _list;
    }

    //-------------------------------------------------------------------------
    // ファイル一覧取得
    //-------------------------------------------------------------------------
    std::vector<std::string> FileList(std::string mask,std::string extend,bool recursion)
    {
        // ファイル一覧取得関数
        std::vector<std::string> _tmp_get_file_list = this->FileList(this->m_name, extend, recursion);

        // ファイル一覧数分繰り返す
        std::vector<std::string> _get_file_list;
        for( size_t i=0; i<_tmp_get_file_list.size(); i++ )
        {
            // マスクに一致したら追加
            RegularExpression _regex;
            if( _regex.Matching( _tmp_get_file_list[i],  mask ) )
            {
                // ファイル一覧に追加
                _get_file_list.push_back(_tmp_get_file_list[i]);
            }
        }

        // ファイル一覧を返却
        return _get_file_list;
    }

    //-------------------------------------------------------------------------
    // ディスク空き容量取得
    //-------------------------------------------------------------------------
    uint64_t DiskFree()
    {
        struct statvfs _statvfs = {0};
        int _ret_statvfs;

        _ret_statvfs = statvfs( this->m_name.c_str(), &_statvfs );
        if( _ret_statvfs != 0 )
        {
            perror("statvfs");
            return 0;
        }

        uint64_t free_size = (int64_t)_statvfs.f_bfree * (int64_t)_statvfs.f_bsize;

        // 空き容量を返却する
        return free_size;
    }
};
#endif                                      // 二重インクルード防止
