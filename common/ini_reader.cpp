//-----------------------------------------------------------------------------
// ini_readerモジュール
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// インクルードファイル
//-----------------------------------------------------------------------------
#include "ini_reader.hpp"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ini_reader::ini_reader(std::string filename)
{
    // 初期化
    ini_reader::m_filename = filename;
    ini_reader::m_stream.clear();
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
ini_reader::~ini_reader()
{
    // ファイルクローズ
    ini_reader::close();
}

//-----------------------------------------------------------------------------
// ファイルオープン
//-----------------------------------------------------------------------------
void ini_reader::open()
{
    ini_reader::m_stream.open( ini_reader::m_filename.c_str(), std::ios::in );
}

//-----------------------------------------------------------------------------
// ファイルクローズ
//-----------------------------------------------------------------------------
void ini_reader::close()
{
    if( !ini_reader::m_stream ) {
        ini_reader::m_stream.close();
    }
}

//-----------------------------------------------------------------------------
// 破棄
//-----------------------------------------------------------------------------
void ini_reader::destroy()
{
    // セッション毎に繰り返し
    for(std::map<std::string, std::map<std::string,std::string> >::const_iterator section_itr = ini_reader::m_items.begin(); section_itr != ini_reader::m_items.end(); ++section_itr){
        // キー、値取り出し
        std::string _section_key = section_itr->first;
        std::map<std::string,std::string> _section_list = section_itr->second;
        // クリア
        _section_list.clear();
    }
    // 全体クリア
    ini_reader::m_items.clear();
}

//-----------------------------------------------------------------------------
// 解析
//-----------------------------------------------------------------------------
void ini_reader::parse()
{
    std::string _readline = "";     // 読込行
    std::string _session = "";      // セッション名
    std::string _key = "";          // キー
    std::string _value = "";;       // 値
    uint32_t _line_no = 0;          // 行数
    regular_expression _regex;      // 正規表現クラス

    // ファイルの開始位置を先頭にする
    ini_reader::m_stream.seekg(0, std::ios_base::beg );

    // 連想配列クリア
    ini_reader::destroy();

    // ファイルを1行ずつ読み込む
    while(std::getline( ini_reader::m_stream, _readline )){
        // 行数加算
        _line_no += 1;
        // コメント行スキップ
        if( _readline[0] == '#' ) {
            continue;
        }
        // 空行スキップ
        if( _readline.empty() ) {
            continue;
        }
        // 先頭から空白のみの行は無視(空行扱い)する
        if( _regex.matching(_readline, "^[ |\t]+$" )) {
            continue;
        }
        // 改行削除
        _readline = ini_reader::string_delete_crlf(_readline);
        // セッション設定
        if( _readline[0] == '[' ) {
            _session = _readline;
            // 最初の'['を削除
            _session = _session.replace( _session.find("["), 1, "" );
            // 後方の']'を削除
            _session = _session.replace( _session.rfind("]"), 1, "" );
            continue;
        }
        // セッション判定
        if ( _session.empty() ) {
            std::stringstream _error_msg;
            std::string _throw_msg;
            _error_msg << "外部定義ファイルフォーマット異常:[";
            _error_msg << "セッション未定義";
            _error_msg << "]";
            _throw_msg = _error_msg.str();
            throw std::string(_error_msg.str().c_str());
        }
        // 空行以外で'='が存在しない場合は、例外(フォーマットエラー)
        if( _readline.find("=") == std::string::npos ) {
            std::stringstream _error_msg;
            std::string _throw_msg;
            _error_msg << "外部定義ファイルフォーマット異常:[";
            _error_msg << _readline.c_str();
            _error_msg << "]";
            _throw_msg = _error_msg.str();
            throw std::string(_error_msg.str().c_str());
        }
        // 正規表現検索(一致リスト取得)関数
        std::vector<std::string> _key_value = _regex.matching_list(_readline, "^([^ |^\t|.]+)[ |\t]*=[ |\t]*(.*)$", 2 );
        // キー登録
        if( _key_value.size() == 2 ) {
            ini_reader::regston( _session, _key_value[0], _key_value[1] );
        }
    }
#ifdef _DEBUG
    printf("--------------------------------------------------------------------------------\n");
    printf("ini file infomation\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("filename:[%s]\n",ini_reader::m_filename.c_str());
    // セッション毎に繰り返し
    for(std::map<std::string, std::map<std::string,std::string> >::const_iterator section_itr = ini_reader::m_items.begin(); section_itr != ini_reader::m_items.end(); ++section_itr){
        // キー、値取り出し
        std::string _section_key = section_itr->first;
        std::map<std::string,std::string> _section_list = section_itr->second;
        printf("[%s]\n",_section_key.c_str());
        for(std::map<std::string,std::string>::const_iterator itr = _section_list.begin(); itr != _section_list.end(); ++itr ){
            // キー、値取り出し
            std::string _key = itr->first;
            std::string _value = itr->second;
            printf("\t[%s] = [%s]\n",_key.c_str(),_value.c_str());
        }
    }
#endif
}

//-----------------------------------------------------------------------------
// セッション存在判定
//-----------------------------------------------------------------------------
bool ini_reader::find_session(std::string session)
{
    std::map<std::string,std::string> _section_list;    // セッション内リスト
    // セッション登録判定
    std::map<std::string, std::map<std::string,std::string> >::const_iterator _section_itr = ini_reader::m_items.find(session);
    if( _section_itr != ini_reader::m_items.end() ) {
        // セッション登録あり
        return true;
    }
    else {
        // セッション登録なし
        return false;
    }
}

//-----------------------------------------------------------------------------
// 登録
//-----------------------------------------------------------------------------
void ini_reader::regston(std::string session,std::string key,std::string value)
{
    std::map<std::string,std::string> _section_list;    // セッション内リスト

    // セッション登録判定
    std::map<std::string, std::map<std::string,std::string> >::const_iterator _section_itr = ini_reader::m_items.find(session);
    if( _section_itr != ini_reader::m_items.end() ) {
        // セッション登録あり
        std::map<std::string,std::string> _map = ini_reader::m_items[session];
        _map[key] = value;
        ini_reader::m_items[session] = _map;
    }
    else {
        // セッション登録なし
        std::map<std::string,std::string> _map;
        _map[key] = value;
        ini_reader::m_items[session] = _map;
    }
}

//-----------------------------------------------------------------------------
// 要素取得
//-----------------------------------------------------------------------------
std::map<std::string, std::map<std::string,std::string> > ini_reader::items()
{
    // 要素を返却
    return ini_reader::m_items;
}

//-----------------------------------------------------------------------------
// 要素取得
//-----------------------------------------------------------------------------
std::map<std::string, std::string> ini_reader::items(std::string session)
{
    // セッション登録判定
    std::map<std::string, std::map<std::string,std::string> >::const_iterator section_itr = ini_reader::m_items.find(session);
    // 要素を返却
    return section_itr->second;
}

//-----------------------------------------------------------------------------
// 要素取得
//-----------------------------------------------------------------------------
std::string ini_reader::items(std::string session,std::string key)
{
    // セッション内要素を取得
    std::map<std::string, std::string> _session_item = ini_reader::items(session);
    // キー登録判定
    std::map<std::string,std::string>::const_iterator _itr = _session_item.find(key);
    if( _itr != _session_item.end() ) {
        // キー登録あり
        return _itr->second;
    }
    else {
        // キー登録なし
        return "";
    }
}

//-----------------------------------------------------------------------------
// 文字列改行削除関数
//-----------------------------------------------------------------------------
std::string ini_reader::string_delete_crlf(const std::string str)
{
    const char CR = '\r';
    const char LF = '\n';
    std::string destStr;

    for(std::string::const_iterator it=str.begin(); it!=str.end(); ++it) {
        if( *it != CR && *it != LF ) {
            destStr += *it;
        }
    }
    return destStr;
}
