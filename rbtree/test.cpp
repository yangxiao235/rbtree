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
	Node(int elem=-1, Color color=BLACK, Node* parent=nullptr, Node* left=nullptr, Node* right=nullptr)
		:elem(elem),color(color),parent(parent),left(left),right(right)
	{}

};

static bool color(Node* node) {
	return node == nullptr ? BLACK : node->color;
}

static bool nodeColor(Node* node) {
	return node == nullptr ? BLACK : node->color;
}

class RBTree {
public:
	void insert(int elem)
	{

	}
	void remove(int elem)
	{
		Color originalColor;
		Node* node = rbremove(elem, originalColor);
		if (originalColor == BLACK) {
			removeFixup(node);
		}
	}

	bool find(int elem)
	{
		return findNode(elem) != nullptr;
	}

private:
	void leftRotate(Node* node) {
		Node* r = node->right;
		if (node->parent) {
			if (node == node->parent->left) {
				node->parent->left = r;
			}
			else {
				node->parent->right = r;
			}
			r->parent = node->parent;
		}
		if (r->left) {
			r->left->parent = node;
			node->right = r;
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
		}
		if (l->right) {
			l->right->parent = node;
			node->left = l->right;
		}
		l->right = node;
		node->parent = l;
	}

	void removeFixup(Node* node) {
		while (node != root && node->color == BLACK) {
			if (node == node->parent->left) {
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
				if (nodeColor(left->left)== BLACK) {
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

	Node* findNode(int elem) {
		Node* node = root;
		while (node && node->elem != elem) {
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
		while (node->left) {
			node = node->left;
		}
		return node;
	}

	Node *rbremove(int elem, Color &originColor)
	{
		Node* node = findNode(elem);
		Node* ret = nullptr;;
		if (!node) {
			return nullptr;
		}
		if (node == root) {
			delete node;
			root = nullptr;
			return nullptr;
		}
		do {
			originColor = node->color;
			if (!node->left) {
				transparent(node->right, node);
				ret = node->right;
				break;
			}
			if (!node->right) {
				transparent(node->left, node);
				ret = node->left;
				break;
			}
			Node* successor = minimum(node->right);
			ret = successor;
			if (successor->parent != node) {
				originColor = successor->color;
				ret = successor->right;
				transparent(successor->right, successor);
			}
			successor->color = node->color;
			transparent(successor, node);
		} while (false);
		delete node;
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
private:
	FRIEND_TEST(RBTree, TestDelete);
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

TEST(RBTree, TestDelete) {
	// dont need to do color fixup
	{
		RBTree tree;
		tree.root = new Node(9);
		makeChilds(tree.root, new Node(5, RED), new Node(17, RED));
		makeChilds(tree.root->right, nullptr, new Node(28, BLACK));
		tree.remove(17);
		EXPECT_EQ(tree.find(17), false);
		EXPECT_EQ(tree.find(9), true);
		EXPECT_EQ(tree.find(28), true);
		EXPECT_EQ(tree.find(5), true);
	}
	// case 1
	{
		RBTree tree;
		tree.root = new Node(9);
		makeChilds(tree.root, new Node(5, RED), new Node(17, BLACK));
		makeChilds(tree.root->left, new Node(1, RED), nullptr);
		makeChilds(tree.root->right, nullptr, new Node(28, BLACK));
		tree.remove(17);
		EXPECT_EQ(tree.find(17), false);
		EXPECT_EQ(tree.find(9), true);
		EXPECT_EQ(tree.find(28), true);
		EXPECT_EQ(tree.find(5), true);
	}
	// case 2
	{
		RBTree tree;
		tree.root = new Node(9);
		makeChilds(tree.root, new Node(5, BLACK), new Node(17, BLACK));
		makeChilds(tree.root->right, nullptr, new Node(28, BLACK));
		tree.remove(17);
		EXPECT_EQ(tree.find(17), false);
		EXPECT_EQ(tree.find(9), true);
		EXPECT_EQ(tree.find(28), true);
		EXPECT_EQ(tree.find(5), true);
	}
	// case 3
	{
		RBTree tree;
		tree.root = new Node(9);
		makeChilds(tree.root, new Node(5, BLACK), new Node(17, BLACK));
		makeChilds(tree.root->left, new Node(1, BLACK), new Node(7, RED));
		makeChilds(tree.root->left->right, new Node(6), new Node(8, BLACK));
		makeChilds(tree.root->right, nullptr, new Node(28, BLACK));
		tree.remove(17);
		EXPECT_EQ(tree.find(17), false);
		EXPECT_EQ(tree.find(9), true);
		EXPECT_EQ(tree.find(28), true);
		EXPECT_EQ(tree.find(5), true);
	}
	// case 4
	{
		RBTree tree;
		tree.root = new Node(9);
		makeChilds(tree.root, new Node(5, BLACK), new Node(17, BLACK));
		makeChilds(tree.root->left, new Node(1, RED), new Node(6, BLACK));
		makeChilds(tree.root->left->left, new Node(-1, BLACK), new Node(3, BLACK));
		makeChilds(tree.root->right, nullptr, new Node(28, BLACK));
		tree.remove(17);
		EXPECT_EQ(tree.find(17), false);
		EXPECT_EQ(tree.find(9), true);
		EXPECT_EQ(tree.find(28), true);
		EXPECT_EQ(tree.find(5), true);
	}
	{
		RBTree tree;
		tree.root = new Node(9);
		makeChilds(tree.root, new Node(5, BLACK), new Node(17, BLACK));
		makeChilds(tree.root->left, new Node(1, RED), new Node(6, BLACK));
		makeChilds(tree.root->left->left, new Node(-1, BLACK), new Node(3, BLACK));
		makeChilds(tree.root->right, nullptr, new Node(28, BLACK));
		int elems[] = {9, 5, 17, 1, 6, -1, 3, 28};
		for (int i = 0; i < sizeof(elems)/sizeof(elems[0]); i++) {
			tree.remove(elems[i]);
			for (int j = 0; j <= i; j++) {
				EXPECT_EQ(tree.find(elems[j]), false);
			}
			for (int j = i+1; j < sizeof(elems)/sizeof(elems[0]); j++) {
				std::cout << "j = " << j << ", elem " << elems[j] << std::endl;
				EXPECT_EQ(tree.find(elems[j]), true);
			}
		}
	}
	
}


