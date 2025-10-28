#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define structure for each node
struct Node {
    char value[20];
    struct Node *left, *right;
};

// Function to create a new node
struct Node *createNode(char *val) {
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    strcpy(node->value, val);
    node->left = node->right = NULL;
    return node;
}

// Preorder traversal: Root → Left → Right
void preorder(struct Node *root) {
    if (root == NULL) {
	    return;
    }
    printf("%s ", root->value);
    preorder(root->left);
    preorder(root->right);
}

// Postorder traversal: Left → Right → Root
void postorder(struct Node *root) {
    if (root == NULL){
	    return;
    }
    postorder(root->left);
    postorder(root->right);
    printf("%s ", root->value);
}

// Evaluate the arithmetic expression tree
int evaluate(struct Node *root) {
    if (!root) return 0;
    if (!root->left && !root->right) return atoi(root->value);

    int leftVal = evaluate(root->left);
    int rightVal = evaluate(root->right);

    if (!strcmp(root->value, "+")) return leftVal + rightVal;
    if (!strcmp(root->value, "-")) return leftVal - rightVal;
    if (!strcmp(root->value, "*")) return leftVal * rightVal;
    if (!strcmp(root->value, "/")) return leftVal / rightVal;

    return 0;
}

// Simple SQL evaluator
void evaluateSQL(const char *query) {
    char col[20], tbl[20], cond[20], op[3], val[20];
    sscanf(query, "SELECT %s FROM %s WHERE %s %2s %s", col, tbl, cond, op, val);

    // Fake data
    char *names[] = {"Alice", "Bob", "Carol"};
    int ages[] = {22, 30, 27};

    printf("SQL Query Result:\n");
    for (int i = 0; i < 3; i++) {
        int v = atoi(val);
        if (!strcmp(cond, "age")) {
            if ((!strcmp(op, ">") && ages[i] > v) ||
                (!strcmp(op, "<") && ages[i] < v) ||
                (!strcmp(op, "=") && ages[i] == v))
                printf("%s (age=%d)\n", names[i], ages[i]);
        }
    }
}

int main() {
    // Arithmetic expression: (5 + 3) * (8 - 2)
    struct Node *root = createNode("*");
    root->left = createNode("+");
    root->left->left = createNode("5");
    root->left->right = createNode("3");
    root->right = createNode("-");
    root->right->left = createNode("8");
    root->right->right = createNode("2");

    printf("\nPreorder Traversal (Prefix):\n");
    preorder(root);

    printf("\n\nPostorder Traversal (Postfix):\n");
    postorder(root);

    int result = evaluate(root);
    printf("\n\nEvaluated Arithmetic Result = %d\n", result);

    // SQL query output
    printf("\nPrinting name of person whose age is more than 25\n");
    evaluateSQL("SELECT name FROM table WHERE age > 25");

    return 0;
}