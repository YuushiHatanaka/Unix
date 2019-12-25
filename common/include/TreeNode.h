//==============================================================================
// TreeNodeクラスヘッダ
//==============================================================================
#ifndef _TREENODE_H_                        // 二重インクルード防止
#define _TREENODE_H_                        // 二重インクルード防止

//==============================================================================
// インクルードファイル
//==============================================================================
#include <list>

//==============================================================================
// クラス定義
//==============================================================================
// TODO:実装中
//------------------------------------------------------------------------------
// TreeNodeCollectionクラス
//------------------------------------------------------------------------------
template <class T>
class TreeNodeCollection : private std::list<T>
{
private:
    typedef typename TreeNodeCollection<T>::iterator Iterator;
    typedef typename TreeNodeCollection<T>::const_iterator CIterator;

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TreeNodeCollection()
    {
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~TreeNodeCollection()
    {
        // クリア
        TreeNodeCollection::Clear();
    }

    //--------------------------------------------------------------------------
    // 追加
    //--------------------------------------------------------------------------
    size_t Add(T& add_node)
    {
        TreeNodeCollection::push_back(add_node);
        return TreeNodeCollection::size();
    }

    //--------------------------------------------------------------------------
    // 挿入
    // ※指定されているindexの前に挿入
    //--------------------------------------------------------------------------
    size_t Insert(size_t index, T insert_node)
    {
        size_t i=0;
        for(Iterator itr=TreeNodeCollection::begin(); itr!=TreeNodeCollection::end(); ++itr)
        {
            if(i==index)
            {
                TreeNodeCollection::insert(itr, insert_node);
                return TreeNodeCollection::size();
            }
            i++;
        }

        // 終端に追加
        return TreeNodeCollection::Add(insert_node);
    }

    //--------------------------------------------------------------------------
    // 削除
    //--------------------------------------------------------------------------
    void Remove(T insert_node)
    {
        for(Iterator itr=TreeNodeCollection::begin(); itr!=TreeNodeCollection::end(); ++itr)
        {
            if(*itr==insert_node)
            {
                TreeNodeCollection::erase(itr);
                return;
            }
        }
    }

    //--------------------------------------------------------------------------
    // 削除
    //--------------------------------------------------------------------------
    void RemoveAt(size_t index)
    {
        size_t i=0;
        for(Iterator itr=TreeNodeCollection::begin(); itr!=TreeNodeCollection::end(); ++itr)
        {
            if(i==index)
            {
                TreeNodeCollection::erase(itr);
                return;
            }
            i++;
        }
    }

    //--------------------------------------------------------------------------
    // クリア
    //--------------------------------------------------------------------------
    void Clear()
    {
        TreeNodeCollection::clear();
    }

    //--------------------------------------------------------------------------
    // operator
    //--------------------------------------------------------------------------
    T operator [] (size_t index)
    {
        size_t i=0;
        for(CIterator itr=TreeNodeCollection::begin(); itr!=TreeNodeCollection::end(); ++itr)
        {
            if(i==index)
            {
                return *itr;
            }
            i++;
        }
        return *(TreeNodeCollection::end());
    }

    //--------------------------------------------------------------------------
    // カウンタ
    //--------------------------------------------------------------------------
    size_t Count()
    {
        return TreeNodeCollection::size();
    }
};

//------------------------------------------------------------------------------
// TreeNodeクラス
//------------------------------------------------------------------------------
template <typename T>
class TreeNode
{
public:
    TreeNode* Parent;                       // 親ノード
    TreeNode* PrevNode;                     // 兄弟ノード(前)
    TreeNode* NextNode;                     // 兄弟ノード(後)
    TreeNodeCollection<TreeNode*> Nodes;    // 子ノード
    TreeNode* FirstNode;                    // 子ノード(先頭)
    TreeNode* LastNode;                     // 子ノード(最終)
    T Value;                                // 値

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TreeNode(T& value)
    {
        // 初期設定
        TreeNode::Value = value;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~TreeNode()
    {
        // 子ノード削除
        TreeNode::Remove();
    }

    //--------------------------------------------------------------------------
    // 子ノード削除
    //--------------------------------------------------------------------------
    void Remove()
    {
        // 子ノード登録分繰り返し
        for(size_t i=TreeNode::Nodes.Count()-1; TreeNode::Nodes.Count()>0; i--)
        {
            // 子ノード削除
            TreeNode::Nodes.RemoveAt(i);
        }
    }
};
#endif                                      // 二重インクルード防止
