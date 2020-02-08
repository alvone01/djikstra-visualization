#ifndef AVL_H
#define AVL_H

#include "component.h"

template <typename T>
class AVL
{
    private:
        struct AVLnode
        {
            Component* value = nullptr;
            int height;
            AVL left; // Left subtree is also an AVL object
            AVL right; // Right subtree is also an AVL object
            AVLnode(Component* x) : value(x), height(0), left(), right() { }
        };

        AVLnode* root = nullptr;
        AVL& right_subtree() { return root->right; }
        AVL& left_subtree() { return root->left; }
        const AVL& right_subtree() const { return root->right; }
        const AVL& left_subtree() const { return root->left; }

        int height() const; // Find the height of tree
        int bfactor() const; // Find the balance factor of tree
        void fix_height() const; // Rectify the height of each node in tree
        void rotate_left(); // Single left or anti-clockwise rotation
        void rotate_right(); // Single right or clockwise rotation
        void balance(); // AVL tree balancing

    public:
        AVL() = default; // Build an empty AVL tree by default
        ~AVL() { delete root; } // Will delete the whole tree recursively!
        bool is_empty() const { return root == nullptr; }
        Component* find_min() const; // Find the minimum value in an AVL
        bool contains(const Component*&) const; // Search an item
        void print(int depth = 0) const; // Print by rotating -90 degrees
        void insert(Component*); // Insert an item in sorted order
        void remove(Component* x); // Remove an item
};


template <typename T>
bool AVL<T>::contains(const Component*& x) const
{
    if (is_empty()) // Base case #1
        return false;
    else if (x == root->value) // Base case #2
        return true;
    else if (x->getDistance() == root->value->getDistance()) {
        return left_subtree().contains(x);
        return right_subtree().contains(x);
    }
    else if (x->getDistance() < root->value->getDistance()) // Recursion on the left subtree
        return left_subtree().contains(x);
    else // Recursion on the right subtree
        return right_subtree().contains(x);
}

template <typename T>
void AVL<T>::rotate_right() // The calling AVL node is node a
{
    AVLnode* b = left_subtree().root; // Points to node b
    left_subtree() = b->right;
    b->right = *this; // Note: *this is node a
    fix_height(); // Fix the height of node a
    this->root = b; // Node b becomes the new root
    fix_height(); // Fix the height of node b, now the new root
}

template <typename T>
void AVL<T>::rotate_left() // The calling AVL node is node a
{
    AVLnode* b = right_subtree().root; // Points to node b
    right_subtree() = b->left;
    b->left = *this; // Note: *this is node a
    fix_height(); // Fix the height of node a
    this->root = b; // Node b becomes the new root
    fix_height(); // Fix the height of node b, now the new root
}

template <typename T>
void AVL<T>::insert(Component* x)
{
    if (is_empty()) // Base case
        root = new AVLnode(x);
    else if (x->getDistance() == root->value->getDistance())
        left_subtree().insert(x);
    else if (x->getDistance() < root->value->getDistance())
        left_subtree().insert(x); // Recursion on the left sub-tree
    else if (x->getDistance() > root->value->getDistance())
        right_subtree().insert(x); // Recursion on the left sub-tree
    balance(); // Re-balance the tree at every visited node
}

template <typename T>
void AVL<T>::balance()
{
    if (is_empty())
        return;
    fix_height();
    int balance_factor = bfactor();
    if (balance_factor == 2) // Right subtree is taller by 2
    {
        if (right_subtree().bfactor() < 0) // Case 4: insertion to the L of RT
        right_subtree().rotate_right();
        return rotate_left(); // Cases 1 or 4: Insertion to the R/L of RT
    }
    else if (balance_factor == -2) // Left subtree is taller by 2
    {
        if (left_subtree().bfactor() > 0) // Case 3: insertion to the R of LT
        left_subtree().rotate_left();
        return rotate_right(); // Cases 2 or 3: insertion to the L/R of LT
    }
    // Balancing is not required for the remaining cases
}

template <typename T>
int AVL<T>::height() const { return is_empty() ? -1 : root->height; }
/* Goal: To rectify the height values of each AVL node */
template <typename T>
void AVL<T>::fix_height() const
{
    if (!is_empty())
    {
        int left_avl_height = left_subtree().height();
        int right_avl_height = right_subtree().height();
        root->height = 1 + std::max(left_avl_height, right_avl_height);
    }
}

/* balance factor = height of right sub-tree - height of left sub-tree */
template <typename T>
int AVL<T>::bfactor() const
{
    return is_empty() ? 0
        : right_subtree().height() - left_subtree().height();
}

template <typename T>
void AVL<T>::remove(Component* x)
{
    if (is_empty()) // Item is not found; do nothing
        return;
    if (x->getDistance() < root->value->getDistance())
        left_subtree().remove(x); // Recursion on the left sub-tree
    else if (x->getDistance() > root->value->getDistance())
        right_subtree().remove(x); // Recursion on the right sub-tree
    else if (x != root->value){
        left_subtree().remove(x);
        right_subtree().remove(x);
    }
    else
    {
    AVL& left_avl = left_subtree();
    AVL& right_avl = right_subtree();

        if (!left_avl.is_empty() && !right_avl.is_empty())
        {
            root->value = right_avl.find_min(); // Copy the min value
            right_avl.remove(root->value); // Remove node with min value
        }
        else // Found node has 0 or 1 child
        {
            AVLnode* node_to_remove = root; // Save the node first
            *this = left_avl.is_empty() ? right_avl : left_avl;
            // Reset the node to be removed with empty children
            right_avl.root = left_avl.root = nullptr;
            delete node_to_remove;
         }
    }
    balance(); // Re-balance the tree at every visited node
}

template <typename T>
Component* AVL<T>::find_min() const
{
    // It is assumed that the calling tree is not empty
    const AVL& left_avl = left_subtree();

    if (left_avl.is_empty()) // Base case: Found!
        return root->value;

    return left_avl.find_min(); // Recursion on the left subtree
}

#endif // AVL_H
