#include "binarytree.h"

BinarySearchTree::BinarySearchTree() : root(0) {
}

BinarySearchTree::~BinarySearchTree() {
  if (root != 0)
    Kill(root);
  else
    delete root;
}

//Kill: Kill is a recursive function that deletes all knots in the
//subtree it receives as an argument.
void BinarySearchTree::Kill(TreeNode* a) {
  if (a != 0) {
    //Delete the left and right children, if they exist.
    if (a->left != 0)
      Kill(a->left);
    if (a->right != 0)
      Kill(a->right);
  }
  delete a;
}

//Place: Place(x,q) returns a pointer to that subtree of q in which x belongs
//If q->value == x, Place returns q.
TreeNode* BinarySearchTree::Place(nodeelem  x, TreeNode* q) const {
  if (x < q->value)
    return q->left;
  if (x > q->value)
    return q->right;
  //if we are here, then (x == q->value)
  return q;
}

//Parent: Parent(x) finds x in the tree and returns a pointer to
//the parent of the node containg x.
//If x is in the root or not in the tree, Parent(x) returns 0.
TreeNode* BinarySearchTree::Parent(nodeelem x) const {
  if (root == 0)
    return 0;
  TreeNode* child = root;
  TreeNode* parent = root;
  int found = 0;
  while (child != 0 && !found) {
    if (child->value == x)
      found = 1;
    else {
      TreeNode* tmp = child;
      child = Place(x, parent);
      parent = tmp;
    }
  }
  if (found && child != root)
    return parent;
  else
    return 0;
}

//Insert: Insert(x) puts x in the tree if x is not there, does nothing
//if x was already there. No action is taken to keep the tree balanced.
void BinarySearchTree::Insert(nodeelem x) {
  TreeNode* p = 0;
  TreeNode* r = root;
  while  (r != 0 && p != r) {  //Find the place in the tree where x is to be
    p = r;
    r = Place(x, p);
  }
  if (root != 0) {   //Put x in its place if tree is not empty.
    if (x < p->value)
      p->left = new TreeNode(x);
    if (x > p->value)
      p->right = new TreeNode(x);
  } else
    root = new TreeNode(x);     //Put x in the root if tree was empty.
}

//DeleteSmallestRight: DeleteSmallestRight(q) removes the node
//containing the smallest value in the right subtree of q and returns it
//returns 0 if there is no such subtree.
nodeelem BinarySearchTree::DeleteSmallestRight(TreeNode* q) {
  nodeelem skila;  //will contain the element to be returned.
  if ((q == 0) || (q->right == 0))  //Return 0 if pointer is 0 or
    //the right subtree of q is empty.
    return 0;
  else {
    //Move as far "down to the left" in q->right as possible; until we
    //find the smallest node. Remove it and bypass.
    if (q->right->left == 0) {
      TreeNode* p = q->right;     //p points to the node to be deleted.
      skila = p->value;
      q->right = q->right->right; //Bypass.
      delete p;
    } else {
      q = q->right;
      while (q->left->left != 0)
    q = q->left;
      skila = q->left->value;
      TreeNode* p = q->left;     //p points to the node to be deleted.
      q->left = q->left->right;  //Bypass.
      delete p;
    }
  }
  return skila;
}

//Delete: Delete(x) deletes the element x from the tree. To do so
//we use the function Parent(x); if the node containing x has more
//than two children, we also use the function DeleteSmallestRight.
void BinarySearchTree::Delete(nodeelem x) {
  if (root == 0)
    return;
  TreeNode* parent = Parent(x); //p points to x's parent or is 0.
  TreeNode* contains = 0;  //Will be a pointer to the node containing x.
  if (parent == 0) {
    if (root->value == x)
      contains = root;
  } else
    contains = Place(x, parent);

  if (contains == 0)  //Then tree is empty or x not in it.
    return;
  //From now on, parent points to the parent of contains.
  if (contains->left == 0 && contains->right == 0) {
    //Then contains is a leaf.
    if (parent != 0) {
      if (parent->left == contains)
    parent->left = 0;
      else
    parent->right = 0;
    } else
      root = 0; //Now tree is empty.
    delete contains;
    return;
  }
  if (parent == 0) {
    //Then x is in the root and contains == root.
    if (contains->left == 0 || contains->right == 0) {
      //if node containing x has only one child, bypass.
      TreeNode* child = (contains->left != 0 ? contains->left : contains->right);
      root = child;
      delete contains;
    } else
      contains->value = DeleteSmallestRight(contains);
    return;
  }
  if (contains->left == 0 || contains->right == 0) {
    //if node containing x has only one child, bypass.
    TreeNode* child = (contains->left != 0 ? contains->left : contains->right);
    if (parent->left == contains)
      parent->left = child;
    else
      parent->right = child;
    delete contains;
    return;
  } else
    contains->value = DeleteSmallestRight(contains);
}

//IsIn: returns 1 if x is in the tree, else 0.
//If x has any parents or is in the root, then x is in the tree, else not.
int BinarySearchTree::IsIn(nodeelem x) const {
  if (root == 0)
    return 0;
  else
    return ((Parent(x) != 0) || (root->value == x));
}

//IsEmpty: returns 1 if tree is empty, else 0.
int BinarySearchTree::IsEmpty() const {
  return (root == 0);
}

//DeleteSmallest: DeleteSmallest() removes node containing the
//smallest element from the tree and returns it.
nodeelem BinarySearchTree::DeleteSmallest() {
  TreeNode* q = root;
  nodeelem smallest;
  if (q == 0)
    return 0;
  else if (q->left == 0) {
    smallest = q->value;
    root = q->right;
    delete q;
  } else {
    while (q->left->left != 0)
      q = q->left;
    smallest = q->left->value;
    TreeNode* p = q->left;
    q->left = q->left->right;
    delete p;
  }
  return smallest;
}
