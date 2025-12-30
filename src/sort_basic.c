#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 10000  // 数组大小

// 计时函数
double now() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

// 归并排序 
void merge(float arr[], int left, int mid, int right, float temp[]) {
    int i = left;    // 左子数组起始位置
    int j = mid + 1; // 右子数组起始位置
    int k = 0;       // 临时数组索引

    // 比较左右子数组元素,将较小的放入临时数组
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    // 复制左子数组剩余元素
    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    // 复制右子数组剩余元素
    while (j <= right) {
        temp[k++] = arr[j++];
    }

    // 将排序好的元素复制回原数组
    for (i = 0; i < k; i++) {
        arr[left + i] = temp[i];
    }
}

void merge_sort_recursive(float arr[], int left, int right, float temp[]) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_recursive(arr, left, mid, temp);      // 排序左半部分
        merge_sort_recursive(arr, mid + 1, right, temp); // 排序右半部分
        merge(arr, left, mid, right, temp);              // 合并两个有序子数组
    }
}

void merge_sort(float arr[], int n) {
    float *temp = (float *)malloc(n * sizeof(float));
    merge_sort_recursive(arr, 0, n - 1, temp);
    free(temp);
}


// 选择排序 (Selection Sort)
void selection_sort(float arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;

        // 在未排序部分找到最小元素的索引
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        // 交换最小元素到已排序部分的末尾
        if (min_idx != i) {
            float temp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = temp;
        }
    }
}


// 冒泡排序 (Bubble Sort)
void bubble_sort(float arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;

        // 每次遍历将最大元素"冒泡"到末尾
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // 交换相邻元素
                float temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }

        // 如果这一轮没有交换,说明已排序完成
        if (!swapped) break;
    }
}


// 辅助函数
// 初始化随机数组
void init_array(float arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = (float)rand() / RAND_MAX * 10000.0f;
    }
}

// 复制数组
void copy_array(float dest[], float src[], int n) {
    memcpy(dest, src, n * sizeof(float));
}

// 验证数组是否已排序
int is_sorted(float arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            return 0;
        }
    }
    return 1;
}



// 主函数
int main() {
    srand(time(NULL));

    // 分配内存
    float *arr_original = (float *)malloc(N * sizeof(float));
    float *arr_test = (float *)malloc(N * sizeof(float));

    if (!arr_original || !arr_test) {
        printf("内存分配失败!\n");
        return 1;
    }

    // 初始化随机数组
    init_array(arr_original, N);

    printf("========================================\n");
    printf("基础排序算法性能测试 (数组大小: %d)\n", N);
    printf("========================================\n\n");

    //归并排序
    copy_array(arr_test, arr_original, N);
    double start = now();
    merge_sort(arr_test, N);
    double end = now();
    printf("归并排序:\n");
    printf("  运行时间: %.3f ms\n", end - start);
    printf("  排序正确: %s\n\n", is_sorted(arr_test, N) ? "是" : "否");

    //选择排序
    copy_array(arr_test, arr_original, N);
    start = now();
    selection_sort(arr_test, N);
    end = now();
    printf("选择排序:\n");
    printf("  运行时间: %.3f ms\n", end - start);
    printf("  排序正确: %s\n\n", is_sorted(arr_test, N) ? "是" : "否");

    
    //冒泡排序
    copy_array(arr_test, arr_original, N);
    start = now();
    bubble_sort(arr_test, N);
    end = now();
    printf("冒泡排序:\n");
    printf("  运行时间: %.3f ms\n", end - start);
    printf("  排序正确: %s\n\n", is_sorted(arr_test, N) ? "是" : "否");

    // 释放内存
    free(arr_original);
    free(arr_test);

    return 0;
}
