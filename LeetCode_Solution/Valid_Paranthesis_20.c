typedef struct {
    char* arr;
    int top;
    int capacity;
} Stack;

Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->arr = (char*)malloc(stack->capacity * sizeof(char));
    return stack;
}

bool isEmpty(Stack* stack) {
    return stack->top == -1;
}

void push(Stack* stack, char item) {
    stack->arr[++stack->top] = item;
}

char pop(Stack* stack) {
    if (isEmpty(stack)) {
        return '\0';
    }
    return stack->arr[stack->top--];
}

char peek(Stack* stack) {
    if (isEmpty(stack)) {
        return '\0';
    }
    return stack->arr[stack->top];
}

bool isValid(char* s) {
    int len = strlen(s);
    if (len == 0) {
        return true;
    }

    Stack* stack = createStack(len); 

    for (int i = 0; i < len; i++) {
        char current_char = s[i];

        if (current_char == '(' || current_char == '[' || current_char == '{') {
            push(stack, current_char);
        } else {
            if (isEmpty(stack)) {
                free(stack->arr);
                free(stack);
                return false; 
            }
            char top_char = pop(stack);
            if ((current_char == ')' && top_char != '(') ||
                (current_char == ']' && top_char != '[') ||
                (current_char == '}' && top_char != '{')) {
                free(stack->arr);
                free(stack);
                return false; 
            }
        }
    }

    bool result = isEmpty(stack); 
    free(stack->arr);
    free(stack);
    return result;
}