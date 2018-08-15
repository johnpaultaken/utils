#include "thread_pool.h"

#include <string>

using namespace utils;

int foo(double d, int i)
{
    std::cout << "\n static function works!";
    return d + i;
}

class myc
{
public:
    float getset(float next)
    {
        auto ret = current_;
        current_ = next;
        return ret;
    }
private:
    float current_=0;
};

void copy(int & dst, int src)
{
    dst = src;
}

void display(std::unique_ptr<std::string> && rvalue)
{
    std::unique_ptr<std::string> lvalue{std::forward<std::unique_ptr<std::string>>(rvalue)};
    std::cout << *lvalue;
}

int main()
{
    thread_pool tp;
    double d = 2.3;
    int i = 7;
    tp.async(foo, d, i);

    // the following needs pointer to member specialization.
    //myc obj;
    //float f = 8.3f;
    //tp.async(&myc::getset, &obj, f);

    myc obj2;
    auto stdfunc = std::bind(&myc::getset, &obj2, std::placeholders::_1);
    float p1 = 9.4f;
    tp.async(stdfunc, p1);

    // temps not copied, but forwarded as rvalue. Hence temps cannot be used.
    // The follows causes crash because temps dont last until async gets invoked.
    //tp.async(std::bind(&myc::getset, &obj2, std::placeholders::_1), 9.4f);

    tp.async([](){std::cout << "\n lambda works!";});

    tp.async([]()->void *{std::cout << "\n lambda works with any return type!"; return nullptr;});

    int dst=0;
    int src=7;
    tp.async(copy, dst, src);

    std::unique_ptr<std::string> ps{new std::string{"\n rvalue reference forwarded"}};
    tp.async(display, std::move(ps));

    tp.wait();

    std::cout << (dst==7 ? "\n lvalue reference forwarded" : "\n error: lvalue reference not forwarded");

    auto obj2val = obj2.getset(0);
    std::cout << (obj2val==9.4f ? "\n std::function works!" : "\n error: std::function doesn't work");

    std::cout << "\n done";
}
