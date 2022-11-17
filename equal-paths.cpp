#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

// Recurse down each possible child, then return back upwards the height of all paths to leafs
// If there is any mismatch in path heights, -1 will be returned
int pathHeight(Node* node) {
    if (node == nullptr) return 0;
    else {
        int leftHeight = pathHeight(node->left);
        int rightHeight = pathHeight(node->right);
        // If there is a mismatch anywhere in the child subtrees, return -1
        if (leftHeight == -1 || rightHeight == -1) return -1;
        else {
            if (leftHeight == 0) return 1 + rightHeight;
            else if (rightHeight == 0) return 1 + leftHeight;
            else if (leftHeight == rightHeight) return 1 + leftHeight;
            else return -1;
        }
    }
}

bool equalPaths(Node * root)
{
    // Add your code below
    if (pathHeight(root) == -1) return false;
    else return true;
}

