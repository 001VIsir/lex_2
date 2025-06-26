#include <stdio.h>
#include <stdlib.h>

/* 
 * 这是一个测试C语言词法分析器的示例程序
 * 包含各种C语言语法元素
 */

// 全局变量定义
int global_var = 100;
float pi = 3.14159;
char greeting[] = "Hello, World!";

// 函数声明
int add(int a, int b);
void print_array(int arr[], int size);

int main() {
    // 局部变量定义
    int numbers[5] = {1, 2, 3, 4, 5};
    char ch = 'A';
    double result = 0.0;
    
    // 控制结构测试
    if (global_var > 50) {
        printf("Global variable is greater than 50\n");
    } else {
        printf("Global variable is not greater than 50\n");
    }
    
    // 循环结构测试
    for (int i = 0; i < 5; i++) {
        printf("Number %d: %d\n", i + 1, numbers[i]);
        result += numbers[i] * pi;
    }
    
    // 函数调用测试
    int sum = add(10, 20);
    printf("Sum: %d\n", sum);
    
    // 指针操作测试
    int *ptr = &sum;
    printf("Address of sum: %p, Value: %d\n", (void*)ptr, *ptr);
    
    // 位运算测试
    int a = 12, b = 5;
    printf("a & b = %d\n", a & b);
    printf("a | b = %d\n", a | b);
    printf("a ^ b = %d\n", a ^ b);
    printf("~a = %d\n", ~a);
    printf("a << 2 = %d\n", a << 2);
    printf("a >> 2 = %d\n", a >> 2);
    
    // 条件运算符测试
    int max = (a > b) ? a : b;
    printf("Max of %d and %d is %d\n", a, b, max);
    
    return 0;
}

// 函数定义
int add(int a, int b) {
    return a + b;
}

void print_array(int arr[], int size) {
    printf("Array elements: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}