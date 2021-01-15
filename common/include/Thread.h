//==============================================================================
// Threadクラスヘッダ
//==============================================================================
#ifndef _THREAD_H_                          // 二重インクルード防止
#define _THREAD_H_                          // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Object.h"
#include "Exception.h"
#include <stdio.h>
#include <bits/types.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// Thread例外クラス
//------------------------------------------------------------------------------
class ThreadException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    ThreadException(std::string format, ...)
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
// Threadクラス
//------------------------------------------------------------------------------
class Thread : public Object
{
protected:
    std::string m_name;                     // 名称
    pthread_t m_id;                         // ID
    pthread_attr_t m_attr;                  // 属性
    sigset_t m_sigmask;                     // SIGNALマスク
    bool m_isSuspend;                       // 中断状態

private:
    //--------------------------------------------------------------------------
    // 実行ランチャー
    //--------------------------------------------------------------------------
    static void* ExecuteLancher(void* args)
    {
        // クリーンアップハンドラ設定
        pthread_cleanup_push(Thread::CleanupLancher, args);

        // 実行メソッド呼出し
        reinterpret_cast<Thread*>(args)->Execute();

        // クリーンアップハンドラ解除
        pthread_cleanup_pop(1);

        // 正常終了
        return NULL;
    }

    //--------------------------------------------------------------------------
    // クリーンナップランチャー
    //--------------------------------------------------------------------------
    static void CleanupLancher(void* args)
    {
        // キャンセルメソッド呼出し
        reinterpret_cast<Thread*>(args)->Cleanup();
    }

    //--------------------------------------------------------------------------
    // シグナルハンドラ(Suspend)
    //--------------------------------------------------------------------------
    static void SuspendSignalHandler(int signum)
    {
        sigset_t signal_set;
        int no = 0;

        // 全シグナルを除外するためのシグナル・マスクの初期化
        if(sigemptyset(&signal_set) != 0)
        {
            // エラー表示
            perror("sigemptyset");
        }

        // シグナル・マスクへのシグナルの追加
        if(sigaddset(&signal_set, SIGUSR2) != 0)
        {
            // エラー表示
            perror("sigaddset");
        }

        // 非同期シグナルの待機
        if(sigwait(&signal_set, &no) != 0)
        {
            // エラー表示
            perror("sigwait");
        }
    }

    //--------------------------------------------------------------------------
    // シグナルハンドラ(Resume)
    //--------------------------------------------------------------------------
    static void ResumeSignalHandler(int signum)
    {
    }

    //--------------------------------------------------------------------------
    // 実行
    //--------------------------------------------------------------------------
    virtual void Execute()
    {
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
    // タイムアウト値取得
    //--------------------------------------------------------------------------
    void GetTimeOut(uint64_t value, struct timespec& timeout)
    {
        // タイムアウト値計算
        timeout.tv_nsec += value * 1000;
        if(timeout.tv_nsec > 1000 * 1000)
        {
            timeout.tv_sec += 1;
            timeout.tv_nsec -= 1000 * 1000;
        }
    }

    //--------------------------------------------------------------------------
    // シグナルハンドラ初期化
    //--------------------------------------------------------------------------
    void InitSignalHandler()
    {
        struct sigaction suspendAction;     // SuspendシグナルAction
        struct sigaction resumeAction;      // ResumeシグナルAction

        // SuspendシグナルAction設定
        suspendAction.sa_flags = 0;
        suspendAction.sa_handler = Thread::SuspendSignalHandler;
        sigemptyset( &suspendAction.sa_mask );

        // ResumeシグナルAction設定
        resumeAction.sa_flags = 0;
        resumeAction.sa_handler = Thread::ResumeSignalHandler;
        sigemptyset( &resumeAction.sa_mask );

        // シグナルAction登録
        sigaction(SIGUSR1, &suspendAction, 0 );
        sigaction(SIGUSR2, &resumeAction, 0 );
    }

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    Thread() : Object()
    {
        // 初期化
        this->m_name = "";
        this->m_id = 0;
        pthread_attr_init(&(this->m_attr));
        this->m_isSuspend = false;
        this->InitSignalHandler();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    Thread(pthread_t id) : Object()
    {
        // 初期設定
        this->m_name = "";
        this->m_id = id;
        pthread_attr_init(&(this->m_attr));
        this->m_isSuspend = false;
        this->InitSignalHandler();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    Thread(pthread_t id, pthread_attr_t attr) : Object()
    {
        // 初期設定
        this->m_name = "";
        this->m_id = id;
        this->m_attr = attr;
        this->m_isSuspend = false;
        this->InitSignalHandler();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    Thread(std::string name) : Object()
    {
        // 初期設定
        this->m_name = name;
        this->m_id = 0;
        pthread_attr_init(&(this->m_attr));
        this->m_isSuspend = false;
        this->InitSignalHandler();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    Thread(std::string name, pthread_attr_t attr) : Object()
    {
        // 初期設定
        this->m_name = name;
        this->m_id = 0;
        this->m_attr = attr;
        this->m_isSuspend = false;
        this->InitSignalHandler();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    Thread(const Thread& thread) : Object(thread)
    {
        // コピー
        this->m_name = thread.m_name;
        this->m_id = thread.m_id;
        this->m_attr = thread.m_attr;
        this->m_errno = thread.m_errno;
        this->m_isSuspend = thread.m_isSuspend;
        this->InitSignalHandler();
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    ~Thread()
    {
        // スレッドが終了？
        if(this->m_id == 0)
        {
            // スレッドが終了しているので、何もしない
            return;
        }

        // スレッド強制終了
        this->Stop();
    }

    //--------------------------------------------------------------------------
    // スレッドID取得
    //--------------------------------------------------------------------------
    pthread_t Id()
    {
        return this->m_id;
    }

    //--------------------------------------------------------------------------
    // 開始
    //--------------------------------------------------------------------------
    bool Start()
    {
        // スレッドが終了？
        if(this->m_id != 0)
        {
            // スレッド起動中
            return false;
        }

        // スレッド生成
        if(pthread_create(&(this->m_id), &(this->m_attr), Thread::ExecuteLancher, this) != 0)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // キャンセル
    //--------------------------------------------------------------------------
    bool Cancel()
    {
        // スレッドが終了？
        if(this->m_id == 0)
        {
            // スレッド停止中
            return false;
        }

        // スレッドキャンセル
        if(pthread_cancel(this->m_id) != 0)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 中断
    //--------------------------------------------------------------------------
    bool Suspend()
    {
        // 中断状態を判定
        if(this->m_isSuspend)
        {
            // 中断中なので何もしない
            return true;
        }

        // シグナル送信(Suspend)
        if(pthread_kill(this->m_id,SIGUSR1) != 0)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // 中断中に設定
        this->m_isSuspend = true;

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 再開
    //--------------------------------------------------------------------------
    bool Resume()
    {
        // スレッドが終了？
        if(this->m_id == 0)
        {
            // スレッド停止中
            return false;
        }

        // 中断状態を判定
        if(!this->m_isSuspend)
        {
            // 中断中ではないので何もしない
            return true;
        }

        // シグナル送信(Resume)
        if(pthread_kill(this->m_id,SIGUSR2) != 0)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // 中断解除に設定
        this->m_isSuspend = false;

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 終了
    //--------------------------------------------------------------------------
    bool Stop()
    {
        // スレッドが終了？
        if(this->m_id == 0)
        {
            // スレッド停止中
            return false;
        }

        // スレッド強制終了
        if(pthread_kill(this->m_id,SIGTERM) != 0)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 動作状態取得
    //--------------------------------------------------------------------------
    bool IsActive()
    {
        // スレッドが終了？
        if(this->m_id == 0)
        {
            // スレッド停止中
            return false;
        }

        // スレッド終了待ち(試用)
        if(pthread_tryjoin_np(this->m_id, NULL) != 0)
        {
            // スレッド動作中
            return true;
        }

        // スレッド停止中
        return false;
    }

    //--------------------------------------------------------------------------
    // 終了待ち(タイムアウトなし)
    //--------------------------------------------------------------------------
    bool WaitingCompletion()
    {
        // スレッドが終了？
        if(this->m_id == 0)
        {
            // スレッド停止中
            return false;
        }

        // スレッド終了待ち
        if(pthread_join(this->m_id, NULL) != 0)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // スレッドID初期化
        this->m_id = 0;

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 終了待ち(タイムアウトあり)
    //--------------------------------------------------------------------------
    bool WaitingCompletion(uint64_t timeOut)
    {
        struct timespec timeInfo;           // 時間情報

        // スレッドが終了？
        if(this->m_id == 0)
        {
            // スレッド停止中
            return false;
        }

        // 時間情報取得
        if (clock_gettime(CLOCK_REALTIME, &timeInfo) == -1)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // タイムアウト値取得
        this->GetTimeOut(timeOut, timeInfo);

        // スレッド終了待ち
        if(pthread_timedjoin_np(this->m_id, NULL, &timeInfo) != 0)
        {
            // エラー番号設定
            this->SetErrno();

            // 異常終了
            return false;
        }

        // スレッドID初期化
        this->m_id = 0;

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // Wait
    //--------------------------------------------------------------------------
    int Wait(time_t seconds)
    {
        // 結果を返却
        return this->Wait(seconds, 0, 0, 0);
    }

    //--------------------------------------------------------------------------
    // Wait
    //--------------------------------------------------------------------------
    int Wait(time_t seconds, long millisecond)
    {
        // 結果を返却
        return this->Wait(seconds, millisecond, 0, 0);
    }

    //--------------------------------------------------------------------------
    // Wait
    //--------------------------------------------------------------------------
    int Wait(time_t seconds, long millisecond, long microseconds)
    {
        // 結果を返却
        return this->Wait(seconds, millisecond, microseconds, 0);
    }

    //--------------------------------------------------------------------------
    // Wait
    //--------------------------------------------------------------------------
    int Wait(time_t seconds, long millisecond, long microseconds, long nanoseconds)
    {
        // timespec構造体設定
        struct timespec req;
        req.tv_sec = seconds;
        req.tv_nsec = (millisecond * 1000000) + ( microseconds * 1000) + nanoseconds;

        // 結果を返却
        return nanosleep(&req, NULL);
    }
};
#endif                                      // 二重インクルード防止
