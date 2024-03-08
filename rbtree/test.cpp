#include "pch.h"

enum Color {
    BLACK,
    RED
};


struct Node {
    int elem;
    Color color;
    struct Node* parent;
    struct Node *left, *right;
    bool isNil;
    Node()
        :isNil(true), elem(-1), color(BLACK), parent(nullptr), left(nullptr), right(nullptr)
    {
    }
    Node(Node* parent, bool left)
        :isNil(true)
    {
        this->parent = parent;
        if (left) {
            this->parent->left = this;
        }
        else {
            this->parent->right= this;
        }
    }
    Node(int elem, Color color=BLACK, Node* parent=nullptr, Node* left = nullptr, Node* right = nullptr)
        :isNil(false), elem(elem),color(color),parent(parent),left(left),right(right)
    {
        if (!left) {
            left = new Node(this, true);
        }
        if (!right) {
            right = new Node(this, right);
        }

        left->parent = this;
        right->parent = this;
    }
};

static bool color(Node* node) {
    return node->color;
}

static bool nodeColor(Node* node) {
    return node->color;
}

static bool nodeIsLeaf(Node* node) {
    return node->left->isNil && node->right->isNil;
}

static void deleteNode(Node* node) {
    assert(node);
    assert(node->left);
    assert(node->right);

    delete node->left;
    delete node->right;
    delete node;
}

class RBTree {
public:
    void insert(int elem)
    {
        Node* node = rbinsert(elem);
        insertFixup(node);
    }

    void remove(int elem)
    {
        Node* node = findNode(elem);
        if (node->isNil) {
            return;
        }
        if (node == root && nodeIsLeaf(node)) {
            deleteNode(node);
            delete node->parent;
            root = new Node;
            return;
        }
        Color originalColor;
        Node* fixupNode = rbremove(node, originalColor);
        if (originalColor == BLACK) {
            removeFixup(fixupNode);
        }
        deleteNode(node);
    }

    bool find(int elem)
    {
        return findNode(elem) != nullptr;
    }

private:
    void leftRotate(Node* node) {
        Node* r = node->right;
        if (!node->parent->isNil) {
            if (node == node->parent->left) {
                node->parent->left = r;
            }
            else {
                node->parent->right = r;
            }
            r->parent = node->parent;
        }
        else {
            root = r;
            r->parent = nullptr;
        }
        if (r->left) {
            r->left->parent = node;
            node->right = r->left;
        }
        else {
            node->right = new Node;
        }
        r->left = node;
        node->parent = r;
    }

    void rightRotate(Node* node) {
        Node* l = node->left;

        if (node->parent) {
            if (node == node->parent->left) {
                node->parent->left = l;
            }
            else {
                node->parent->right = l;
            }
            l->parent = node->parent;
        }
        else {
            root = l;
            l->parent = new Node;
        }
        if (l->right) {
            l->right->parent = node;
            node->left = l->right;
        }
        else {
            node->left = new Node;
        }
        l->right = node;
        node->parent = l;
    }

    Node* findNode(int elem) {
        Node* node = root;
        while (!node->isNil && node->elem != elem) {
            if (node->elem < elem) {
                node = node->right;
            }
            else {
                node = node->left;
            }
        }
        return node;
    }

    Node* minimum(Node* node) {
        assert(node);
        while (!node->left->isNil) {
            node = node->left;
        }
        return node;
    }

    void removeFixup(Node* node) {
        while (node != root && node->color == BLACK) {
            /*
            情况1: B的左右孩子均不为nil
                        |             
                        A             
                      /   \           
             remove->B     C   ->     
                   /   \
                  D     E
            */
            if (node == node->parent->left) {
                assert(node->parent->right);
                Node* right = node->parent->right;
                if (right->color == RED) {
                    right->color = BLACK;
                    leftRotate(node->parent);
                    right = node->parent->right;
                }
                if (nodeColor(right->left) == BLACK && nodeColor(right->right) == BLACK) {
                    right->color = RED;
                    node = node->parent;
                    continue;
                }
                // right->left颜色为红色
                if (nodeColor(right->right) == BLACK) {
                    right->left->color = BLACK;
                    right->color = RED;
                    rightRotate(right);
                    right = node->parent->right;
                }
                right->right->color = BLACK;
                leftRotate(node->parent);
                node = root;
            }
            else {
                Node* left = node->parent->left;
                if (left->color == RED) {
                    left->color = BLACK;
                    rightRotate(node->parent);
                    left = node->parent->left;
                }
                // node有两层黑色, left的child必然不为nullptr
                if (nodeColor(left->left) == BLACK && nodeColor(left->right) == BLACK) {
                    left->color = RED;
                    node = node->parent;
                    continue;
                }
                // left->right为红色
                if (nodeColor(left->left) == BLACK) {
                    left->right->color = BLACK;
                    left->color = RED;
                    leftRotate(left);
                    left = node->parent->left;
                }
                left->left->color = BLACK;
                rightRotate(node->parent);
                node = root;
            }
        }
        root->color = BLACK;
    }

    Node *rbremove(Node* node, Color &originColor)
    {
        Node* ret = nullptr;
        assert(node);

        do {
            /*
            
            ---
            1.1
            ---
            情况1: 删除B, B的左孩子为nil, E代表的子树接到B位置上,E节点不为nil
                    A
                  /   \
             --> B     C
                / \   /  \
              nil  E  F   G

            情况2: 和情况1一样, 但E节点为nil
                     A
                  /     \
                 B        C
                / \      /  \
              nil1 nil2 F    G
            */
            originColor = node->color;
            if (node->left->isNil) {
                transparent(node->right, node);
                ret = node->right;
                break;
            }
            /*
            ----
            1.2
            ----
               删除B, B的左孩子非nil,右孩子为nil, E代表的子树接到B位置上
                    
                    A
                  /   \
                 B     C
               /   \
              E    nil
            */
            if (node->right->isNil) {
                transparent(node->left, node);
                ret = node->left;
                break;
            }
            /*
             ----
             2.1
             ----
             B节点的左孩子, 右孩子均不为nil, 找到B节点的后继(successor), 移到B节点位置, 原来的successor位置由
             successor的右孩子接上(左孩子必为nil), fixupNode是successor的右孩子

               情况1: successor(G)非B节点的孩子, G节点的右孩子为非nil
                            A                 A                  A    
                          /   \             /   \              /   \  
               remove -> B     C   -->     G     C   -->      G     C 
                       /   \             /   \              /   \     
                      E     F           E     F            E     F    
                           / \               / \                / \   
                          G   H         ->  G   H          ->  J   H  
                         / \               / \                / \     
                       nil  J             nil J              nil &    
                                                                      
               情况2: successor(G)为B节点的孩子, 但G节点的右孩子为nil
                            A                   A                         
                          /   \               /   \                       
               remove -> B     C     -->     G     C                      
                       /   \               /   \                          
                      E     F             E     F                         
                           / \                 / \                        
                          G   H           ->  G   H                       
                         / \                 / \                          
                       nil1 nil2           nil1 nil2                      
                                                                         
               情况3: successor(F)为B节点的孩子
                            A                 A                      
                          /   \             /   \                    
               remove -> B     C   -->     F     C                   
                       /   \             /   \                       
                      E     F           E  -> H                      
                           / \               / \                     
                         nil  H             nil &                  
                              |
                              &

            */
            Node* successor = minimum(node->right);
            assert(successor->left->isNil);
            ret = successor;
            originColor = successor->color;
            if (successor->parent != node) {
                ret = successor->right;
                transparent(successor->right, successor);
                successor->right = node->right;
                node->right->parent = successor;
            }
            successor->left = node->left;
            node->left->parent = successor;
            successor->color = node->color;
            transparent(successor, node);
        } while (false);
        return ret;
    }

    void transparent(Node* src, Node* dest) {
        assert(dest);
        if (!dest->parent) {  // dest is the root node
            root = src;
        } else if (dest == dest->parent->left) {
            dest->parent->left = src;
        }
        else {
            dest->parent->right = src;
        }
        if (src) {
            src->parent = dest->parent;
        }
    }

    Node *rbinsert(int elem) {
        if (!root) {
            root = new Node(elem);
            return root;
        }
        Node* parent = root;
        Node* node = root;
        while (node) {
            parent = node;
            if (node->elem == elem) {
                return node;
            }
            if (node->elem < elem) {
                node = node->right;
            }
            else {
                node = node->left;
            }
        }
        node = new Node(elem, RED);
        if (parent->elem < elem) {
            parent->right = node;
        }
        else {
            parent->left = node;
        }
        node->parent = parent;
        return node;
    }

    void insertFixup(Node* node) {
        Node* parent;
        if (node == root) {
            node->color = BLACK;
            return;
        }
        while ((parent = node->parent) != root && parent->color == RED) {
            if (parent == parent->parent->left) {
                if (nodeColor(parent->parent->right) == RED) {
                    parent->parent->left->color = BLACK;
                    if (parent->parent->right) {
                        parent->parent->right->color = BLACK;
                    }
                    parent->parent->color = RED;
                    node = parent->parent;
                    if (node == root) {
                        parent = root;
                        break;
                    }
                    continue;
                }
                if (node == node->parent->right) {
                    assert(node->elem > node->parent->elem);
                    node = parent;
                    leftRotate(parent);
                    parent = node->parent;
                }
                rightRotate(parent->parent);
                parent->right->color = RED;
                break;
            }
            else {
                if (nodeColor(parent->parent->left) == RED) {
                    parent->parent->right->color = BLACK;
                    if (parent->parent->left) {
                        parent->parent->left->color = BLACK;
                    }
                    parent->parent->color = RED;
                    node = parent->parent;
                    if (node == root) {
                        parent = root;
                        break;
                    }
                    continue;
                }
                if (node == node->parent->left) {
                    assert(node->elem <= node->parent->elem);
                    node = parent;
                    rightRotate(parent);
                    parent = node->parent;
                }
                leftRotate(parent->parent);
                parent->left->color = RED;
                break;

            }
        }
        if (parent) {
            parent->color = BLACK;
        }
    }
private:
    FRIEND_TEST(RBTree, Rotate);
    FRIEND_TEST(RBTree, Insert);
    FRIEND_TEST(RBTree, Delete);
private:
    Node* root = nullptr;
};

static void makeChilds(Node* node, Node* left, Node* right)
{
    node->left = left;
    if (left) {
        left->parent = node;
    }
    node->right = right;
    if (right) {
        right->parent = node;
    }
}

TEST(RBTree, Rotate) {
    // left rotate
    /*
    
               20                            |10|                               20  
             /    \       rightRotate(20)   /    \        leftRotate(10)       /   \ 
           |10|    |40|     --->           5     20         --->             |10|   |40|
           /  \     /  \                         /  \                        /  \    /   \
          5    15   35  45                      15   |40|                   5   15  35    45
                                                     /   \                           
                                                    35   45                          

    */
    RBTree tree;
    tree.root = new Node(20);
    makeChilds(tree.root, new Node(10, RED), new Node(40, RED));
    makeChilds(tree.root->left, new Node(5, BLACK), new Node(15, BLACK));
    makeChilds(tree.root->right, new Node(35, BLACK), new Node(45, BLACK));
    tree.rightRotate(tree.root);
    EXPECT_EQ(tree.root->elem, 10);
    EXPECT_EQ(tree.root->color, RED);
    EXPECT_EQ(tree.root->parent->isNil, true);

    EXPECT_EQ(tree.root->left->elem, 5);
    EXPECT_EQ(tree.root->left->color, BLACK);
    EXPECT_EQ(tree.root->left->parent, tree.root);
    EXPECT_EQ(tree.root->right->elem, 20);
    EXPECT_EQ(tree.root->right->color, BLACK);
    EXPECT_EQ(tree.root->right->parent, tree.root);

    EXPECT_EQ(tree.root->right->left->elem, 15);
    EXPECT_EQ(tree.root->right->left->color, BLACK);
    EXPECT_EQ(tree.root->right->left->parent, tree.root->right);
    EXPECT_EQ(tree.root->right->right->elem, 40);
    EXPECT_EQ(tree.root->right->right->color, RED);
    EXPECT_EQ(tree.root->right->right->parent, tree.root->right);
    EXPECT_EQ(tree.root->right->right->left->elem, 35);
    EXPECT_EQ(tree.root->right->right->left->color, BLACK);
    EXPECT_EQ(tree.root->right->right->left->parent, tree.root->right->right);
    EXPECT_EQ(tree.root->right->right->right->elem, 45);
    EXPECT_EQ(tree.root->right->right->right->color, BLACK);
    EXPECT_EQ(tree.root->right->right->right->parent, tree.root->right->right);

    tree.leftRotate(tree.root);
    EXPECT_EQ(tree.root->elem, 20);
    EXPECT_EQ(tree.root->color, BLACK);
    EXPECT_EQ(tree.root->parent, nullptr);

    EXPECT_EQ(tree.root->left->elem, 10);
    EXPECT_EQ(tree.root->left->color, RED);
    EXPECT_EQ(tree.root->left->parent, tree.root);
    EXPECT_EQ(tree.root->right->elem, 40);
    EXPECT_EQ(tree.root->right->color, RED);
    EXPECT_EQ(tree.root->right->parent, tree.root);

    EXPECT_EQ(tree.root->left->left->elem, 5);
    EXPECT_EQ(tree.root->left->left->color, BLACK);
    EXPECT_EQ(tree.root->left->left->parent, tree.root->left);
    EXPECT_EQ(tree.root->left->right->elem, 15);
    EXPECT_EQ(tree.root->left->right->color, BLACK);
    EXPECT_EQ(tree.root->left->right->parent, tree.root->left);

    EXPECT_EQ(tree.root->right->left->elem, 35);
    EXPECT_EQ(tree.root->right->left->color, BLACK);
    EXPECT_EQ(tree.root->right->left->parent, tree.root->right);
    EXPECT_EQ(tree.root->right->right->elem, 45);
    EXPECT_EQ(tree.root->right->right->color, BLACK);
    EXPECT_EQ(tree.root->right->right->parent, tree.root->right);
}

//TEST(RBTree, Insert) {
//    // scratch case
//    {
//        RBTree tree;
//        tree.insert(20);
//        EXPECT_EQ(tree.root->elem, 20);
//        EXPECT_EQ(tree.root->color, BLACK);
//        tree.insert(10);
//        tree.insert(40);
//        EXPECT_EQ(tree.root->left->elem, 10);
//        EXPECT_EQ(tree.root->left->color, RED);
//        EXPECT_EQ(tree.root->right->elem, 40);
//        EXPECT_EQ(tree.root->right->color, RED);
//    }
//    // case 1
//    {
//        RBTree tree;
//        tree.root = new Node(20);
//        makeChilds(tree.root, new Node(10, RED), new Node(40, RED));
//        tree.insert(5);
//        EXPECT_EQ(tree.root->elem, 20);
//        EXPECT_EQ(tree.root->color, BLACK);
//        EXPECT_EQ(tree.root->left->elem, 10);
//        EXPECT_EQ(tree.root->left->color, BLACK);
//        EXPECT_EQ(tree.root->right->elem, 40);
//        EXPECT_EQ(tree.root->right->color, BLACK);
//        EXPECT_EQ(tree.root->left->left->elem, 5);
//        EXPECT_EQ(tree.root->left->left->color, RED);
//    }
//    // case 2
//    {
//        RBTree tree;
//        tree.root = new Node(20);
//        makeChilds(tree.root, new Node(10, BLACK), new Node(40, BLACK));
//        makeChilds(tree.root->left, new Node(7, RED), nullptr);
//        tree.insert(9);
//        EXPECT_EQ(tree.root->elem, 20);
//        EXPECT_EQ(tree.root->color, BLACK);
//        EXPECT_EQ(tree.root->left->elem, 9);
//        EXPECT_EQ(tree.root->left->color, BLACK);
//        EXPECT_EQ(tree.root->right->elem, 40);
//        EXPECT_EQ(tree.root->right->color, BLACK);
//        EXPECT_EQ(tree.root->left->left->elem, 7);
//        EXPECT_EQ(tree.root->left->left->color, RED);
//        EXPECT_EQ(tree.root->left->right->elem, 10);
//        EXPECT_EQ(tree.root->left->right->color, RED);
//    }
//
//    // case 3
//    {
//        RBTree tree;
//        tree.root = new Node(20);
//        makeChilds(tree.root, new Node(10, BLACK), new Node(40, BLACK));
//        makeChilds(tree.root->left, new Node(7, RED), nullptr);
//        tree.insert(5);
//        EXPECT_EQ(tree.root->elem, 20);
//        EXPECT_EQ(tree.root->color, BLACK);
//        EXPECT_EQ(tree.root->left->elem, 7);
//        EXPECT_EQ(tree.root->left->color, BLACK);
//        EXPECT_EQ(tree.root->right->elem, 40);
//        EXPECT_EQ(tree.root->right->color, BLACK);
//        EXPECT_EQ(tree.root->left->left->elem, 5);
//        EXPECT_EQ(tree.root->left->left->color, RED);
//        EXPECT_EQ(tree.root->left->left->left, nullptr);
//        EXPECT_EQ(tree.root->left->left->right, nullptr);
//        EXPECT_EQ(tree.root->left->right->elem, 10);
//        EXPECT_EQ(tree.root->left->right->color, RED);
//        EXPECT_EQ(tree.root->left->right->left, nullptr);
//        EXPECT_EQ(tree.root->left->right->right, nullptr);
//        // insert & delete
//        int elems[] = {7, 40, 5, 20, 10};
//        for (int i = 0; i < sizeof(elems) / sizeof(elems[0]); i++) {
//            EXPECT_EQ(tree.find(elems[i]), true);
//        }
//
//        for (int i = 0; i < sizeof(elems) / sizeof(elems[0]); i++) {
//            tree.remove(elems[i]);
//            for (int j = 0; j <= i; j++) {
//                EXPECT_EQ(tree.find(elems[j]), false);
//            }
//            for (int j = i + 1; j < sizeof(elems) / sizeof(elems[0]); j++) {
//                EXPECT_EQ(tree.find(elems[j]), true);
//            }
//        }
//    }
//
//
//}

//TEST(RBTree, Delete) {
//    // dont need to do color fixup
//    {
//        RBTree tree;
//        tree.root = new Node(9);
//        makeChilds(tree.root, new Node(5, RED), new Node(17, RED));
//        tree.remove(17);
//        EXPECT_EQ(tree.find(17), false);
//        EXPECT_EQ(tree.find(9), true);
//        EXPECT_EQ(tree.find(5), true);
//    }
//    // case 1
//    {
//        RBTree tree;
//        tree.root = new Node(9);
//        makeChilds(tree.root, new Node(5, RED), new Node(17, BLACK));
//        makeChilds(tree.root->left, new Node(1, BLACK), new Node(6, BLACK));
//        tree.remove(17);
//        EXPECT_EQ(tree.find(17), false);
//        EXPECT_EQ(tree.find(9), true);
//        EXPECT_EQ(tree.find(5), true);
//        EXPECT_EQ(tree.find(1), true);
//        EXPECT_EQ(tree.find(6), true);
//    }
//    // case 2
//    {
//        RBTree tree;
//        tree.root = new Node(9);
//        makeChilds(tree.root, new Node(5, BLACK), new Node(17, BLACK));
//        makeChilds(tree.root->right, new Node(11, BLACK), new Node(28, BLACK));
//        tree.remove(17);
//        EXPECT_EQ(tree.find(17), false);
//        EXPECT_EQ(tree.find(9), true);
//        EXPECT_EQ(tree.find(28), true);
//        EXPECT_EQ(tree.find(11), true);
//        EXPECT_EQ(tree.find(5), true);
//    }
//    // case 3
//    {
//        RBTree tree;
//        tree.root = new Node(9);
//        makeChilds(tree.root, new Node(5, BLACK), new Node(17, BLACK));
//        makeChilds(tree.root->left, new Node(1, BLACK), new Node(7, RED));
//        makeChilds(tree.root->left->right, new Node(6), new Node(8, BLACK));
//        tree.remove(17);
//        EXPECT_EQ(tree.find(17), false);
//        EXPECT_EQ(tree.find(9), true);
//        EXPECT_EQ(tree.find(5), true);
//        EXPECT_EQ(tree.find(1), true);
//        EXPECT_EQ(tree.find(7), true);
//        EXPECT_EQ(tree.find(6), true);
//        EXPECT_EQ(tree.find(8), true);
//    }
//    // case 4
//    {
//        RBTree tree;
//        tree.root = new Node(9);
//        makeChilds(tree.root, new Node(5, BLACK), new Node(17, BLACK));
//        makeChilds(tree.root->left, new Node(1, RED), new Node(6, BLACK));
//        makeChilds(tree.root->left->left, new Node(-1, BLACK), new Node(3, BLACK));
//        makeChilds(tree.root->right, new Node(11, BLACK), new Node(28, BLACK));
//        tree.remove(17);
//        EXPECT_EQ(tree.find(17), false);
//        EXPECT_EQ(tree.find(9), true);
//        EXPECT_EQ(tree.find(5), true);
//        EXPECT_EQ(tree.find(1), true);
//        EXPECT_EQ(tree.find(6), true);
//        EXPECT_EQ(tree.find(-1), true);
//        EXPECT_EQ(tree.find(3), true);
//        EXPECT_EQ(tree.find(11), true);
//        EXPECT_EQ(tree.find(28), true);
//    }
//
//    {
//        RBTree tree;
//        tree.root = new Node(9);
//        makeChilds(tree.root, new Node(5, BLACK), new Node(17, BLACK));
//        makeChilds(tree.root->left, new Node(1, RED), new Node(6, BLACK));
//        makeChilds(tree.root->left->left, new Node(-1, BLACK), new Node(3, BLACK));
//        makeChilds(tree.root->right, new Node(11, BLACK), new Node(28, BLACK));
//        Node* left = tree.findNode(11);
//        tree.remove(28);
//        EXPECT_EQ(left->color, RED);
//        EXPECT_EQ(tree.find(28), false);
//        int elems[] = { 9, 5, 17, 1, 6, -1, 3};
//        for (int i = 0; i < sizeof(elems)/sizeof(elems[0]); i++) {
//            tree.remove(elems[i]);
//            for (int j = 0; j <= i; j++) {
//                EXPECT_EQ(tree.find(elems[j]), false);
//            }
//            for (int j = i+1; j < sizeof(elems)/sizeof(elems[0]); j++) {
//                std::cout << "elems[j] " << elems[j] << std::endl;
//                EXPECT_EQ(tree.find(elems[j]), true);
//            }
//        }
//    }
//}


//#include <cstdlib>
//#include <set>
//#include <vector>
//TEST(RBTree, RandomModify) {
//    std::vector<int> nums1;
//    std::set<int> nums2;
//    //int elemMax = 10000000;
//    int elemMax = 30;
//    //int nelems = elemMax*7/8;
//    int nelems = 99;
//    nums1.resize(nelems);
//    for (int i = 0; i < nums1.size(); i++) {
//        nums1[i] = rand() % elemMax;
//        nums2.insert(nums1[i]);
//    }
//#if 0
//    nums1 = { 41,17,34,0,19,24,28,8,12,14,5,45,31,27,11,41,45,42,27,36 };
//    nums2 = { 0,5,8,11,12,14,17,19,24,27,28,31,34,36,41,42,45 };
//#endif
//
//#if 1
//    nums1 = { 11,17,4,10,29,4,18,18,22,14,5,5,1,27,1,11,25,2,27,6,21,24,2,3,22,22,21,26,8,5,17,6,11,18,9,22,17,19,25,24,23,21,2,3,3,14,21,1,23,28,17,14,22,27,27,19,23,21,19,28,16,5,20,12,18,16,10,2,4,28,26,15,20,9,10,20,6,21,3,8,9,23,24,4,6,20,16,26,11,28,24,9,26,13,17,28,8,12,9 };
//    nums2 = { 1,2,3,4,5,6,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29 };
//#endif 
//    RBTree tree;
//    for (int i = 0; i < nums1.size(); i++) {
//        tree.insert(nums1[i]);
//    }
//    tree.find(2);
//    for (auto num : nums2) {
//        EXPECT_EQ(tree.find(num), true);
//        tree.remove(num);
//        EXPECT_EQ(tree.find(num), false);
//    }
//    for (auto num : nums2) {
//        EXPECT_EQ(tree.find(num), false);
//    }
//}