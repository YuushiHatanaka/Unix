//==============================================================================
// IntervalTimerThreadクラスヘッダ
//==============================================================================
#ifndef _INTERVAL_TIMER_THREAD_H_           // 二重インクルード防止
#define _INTERVAL_TIMER_THREAD_H_           // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Thread.h"
#include <list>

//==============================================================================
// 型定義
//==============================================================================
//------------------------------------------------------------------------------
// イベントコールバック関数
//------------------------------------------------------------------------------
                                            // イベントコールバック関数
typedef void* (*IntervalTimeOutEvent)(void*);

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// IntervalTimerThread例外クラス
//------------------------------------------------------------------------------
class IntervalTimerThreadException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IntervalTimerThreadException(std::string format, ...)
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
// IntervalTimerThreadクラス
//------------------------------------------------------------------------------
class IntervalTimerThread : public Thread
{
private:
    uint64_t m_timer;                       // T.O.タイマー(msec)
    void* m_event_parameter;                // イベントパラメータ
    IntervalTimeOutEvent m_on_time_out;     // タイムアウトイベント
    uint64_t m_interval_count;              // インターバル回数(0:無限)

private:
    //--------------------------------------------------------------------------
    // 実行
    //--------------------------------------------------------------------------
    virtual void Execute()
    {
        uint64_t _count = 0;                // インターバルカウンタ

        // 無限ループ
        while(true)
        {
            // キャンセルポイント設定
            pthread_testcancel();

            // 周期動作のためsleep(msec)
            usleep(this->m_timer * 1000);

            // イベント呼出し
            this->Event();

            // インターバル回数判定
            if(this->m_interval_count)
            {
                // インターバル回数加算
                _count++;

                // 回数を超えていたら繰り返し終了
                if(_count >= this->m_interval_count)
                {
                    break;
                }
            }
        }
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
    IntervalTimerThread(uint64_t timer) : Thread()
    {
        // 初期化
        this->m_timer = timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = NULL;
        this->m_interval_count = 0;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IntervalTimerThread(uint64_t timer, uint64_t interval_count) : Thread()
    {
        // 初期化
        this->m_timer = timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = NULL;
        this->m_interval_count = interval_count;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IntervalTimerThread(std::string name, uint64_t timer) : Thread(name)
    {
        // 初期化
        this->m_timer = timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = NULL;
        this->m_interval_count = 0;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IntervalTimerThread(std::string name, uint64_t timer, uint64_t interval_count) : Thread(name)
    {
        // 初期化
        this->m_timer = timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = NULL;
        this->m_interval_count = interval_count;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IntervalTimerThread(std::string name, pthread_attr_t attr, uint64_t timer) : Thread(name, attr)
    {
        // 初期化
        this->m_timer = timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = NULL;
        this->m_interval_count = 0;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IntervalTimerThread(std::string name, pthread_attr_t attr, uint64_t timer, uint64_t interval_count) : Thread(name, attr)
    {
        // 初期化
        this->m_timer = timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = NULL;
        this->m_interval_count = interval_count;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    IntervalTimerThread(const IntervalTimerThread& thread) : Thread(thread)
    {
        // 初期設定
        this->m_timer = thread.m_timer;
        this->m_event_parameter = NULL;
        this->m_on_time_out = thread.m_on_time_out;
        this->m_interval_count = thread.m_interval_count;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~IntervalTimerThread()
    {
        // 削除
        this->RemoveTimeOutEvent();
    }

    //--------------------------------------------------------------------------
    // イベント設定
    //--------------------------------------------------------------------------
    void SetTimeOutEvent(IntervalTimeOutEvent event, void* param)
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
