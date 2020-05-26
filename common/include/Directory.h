//==============================================================================
// Directoryクラスヘッダ
//==============================================================================
#ifndef _DIRECTORY_H_                       // 二重インクルード防止
#define _DIRECTORY_H_                       // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Object.h"
#include "Exception.h"

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// Directory例外クラス
//------------------------------------------------------------------------------
class DirectoryException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    DirectoryException(std::string format, ...)
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
// Directoryクラス
//------------------------------------------------------------------------------
class Directory : public Object
{
public:
    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    Directory() : Object()
    {
    }

    //-------------------------------------------------------------------------
    // コンストラクタ
    //-------------------------------------------------------------------------
    Directory(const Directory& directory) : Object(directory)
    {
    }

    //-------------------------------------------------------------------------
    // カレントディレクトリ取得
    //-------------------------------------------------------------------------
    std::string GetCurrentDirectory()
    {
        // 変数定義
        char pathname[4096+1];              // ファイルパス

        // 変数初期化
        memset(pathname, 0x00, sizeof(pathname)); 

        // カレントディレクトリ取得
        getcwd(pathname, sizeof(pathname)-1);

        // 文字列化
        std::stringstream _path;
        _path << pathname;

        // 文字列返却
        return _path.str();
    }
};
#endif                                      // 二重インクルード防止