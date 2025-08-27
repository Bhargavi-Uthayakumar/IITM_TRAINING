/**
 * Note: The returned array must be malloced, assume caller calls free().
 */

typedef struct {
    int value;
    int original_index;
} Element;

void mergeSortAndCount(Element* array, int left, int right, int* counts) {
    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;
    mergeSortAndCount(array, left, mid, counts);
    mergeSortAndCount(array, mid + 1, right, counts);
}

int* countSmaller(int* nums, int numsSize, int* returnSize) {
     *returnSize = numsSize;
    if (numsSize == 0) {
        return NULL;
    }

    int* counts = (int*)calloc(numsSize, sizeof(int));
    Element* elements = (Element*)malloc(numsSize * sizeof(Element));

    for (int i = 0; i < numsSize; ++i) {
        elements[i].value = nums[i];
        elements[i].original_index = i;
    }

    mergeSortAndCount(elements, 0, numsSize - 1, counts);

    free(elements);
    return counts;
}
