//------------------------------------------------------------------------------
// Threadクラスヘッダ
//------------------------------------------------------------------------------
#ifndef _THREAD_H_                          // 二重インクルード防止
#define _THREAD_H_                          // 二重インクルード防止

//------------------------------------------------------------------------------
// インクルードヘッダ
//------------------------------------------------------------------------------
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
//------------------------------------------------------------------------------
// クラス定義
//------------------------------------------------------------------------------
class Thread
{
private :
    // 開始中フラグ
    bool m_startup;
    // ID
    pthread_t      m_tid;
    // 属性
    pthread_attr_t m_attr;
public:
    // コンストラクタ
    Thread()
    {
        // 初期化
        Thread::m_startup = false;
        pthread_attr_init(&(Thread::m_attr));
    }

    // コンストラクタ
    Thread(pthread_attr_t attr)
    {
        // 初期化
        Thread::m_startup = false;
        Thread::m_attr = attr;
    }

    // コンストラクタ
    Thread(Thread& thread)
    {
        // 初期化
        Thread::m_startup = thread.m_startup;
        Thread::m_tid = thread.m_tid;
        Thread::m_attr = thread.m_attr;
    }

    // デストラクタ
    virtual ~Thread()
    {
        // スレッド起動中か？
        if(Thread::m_startup)
        {
            // 停止
            Thread::stop();
        }
    }

    // 開始
    bool start()
    {
        // スレッド起動中か？
        if(Thread::m_startup)
        {
            // 異常終了
            return false;
        }

        // スレッド生成
        if(pthread_create(&(Thread::m_tid), &(Thread::m_attr), Thread::launchThread, this) != 0)
        {
            // 異常終了
            return false;
        }

        // スレッド開始中
        Thread::m_startup = true;

        // 正常終了
        return true;
    }

    // 停止
    bool stop()
    {
        int _detachstate;                   // デタッチ状態

        // デタッチ状態取得
        if(pthread_attr_getdetachstate(&(Thread::m_attr), &_detachstate) != 0)
        {
            // 異常終了
            return false;
        }

        // デタッチ状態を判定
        if(_detachstate == PTHREAD_CREATE_JOINABLE)
        {
            // キャンセル
            if(!Thread::cancel())
            {
                // 異常終了
                return false;
            }

            // スレッド終了待ち
            if(!Thread::join())
            {
                // 異常終了
                return false;
            }
        }
        else
        {
            // 強制終了
            if(!Thread::kill() != 0)
            {
                // 異常終了
                return false;
            }
        }

        // スレッド停止中
        Thread::m_startup = false;

        // 正常終了
        return true;
    }

    // キャンセル
    bool cancel()
    {
        // スレッドキャンセル
        if(pthread_cancel(Thread::m_tid) != 0)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    // 強制終了
    bool kill()
    {
        // 強制終了
        return Thread::kill(SIGTERM);
    }

    // 強制終了
    bool kill(int sig)
    {
        // 強制終了(シグナル送信)
        if(pthread_kill(Thread::m_tid, sig) != 0)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }

    // デタッチ
    bool detach()
    {
        // スレッド切り離し
        if(pthread_detach(Thread::m_tid) != 0)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }
    // 終了待ち
    bool join()
    {
        // スレッド終了待ち
        if(pthread_join(Thread::m_tid, NULL) != 0)
        {
            // 異常終了
            return false;
        }

        // 正常終了
        return true;
    }
private :
    // スレッドランチャー
    static void* launchThread(void *pParam)
    {
        // スレッド実行
        reinterpret_cast<Thread*>(pParam)->execute();
    }
    // 破棄
    void destroy()
    {
        pthread_attr_destroy(&(Thread::m_attr));
    }
protected:
    // 実行
    virtual void execute()
    {
    }
};
#endif                                      // 二重インクルード防止
