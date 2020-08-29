//==============================================================================
// LocalQueueクラスヘッダ
//==============================================================================
#ifndef _LOCAL_QUEUE_H_                     // 二重インクルード防止
#define _LOCAL_QUEUE_H_                     // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Object.h"
#include "Exception.h"
#include <sys/msg.h>

//==============================================================================
// 構造体定義
//==============================================================================
//------------------------------------------------------------------------------
// LocalQueueバッファ
//------------------------------------------------------------------------------
typedef struct _LocalQueueBuf
{
    long mtype;                             /* message type, must be > 0 */
    char mtext[1];                          /* message data              */
} LocalQueueBuf;

//------------------------------------------------------------------------------
// LocalQueueノード
//------------------------------------------------------------------------------
typedef struct _LocalQueueNode
{
    void* data;
    size_t size;
    long mtype;
    struct _LocalQueueNode* next;
} LocalQueueNode;

//------------------------------------------------------------------------------
// LocalQueue構造体
//------------------------------------------------------------------------------
typedef struct _LocalQueueRoot
{
    char name[256+1];
    size_t size;
    LocalQueueNode* front;
    LocalQueueNode* rear;
} LocalQueueRoot;

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// LocalQueue例外クラス
//------------------------------------------------------------------------------
class LocalQueueException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalQueueException(std::string format, ...)
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
    LocalQueueException(int err) : Exception(err)
    {
    };

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalQueueException(std::string msg, int err) : Exception(msg, err)
    {
    };
};

//------------------------------------------------------------------------------
// LocalQueueクラス
//------------------------------------------------------------------------------
class LocalQueue : public Object
{
private:
    size_t m_Capacity;                      // LocalQueue最大数
    LocalQueueRoot* m_RootNode;                  // ルートノード構造体

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalQueue()
    {
        // 初期化
        this->m_Capacity = 512;
        this->m_RootNode = new LocalQueueRoot();
        memset(this->m_RootNode, 0x00, sizeof(LocalQueueRoot));
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalQueue(std::string name)
    {
        // 初期化
        this->m_Capacity = 512;
        this->m_RootNode = new LocalQueueRoot();
        memset(this->m_RootNode, 0x00, sizeof(LocalQueueRoot));
        strncpy(this->m_RootNode->name, name.c_str(), sizeof(this->m_RootNode->name)-1);
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalQueue(size_t capacity)
    {
        // 初期化
        this->m_Capacity = capacity;
        this->m_RootNode = new LocalQueueRoot();
        memset(this->m_RootNode, 0x00, sizeof(LocalQueueRoot));
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalQueue(std::string name, size_t capacity)
    {
        // 初期化
        this->m_Capacity = capacity;
        this->m_RootNode = new LocalQueueRoot();
        memset(this->m_RootNode, 0x00, sizeof(LocalQueueRoot));
        strncpy(this->m_RootNode->name, name.c_str(), sizeof(this->m_RootNode->name)-1);
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    LocalQueue(const LocalQueue& queue)
    {
        // 初期化
        this->m_Capacity = queue.m_Capacity;
        this->m_RootNode = queue.m_RootNode;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~LocalQueue()
    {
        // 破棄
        this->Clear();
        delete this->m_RootNode;
    }

    //--------------------------------------------------------------------------
    // 空判定
    //--------------------------------------------------------------------------
    bool IsEmpty()
    {
        // ROOTノード判定
        if(this->m_RootNode->front != NULL)
        {
            // LocalQueueあり
            return true;
        }

        // LocalQueueなし
        return false;
    }

    //--------------------------------------------------------------------------
    // Enqueue
    //--------------------------------------------------------------------------
    bool Enqueue(const void* elements, size_t size)
    {
        if(this->m_RootNode->size >= this->m_Capacity)
        {
            // TODO:異常終了
            errno = EAGAIN;
            return false;
        }

        LocalQueueNode* node = (LocalQueueNode*) malloc(sizeof(LocalQueueNode));
        if (!node)
        {
            // TODO:異常終了
            return false;
        }

        node->data = malloc(size + sizeof(long));
        if (!node->data)
        {
            // TODO:異常終了

            // リソース解放
            free(node);
            return false;
        }

        memcpy(node->data, elements, size + sizeof(long));

        node->size = size;
        node->next = NULL;

        LocalQueueBuf* buf = (LocalQueueBuf*)elements;
        node->mtype = buf->mtype;

        if (this->m_RootNode->front == NULL)
        {
            this->m_RootNode->front = this->m_RootNode->rear = node;
        }
        else
        {
            this->m_RootNode->rear->next = node;
            this->m_RootNode->rear = this->m_RootNode->rear->next;
        }
        ++this->m_RootNode->size;

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // Dequeue
    //--------------------------------------------------------------------------
    ssize_t Dequeue(void* elements, size_t size, long mtype, int flag)
    {
        if (this->m_RootNode->front == NULL)
        {
            // TODO:異常終了
            errno = EAGAIN;
            return false;
        }

        LocalQueueNode* node;
        LocalQueueNode* pre_node = this->m_RootNode->front;

        if(mtype == 0)
        {
            node = this->m_RootNode->front;
        }
        else
        {
            for(node = this->m_RootNode->front; node != NULL; node = node->next)
            {
                if(node->mtype == mtype)
                {
                    break;
                }
                pre_node = node;
            }
        }

        if(node == NULL)
        {
            // TODO:異常終了
            errno = EAGAIN;
            return false;
        }

        if(node == this->m_RootNode->front)
        {
            this->m_RootNode->front = node->next;
        }
        else
        {
            pre_node->next = node->next;
        }

        if(node == this->m_RootNode->rear)
        {
            this->m_RootNode->rear = pre_node;
            pre_node->next = NULL;
        }

        ssize_t _size;

        if (size < this->m_RootNode->size)
        {
            if (flag & MSG_NOERROR)
            {
                _size = size;
            }
            else
            {
                // TODO:異常終了
                return false;
            }
        }
        else
        {
            _size = this->m_RootNode->size;
        }

        memcpy(elements, node->data, _size + sizeof(long));

        free(node->data);
        free(node);

        --(this->m_RootNode->size);

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // 先頭取得
    //--------------------------------------------------------------------------
    bool Top(void* elements, size_t size)
    {
        if (this->m_RootNode->front == NULL)
        {
            // 異常終了(登録なし)
            return false;
        }
        if (size < this->m_RootNode->front->size)
        {
            // TODO:異常終了
            return false;
        }

        memcpy(elements, this->m_RootNode->front->data, this->m_RootNode->front->size + sizeof(long));

        // 正常終了
        return true;
    }

    //--------------------------------------------------------------------------
    // Clear
    //--------------------------------------------------------------------------
    bool Clear()
    {
        LocalQueueNode* p;

        while (this->m_RootNode->front)
        {
            p = this->m_RootNode->front;
            this->m_RootNode->front = p->next;
            free(p->data);
            free(p);
        }

        this->m_RootNode->rear = NULL;
        this->m_RootNode->size = 0;

        // 正常終了
        return true;
    }
};
#endif                                      // 二重インクルード防止
