#ifndef binarytree_h
#define binarytree_h

#include "gadget.h"

typedef int nodeelem;

class TreeNode {
public:
  nodeelem  value;      //The contents of the node.
  TreeNode* left;       //Pointer to left child.
  TreeNode* right;      //Pointer to right sibling.
  TreeNode() : value(0), left(0), right(0) {};
  TreeNode(nodeelem x) : value(x), left(0), right(0) {};
  ~TreeNode() { left = 0; right = 0; delete left; delete right; };
};

class  BinarySearchTree {
public:
  BinarySearchTree();
  ~BinarySearchTree();
  void Insert(nodeelem x);
  void Delete(nodeelem x);
  int IsIn(nodeelem x) const;
  int IsEmpty() const;
  nodeelem DeleteSmallest();
  friend ostream& operator << (ostream&, const BinarySearchTree&);
private:
  TreeNode* root;   //Pointer to the root of the tree.
  TreeNode* Parent(nodeelem) const;
  nodeelem DeleteSmallestRight(TreeNode*);
  void Kill(TreeNode*);
  TreeNode* Place(nodeelem, TreeNode*) const;
};

#endif
