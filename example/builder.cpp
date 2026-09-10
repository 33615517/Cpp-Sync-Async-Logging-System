/*通过苹果笔记本*电脑的构造理解建造者模式*/

#include <iostream>
#include <string>
#include <memory>

class Computer
{
public:
    Computer() {};
    void setBoard(const std::string &board)
    {
        _board = board;
    }
    void setDisplay(const std::string &display)
    {
        _display = display;
    }
    virtual void setOS() = 0;
    void showParamaters()
    {
        std::cout << "电脑参数: " << std::endl;
        std::cout << "主板: " << _board << std::endl;
        std::cout << "显示器: " << _display << std::endl;
        std::cout << "操作系统: " << _os << std::endl;
    }

protected:
    std::string _board;
    std::string _display;
    std::string _os;
};

class MacBook : public Computer
{
public:
    void setOS() override
    {
        _os = "Mac OS x12";
    }
};

class Builder
{
public:
    virtual void buildBoard(const std::string &board) = 0;
    virtual void buildDisplay(const std::string &display) = 0;
    virtual void buildOS() = 0;
    virtual std::shared_ptr<Computer> build() = 0;
};

class MacBookBuilder : public Builder
{
public:
    MacBookBuilder() : _computer(std::make_shared<MacBook>())
    {
    }
    void buildBoard(const std::string &board) override
    {
        _computer->setBoard(board);
    }
    void buildDisplay(const std::string &display) override
    {
        _computer->setDisplay(display);
    }
    void buildOS() override
    {
        _computer->setOS();
    }
    std::shared_ptr<Computer> build()
    {
        return _computer;
    }

private:
    std::shared_ptr<Computer> _computer;
};

class Director
{
public:
    Director(std::shared_ptr<Builder> builder) : _builder(builder)
    {
    }
    void construct(const std::string &board, const std::string &display)
    {
        _builder->buildBoard(board);
        _builder->buildDisplay(display);
        _builder->buildOS();
    }
private:
    std::shared_ptr<Builder> _builder;
};

int main()
{
    std::shared_ptr<Builder> builder = std::make_shared<MacBookBuilder>();
    Director director(builder);
    director.construct("华硕主板", "三星显示器");
    std::shared_ptr<Computer> computer = builder->build();
    computer->showParamaters();
    return 0;
}