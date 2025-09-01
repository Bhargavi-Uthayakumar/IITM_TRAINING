/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     struct TreeNode *left;
 *     struct TreeNode *right;
 * };
 */

void inorder(struct TreeNode* root, int* values, int* size) {
    if (!root)
        return;
    inorder(root->left, values, size);
    values[(*size)++] = root->val;
    inorder(root->right, values, size);
}

struct TreeNode* newNode(int val) {
    struct TreeNode* node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    node->val = val;
    node->left = NULL;
    node->right = NULL;
    return node;
}


struct TreeNode* buildBalancedBST(int* values, int left, int right) {
    if (left > right)
        return NULL;

    int mid = left + (right - left) / 2;
    struct TreeNode* root = newNode(values[mid]);
    root->left = buildBalancedBST(values, left, mid - 1);
    root->right = buildBalancedBST(values, mid + 1, right);
    return root;
}

struct TreeNode* balanceBST(struct TreeNode* root) {

    int* values = (int*)malloc(sizeof(int) * 10000);
    int size = 0;

    inorder(root, values, &size);
    struct TreeNode* balancedRoot = buildBalancedBST(values, 0, size - 1);
    free(values); // Free allocated memory for values array

    return balancedRoot;
}