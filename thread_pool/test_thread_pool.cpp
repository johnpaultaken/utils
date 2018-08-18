#include "thread_pool.h"

#include <string>

using namespace utils;

int foo(double d, int i)
{
    if (d==2.3 && i==7)
    {
        std::cout << "\n OK : ";
    }
    else
    {
        std::cout << "\n FAIL : ";
    }
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

    // static function pointer call.
    double d = 2.3;
    int i = 7;
    tp.async(&foo, d, i);
    tp.wait();
    std::cout << "static function pointer.";

    // static function reference call.
    tp.async(foo, d, i);
    tp.wait();
    std::cout << "static function reference.";

    // member function call.
    myc obj;
    float f = 8.3f;
    tp.async(&myc::getset, &obj, f);
    tp.wait();
    auto objval = obj.getset(0);
    if (objval==8.3f)
    {
        std::cout << "\n OK : member function.";
    }
    else
    {
        std::cout << "\n FAIL : member function.";
    }

    // std::function call.
    // std::function created with member function bound to parameters using std::bind
    myc obj2;
    auto stdfunc = std::bind(&myc::getset, &obj2, std::placeholders::_1);
    float f2 = 9.4f;
    tp.async(stdfunc, f2);
    tp.wait();
    auto objval2 = obj2.getset(0);
    if (objval2==9.4f)
    {
        std::cout << "\n OK : std::function.";
    }
    else
    {
        std::cout << "\n FAIL : std::function.";
    }

    // lambda call void return.
    tp.async([](){std::cout << "\n OK : lambda void return type.";});
    tp.wait();

    // lambda call non-void return.
    tp.async([]()->void *{std::cout << "\n OK : lambda non-void return type."; return nullptr;});
    tp.wait();

    // lvalue parameter
    int dst=0;
    int src=7;
    tp.async(copy, std::ref(dst), src);
    tp.wait();
    if (dst==7)
    {
        std::cout << "\n OK : lvalue forwarding using std::ref.";
    }
    else
    {
        std::cout << "\n FAIL : lvalue forwarding using std::ref.";
    }


    // rvalue parameter
    // Note: cannot call functions with rvalue parameters for now.
    //std::unique_ptr<std::string> ps{new std::string{"\n OK : rvalue forwarding."}};
    //tp.async(display, std::move(ps));
    //tp.wait();

    // temp parameter
    tp.async(foo, 2.3, 7);
    tp.async(stdfunc, 23.17f);
    tp.wait();
    objval2 = obj2.getset(0);
    if (objval2==23.17f)
    {
        std::cout << "\n OK : temp parameter copy.";
    }
    else
    {
        std::cout << "\n FAIL : temp parameter copy.";
    }

    std::cout << "\n done";
}
