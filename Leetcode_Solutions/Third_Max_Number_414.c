int thirdMax(int* nums, int numsSize) {
    for (int i = 0; i < numsSize - 1; i++) {
        for (int j = i + 1; j < numsSize; j++) {
            if (nums[i] < nums[j]) {
                int temp = nums[i];
                nums[i] = nums[j];
                nums[j] = temp;
            }
        }
    }

    int distinct = 1;
    for (int i = 1; i < numsSize; i++) 
    {
        if (nums[i] != nums[i - 1]) 
        {
            distinct++;
            if (distinct == 3) 
            {
                return nums[i];
            }
        }
    }
    return nums[0]; 
}