/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     struct TreeNode *left;
 *     struct TreeNode *right;
 * };
 */

struct TreeNode* find_max(struct TreeNode* t){
    if(t!=NULL){
        while(t->right!=NULL){
            t=t->right;
        }
    }
    return t;
}

struct TreeNode* deleteNode(struct TreeNode* root, int key) {
     struct TreeNode *temp,*child;
    if(root==NULL){
        return NULL;
    }
    else if(key<root->val){
        root->left=deleteNode(root->left,key);
    }
    else if(key>root->val){
        root->right=deleteNode(root->right,key);
    }
    else if(root->left && root->right){
        temp=find_max(root->left);
        root->val=temp->val;
        root->left=deleteNode(root->left,temp->val);
    }
    else{
        temp=root;
        if(root->left==NULL){
            child=root->right;
        }
        if(root->right==NULL){
            child=root->left;
        }
        free(temp);
        return child;
    }
    return root;
}