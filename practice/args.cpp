#include <iostream>
#include <cstdarg>
#include <memory>
#include <functional>
void xprintf()
{
    std::cout << std::endl;
}
template <typename T, typename... Args>
void xprintf(const T &value, Args &&...args)
{
    std::cout << value << " ";
    if ((sizeof...(args)) > 0)
    {
        xprintf(std::forward<Args>(args)...);
    }
    else
    {
        xprintf();
    }
}
int main()
{
    xprintf("Xingguichutu");
    xprintf("Xingguichutu", 666);
    xprintf("Xingguichutu", "Xing", 666);
    return 0;
}