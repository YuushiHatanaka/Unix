//==============================================================================
// MsgQueueクラスヘッダ
//==============================================================================
#ifndef _MSG_QUEUE_H_                       // 二重インクルード防止
#define _MSG_QUEUE_H_                       // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include "Object.h"
#include "Exception.h"
#include "DateTime.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <iomanip>
#include <sstream>

//==============================================================================
// クラス定義
//==============================================================================
//------------------------------------------------------------------------------
// MsgQueue例外クラス
//------------------------------------------------------------------------------
class MsgQueueException : public Exception
{
public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MsgQueueException(std::string format, ...)
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
    MsgQueueException(int err) : Exception(err)
    {
    };

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MsgQueueException(std::string msg, int err) : Exception(msg, err)
    {
    };
};

//------------------------------------------------------------------------------
// MsgQueueクラス
//------------------------------------------------------------------------------
class MsgQueue : public Object
{
private:
    key_t m_key;                            // KEY値
    int m_msqid;                            // System V メッセージキュー識別子
    ssize_t m_rcvsize;                      // 直近の受信データサイズ
    bool m_destroy;                         // 破棄フラグ

public:
    int Msqid() { return this->m_msqid; }

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MsgQueue(int msgflg) : Object()
    {
        // 初期化
        this->m_key = IPC_PRIVATE;
        this->m_msqid = 0;
        this->m_rcvsize = 0;
        this->m_destroy = true;

        // メッセージキュー識別子取得
        if(!this->Get(msgflg))
        {
            // 例外
            throw new MsgQueueException("MsgQueue(int msgflg)", this->GetErrno());
        }
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MsgQueue(int msgflg, bool destroy) : Object()
    {
        // 初期化
        this->m_key = IPC_PRIVATE;
        this->m_msqid = 0;
        this->m_rcvsize = 0;
        this->m_destroy = destroy;

        // メッセージキュー識別子取得
        if(!this->Get(msgflg))
        {
            // 例外
            throw new MsgQueueException("MsgQueue(bool destroy, int msgflg)", this->GetErrno());
        }
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MsgQueue(key_t key, int msgflg) : Object()
    {
        // 初期化
        this->m_key = key;
        this->m_msqid = 0;
        this->m_rcvsize = 0;
        this->m_destroy = true;

        // メッセージキュー識別子取得
        if(!this->Get(msgflg))
        {
            // 例外
            throw new MsgQueueException("MsgQueue(int msgflg, key_t key)", this->GetErrno());
        }
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MsgQueue(key_t key, int msgflg, bool destroy) : Object()
    {
        // 初期化
        this->m_key = key;
        this->m_msqid = 0;
        this->m_rcvsize = 0;
        this->m_destroy = destroy;

        // メッセージキュー識別子取得
        if(!this->Get(msgflg))
        {
            // 例外
            throw new MsgQueueException("MsgQueue(key_t key, int msgflg, bool destroy)", this->GetErrno());
        }
    }

    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    MsgQueue(const MsgQueue& queue) : Object(queue)
    {
        // コピー
        this->m_key = queue.m_key;
        this->m_msqid = queue.m_key;
        this->m_errno = queue.m_errno;
        this->m_rcvsize = queue.m_rcvsize;
        this->m_destroy = queue.m_destroy;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~MsgQueue()
    {
        // 破棄
        if(this->m_destroy)
        {
            // メッセージ制御(IPC_RMID)
            this->CtrlRmid();
        }
    }

    //--------------------------------------------------------------------------
    // System V IPC キー生成
    //--------------------------------------------------------------------------
    bool Key(std::string pathname, int proj_id)
    {
        // エラー初期化
        this->ResetErrno();

        // キー生成
        this->m_key = ftok(pathname.c_str(), proj_id);

        // キー生成結果を判定
        if(this->m_key < 0)
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
    // メッセージキュー識別子取得
    //--------------------------------------------------------------------------
    bool Get(int msgflg)
    {
        // エラー初期化
        this->ResetErrno();

        // メッセージキュー識別子を取得
        this->m_msqid = msgget(this->m_key, msgflg);

        // 識別子を判定
        if(this->m_msqid < 0)
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
    // メッセージ制御(IPC_STAT)
    //--------------------------------------------------------------------------
    bool CtrlStat(struct msqid_ds& buf)
    {
        // エラー初期化
        this->ResetErrno();

        // メッセージ制御
        int _result = msgctl(this->m_msqid, IPC_STAT, &buf);

        // メッセージ制御結果判定
        if(_result < 0)
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
    // メッセージ制御(IPC_SET)
    //--------------------------------------------------------------------------
    bool CtrlSet(struct msqid_ds*& buf)
    {
        // エラー初期化
        this->ResetErrno();

        // メッセージ制御
        int _result = msgctl(this->m_msqid, IPC_SET, buf);

        // メッセージ制御結果判定
        if(_result < 0)
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
    // メッセージ制御(IPC_RMID)
    //--------------------------------------------------------------------------
    bool CtrlRmid()
    {
        // メッセージ制御(IPC_RMID)
        return this->CtrlRmid(this->m_msqid);
    }

    //--------------------------------------------------------------------------
    // メッセージ制御(IPC_RMID)
    //--------------------------------------------------------------------------
    bool CtrlRmid(int msqid)
    {
        // エラー初期化
        this->ResetErrno();

        // メッセージ制御
        int _result = msgctl(msqid, IPC_RMID, NULL);

        // メッセージ制御結果判定
        if(_result < 0)
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
    // メッセージ送信
    //--------------------------------------------------------------------------
    bool Send(const void* msgp, size_t msgsz, int msgflg)
    {
        // エラー初期化
        this->ResetErrno();

        // メッセージ送信
        int _result = msgsnd(this->m_msqid, msgp, msgsz, msgflg);

        // メッセージ送信結果判定
        if(_result < 0)
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
    // メッセージ受信
    //--------------------------------------------------------------------------
    bool Recv(void*& msgp, size_t msgsz,  long msgtyp, int msgflg, ssize_t& rcvsize)
    {
        // エラー初期化
        this->ResetErrno();

        // メッセージ受信
        this->m_rcvsize = msgrcv(this->m_msqid, msgp, msgsz, msgtyp, msgflg);

        // メッセージ受信結果格納
        rcvsize = this->m_rcvsize;

        // メッセージ送信結果判定
        if(this->m_rcvsize < 0)
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
    // 文字列化
    //--------------------------------------------------------------------------
    std::string ToString()
    {
        struct msqid_ds _msqid_ds;          // msqid_ds データ構造体
        std::stringstream _sstream;         // 文字列化Stream

        // エラーがあった場合はエラー番号を返却
        if(this->GetErrno() != 0)
        {
            // エラー番号を文字列化
            _sstream << Object::ToErrMsg();

            // 文字列を返却
            return _sstream.str();
        }

        // メッセージ制御(IPC_STAT)
        if(!this->CtrlStat(_msqid_ds))
        {
            // 文字列を返却
            return _sstream.str();
        }

        // 時間を変換
        DateTime _msg_stime(_msqid_ds.msg_stime);
        DateTime _msg_rtime(_msqid_ds.msg_rtime);
        DateTime _msg_ctime(_msqid_ds.msg_ctime);

        // メッセージキュー情報表示
        _sstream << "msqid : " << std::dec << this->m_msqid << "\n";
        _sstream << "　所有権と許可" << "\n";
        _sstream << "　└ msgget(2) に与えるキー                 : [0x" << std::setw(8) << std::setfill('0') << std::hex << _msqid_ds.msg_perm.__key << "]" << "\n";
        _sstream << "　└ 所有者の実効 UID                       : [" << std::dec << _msqid_ds.msg_perm.uid << "]" << "\n";
        _sstream << "　└ 所有者の実効 GID                       : [" << std::dec << _msqid_ds.msg_perm.gid << "]" << "\n";
        _sstream << "　└ 作成者の実効 UID                       : [" << std::dec << _msqid_ds.msg_perm.cuid << "]" << "\n";
        _sstream << "　└ 作成者の実効 GID                       : [" << std::dec << _msqid_ds.msg_perm.cgid << "]" << "\n";
        _sstream << "　└ 許可                                   : [" << std::setw(4) << std::setfill('0') << std::oct << _msqid_ds.msg_perm.mode << "]" << "\n";
        _sstream << "　└ シーケンス番号                         : [" << std::dec << _msqid_ds.msg_perm.__seq << "]" << "\n";
        _sstream << "　最後の msgsnd(2) の時刻                  : [" << _msg_stime.ToString() << "]" << "\n";
        _sstream << "　最後の msgrcv(2) の時刻                  : [" << _msg_rtime.ToString() << "]" << "\n";
        _sstream << "　最後に変更が行われた時刻                 : [" << _msg_ctime.ToString() << "]" << "\n";
        _sstream << "　キューにある現在のバイト数(非標準)       : [" << std::dec << _msqid_ds.__msg_cbytes << "]" << "\n";
        _sstream << "　キューにある現在入っているメッセージの数 : [" << std::dec << _msqid_ds.msg_qnum << "]" << "\n";
        _sstream << "　キューに許可されている最大バイト数       : [" << std::dec << _msqid_ds.msg_qbytes << "]" << "\n";
        _sstream << "　最後の msgsnd(2) の PID                  : [" << std::dec << _msqid_ds.msg_lspid << "]" << "\n";
        _sstream << "　最後の msgrcv(2) の PID                  : [" << std::dec << _msqid_ds.msg_lrpid << "]" << "\n";

        // 文字列を返却
        return _sstream.str();
    }
};
#endif                                      // 二重インクルード防止
