//==============================================================================
// MemoryPoolクラスヘッダ
//==============================================================================
#ifndef _MEMORY_POOL_H_                     // 二重インクルード防止
#define _MEMORY_POOL_H_                     // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Exception.h"
#include "Mutex.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <list>
#include <map>

//==============================================================================
// 定数定義
//==============================================================================
//------------------------------------------------------------------------------
// メモリプールID
//------------------------------------------------------------------------------
typedef enum _MemoryPoolId
{
    MemoryPoolId_0        = 0x00000000,     //    0
    MemoryPoolId_32       = 0x00000020,     //   32byte以下
    MemoryPoolId_64       = 0x00000040,     //   64byte以下
    MemoryPoolId_128      = 0x00000080,     //  128byte以下
    MemoryPoolId_256      = 0x00000100,     //  256byte以下
    MemoryPoolId_512      = 0x00000200,     //  512byte以下
    MemoryPoolId_1024     = 0x00000400,     // 1024byte(1Kbyte)以下
    MemoryPoolId_2048     = 0x00000800,     // 2048byte(2Kbyte)以下
    MemoryPoolId_4096     = 0x00001000,     // 4096byte(4Kbyte)以下
    MemoryPoolId_Variable = 0xffffffff      // 可変長
} MemoryPoolId;

//==============================================================================
// 構造体定義
//==============================================================================
//------------------------------------------------------------------------------
// メモリ情報構造体
//------------------------------------------------------------------------------
typedef struct _PoolMemoryInfo
{
    MemoryPoolId id;                       // メモリブロックID
    size_t size;                            // データサイズ
    bool status;                            // 状態(true:使用中,false:解放)
    struct tm action;                       // 操作時間
} PoolMemoryInfo;

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// MemoryPool例外クラス
//------------------------------------------------------------------------------
class MemoryPoolException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MemoryPoolException(std::string format, ...)
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
    MemoryPoolException(int no): Exception(no)
    {
    };
};

//------------------------------------------------------------------------------
// MemoryPoolクラス
//------------------------------------------------------------------------------
class MemoryPool
{
private:
    Mutex m_mutex;                          // 排他制御クラス
    uint64_t m_currentSize;                 // 補足メモリサイズ
    std::list<MemoryPoolId> m_idlist;       // IDリストテーブル
                                            // ブロック最大サイズ
    std::map<MemoryPoolId,uint64_t> m_capacity;
                                            // 使用中テーブル
    std::map<MemoryPoolId,std::vector<void*>*> m_usedtable;
                                            // 確保テーブル
    std::map<MemoryPoolId,std::vector<void*>*> m_alloctable;
    int m_errno;                            // エラー番号

    //--------------------------------------------------------------------------
    // IDリスト初期化
    //--------------------------------------------------------------------------
    void InitializationIdList()
    {
        // IDリスト初期化
        this->m_idlist.push_back(MemoryPoolId_32);
        this->m_idlist.push_back(MemoryPoolId_64);
        this->m_idlist.push_back(MemoryPoolId_128);
        this->m_idlist.push_back(MemoryPoolId_256);
        this->m_idlist.push_back(MemoryPoolId_512);
        this->m_idlist.push_back(MemoryPoolId_1024);
        this->m_idlist.push_back(MemoryPoolId_2048);
        this->m_idlist.push_back(MemoryPoolId_4096);
    }

    //--------------------------------------------------------------------------
    // 初期化
    //--------------------------------------------------------------------------
    void Initialization()
    {
        // IDリスト初期化
        this->InitializationIdList();

        // 初期化
        for(std::list<MemoryPoolId>::const_iterator _itr = this->m_idlist.begin(); _itr != this->m_idlist.end(); ++_itr)
        {
            this->Initialization(*_itr, 0);
        }

        // 可変長利用域登録
        this->m_usedtable[MemoryPoolId_Variable] = new std::vector<void*>;
        this->m_alloctable[MemoryPoolId_Variable] = new std::vector<void*>;
    }

    //--------------------------------------------------------------------------
    // 初期化
    //--------------------------------------------------------------------------
    void Initialization(uint64_t capacity)
    {
        // IDリスト初期化
        this->InitializationIdList();

        // ブロックID毎に初期化
        for(std::list<MemoryPoolId>::const_iterator _itr = this->m_idlist.begin(); _itr != this->m_idlist.end(); ++_itr)
        {
            this->Initialization(*_itr, capacity);
        }

        // 可変長利用域登録
        this->m_usedtable[MemoryPoolId_Variable] = new std::vector<void*>;
        this->m_alloctable[MemoryPoolId_Variable] = new std::vector<void*>;
    }

    //--------------------------------------------------------------------------
    // 初期化
    //--------------------------------------------------------------------------
    void Initialization(MemoryPoolId id, uint64_t capacity)
    {
        // ブロック最大サイズ設定
        this->m_capacity[id] = capacity;

        // 最大数を算出
        uint64_t _maxsize = this->m_capacity[id] / id;

        // ブロックオブジェクトを登録
        this->m_usedtable[id] = new std::vector<void*>;
        this->m_alloctable[id] = new std::vector<void*>;

        // 領域拡張
        this->Expansion(id, _maxsize);
    }

    //--------------------------------------------------------------------------
    // 破棄
    //--------------------------------------------------------------------------
    void Destroy()
    {
        // 全てを破棄
        for(std::list<MemoryPoolId>::const_iterator _itr = this->m_idlist.begin(); _itr != this->m_idlist.end(); ++_itr)
        {
            this->Destroy(*_itr);
        }
        this->Destroy(MemoryPoolId_Variable);

        // ブロック最大サイズクリア
        this->m_capacity.clear();
    }

    //--------------------------------------------------------------------------
    // 破棄
    //--------------------------------------------------------------------------
    void Destroy(MemoryPoolId id)
    {
        // 排他設定
        this->m_mutex.Lock();

        // ブロックリストを取得
        std::vector<void*>* _used_block = this->m_usedtable[id];

        // 登録数分繰り返し
        for(std::vector<void*>::const_iterator _itr = _used_block->begin(); _itr != _used_block->end(); ++_itr)
        {
            // データ解放
            void* _freeData = *_itr;
            free(_freeData);
        }

        // リスト解放
        delete _used_block;

        // ブロックリストを取得
        std::vector<void*>* _alloc_block = this->m_alloctable[id];

        // 登録数分繰り返し
        for(std::vector<void*>::const_iterator _itr = _alloc_block->begin(); _itr != _alloc_block->end(); ++_itr)
        {
            // データ解放
            void* _freeData = *_itr;
            free(_freeData);
        }

        // リスト解放
        delete _alloc_block;

        // 排他解除
        this->m_mutex.Unlock();
    }

    //--------------------------------------------------------------------------
    // ブロックID決定
    //--------------------------------------------------------------------------
    MemoryPoolId Decision(size_t size)
    {
        MemoryPoolId _memoryPoolId = MemoryPoolId_Variable;

        // 0以上32以下
        if( size > MemoryPoolId_0 && size <= MemoryPoolId_32)
        {
            _memoryPoolId = MemoryPoolId_32;
        }
        // 32を超えて64以下
        else if( size > MemoryPoolId_32 && size <= MemoryPoolId_64)
        {
            _memoryPoolId = MemoryPoolId_64;
        }
        // 64を超えて128以下
        else if( size > MemoryPoolId_64 && size <= MemoryPoolId_128)
        {
            _memoryPoolId = MemoryPoolId_128;
        }
        // 128を超えて256以下
        else if( size > MemoryPoolId_128 && size <= MemoryPoolId_256)
        {
            _memoryPoolId = MemoryPoolId_256;
        }
        // 256を超えて512以下
        else if( size > MemoryPoolId_256 && size <= MemoryPoolId_512)
        {
            _memoryPoolId = MemoryPoolId_512;
        }
        // 512を超えて1024以下
        else if( size > MemoryPoolId_512 && size <= MemoryPoolId_1024)
        {
            _memoryPoolId = MemoryPoolId_1024;
        }
        // 1024を超えて2048以下
        else if( size > MemoryPoolId_1024 && size <= MemoryPoolId_2048)
        {
            _memoryPoolId = MemoryPoolId_2048;
        }
        // 2048を超えて4096以下
        else if( size > MemoryPoolId_2048 && size <= MemoryPoolId_4096)
        {
            _memoryPoolId = MemoryPoolId_4096;
        }
        // それ以外
        else
        {
            // 可変長利用域に収める
            return MemoryPoolId_Variable;
        }

        // 空き判定
        if(!this->IsFree(_memoryPoolId))
        {
            // 相手なければ可変長を利用する
            return MemoryPoolId_Variable;
        }

        // ブロックIDを返却
        return _memoryPoolId;
    }

    //--------------------------------------------------------------------------
    // 現在時刻取得
    //--------------------------------------------------------------------------
    void GetTime(struct tm* t)
    {
        time_t _current_time = time(NULL);
        struct tm* _now = localtime(&_current_time);
        memcpy(t, _now, sizeof(struct tm));
    }

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MemoryPool()
    {
        // 初期設定
        this->m_currentSize = 0;
        this->m_errno = 0;

        // 初期化
        this->Initialization();
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MemoryPool(uint64_t capacity)
    {
        // 初期設定
        this->m_currentSize = 0;
        this->m_errno = 0;

        // 初期化
        this->Initialization(capacity);
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MemoryPool(std::map<MemoryPoolId,uint64_t> capacity)
    {
        // 初期設定
        this->m_currentSize = 0;
        this->m_errno = 0;

        // 初期化
        this->Initialization();

        // 初期化
        for(std::map<MemoryPoolId,uint64_t>::iterator _itr = capacity.begin(); _itr != capacity.end(); ++_itr)
        {
            // ブロックID判定
            if( _itr->first == MemoryPoolId_0 || _itr->first == MemoryPoolId_Variable )
            {
                // 例外
                throw new MemoryPoolException("MemoryPool:想定外のブロックIDが指定されました");
            }

            // 個別初期化
            this->Initialization(_itr->first, _itr->second);
        }
    }

    //--------------------------------------------------------------------------
    // コピーコンストラクタ
    //--------------------------------------------------------------------------
    MemoryPool(MemoryPool& memoryBlock)
    {
        // コピー
        this->m_currentSize = memoryBlock.m_currentSize;
        this->m_capacity = memoryBlock.m_capacity;
        this->m_idlist = memoryBlock.m_idlist;
        this->m_usedtable = memoryBlock.m_usedtable;
        this->m_alloctable = memoryBlock.m_alloctable;
        this->m_errno = memoryBlock.m_errno;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~MemoryPool()
    {
        // 排他設定
        this->m_mutex.Lock();

        // 破棄
        this->Destroy();

        // 排他解除
        this->m_mutex.Unlock();
    }

    //--------------------------------------------------------------------------
    // 空き判定
    //--------------------------------------------------------------------------
    bool IsFree(MemoryPoolId id)
    {
        // ブロックリストを取得
        std::vector<void*>* _alloc_block = this->m_alloctable[id];

        // 補足メモリの空きを判定
        if(_alloc_block->size() == 0)
        {
            // 塞がりを返却
            return false;
        }

        // 空きを返却
        return true;
    }

    //--------------------------------------------------------------------------
    // 取得
    //--------------------------------------------------------------------------
    void* Get(MemoryPoolId id)
    {
        // 排他設定
        this->m_mutex.Lock();

        // 補足メモリの空きを判定
        if(!this->IsFree(id))
        {
            // 排他解除
            this->m_mutex.Unlock();

            // 可変長メモリブロックを取得
            return this->GetVariable(id);
        }

        // ブロックリストを取得
        std::vector<void*>* _alloc_block = this->m_alloctable[id];

        // データ取得
        void* _data = _alloc_block->front();

        // ヘッダ設定
        PoolMemoryInfo* _head = (PoolMemoryInfo*)_data;
        _head->status = true;
        this->GetTime(&(_head->action));

        // 先頭要素削除
        _alloc_block->erase(_alloc_block->begin());

        // ブロックリストを取得
        std::vector<void*>* _used_block = this->m_usedtable[id];

        // メモリ登録
        _used_block->push_back(_data);

        // 返却データ設定
        void* _body = (_data + sizeof(PoolMemoryInfo));

        // 排他解除
        this->m_mutex.Unlock();

        // アドレスを返却
        return _body;
    }

    //--------------------------------------------------------------------------
    // 取得
    //--------------------------------------------------------------------------
    void* Get(size_t size)
    {
        // サイズ判定
        if(size <= 0)
        {
            // 異常終了
            return NULL;
        }

        // ブロックID取得
        MemoryPoolId _id = this->Decision(size);

        // ブロックID判定
        if(_id == MemoryPoolId_Variable)
        {
            // エリア取得(可変長領域)
            return this->GetVariable(size);
        }

        // エリア取得
        return this->Get(_id);
    }

    //--------------------------------------------------------------------------
    // 取得(可変長領域)
    //--------------------------------------------------------------------------
    void* GetVariable(int size)
    {
        // 排他設定
        this->m_mutex.Lock();

        // ブロックリストを取得
        std::vector<void*>* _used_block = this->m_usedtable[MemoryPoolId_Variable];

        // メモリ補足
        void* _alloc = malloc(sizeof(PoolMemoryInfo)+size);

        // 補足判定
        if( _alloc == NULL )
        {
            // 排他解除
            this->m_mutex.Unlock();

            // エラー番号設定
            this->m_errno = errno;

            // 例外
            throw new MemoryPoolException(this->m_errno);
        }

        // ヘッダ設定
        PoolMemoryInfo* _head = (PoolMemoryInfo*)_alloc;
        _head->id = MemoryPoolId_Variable;
        _head->size = size;
        _head->status = true;
        this->GetTime(&(_head->action));

        // メモリ登録
        _used_block->push_back(_alloc);

        // 補足サイズ加算
        this->m_currentSize += (sizeof(PoolMemoryInfo)+size);
        this->m_capacity[_head->id] += (sizeof(PoolMemoryInfo)+size);

        // 返却データ設定
        void* _body = (_alloc + sizeof(PoolMemoryInfo));

        // 排他解除
        this->m_mutex.Unlock();

        // アドレスを返却
        return _body;
    }

    //--------------------------------------------------------------------------
    // 解放
    //--------------------------------------------------------------------------
    void Free(void* addr)
    {
        // データヘッダ設定
        PoolMemoryInfo* _head = (PoolMemoryInfo*)(addr - sizeof(PoolMemoryInfo));

        // 可変長領域か？
        if(_head->id == MemoryPoolId_Variable)
        {
            // 可変長領域解放
            this->FreeVariable(addr);

            // 処理終了
            return;
        }

        // 排他設定
        this->m_mutex.Lock();

        // ブロックリストを取得
        std::vector<void*>* _used_block = this->m_usedtable[_head->id];

        // ブロックリスト判定
        if(_used_block == NULL)
        {
            // 例外
            throw new MemoryPoolException("Free:ブロックIDが一致しません");
        }

        // 検索
        void* _find = NULL;
        for(std::vector<void*>::iterator _itr = _used_block->begin(); _itr != _used_block->end(); ++_itr)
        {
            // 一致判定
            if(_head  == *_itr)
            {
                // 要素保存
                _find = *_itr;

                // 要素削除
                _used_block->erase(_itr);

                // 処理終了
                break;
            }
        }

        // 検索結果判定
        if(_find == NULL)
        {
            // 排他解除
            this->m_mutex.Unlock();

            // 例外
            throw new MemoryPoolException("Frre:当該エリアなし");
        }

        // ブロックリストを取得
        std::vector<void*>* _alloc_block = this->m_alloctable[_head->id];

        // メモリ登録
        _alloc_block->push_back(_head);

        // ヘッダ状態設定
        _head->status = false;
        this->GetTime(&(_head->action));

        // 排他解除
        this->m_mutex.Unlock();
    }

    //--------------------------------------------------------------------------
    // 解放(可変長領域)
    //--------------------------------------------------------------------------
    void FreeVariable(void* addr)
    {
        // 排他設定
        this->m_mutex.Lock();

        // データヘッダ設定
        PoolMemoryInfo* _head = (PoolMemoryInfo*)(addr - sizeof(PoolMemoryInfo));

        // ブロックリストを取得
        std::vector<void*>* _used_block = this->m_usedtable[MemoryPoolId_Variable];

        // ブロックリスト判定
        if(_used_block == NULL)
        {
            // 排他解除
            this->m_mutex.Unlock();

            // 例外
            throw new MemoryPoolException("Free:ブロックIDが一致しません");
        }

        // 検索
        void* _find = NULL;
        for(std::vector<void*>::iterator _itr = _used_block->begin(); _itr != _used_block->end(); ++_itr)
        {
            // 一致判定
            if(_head  == *_itr)
            {
                // 要素保存
                _find = *_itr;

                // 要素削除
                _used_block->erase(_itr);

                // 処理終了
                break;
            }
        }

        // 検索結果判定
        if(_find == NULL)
        {
            // 排他解除
            this->m_mutex.Unlock();

            // 例外
            throw new MemoryPoolException("Frre:当該エリアなし");
        }

        // 補足サイズ減算
        this->m_currentSize -= (sizeof(PoolMemoryInfo)+_head->size);
        this->m_capacity[_head->id] -= (sizeof(PoolMemoryInfo)+_head->size);

        // ヘッダ状態設定
        _head->status = false;
        this->GetTime(&(_head->action));

        // エリア解放
        free(_find);

        // 排他解除
        this->m_mutex.Unlock();
    }

    //--------------------------------------------------------------------------
    // メモリ領域増設
    //--------------------------------------------------------------------------
    void Expansion(MemoryPoolId id, uint64_t count)
    {
        // 排他設定
        this->m_mutex.Lock();

        // メモリ領域を補足
        for(uint64_t i=0; i<count; i++)
        {
            // メモリ補足
            void* _alloc = malloc(sizeof(PoolMemoryInfo)+id);

            // 補足判定
            if( _alloc == NULL )
            {
                // エラー番号設定
                this->m_errno = errno;

                // 例外
                throw new MemoryPoolException(this->m_errno);
            }

            // ヘッダ設定
            PoolMemoryInfo* _head = (PoolMemoryInfo*)_alloc;
            _head->id = id;
            _head->size = id;
            _head->status = false;
            this->GetTime(&(_head->action));

            // メモリ登録
            this->m_alloctable[id]->push_back(_alloc);

            // 補足サイズ加算
            this->m_currentSize += (sizeof(PoolMemoryInfo)+id);
        }

        // 排他解除
        this->m_mutex.Unlock();
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString()
    {
        std::stringstream _stream;          // 文字列化オブジェクト

        // 初期設定
        _stream << "メモリ使用状況:\n";
        _stream << "　補足メモリサイズ合計     : " << std::setw(12) << this->m_currentSize << " byte\n";

        // 文字列化
        for(std::list<MemoryPoolId>::const_iterator _itr = this->m_idlist.begin(); _itr != this->m_idlist.end(); ++_itr)
        {
            _stream << this->ToString(*_itr);
        }
        _stream << this->ToString(MemoryPoolId_Variable);

        // 文字列を返却
        return _stream.str();
    }

    //--------------------------------------------------------------------------
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString(MemoryPoolId id)
    {
        std::stringstream _stream;          // 文字列化オブジェクト

        // 初期設定
        _stream << "　　ブロックID : 0x";
        _stream << std::hex << std::setw(8) << std::setfill('0');
        _stream << id << " ⇒ ";

        // ブロックリストを取得
        std::vector<void*>* _used_block = this->m_usedtable[id];
        std::vector<void*>* _alloc_block = this->m_alloctable[id];
        _stream << "[";
        _stream << std::dec  << std::setfill(' ');
        if(id == MemoryPoolId_Variable)
        {
            _stream << "使用中サイズ   : " << std::setw(12) << this->m_capacity[id] << " byte,";
        }
        else
        {
            _stream << "ブロックサイズ : " << std::setw(12) << this->m_capacity[id] << " byte,";
        }
        _stream << "使用中 : " << std::setw(8) << _used_block->size() << ",";;
        _stream << "空き : " << std::setw(8) << _alloc_block->size() << ",";;
        _stream << "合計 : " << std::setw(8) << _used_block->size() + _alloc_block->size()  ;
        _stream << "]\n";

        // 文字列を返却
        return _stream.str();
    }
};
#endif                                      // 二重インクルード防止
