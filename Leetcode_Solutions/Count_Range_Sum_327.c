long long* calculatePrefixSums(int* nums, int numsSize) {
    long long* prefixSums = (long long*)malloc((numsSize + 1) * sizeof(long long));
    prefixSums[0] = 0;
    for (int i = 0; i < numsSize; ++i) {
        prefixSums[i + 1] = prefixSums[i] + nums[i];
    }
    return prefixSums;
}

int mergeSortAndCount(long long* prefixSums, int start, int end, int lower, int upper) {
    if (end - start <= 1) {
        return 0; 
    }

    int mid = start + (end - start) / 2;
    int count = mergeSortAndCount(prefixSums, start, mid, lower, upper) +
                mergeSortAndCount(prefixSums, mid, end, lower, upper);

    int j = mid, k = mid;
    for (int i = start; i < mid; ++i) {
        while (j < end && prefixSums[j] - prefixSums[i] < lower) {
            j++;
        }
        while (k < end && prefixSums[k] - prefixSums[i] <= upper) {
            k++;
        }
        count += (k - j);
    }

    long long* temp = (long long*)malloc((end - start) * sizeof(long long));
    int p1 = start, p2 = mid, current = 0;
    while (p1 < mid && p2 < end) {
        if (prefixSums[p1] < prefixSums[p2]) {
            temp[current++] = prefixSums[p1++];
        } else {
            temp[current++] = prefixSums[p2++];
        }
    }
    while (p1 < mid) {
        temp[current++] = prefixSums[p1++];
    }
    while (p2 < end) {
        temp[current++] = prefixSums[p2++];
    }
    for (int i = 0; i < (end - start); ++i) {
        prefixSums[start + i] = temp[i];
    }
    free(temp);

    return count;
}

int countRangeSum(int* nums, int numsSize, int lower, int upper) {
    long long* prefixSums = calculatePrefixSums(nums, numsSize);
    int result = mergeSortAndCount(prefixSums, 0, numsSize + 1, lower, upper);
    free(prefixSums);
    return result;
}