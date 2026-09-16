/*房东要把一个房子通过中介出去理解代理模式*/
#include <iostream>
#include <memory>
#include <string>
class RentHouse
{
public:
    virtual void rentHouse() = 0;
};
class Landlord : public RentHouse
{
public:
    void rentHouse() 
    {
        std::cout << "房东出租房子" << std::endl;
    }
};
class Intermediary : public RentHouse
{
public:
    void rentHouse() 
    {
        std::cout << "发布招租启示" << std::endl;
         std::cout << "带人看房" << std::endl;
         _landlord->rentHouse();
        std::cout << "负责租后维修" << std::endl;
    }

private:
    std::shared_ptr<Landlord> _landlord;
};

int main()
{
    std::shared_ptr<Intermediary> intermediary = std::make_shared<Intermediary>();
    intermediary->rentHouse();
    return 0;
}