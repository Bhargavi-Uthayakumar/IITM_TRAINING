#define MAX_SIZE 100000 

typedef struct {
    int arr[MAX_SIZE];
    int size;
} Heap;

void maxHeapPush(Heap *h, int val) {
    int i = h->size++;
    h->arr[i] = val;
    while (i > 0 && h->arr[(i-1)/2] < h->arr[i]) {
        int tmp = h->arr[i];
        h->arr[i] = h->arr[(i-1)/2];
        h->arr[(i-1)/2] = tmp;
        i = (i-1)/2;
    }
}

int maxHeapPop(Heap *h) {
    int top = h->arr[0];
    h->arr[0] = h->arr[--h->size];
    int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, largest = i;
        if (l < h->size && h->arr[l] > h->arr[largest]) largest = l;
        if (r < h->size && h->arr[r] > h->arr[largest]) largest = r;
        if (largest == i) break;
        int tmp = h->arr[i]; h->arr[i] = h->arr[largest]; h->arr[largest] = tmp;
        i = largest;
    }
    return top;
}

int maxHeapTop(Heap *h) {
    return h->arr[0];
}

void minHeapPush(Heap *h, int val) {
    int i = h->size++;
    h->arr[i] = val;
    while (i > 0 && h->arr[(i-1)/2] > h->arr[i]) {
        int tmp = h->arr[i];
        h->arr[i] = h->arr[(i-1)/2];
        h->arr[(i-1)/2] = tmp;
        i = (i-1)/2;
    }
}

int minHeapPop(Heap *h) {
    int top = h->arr[0];
    h->arr[0] = h->arr[--h->size];
    int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, smallest = i;
        if (l < h->size && h->arr[l] < h->arr[smallest]) smallest = l;
        if (r < h->size && h->arr[r] < h->arr[smallest]) smallest = r;
        if (smallest == i) break;
        int tmp = h->arr[i]; h->arr[i] = h->arr[smallest]; h->arr[smallest] = tmp;
        i = smallest;
    }
    return top;
}

int minHeapTop(Heap *h) {
    return h->arr[0];
}

typedef struct {
    Heap lo; 
    Heap hi; 
} MedianFinder;

MedianFinder* medianFinderCreate() {
    MedianFinder* mf = (MedianFinder*)malloc(sizeof(MedianFinder));
    mf->lo.size = 0;
    mf->hi.size = 0;
    return mf;
}

void medianFinderAddNum(MedianFinder* mf, int num) {
    maxHeapPush(&mf->lo, num);

    minHeapPush(&mf->hi, maxHeapPop(&mf->lo));

    if (mf->lo.size < mf->hi.size) {
        maxHeapPush(&mf->lo, minHeapPop(&mf->hi));
    }
}

double medianFinderFindMedian(MedianFinder* mf) {
    if (mf->lo.size > mf->hi.size) {
        return (double)maxHeapTop(&mf->lo);
    } else {
        return (maxHeapTop(&mf->lo) + minHeapTop(&mf->hi)) / 2.0;
    }
}

void medianFinderFree(MedianFinder* mf) {
    free(mf);
}

