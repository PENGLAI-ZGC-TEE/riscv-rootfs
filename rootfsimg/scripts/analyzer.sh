#!/bin/sh

if [ $# -ne 1 ]; then
    echo "Usage: $0 <logfile>"
    exit 1
fi

logfile=$1

# 提取所有数字（兼容BusyBox）
numbers=$(sed -n 's/.*speed tick: \([0-9]\+\).*/\1/p' "$logfile")

# 检查数字数量
count=$(echo "$numbers" | wc -w)
if [ "$count" -ne 8 ]; then
    echo "Error: Found $count numbers, expected 8"
    exit 1
fi

# 将数字加载到位置参数
set -- $numbers

# 定义组名
groups="sha rijndael_enc rijndael_dec bitcnts"

# 计算百分比函数
calc_percent() {
    a=$1
    b=$2
    if [ "$a" -eq 0 ] 2>/dev/null; then
        echo "NaN"
        return
    fi
    
    # 使用bc计算浮点结果
    percent=$(echo "scale=4; ($b - $a)*100/$a" | bc 2>/dev/null)
    
    # 处理bc错误/空输出
    if [ -z "$percent" ]; then
        echo "Error"
    else
        # 格式化为两位小数
        printf "%.2f%%" "$percent"
    fi
}

# 处理并输出每组结果
i=1
for group in $groups; do
    # 使用位置参数直接访问
    a=$1
    shift
    b=$1
    shift
    
    # 修复组名中的空格问题
    case $group in
        rijndael_enc) display_name="rijndael enc" ;;
        rijndael_dec) display_name="rijndael dec" ;;
        *) display_name=$group ;;
    esac
    
    percent=$(calc_percent "$a" "$b")
    printf "%-12s %-12s %-12s %s\n" "$display_name" "$a" "$b" "$percent"
done
