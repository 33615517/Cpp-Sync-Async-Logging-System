#include<iostream>
#include<memory>
class Fruit
{
public:
    virtual void name() = 0;

};

class Apple : public Fruit
{
public:
    void name() override
    {
        std::cout << "我是一个苹果" << std::endl;
    }
};

class Banana : public Fruit
{
public:
    void name() override
    {
        std::cout << "我是一个香蕉" << std::endl;
    }
};

// class FruitFactory
// {
// public:
//     static std::shared_ptr<Fruit> createFruit(const std::string &type)
//     {
//         if (type == "苹果")
//         {
//             return std::make_shared<Apple>();
//         }
//         else if (type == "香蕉")
//         {
//             return std::make_shared<Banana>();
//         }
//         else
//         {
//             return nullptr;
//         }
//     }
// };

// class FruitFactory
// {
// public:
//     virtual std::shared_ptr<Fruit> createFruit() = 0;
// };
// class AppleFactory : public FruitFactory
// {
// public:
//     std::shared_ptr<Fruit> createFruit() override
//     {
//         return std::make_shared<Apple>();
//     }       
// };
// class BananaFactory : public FruitFactory
// {
// public:
//     std::shared_ptr<Fruit> createFruit() override
//     {
//         return std::make_shared<Banana>();
//     }
// };
class Animal{
    public:
        virtual void name() = 0;
};
class Lamp:public Animal{
    public:
        void name() override{
            std::cout << "我是一只山羊!" << std::endl;
        }
};
class Dog:public Animal{
    public:
        void name() override{
            std::cout << "我是一只小狗!" << std::endl;
        }
};
class Factory{
public:
    virtual std::shared_ptr<Fruit> getFruit(const std::string &name) = 0;
    virtual std::shared_ptr<Animal> getAnimal(const std::string &name) = 0;
};
class FruitFactory:public Factory{
    public:
        std::shared_ptr<Fruit> getFruit(const std::string &name) override{
            if(name == "苹果"){
                return std::make_shared<Apple>();
            }else if(name == "香蕉"){
                return std::make_shared<Banana>();
            }else{
                return nullptr;
            }
        }
        std::shared_ptr<Animal> getAnimal(const std::string &name) override{
            return nullptr;
        }
};
class AnimalFactory:public Factory
{
    public:
        std::shared_ptr<Fruit> getFruit(const std::string &name) override{
            return nullptr;
        }
        std::shared_ptr<Animal> getAnimal(const std::string &name) override{
            if(name == "山羊"){
                return std::make_shared<Lamp>();
            }else if(name == "小狗"){
                return std::make_shared<Dog>();
            }else{
                return nullptr;
            }
        }
};
class FactoryProducer{
    public:
        static std::shared_ptr<Factory> create(const std::string &name){
            if(name == "水果"){
                return std::make_shared<FruitFactory>();
            }else if(name == "动物"){
                return std::make_shared<AnimalFactory>();
            }else{
                return nullptr;
            }
        }
};
int main()
{
    std::shared_ptr<Factory> ff = FactoryProducer::create("水果"); 
    std::shared_ptr<Fruit> fruit = ff->getFruit("苹果");
    fruit->name();
    fruit = ff->getFruit("香蕉");
    fruit->name();
    std::shared_ptr<Factory> af = FactoryProducer::create("动物");
    std::shared_ptr<Animal> animal = af->getAnimal("山羊");
    animal->name(); 
    animal = af->getAnimal("小狗");
    animal->name(); 





    // std::shared_ptr<FruitFactory> ff(new AppleFactory());
    // std::shared_ptr<Fruit> fruit = ff->createFruit();
    // fruit->name();
    // ff.reset(new BananaFactory());
    // fruit = ff->createFruit();
    // fruit->name();

    // std::shared_ptr<Fruit> fruit = FruitFactory::createFruit("苹果");
    // fruit->name();
    // fruit = FruitFactory::createFruit("香蕉");
    // fruit->name();


    return 0;
}