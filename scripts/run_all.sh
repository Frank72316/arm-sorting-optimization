#!/bin/bash

# 排序优化实验 - 运行所有测试
# 说明:运行所有编译好的程序并比较性能


echo "排序优化实验 - 性能对比测试"
echo "数组大小: 10000"
echo "测试时间: $(date '+%Y-%m-%d %H:%M:%S')"


#1. 基础版本 (GCC)
if [ -f bin/sort_basic ]; then
    echo ""
    echo "测试 1: 基础版本 (GCC -O2)"
    ./bin/sort_basic
else
    echo "⚠ bin/sort_basic 不存在,请先运行 bash scripts/compile.sh"
fi

#2. NEON优化版本
if [ -f bin/sort_neon ]; then
    echo ""
    echo "测试 2: NEON优化版本 (ARM NEON指令集)"
    ./bin/sort_neon
else
    echo "⚠ bin/sort_neon 不存在,请先运行 bash scripts/compile.sh"
fi

#3. KML优化版本
if [ -f bin/sort_kml ]; then
    echo ""
    echo "测试 3: KML优化版本 (鲲鹏数学库)"
    ./bin/sort_kml
else
    echo "⚠ bin/sort_kml 不存在,可能KML库未安装"
fi

#4. 毕昇编译器版本
if [ -f bin/sort_bisheng ]; then
    echo ""
    echo "测试 4: 毕昇编译器版本 (BiSheng Compiler)"
    ./bin/sort_bisheng
else
    echo "⚠ bin/sort_bisheng 不存在,可能毕昇编译器未安装"
fi


