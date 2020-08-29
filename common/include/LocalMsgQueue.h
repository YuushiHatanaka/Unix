//==============================================================================
// LocalMsgQueueクラスヘッダ
//==============================================================================
#ifndef _LOCAL_MSG_QUEUE_H_                 // 二重インクルード防止
#define _LOCAL_MSG_QUEUE_H_                 // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Object.h"
#include "Exception.h"
#include "LocalQueue.h"

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// LocalMsgQueue例外クラス
//------------------------------------------------------------------------------
class LocalMsgQueueException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalMsgQueueException(std::string format, ...)
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
    LocalMsgQueueException(int err) : Exception(err)
    {
    };

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalMsgQueueException(std::string msg, int err) : Exception(msg, err)
    {
    };
};

//------------------------------------------------------------------------------
// LocalMsgQueueクラス
//------------------------------------------------------------------------------
class LocalMsgQueue : public LocalQueue
{
private:
    pthread_mutex_t m_Mutex;
    pthread_cond_t m_Cond;

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalMsgQueue()
        : LocalQueue()
    {
        // 初期化
        this->m_Mutex = PTHREAD_MUTEX_INITIALIZER;
        this->m_Cond = PTHREAD_COND_INITIALIZER;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalMsgQueue(std::string name)
        : LocalQueue(name)
    {
        // 初期化
        this->m_Mutex = PTHREAD_MUTEX_INITIALIZER;
        this->m_Cond = PTHREAD_COND_INITIALIZER;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalMsgQueue(size_t capacity)
        : LocalQueue(capacity)
    {
        // 初期化
        this->m_Mutex = PTHREAD_MUTEX_INITIALIZER;
        this->m_Cond = PTHREAD_COND_INITIALIZER;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalMsgQueue(std::string name, size_t capacity)
        : LocalQueue(name, capacity)
    {
        // 初期化
        this->m_Mutex = PTHREAD_MUTEX_INITIALIZER;
        this->m_Cond = PTHREAD_COND_INITIALIZER;
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalMsgQueue(const LocalMsgQueue& queue)
        : LocalQueue(queue)
    {
        // 初期化
        this->m_Mutex = PTHREAD_MUTEX_INITIALIZER;
        this->m_Cond = PTHREAD_COND_INITIALIZER;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~LocalMsgQueue()
    {
        // 破棄
        pthread_mutex_destroy(&(this->m_Mutex));
        pthread_cond_destroy(&(this->m_Cond));
    }

    //--------------------------------------------------------------------------
    // Send
    //--------------------------------------------------------------------------
    bool Send(void* elements, size_t size, long mtype, int flag)
    {
        int ret = -1;

        pthread_mutex_lock(&(this->m_Mutex));

        if (flag & IPC_NOWAIT)
        {
            ret = this->Enqueue(elements, size);
        }
        else
        {
            while ((ret = this->Enqueue(elements, size)) == -1)
            {
                ret = pthread_cond_wait(&(this->m_Cond), &(this->m_Mutex));
                if (ret != 0)
                {
                    break;
                }
            }
        }

        pthread_mutex_unlock(&(this->m_Mutex));

        pthread_cond_broadcast(&(this->m_Cond));

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // Recv
    //--------------------------------------------------------------------------
    ssize_t Recv(void* elements, size_t size, long mtype, int flag)
    {
        ssize_t _size = -1;
        int ret;

        pthread_mutex_lock(&(this->m_Mutex));

        if (flag & IPC_NOWAIT)
        {
            _size = this->Dequeue(elements, size, mtype, flag);
        }
        else
        {
            while ((_size = this->Dequeue(elements, size, mtype, flag)) == -1)
            {
                ret = pthread_cond_wait(&(this->m_Cond), &(this->m_Mutex));
                if (ret != 0)
                {
                    break;
                }
            }
        }

        pthread_mutex_unlock(&(this->m_Mutex));

        if (_size != -1)
        {
            pthread_cond_signal(&(this->m_Cond));
        }

        // 受信サイズを返却
        return _size;
    }
};
#endif                                      // 二重インクルード防止
