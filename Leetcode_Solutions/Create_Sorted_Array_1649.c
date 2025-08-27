#define MOD 1000000007

typedef struct Node {
    int key;           
    int count;         
    int height;        
    int size;          
    struct Node *left, *right;
} Node;

int height(Node *n) { return n ? n->height : 0; }
int size(Node *n) { return n ? n->size : 0; }
int max(int a, int b) { return a > b ? a : b; }

void update(Node *n) {
    if (n) {
        n->height = 1 + max(height(n->left), height(n->right));
        n->size = n->count + size(n->left) + size(n->right);
    }
}

Node* rotateRight(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;
    x->right = y;
    y->left = T2;
    update(y);
    update(x);
    return x;
}

Node* rotateLeft(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;
    y->left = x;
    x->right = T2;
    update(x);
    update(y);
    return y;
}

int getBalance(Node *n) { return n ? height(n->left) - height(n->right) : 0; }

Node* insert(Node* root, int key, long long *less, long long *greater) {
    if (!root) {
        Node *node = (Node*)malloc(sizeof(Node));
        node->key = key;
        node->count = 1;
        node->height = 1;
        node->size = 1;
        node->left = node->right = NULL;
        return node;
    }

    if (key == root->key) {
        *less += size(root->left);
        *greater += size(root->right);
        root->count++;
    } 
    else if (key < root->key) {
        *greater += root->count + size(root->right);
        root->left = insert(root->left, key, less, greater);
    } 
    else {
        *less += root->count + size(root->left);
        root->right = insert(root->right, key, less, greater);
    }

    update(root);
    int balance = getBalance(root);

    if (balance > 1 && key < root->left->key) return rotateRight(root);
    if (balance < -1 && key > root->right->key) return rotateLeft(root);
    if (balance > 1 && key > root->left->key) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    if (balance < -1 && key < root->right->key) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

int createSortedArray(int* instructions, int n) {
    Node *root = NULL;
    long long ans = 0;

    for (int i = 0; i < n; i++) {
        long long less = 0, greater = 0;
        root = insert(root, instructions[i], &less, &greater);
        ans = (ans + (less < greater ? less : greater)) % MOD;
    }

    return (int)ans;
}
