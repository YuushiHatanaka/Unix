//==============================================================================
// StopWatchクラスヘッダ
//==============================================================================
#ifndef _STOPWATCH_H_                       // 二重インクルード防止
#define _STOPWATCH_H_                       // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "DateTime.h"
#include "TimeSpan.h"

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// StopWatch例外クラス
//------------------------------------------------------------------------------
class StopWatchException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    StopWatchException(std::string format, ...)
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
// StopWatchクラス
//------------------------------------------------------------------------------
class StopWatch
{
private:
     DateTime m_start;                      // 開始
     DateTime m_stop;                       // 終了

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    StopWatch()
    {
    }

    //--------------------------------------------------------------------------
    // コピーコンストラクタ
    //--------------------------------------------------------------------------
    StopWatch(StopWatch& stopwatch)
    {
        // コピー
        this->m_start = stopwatch.m_start;
        this->m_stop = stopwatch.m_stop;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~StopWatch()
    {
    }

    //--------------------------------------------------------------------------
    // 開始
    //--------------------------------------------------------------------------
    void Start()
    {
        // 開始
        this->m_start.Update();
    }

    //--------------------------------------------------------------------------
    // 停止
    //--------------------------------------------------------------------------
    void Stop()
    {
        // 停止
        this->m_stop.Update();
    }

    //--------------------------------------------------------------------------
    // リセット
    //--------------------------------------------------------------------------
    void Reset()
    {
        // リセット
        this->m_start.Update();
        this->m_stop.Copy(this->m_start);
    }

    //--------------------------------------------------------------------------
    // 計測結果
    //--------------------------------------------------------------------------
    TimeSpan Result()
    {
        // 計測結果を返却
        return this->m_stop - this->m_start;
    }
};
#endif                                      // 二重インクルード防止