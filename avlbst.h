#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void rotateRight(AVLNode<Key,Value>* n);
    void rotateLeft(AVLNode<Key,Value>* n);
    void removePointer(AVLNode<Key, Value>* rem);
    void removeUpdateParent(AVLNode<Key, Value>* insert, AVLNode<Key, Value>* rem, AVLNode<Key, Value>* parent);
    void removeFix(AVLNode<Key, Value>* n, int8_t diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if (this->root_ == NULL) this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
    else {
        AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
        while (curr != NULL) {
            if (curr->getKey() == new_item.first) {
                curr->setValue(new_item.second);
                return;
            }
            else if (curr->getKey() > new_item.first) {
                AVLNode<Key, Value>* next = curr->getLeft();
                if (next == NULL) {
                    curr->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, curr));
                    curr->updateBalance(-1);
                    if (curr->getBalance() == 0) insertFix(curr, curr->getLeft());
                    return;
                }
                else curr = next;
            }
            else if (curr->getKey() < new_item.first) {
                AVLNode<Key, Value>* next = curr->getRight();
                if (next == NULL) {
                    curr->setRight(new AVLNode<Key, Value>(new_item.first, new_item.second, curr));
                    curr->updateBalance(1);
                    if (curr->getBalance() == 0) insertFix(curr, curr->getRight());
                    return;
                }
                else curr = next;
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n) {
    if (p == NULL) return;
    AVLNode<Key, Value>* g = p->getParent();
    if (g == NULL) return;
    // p is left child of g
    if (g->getLeft() == p) {
        g->updateBalance(-1);
        if(g->getBalance() == 0) return;
        else if (g->getBalance() == -1) insertFix(g, p);
        else if (g->getBalance() == -2) {
            // n is left child of p (zig zig)
            if (p->getLeft() == n) {
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            // n is right child of p (zig zag)
            else {
                rotateLeft(p);
                rotateRight(g);
                if (n->getBalance() == -1) {
                    p->setBalance(0);
                    g->setBalance(1);
                } else if (n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                } else if (n->getBalance() == 1) {
                    p->setBalance(-1);
                    g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }
    // p is right child of g
    else {
        g->updateBalance(1);
        if(g->getBalance() == 0) return;
        else if (g->getBalance() == 1) insertFix(g, p);
        else if (g->getBalance() == 2) {
            // n is right child of p (zig zig)
            if (p->getRight() == n) {
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            // n is left child of p (zig zag)
            else {
                rotateRight(p);
                rotateLeft(g);
                if (n->getBalance() == 1) {
                    p->setBalance(0);
                    g->setBalance(1);
                } else if (n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                } else if (n->getBalance() == -1) {
                    p->setBalance(-1);
                    g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* n) {
    AVLNode<Key,Value>* newParent = n->getLeft();
    AVLNode<Key,Value>* newNLeftChild = newParent->getRight();
    if (n == this->root_) {
        this->root_ = newParent;
    }
    else {
        AVLNode<Key,Value>* grandparent = n->getParent();
        if (grandparent->getLeft() == n) {
            grandparent->setLeft(newParent);
        }
        else {
            grandparent->setRight(newParent);
        }
        newParent->setParent(grandparent);
    }
    newParent->setRight(n);
    n->setParent(newParent);
    n->setLeft(newNLeftChild);
    newNLeftChild->setParent(n);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* n) {
    AVLNode<Key,Value>* newParent = n->getRight();
    AVLNode<Key,Value>* newNRightChild = newParent->getLeft();
    if (n == this->root_) {
        this->root_ = newParent;
    }
    else {
        AVLNode<Key,Value>* grandparent = n->getParent();
        if (grandparent->getLeft() == n) {
            grandparent->setLeft(newParent);
        }
        else {
            grandparent->setRight(newParent);
        }
        newParent->setParent(grandparent);
    }
    newParent->setLeft(n);
    n->setParent(newParent);
    n->setRight(newNRightChild);
    newNRightChild->setParent(n);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    removePointer(static_cast<AVLNode<Key, Value>*>(this->internalFind(key)));
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::removePointer(AVLNode<Key, Value>* rem) {
    if (rem == NULL) return;
    AVLNode<Key, Value>* parent = rem->getParent();
    // 2 children
    if (rem->getLeft() != NULL && rem->getRight() != NULL) {
        // Swap with predecessor
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(rem));
        nodeSwap(rem, pred);
        removePointer(rem);
    }
    // only one child - left
    else if (rem->getLeft() != NULL) {
        if (rem == this->root_) {
            this->root_ = rem->getLeft();
            this->root_->setParent(NULL);
        } else {
            removeUpdateParent(rem->getLeft(), rem, parent);
            rem->getLeft()->setParent(parent);
        }
        delete rem;
    }
    // only one child - right
    else if (rem->getRight() != NULL) {
        if (rem == this->root_) {
            this->root_ = rem->getRight();
            this->root_->setParent(NULL);
        } else {
            removeUpdateParent(rem->getRight(), rem, parent);
            rem->getRight()->setParent(parent);
        }
        delete rem;
    }
    // no children
    else {
        if (rem == this->root_) {
            this->root_ = NULL;
        } else {
            if (parent->getLeft() == rem) parent->setLeft(NULL);
            else parent->setRight(NULL);
        }
        delete rem;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeUpdateParent(AVLNode<Key, Value>* insert, AVLNode<Key, Value>* rem, AVLNode<Key, Value>* parent) {
    if (parent->getLeft() == rem) {
        parent->setLeft(insert);
        removeFix(parent, 1);
    }
    else {
        parent->setRight(insert);
        removeFix(parent, -1);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int8_t diff) {
    if (n == NULL) return;
    AVLNode<Key, Value>* p = n->getParent();
    int8_t newdiff;
    if (p != NULL) {
        if (p->getLeft() == n) newdiff = 1;
        else if (p->getRight() == n) newdiff = -1;
    }
    if (n->getBalance() + diff == -2) {
        AVLNode<Key, Value>* child = n->getLeft();
        if (child->getBalance() == -1) {
            rotateRight(n);
            n->setBalance(0);
            child->setBalance(0);
            removeFix(p, newdiff);
        }
        else if (child->getBalance() == 0) {
            rotateRight(n);
            n->setBalance(-1);
            child->setBalance(1);
        }
        else if (child->getBalance() == 1) {
            AVLNode<Key, Value>* g = child->getRight();
            rotateLeft(child);
            rotateRight(n);
            if (g->getBalance() == 1) {
                n->setBalance(0);
                child->setBalance(-1);
                g->setBalance(0);
            }
            else if (g->getBalance() == 0) {
                n->setBalance(0);
                child->setBalance(0);
                g->setBalance(0);
            }
            else if (g->getBalance() == -1) {
                n->setBalance(1);
                child->setBalance(0);
                g->setBalance(0);
            }
            removeFix(p, newdiff);
        }
    }
    else if (n->getBalance() + diff == -1) {
        n->setBalance(-1);
    }
    else if (n->getBalance() + diff == 0) {
        n->setBalance(0);
        removeFix(p, newdiff);
    }
    else if (n->getBalance() + diff == 1) {
        n->setBalance(1);
    }
    else if (n->getBalance() + diff == 2) {
        AVLNode<Key, Value>* child = n->getRight();
        if (child->getBalance() == 1) {
            rotateLeft(n);
            n->setBalance(0);
            child->setBalance(0);
            removeFix(p, newdiff);
        }
        else if (child->getBalance() == 0) {
            rotateLeft(n);
            n->setBalance(1);
            child->setBalance(-1);
        }
        else if (child->getBalance() == -1) {
            AVLNode<Key, Value>* g = child->getLeft();
            rotateRight(child);
            rotateLeft(n);
            if (g->getBalance() == -1) {
                n->setBalance(0);
                child->setBalance(1);
                g->setBalance(0);
            }
            else if (g->getBalance() == 0) {
                n->setBalance(0);
                child->setBalance(0);
                g->setBalance(0);
            }
            else if (g->getBalance() == 1) {
                n->setBalance(-1);
                child->setBalance(0);
                g->setBalance(0);
            }
            removeFix(p, newdiff);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
