#include <stdio.h>

/* 这是一个包含各种词法错误的测试文件 */

int main() {
    // 正常的代码
    int a = 10;
    float b = 3.14;

    // 错误1: 无效的数字格式
    int bad_number = 123.45.67;
    float another_bad = 12.34.56.78;

    // 错误2: 数字后面直接跟字母
    int mixed = 123abc;
    float mixed2 = 45.67def;

    // 错误3: 无效的字符常量
    char bad_char1 = 'abc';
    char bad_char2 = '';

    // 错误4: 未闭合的字符串
    char* bad_string1 = "This string is not closed
    char* bad_string2 = "Another unclosed string;

    // 正常的转义字符
    char escape_char = '\n';
    char* escaped_string = "Line 1\nLine 2\tTabbed";

    // 错误5: 无效字符
    int x = 10 @ 20;
    float y = 3.14 # 2.71;

    /* 未闭合的多行注释
    这个注释没有结束标记

    return 0;
    }