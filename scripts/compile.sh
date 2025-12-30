#!/bin/bash
# 排序优化实验 - 编译脚本

# 说明:此脚本需要在ARM服务器上运行
# 用途:编译所有版本的排序程序


echo "开始编译排序优化实验程序"


# 创建bin目录
mkdir -p bin

#编译基础版本 (使用GCC) 
echo ""
echo "[1/4] 编译基础版本 (gcc -O2)..."
gcc -O2 -o bin/sort_basic src/sort_basic.c
if [ $? -eq 0 ]; then
    echo "✓ 基础版本编译成功: bin/sort_basic"
else
    echo "✗ 基础版本编译失败"
    exit 1
fi

#2. 编译NEON优化版本
echo ""
echo "[2/4] 编译NEON优化版本 (gcc -O2 -march=armv8-a+simd)..."
gcc -O2 -o bin/sort_neon src/sort_neon.c -march=armv8-a+simd
if [ $? -eq 0 ]; then
    echo "✓ NEON优化版本编译成功: bin/sort_neon"
else
    echo "✗ NEON优化版本编译失败"
    exit 1
fi

#3. 编译KML优化版本
echo ""
echo "[3/4] 编译KML优化版本 (gcc -O2 + KML库)..."
# 关键点:
# -fno-builtin-fminf -fno-builtin-fmaxf: 禁用编译器对这些函数的内建优化
#   → 如果不加这个选项，GCC会直接将fminf优化成ARM的FMIN指令
#   → 这样就完全绕过了函数调用，KML库根本没机会工作！
# -lkm: 链接KML库，会替换标准libm的fminf/fmaxf实现
gcc -O2 -fno-builtin-fminf -fno-builtin-fmaxf \
    -o bin/sort_kml src/sort_kml.c \
    -I /usr/local/kml/include \
    -L /usr/local/kml/lib/noarch \
    -lkm -lm

if [ $? -eq 0 ]; then
    echo "✓ KML优化版本编译成功: bin/sort_kml"
else
    echo "✗ KML优化版本编译失败"
    echo "  (可能是KML库未安装或路径不正确)"
fi

#4. 使用毕昇编译器编译基础版本
echo ""
echo "[4/4] 使用毕昇编译器编译基础版本 (clang -O2)..."

# 检查是否安装了毕昇编译器
if command -v clang &> /dev/null; then
    clang_version=$(clang -v 2>&1 | head -n 1)
    echo "检测到编译器: $clang_version"

    clang -O2 -o bin/sort_bisheng src/sort_basic.c
    if [ $? -eq 0 ]; then
        echo "✓ 毕昇编译器版本编译成功: bin/sort_bisheng"
    else
        echo "✗ 毕昇编译器版本编译失败"
    fi
else
    echo "⚠ 未检测到clang编译器"
    echo "  如需使用毕昇编译器,请先安装:"
    echo "  yum install bisheng-compiler"
fi

echo ""
echo "编译完成!"
echo "生成的可执行文件:"
ls -lh bin/
echo ""

