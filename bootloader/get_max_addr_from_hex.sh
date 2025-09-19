#!/bin/bash

# 初始化变量
max_address=0
max_length=0
result=0

# 处理输入的函数
process_input() {
    while IFS= read -r line; do
        if [[ ! "$line" =~ ^: ]]; then
            continue
        fi

        len_hex="${line:1:2}"
        addr_hex="${line:3:4}"

        # 如果长度或地址为空，跳过（例如空行）
        if [ -z "$len_hex" ] || [ -z "$addr_hex" ]; then
            continue
        fi

        len=$((16#$len_hex))
        addr=$((16#$addr_hex))

        if [ "$len" -eq 0 ]; then
            continue
        fi

        if [ "$addr" -gt "$max_address" ]; then
            max_address=$addr
            max_length=$len
        fi
    done
}

# 检查是否有命令行参数
if [ $# -ge 1 ]; then
    # 有参数，使用第一个文件作为输入
    if [ ! -f "$1" ]; then
        echo "错误: 文件不存在: $1" >&2
        exit 1
    fi
    process_input < "$1"
else
    # 没有参数，从标准输入读取
    process_input
fi

# 计算并输出结果
if [ $max_address -eq 0 ] && [ $max_length -eq 0 ]; then
    echo "错误: 未找到有效的HEX数据行" >&2
    exit 1
fi

result=$((max_address + max_length))
# printf "最大地址的行: 0x%X\n" "$max_address"
# printf "数据长度: %d (0x%X)\n" "$max_length" "$max_length"
# printf "地址 + 长度 = 0x%X\n" "$result"

echo "0x$(printf '%X' $result)"
