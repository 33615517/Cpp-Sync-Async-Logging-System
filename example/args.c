#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define LOG(fmt, ...) printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
void printNum(int n, ...)
{
    va_list al;
    va_start(al, n); // 让al指向n参数之后的第一个可变参数​
    for (int i = 0; i < n; i++)
    {
        int num = va_arg(al, int); // 从可变参数中取出一个整形参数​
        printf("%d\n", num);
    }
    va_end(al); // 清空可变参数列表--其实是将al置空​
}
void myprintf(const char *fmt, ...)
{
    // int vasprintf(char **strp, const char *fmt, va_list ap);
    char *res;
    va_list al;
    va_start(al, fmt);
    int len = vasprintf(&res, fmt, al);
    va_end(al);
    printf("%s\n", res);
    free(res);
}
int main()
{
    printNum(3, 11, 22, 33);
    printNum(5, 44, 55, 66, 77, 88);
    return 0;
}