#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <arm_neon.h> 

#define N 10000  // 数组大小


// 计时函数
double now() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

// 归并排序 - NEON优化版
void merge_neon(float arr[], int left, int mid, int right, float temp[]) {
    int i = left;
    int j = mid + 1;
    int k = 0;
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    // 使用NEON向量化复制剩余元素
    // 复制左侧剩余元素
    int left_remaining = mid - i + 1;
    int vec_count = left_remaining / 4;
    for (int v = 0; v < vec_count; v++) {
        float32x4_t vec_data = vld1q_f32(&arr[i]);
        vst1q_f32(&temp[k], vec_data);
        i += 4;
        k += 4;
    }
    // 处理不足4个的剩余元素
    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    // 复制右侧剩余元素
    int right_remaining = right - j + 1;
    vec_count = right_remaining / 4;
    for (int v = 0; v < vec_count; v++) {
        float32x4_t vec_data = vld1q_f32(&arr[j]);
        vst1q_f32(&temp[k], vec_data);
        j += 4;
        k += 4;
    }
    // 处理不足4个的剩余元素
    while (j <= right) {
        temp[k++] = arr[j++];
    }

    // 使用NEON向量化复制结果回原数组(一次复制4个元素)
    int total_elements = right - left + 1;
    vec_count = total_elements / 4;
    for (int v = 0; v < vec_count; v++) {
        float32x4_t vec_data = vld1q_f32(&temp[v * 4]);
        vst1q_f32(&arr[left + v * 4], vec_data);
    }
    // 处理不足4个的剩余元素
    for (int idx = vec_count * 4; idx < total_elements; idx++) {
        arr[left + idx] = temp[idx];
    }
}

void merge_sort_neon_recursive(float arr[], int left, int right, float temp[]) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_neon_recursive(arr, left, mid, temp);
        merge_sort_neon_recursive(arr, mid + 1, right, temp);
        merge_neon(arr, left, mid, right, temp);
    }
}

void merge_sort_neon(float arr[], int n) {
    float *temp = (float *)malloc(n * sizeof(float));
    merge_sort_neon_recursive(arr, 0, n - 1, temp);
    free(temp);
}


// 选择排序 - NEON优化版
void selection_sort_neon(float arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        float min_val = arr[i];

        int j = i + 1;

        // 向量化查找最小值:一次比较4个元素
        // 每次处理4个元素，用NEON找到这4个中的最小值
        for (; j + 3 < n; j += 4) {
            float32x4_t vec_data = vld1q_f32(&arr[j]);  // 加载4个元素

            // 水平归约找出4个元素中的最小值
            float32x2_t min_pair = vpmin_f32(vget_low_f32(vec_data), vget_high_f32(vec_data));
            min_pair = vpmin_f32(min_pair, min_pair);
            float block_min = vget_lane_f32(min_pair, 0);

            // 如果这个块的最小值比当前最小值小，找出具体索引
            if (block_min < min_val) {
                for (int k = 0; k < 4; k++) {
                    if (arr[j + k] < min_val) {
                        min_val = arr[j + k];
                        min_idx = j + k;
                    }
                }
            }
        }

        // 处理剩余元素
        for (; j < n; j++) {
            if (arr[j] < min_val) {
                min_val = arr[j];
                min_idx = j;
            }
        }

        // 交换最小元素
        if (min_idx != i) {
            float temp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = temp;
        }
    }
}


// 冒泡排序 - NEON优化版
void bubble_sort_neon(float arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        int j = 0;

        // 向量化比较交换:一次处理4个元素
        for (; j + 3 < n - i - 1; j += 4) {
            float32x4_t vec_a = vld1q_f32(&arr[j]);      // [a0, a1, a2, a3]
            float32x4_t vec_b = vld1q_f32(&arr[j + 1]);  // [a1, a2, a3, a4]

            // 使用vpmin和vpmax进行水平归约检测是否需要交换
            float32x2_t low_a = vget_low_f32(vec_a);
            float32x2_t high_a = vget_high_f32(vec_a);
            float32x2_t low_b = vget_low_f32(vec_b);
            float32x2_t high_b = vget_high_f32(vec_b);

            // 计算vec_a的最大值和vec_b的最小值
            float32x2_t max_pair = vpmax_f32(low_a, high_a);
            max_pair = vpmax_f32(max_pair, max_pair);
            float max_a = vget_lane_f32(max_pair, 0);

            float32x2_t min_pair = vpmin_f32(low_b, high_b);
            min_pair = vpmin_f32(min_pair, min_pair);
            float min_b = vget_lane_f32(min_pair, 0);

            // 如果vec_a的最大值大于vec_b的最小值，说明可能需要交换
            if (max_a > min_b) {
                // 逐个检查并交换相邻元素
                for (int k = 0; k < 4 && (j + k + 1) < n - i; k++) {
                    if (arr[j + k] > arr[j + k + 1]) {
                        float temp = arr[j + k];
                        arr[j + k] = arr[j + k + 1];
                        arr[j + k + 1] = temp;
                        swapped = 1;
                    }
                }
            }
        }

        // 处理剩余元素
        for (; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                float temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
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
    printf("NEON优化排序算法性能测试 (数组大小: %d)\n", N);
    printf("========================================\n\n");

    // ========== 归并排序 - NEON优化 ==========
    copy_array(arr_test, arr_original, N);
    double start = now();
    merge_sort_neon(arr_test, N);
    double end = now();
    printf("归并排序 (NEON优化):\n");
    printf("  运行时间: %.3f ms\n", end - start);
    printf("  排序正确: %s\n\n", is_sorted(arr_test, N) ? "是" : "否");

    // ========== 选择排序 - NEON优化 ==========
    copy_array(arr_test, arr_original, N);
    start = now();
    selection_sort_neon(arr_test, N);
    end = now();
    printf("选择排序 (NEON优化):\n");
    printf("  运行时间: %.3f ms\n", end - start);
    printf("  排序正确: %s\n\n", is_sorted(arr_test, N) ? "是" : "否");

    // ========== 冒泡排序 - NEON优化 ==========
    copy_array(arr_test, arr_original, N);
    start = now();
    bubble_sort_neon(arr_test, N);
    end = now();
    printf("冒泡排序 (NEON优化):\n");
    printf("  运行时间: %.3f ms\n", end - start);
    printf("  排序正确: %s\n\n", is_sorted(arr_test, N) ? "是" : "否");

    free(arr_original);
    free(arr_test);

    return 0;
}
