//-----------------------------------------------------------------------------
// dirモジュール
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include "dir.hpp"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
dir::dir(std::string dirname)
{
    // 初期化
    dir::m_path = dirname;
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
dir::dir(const dir& dir_data)
{
    // 初期化
    dir::m_path = dir_data.m_path;
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
dir::~dir()
{
}

//-------------------------------------------------------------------------
// 情報取得
//-------------------------------------------------------------------------
bool dir::get_stat(struct stat& dir_stat)
{
    // 情報取得
    return dir::get_stat( dir::m_path, dir_stat);
}

//-------------------------------------------------------------------------
// 情報取得
//-------------------------------------------------------------------------
bool dir::get_stat(std::string path, struct stat& dir_stat)
{
    // ファイル情報取得
    if( stat(path.c_str(),&dir_stat) != 0 ) {
        // 異常終了
        return false;
    }

    // 正常終了
    return true;
}

//-------------------------------------------------------------------------
// 存在判定
//-------------------------------------------------------------------------
bool dir::exist()
{
    struct stat _stat;    // ファイル情報

    // ファイル存在判定
    if(!dir::get_stat(_stat)) {
        // ファイルは存在していない
        return false;
    }

    // ファイルは存在している
    return true;
}

//-------------------------------------------------------------------------
// ディレクトリ判定
//-------------------------------------------------------------------------
bool dir::is_dir()
{
    // ディレクトリ判定
    return dir::is_dir(dir::m_path);
}

//-------------------------------------------------------------------------
// ディレクトリ判定
//-------------------------------------------------------------------------
bool dir::is_dir(std::string path)
{
    // ファイル情報取得
    struct stat _stat;
    if(!dir::get_stat(path,_stat)) {
        // 異常終了
        return false;
    }

    // ディレクトリとそれ以外で処理を分ける
    if( ( _stat.st_mode & S_IFMT ) == S_IFDIR ) {
        // ディレクトリの場合
        return true;
    }

    // ディレクトリでない場合
    return false;
}

//-------------------------------------------------------------------------
// ディレクトリ作成
//-------------------------------------------------------------------------
bool dir::mk_dir()
{
    // ディレクトリが存在しているか判定
    if( !dir::exist() ) {
        // 存在していない場合作成
        if( mkdir( dir::m_path.c_str(), 0777 ) != 0 ) {
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
std::vector<std::string> dir::get_file_list(bool recursion)
{
    // ファイルリストを返却
    return dir::get_file_list("", recursion);
}

//-------------------------------------------------------------------------
// ファイル一覧取得
//-------------------------------------------------------------------------
std::vector<std::string> dir::get_file_list(std::string extend,bool recursion)
{
    DIR* _dir_hander;               // ディレクトリハンドラ
    std::vector<std::string> _list; // ファイルリスト

    // ディレクトリ存在判定
    if(!dir::exist()) {
        throw(dir::m_path+" : No such file or directory");
    }

    // ディレクトリオープン
    _dir_hander = opendir(dir::m_path.c_str());
    if( _dir_hander == NULL ) {
        // ファイルリストを返却
        throw(dir::m_path+" : opendir failed.");
    }

    do {
        // ディレクトリを読み込む
        struct dirent* _dirent = readdir(_dir_hander);
        // 読込結果を判定
        if( _dirent == NULL ) {
            // 読込が終わったら終了
            break;
        }
        // 特殊ディレクトリはスキップ
        if( _dirent->d_name[0] == '.' ) {
            continue;
        }
        // ディレクトリとそれ以外で処理を分ける
        std::string _filename = dir::m_path + "/" + std::string(_dirent->d_name);
        if( dir::is_dir( _filename ) ) {
            // ディレクトリである場合再帰読出し
            if( recursion ) {
                std::vector<std::string> _tmp_list = dir::get_file_list( extend, recursion );
                for(std::vector<std::string>::const_iterator _itr = _tmp_list.begin(); _itr != _tmp_list.end(); ++_itr){
                    _list.push_back(*_itr);
                }
            }
        }
        else {
            // ディレクトリでない場合は拡張子判定
            if(!extend.empty()) {
                // 拡張子が指定されている場合のみ
                regular_expression _regex;
                if( _regex.matching( _filename,  "\\."+extend+"$" ) ) {
                    // 一致する場合追加
                    _list.push_back(_filename);
                }
            }
            else {
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
std::vector<std::string> dir::get_file_list(std::string mask,std::string extend,bool recursion)
{
    // ファイル一覧取得関数
    std::vector<std::string> _tmp_get_file_list = dir::get_file_list( dir::m_path, extend, recursion );

    // ファイル一覧数分繰り返す
    std::vector<std::string> _get_file_list;
    for( size_t i=0; i<_tmp_get_file_list.size(); i++ ) {
        // マスクに一致したら追加
        regular_expression _regex;
        if( _regex.matching( _tmp_get_file_list[i],  mask ) ) {
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
uint64_t dir::get_diskfree()
{
    struct statvfs _statvfs = {0};
    int _ret_statvfs;

    _ret_statvfs = statvfs( dir::m_path.c_str(), &_statvfs );
    if( _ret_statvfs != 0 ) {
        perror("statvfs");
        return 0;
    }

    uint64_t free_size = (int64_t)_statvfs.f_bfree * (int64_t)_statvfs.f_bsize;

    // 空き容量を返却する
    return free_size;
}
