/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     struct TreeNode *left;
 *     struct TreeNode *right;
 * };
 */

bool isValid(struct TreeNode* p, struct TreeNode** prev) {
    if (!p) {
        return true;
    }

    if (!isValid(p->left, prev)) {
        return false;
    }

    if (*prev && p->val <= (*prev)->val) {
        return false;
    }

    *prev = p;

    return isValid(p->right, prev);
}

bool isValidBST(struct TreeNode* root) {
    struct TreeNode *prev = NULL;
    return isValid(root, &prev);
}