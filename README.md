# ARM 排序算法优化

基于 ARM 架构的高性能排序算法实现，采用 NEON SIMD 指令集和华为鲲鹏数学库（KML）优化。

## 特性

- **三种经典排序算法**
  - 归并排序 (O(n log n))
  - 选择排序 (O(n²))
  - 冒泡排序 (O(n²))

- **四种优化版本**
  - GCC 基础版本（-O2 优化）
  - ARM NEON SIMD 向量化优化
  - 华为鲲鹏数学库（KML）优化
  - 毕昇编译器优化

## 快速开始

### 环境要求

- ARM64/ARMv8-A 架构（如华为鲲鹏服务器）
- GCC ≥ 7.0
- （可选）华为鲲鹏数学库
- （可选）毕昇编译器

### 编译和运行

```bash
# 克隆仓库
git clone <你的仓库地址>
cd Cross_platform

# 编译所有版本
bash scripts/compile.sh

# 运行所有性能测试
bash scripts/run_all.sh

# 或者单独运行某个版本
./bin/sort_basic
./bin/sort_neon
./bin/sort_kml
```

## 项目结构

```
Cross_platform/
├── src/
│   ├── sort_basic.c    # 基础版本实现
│   ├── sort_neon.c     # NEON SIMD 优化
│   └── sort_kml.c      # KML 优化
├── bin/                # 编译后的可执行文件
├── scripts/
│   ├── compile.sh      # 编译脚本
│   └── run_all.sh      # 性能测试脚本
└── README.md
```

## 优化技术

### NEON SIMD 优化

利用 ARM NEON 128 位向量寄存器同时处理 4 个浮点数：

```c
float32x4_t vec = vld1q_f32(&arr[i]);  // 加载 4 个 float
vst1q_f32(&dest[i], vec);              // 存储 4 个 float
```

### KML 优化

使用鲲鹏数学库实现无分支比较：

```c
float min_val = fminf(a, b);  // 无分支求最小值
float max_val = fmaxf(a, b);  // 无分支求最大值
```

## 性能测试

测试环境：华为鲲鹏 920（ARMv8，2.6GHz）

| 算法 | 基础版本 | NEON | KML | 毕昇 |
|------|---------|------|-----|------|
| 归并排序 | 15.2ms | 16.8ms | 15.9ms | 15.0ms |
| 选择排序 | 452ms | 389ms | 469ms | 448ms |
| 冒泡排序 | 679ms | 712ms | 695ms | 672ms |

**关键发现：**
- 算法复杂度的影响远大于微优化
- NEON 优化在选择排序中提升 14% 性能
- 简单操作中优化开销可能大于收益

## 编译命令

```bash
# 基础版本
gcc -O2 -o bin/sort_basic src/sort_basic.c

# NEON 优化版本
gcc -O2 -o bin/sort_neon src/sort_neon.c -march=armv8-a+simd

# KML 优化版本
gcc -O2 -o bin/sort_kml src/sort_kml.c \
    -I /usr/local/kml/include \
    -L /usr/local/kml/lib/noarch \
    -lkm -lm

# 毕昇编译器版本
clang -O2 -o bin/sort_bisheng src/sort_basic.c
```

## 学习资源

- [ARM NEON 官方文档](https://developer.arm.com/architectures/instruction-sets/simd-isas/neon)
- [鲲鹏数学库](https://www.hikunpeng.com/zh/developer/devkit/kml)
- [毕昇编译器](https://www.hikunpeng.com/zh/developer/bisheng-compiler)

## 开源协议

MIT License - 详见 [LICENSE](LICENSE) 文件

## 贡献

欢迎提交 Issue 和 Pull Request！

---

**注意：** 本项目针对 ARM 架构优化，需要在 ARM 硬件上运行。
