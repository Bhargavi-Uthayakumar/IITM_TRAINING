/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* maxSlidingWindow(int* nums, int n, int k, int* returnSize) 
{
    int *result = (int*)malloc((n - k + 1) * sizeof(int));
    int *deque = (int*)malloc(n * sizeof(int)); 
    int left = 0, right = 0, index = 0;

    for (int i = 0; i < n; i++) {
        while (left < right && nums[deque[right - 1]] < nums[i]) right--;
        deque[right++] = i;

        if (deque[left] <= i - k) left++;

        if (i >= k - 1) {
            result[index++] = nums[deque[left]];
        }
    }

    *returnSize = index;
    free(deque);
    return result;
}