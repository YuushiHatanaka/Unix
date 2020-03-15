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
class TreeNodeCollection : public std::list<T>
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
    void Remove()
    {
        // 子ノード登録分繰り返し
        for(size_t i=TreeNodeCollection::Count()-1; TreeNodeCollection::Count()>0; i--)
        {
            // 子ノード削除
            TreeNodeCollection::RemoveAt(i);
        }
    }

    //--------------------------------------------------------------------------
    // 削除
    //--------------------------------------------------------------------------
    void Remove(T remove_node)
    {
        for(Iterator itr=TreeNodeCollection::begin(); itr!=TreeNodeCollection::end(); ++itr)
        {
            if(*itr==remove_node)
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
private:
    TreeNode<T>* m_Parent;                  // 親ノード
    TreeNode<T>* m_PrevNode;                // 兄ノード
    TreeNode<T>* m_NextNode;                // 弟ノード
                                            // 子ノードリスト
    TreeNodeCollection<TreeNode<T>*> m_Nodes;
    T m_Value;                              // 値

public:
    //--------------------------------------------------------------------------
    // コンストラクタ
    //--------------------------------------------------------------------------
    TreeNode(T& value)
    {
        // 初期設定
        TreeNode::m_Parent = NULL;
        TreeNode::m_PrevNode = NULL;
        TreeNode::m_NextNode = NULL;
        TreeNode::m_Value = value;
    }

    //--------------------------------------------------------------------------
    // デストラクタ
    //--------------------------------------------------------------------------
    virtual ~TreeNode()
    {
    }

    //--------------------------------------------------------------------------
    // 値
    //--------------------------------------------------------------------------
    T& Value()
    {
        return TreeNode<T>::m_Value;
    }

    //--------------------------------------------------------------------------
    // 親ノード
    //--------------------------------------------------------------------------
    TreeNode<T>*& Parent()
    {
        return TreeNode<T>::m_Parent;
    }

    //--------------------------------------------------------------------------
    // 兄ノード
    //--------------------------------------------------------------------------
    TreeNode<T>*& PrevNode()
    {
        return TreeNode<T>::m_PrevNode;
    }

    //--------------------------------------------------------------------------
    // 弟ノード
    //--------------------------------------------------------------------------
    TreeNode<T>*& NextNode()
    {
        return TreeNode<T>::m_NextNode;
    }

    //--------------------------------------------------------------------------
    // 子ノード
    //--------------------------------------------------------------------------
    TreeNodeCollection<TreeNode<T>*>& Child()
    {
        return TreeNode<T>::m_Nodes;
    }

    //--------------------------------------------------------------------------
    // 子ノード(先頭)
    //--------------------------------------------------------------------------
    TreeNode<T>* FirstNode()
    {
        return TreeNode<T>::m_Nodes.begin();
    }

    //--------------------------------------------------------------------------
    // 子ノード(最終)
    //--------------------------------------------------------------------------
    TreeNode<T>* LastNode()
    {
        return TreeNode<T>::m_Nodes.end();
    }

    //--------------------------------------------------------------------------
    // 子ノード追加
    //--------------------------------------------------------------------------
    TreeNode* AddChild(T& value)
    {
        // 子ノードオブジェクト生成
        TreeNode<T>* _AddChild = new TreeNode<T>(value);

        // 兄弟ノード設定
        if(TreeNode<T>::m_Nodes.size() > 0)
        {
            // リストの1つ前の弟ノード
            TreeNode<T>::m_Nodes[TreeNode<T>::m_Nodes.size()-1]->m_NextNode = _AddChild;

            // リストの1つ前が兄ノード
            _AddChild->m_PrevNode = TreeNode<T>::m_Nodes[TreeNode<T>::m_Nodes.size()-1];
            _AddChild->m_NextNode = NULL;
        }

        // 子ノードオブジェクト登録
        TreeNode<T>::m_Nodes.push_back(_AddChild);

        // 親ノード登録
        _AddChild->m_Parent = this;

        // 子ノードオブジェクト返却
        return _AddChild;
    }
};
#endif                                      // 二重インクルード防止
