//------------------------------------------------------------------------------
// TreeNodeクラスヘッダ
//------------------------------------------------------------------------------
#ifndef _TREE_NODE_H_                       // 二重インクルード防止
#define _TREE_NODE_H_                       // 二重インクルード防止

//------------------------------------------------------------------------------
// インクルードヘッダ
//------------------------------------------------------------------------------
#include <string>
#include <list>

//------------------------------------------------------------------------------
// テンプレートクラス定義
//------------------------------------------------------------------------------
template <typename TYPE>
class TreeNode {
public:
    struct Node {
        typedef std::list<Node*> Children;
        typedef typename Children::iterator Iterator;
        typedef typename Children::const_iterator CIterator;
 
        TYPE value;//ノードに関連づけられたあたい
        Children children;//子ノード
 
        Node(const TYPE& value) : value(value) {}
    };
private:
    Node* m_root;//ルートノード
public :
    typedef typename Node::Children Children;
    typedef typename Node::Iterator Iterator;
    typedef typename Node::CIterator CIterator;

public:
    // コンストラクタ
    explicit TreeNode(const TYPE& value)
    {
        //ルートノードを作成
        TreeNode::m_root = new Node(value);
    }
    // デストラクタ
    virtual ~TreeNode()
    {
        TreeNode::Clear();
        delete TreeNode::m_root;//Clearはルートノードを破棄しないので、ここで破棄します
    }
    void Clear()
    {
        TreeNode::Clear(TreeNode::m_root);
    }
    //nodeの子ノードにvalueを持ったノードを作成
    static Node* Append(Node* node, const TYPE& value) {
        Node* child = new Node(value);
        node->children.push_back(child);
        return child;
    }
    //ルートノードを取得
    Node* GetRoot() { return m_root; }
    const Node* GetRoot()const { return m_root; }
private:
    void Clear(Node* node)
    {
        //子ノードをすべて破壊
        Children& children = node->children;
        for (Iterator it = children.begin(); it != children.end(); ++it) {
            TreeNode::Clear(*it);
        }
        children.clear();//listクラステンプレートのclear

        //ルートノードだけはdeleteしない
        if (node != TreeNode::m_root) { delete node; }
    }
};
#endif                                      // 二重インクルード防止
