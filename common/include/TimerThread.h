//==============================================================================
// TimerThreadクラスヘッダ
//==============================================================================
#ifndef _TIMER_THREAD_H_                    // 二重インクルード防止
#define _TIMER_THREAD_H_                    // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Thread.h"
#include <list>

//==============================================================================
// 型定義
//==============================================================================
typedef void* (*TimeOutEvent)(void*);       // イベントコールバック関数

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// TimerThread例外クラス
//------------------------------------------------------------------------------
class TimerThreadException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimerThreadException(std::string format, ...)
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
// TimerThreadクラス
//------------------------------------------------------------------------------
class TimerThread : public Thread
{
private:
    uint64_t m_timer;                       // T.O.タイマー(msec)
    void* m_event_parameter;                // イベントパラメータ
    TimeOutEvent m_on_time_out;             // タイムアウトイベント

private:
    //--------------------------------------------------------------------------
    // 実行
    //--------------------------------------------------------------------------
    virtual void Execute()
    {
        // キャンセルポイント設定
        pthread_testcancel();

        // 周期動作のためsleep(msec)
        usleep(this->m_timer * 1000);

        // イベント呼出し
        this->Event();

        // スレッド終了
        pthread_exit(this);
    }

    //--------------------------------------------------------------------------
    // クリーンナップ
    //--------------------------------------------------------------------------
    virtual void Cleanup()
    {
    }

    //--------------------------------------------------------------------------
    // イベント
    //--------------------------------------------------------------------------
    virtual void Event()
    {
        // イベントハンドラ呼出し
        if(this->m_on_time_out != NULL)
        {
            this->m_on_time_out(this->m_event_parameter);
        }
    }

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimerThread(uint64_t timer) : Thread()
    {
        // 初期化
        this->m_timer = timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = NULL;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimerThread(std::string name, uint64_t timer) : Thread(name)
    {
        // 初期化
        this->m_timer = timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = NULL;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimerThread(std::string name, pthread_attr_t attr, uint64_t timer) : Thread(name, attr)
    {
        // 初期化
        this->m_timer = timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = NULL;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TimerThread(const TimerThread& thread) : Thread(thread)
    {
        // 初期設定
        this->m_timer = thread.m_timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = thread.m_on_time_out;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~TimerThread()
    {
        // 削除
        this->RemoveTimeOutEvent();
    }

    //--------------------------------------------------------------------------
    // イベント設定
    //--------------------------------------------------------------------------
    void SetTimeOutEvent(TimeOutEvent event, void* param)
    {
        // 追加
        this->m_on_time_out = event;
        this->m_event_parameter = param;
    }

    //--------------------------------------------------------------------------
    // イベント削除
    //--------------------------------------------------------------------------
    void RemoveTimeOutEvent()
    {
        // 削除
        this->m_on_time_out = NULL;
        this->m_event_parameter = NULL;
    }
};
#endif                                      // 二重インクルード防止
