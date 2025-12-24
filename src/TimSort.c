#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define RUN 32

typedef struct {
    int *arr;
    int left;
    int right;
    sem_t *semaphore;
} ThreadData;

void insertionSort(int arr[], int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int temp = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

void merge(int arr[], int l, int m, int r) {
    int len1 = m - l + 1, len2 = r - m;
    int *left = malloc(len1 * sizeof(int));
    int *right = malloc(len2 * sizeof(int));

    for (int i = 0; i < len1; i++) left[i] = arr[l + i];
    for (int i = 0; i < len2; i++) right[i] = arr[m + 1 + i];

    int i = 0, j = 0, k = l;
    while (i < len1 && j < len2) {
        if (left[i] <= right[j]) arr[k++] = left[i++];
        else arr[k++] = right[j++];
    }
    while (i < len1) arr[k++] = left[i++];
    while (j < len2) arr[k++] = right[j++];

    free(left);
    free(right);
}

void* threadSort(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    insertionSort(data->arr, data->left, data->right);
    
    sem_post(data->semaphore);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Использование: %s <размер_массива> <макс_потоков>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int max_threads = atoi(argv[2]);

    int *arr = malloc(n * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < n; i++) arr[i] = rand() % 10000;

    sem_t semaphore;
    sem_init(&semaphore, 0, max_threads);

    int num_segments = (n + RUN - 1) / RUN;
    pthread_t threads[num_segments];
    ThreadData t_data[num_segments];

    printf("Начало сортировки %d элементов с лимитом %d потоков...\n", n, max_threads);

    for (int i = 0; i < num_segments; i++) {
        sem_wait(&semaphore); 
        
        t_data[i].arr = arr;
        t_data[i].left = i * RUN;
        t_data[i].right = (i + 1) * RUN - 1;
        if (t_data[i].right >= n) t_data[i].right = n - 1;
        t_data[i].semaphore = &semaphore;

        pthread_create(&threads[i], NULL, threadSort, &t_data[i]);
    }

    for (int i = 0; i < num_segments; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int size = RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = (left + 2 * size - 1 < n - 1) ? (left + 2 * size - 1) : (n - 1);
            if (mid < right) merge(arr, left, mid, right);
        }
    }

    printf("Сортировка завершена.\n");

    sem_destroy(&semaphore);
    free(arr);
    return 0;
}
