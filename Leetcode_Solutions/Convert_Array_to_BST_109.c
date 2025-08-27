/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     struct TreeNode *left;
 *     struct TreeNode *right;
 * };
 */

struct TreeNode* buildBSTFromArray(int* arr, int start, int end) {
    if (start > end) {
        return NULL;
    }

    int mid = start + (end - start) / 2;
    struct TreeNode* root = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    root->val = arr[mid];
    root->left = buildBSTFromArray(arr, start, mid - 1);
    root->right = buildBSTFromArray(arr, mid + 1, end);
    return root;
}

struct TreeNode* sortedListToBST(struct ListNode* head) {
    if (head == NULL) {
        return NULL;
    }

    int* arr = NULL;
    int count = 0;
    struct ListNode* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    arr = (int*)malloc(sizeof(int) * count);
    current = head;
    int i = 0;
    while (current != NULL) {
        arr[i++] = current->val;
        current = current->next;
    }

    struct TreeNode* root = buildBSTFromArray(arr, 0, count - 1);

    free(arr); 
    return root;
}