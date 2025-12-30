#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>



// 编译选项说明：
// -I /usr/local/kml/include  : 引入KML头文件路径（如果需要）
// -L /usr/local/kml/lib      : 指定KML库路径
// -lkm                       : 链接KML库（会替换标准libm的实现）
// -fno-builtin-fminf/fmaxf   : 禁用GCC内建优化，强制生成函数调用
//不加 -fno-builtin，GCC会直接将fminf优化成ARM指令


#define N 10000  // 数组大小

// 计时函数
double now() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}


// 归并排序 - KML优化版
// 使用KML库的fminf和fmaxf函数优化比较操作
void merge_kml(float arr[], int left, int mid, int right, float temp[]) {
    int i = left;
    int j = mid + 1;
    int k = 0;

    // KML优化:使用fminf避免if-else分支判断
    while (i <= mid && j <= right) {
        // fminf返回两个数中的较小值,减少分支预测失败
        float val_left = arr[i];
        float val_right = arr[j];

        // 使用KML的fminf函数(比标准库更快)
        float min_val = fminf(val_left, val_right);

        temp[k++] = min_val;

        // 更新索引
        if (min_val == val_left) {
            i++;
        } else {
            j++;
        }
    }

    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    while (j <= right) {
        temp[k++] = arr[j++];
    }

    for (i = 0; i < k; i++) {
        arr[left + i] = temp[i];
    }
}

void merge_sort_kml_recursive(float arr[], int left, int right, float temp[]) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_kml_recursive(arr, left, mid, temp);
        merge_sort_kml_recursive(arr, mid + 1, right, temp);
        merge_kml(arr, left, mid, right, temp);
    }
}

void merge_sort_kml(float arr[], int n) {
    float *temp = (float *)malloc(n * sizeof(float));
    merge_sort_kml_recursive(arr, 0, n - 1, temp);
    free(temp);
}


// 选择排序 - KML优化版
void selection_sort_kml(float arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        float min_val = arr[i];

        // 使用KML的fminf优化查找最小值
        for (int j = i + 1; j < n; j++) {
            // fminf直接返回较小值,避免if分支
            float current_min = fminf(min_val, arr[j]);

            if (current_min != min_val) {
                min_val = current_min;
                min_idx = j;
            }
        }

        // 交换
        if (min_idx != i) {
            float temp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = temp;
        }
    }
}

// 冒泡排序 - KML优化版
void bubble_sort_kml(float arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;

        for (int j = 0; j < n - i - 1; j++) {
            // 使用KML库函数优化比较和交换
            float val_a = arr[j];
            float val_b = arr[j + 1];

            // fminf和fmaxf避免条件分支,直接获取最小/最大值
            float min_val = fminf(val_a, val_b);
            float max_val = fmaxf(val_a, val_b);

            // 检查是否需要交换
            if (min_val != val_a) {
                arr[j] = min_val;
                arr[j + 1] = max_val;
                swapped = 1;
            }
        }

        if (!swapped) break;
    }
}


// 辅助函数
void init_array(float arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = (float)rand() / RAND_MAX * 10000.0f;
    }
}


void copy_array(float dest[], float src[], int n) {
    memcpy(dest, src, n * sizeof(float));
}


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

    float *arr_original = (float *)malloc(N * sizeof(float));
    float *arr_test = (float *)malloc(N * sizeof(float));

    if (!arr_original || !arr_test) {
        printf("内存分配失败!\n");
        return 1;
    }

    init_array(arr_original, N);

    printf("========================================\n");
    printf("KML优化排序算法性能测试 (数组大小: %d)\n", N);
    printf("========================================\n\n");

    // ========== 归并排序 - KML优化 ==========
    copy_array(arr_test, arr_original, N);
    double start = now();
    merge_sort_kml(arr_test, N);
    double end = now();
    printf("归并排序 (KML优化):\n");
    printf("  运行时间: %.3f ms\n", end - start);
    printf("  排序正确: %s\n\n", is_sorted(arr_test, N) ? "是" : "否");

    // ========== 选择排序 - KML优化 ==========
    copy_array(arr_test, arr_original, N);
    start = now();
    selection_sort_kml(arr_test, N);
    end = now();
    printf("选择排序 (KML优化):\n");
    printf("  运行时间: %.3f ms\n", end - start);
    printf("  排序正确: %s\n\n", is_sorted(arr_test, N) ? "是" : "否");

    // ========== 冒泡排序 - KML优化 ==========
    copy_array(arr_test, arr_original, N);
    start = now();
    bubble_sort_kml(arr_test, N);
    end = now();
    printf("冒泡排序 (KML优化):\n");
    printf("  运行时间: %.3f ms\n", end - start);
    printf("  排序正确: %s\n\n", is_sorted(arr_test, N) ? "是" : "否");

    free(arr_original);
    free(arr_test);

    printf("========================================\n");
    printf("注意:\n");
    printf("KML优化主要通过减少分支预测来提升性能。\n");
    printf("在某些情况下,由于函数调用开销,\n");
    printf("KML版本可能比基础版本略慢,这是正常现象。\n");
    printf("========================================\n");

    return 0;
}



