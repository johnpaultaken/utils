#include "queue_mt.h"
#include "../test/test.h"

#include <vector>
#include <list>
#include <memory>
#include <thread>
#include <future>
#include <chrono>

using namespace utils;
using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;

void  test_push_lvalue()
{
    shared_ptr<int> spi = std::make_shared<int>(9);
    utils::queue_mt<shared_ptr<int>> q2;
    q2.push(spi);
    ASSERT_M(spi != nullptr, "interface push lvalue");
}

void  test_push_rvalue()
{
    unique_ptr<int> pi{ new int(7) };
    utils::queue_mt<unique_ptr<int>> q;
    q.push(std::move(pi));
    ASSERT_M(pi == nullptr, "interface push rvalue");
    auto p = q.pop();
    ASSERT_M(*p == 7, "interface push rvalue");

    shared_ptr<int> spi = std::make_shared<int>(9);
    utils::queue_mt<shared_ptr<int>> q2;
    q2.push(std::move(spi));
    ASSERT_M(spi == nullptr, "interface push rvalue");
}

void test_emplace()
{
    class testclass
    {
    public:
        testclass(int i, int *pi): i_{i}, pi_{pi}
        {
        }

        bool operator== (const testclass & rhs)
        {
            return ((i_==rhs.i_) && (pi_ == rhs.pi_));
        }
    private:
        int i_;
        int * pi_;
    };

    int i1=0;
    testclass obj1{i1, &i1};
    int i2=0;
    testclass obj2{i2, &i2};
    utils::queue_mt<testclass> q;
    q.emplace(i1, &i1);
    q.emplace(i2, &i2);
    auto o1 = q.pop();
    ASSERT_M(o1 == obj1, "interface emplace");
    auto o2 = q.pop();
    ASSERT_M(o2 == obj2, "interface emplace");
}

//
// verify that after pop the queue will no longer hold any reference
// to the popped item.
//
void test_pop_releaseref()
{
    shared_ptr<int> spi = std::make_shared<int>(9);
    weak_ptr<int> wpi{ spi };
    utils::queue_mt<shared_ptr<int>> q2;
    q2.push(spi);
    spi.reset();
    ASSERT_M(wpi.lock() != nullptr, "interface pop release reference");
    q2.pop();
    ASSERT_M(wpi.lock() == nullptr, "interface pop release reference");
}

void test_size_empty()
{
    utils::queue_mt<int> q;
    ASSERT_M(q.size() == 0, "interface size");
    ASSERT_M(q.empty() == true, "interface empty");
    q.push(2);
    q.push(2);
    ASSERT_M(q.size() == 2, "interface size");
    ASSERT_M(q.empty() == false, "interface empty");
    q.pop();
    q.push(2);
    q.push(2);
    ASSERT_M(q.size() == 3, "interface size");
    ASSERT_M(q.empty() == false, "interface empty");
    q.pop();
    q.pop();
    q.pop();
    ASSERT_M(q.size() == 0, "interface size");
    ASSERT_M(q.empty() == true, "interface empty");
}

void  test_interface()
{
    test_push_lvalue();
    test_push_rvalue();
    test_emplace();
    test_pop_releaseref();
    test_size_empty();
}

void  test_basic_functionality()
{
    utils::queue_mt<int> q;
    q.push(2);
    q.push(3);
    q.push(4);
    ASSERT_M(q.pop() == 2, "basic");
    q.push(5);
    q.push(6);
    ASSERT_M(q.pop() == 3, "basic");

    std::vector<int> expected{ 4,5,6 };
    std::vector<int> actual;
    while (!q.empty())
    {
        actual.emplace_back(q.pop());
    }
    ASSERT_M(actual == expected, "basic");
}

void test_concurrent_push()
{
    utils::queue_mt<int> q;
    std::atomic<bool> wait{ true };
    std::atomic<unsigned int> count{ 0 };
    auto t1 = std::async(
        std::launch::async,
        [&q, &count, &wait]() {
        count++;
        while (wait) {};
        for (int i = 10; i < 20; ++i)
        {
            q.push(i);
            std::this_thread::yield();
        }
    }
    );
    auto t2 = std::async(
        std::launch::async,
        [&q, &count, &wait]() {
        count++;
        while (wait) {};
        for (int i = 20; i < 30; ++i)
        {
            q.push(i);
            std::this_thread::yield();
        }
    }
    );
    auto t3 = std::async(
        std::launch::async,
        [&q, &count, &wait]() {
        count++;
        while (wait) {};
        for (int i = 30; i < 40; ++i)
        {
            q.push(i);
            std::this_thread::yield();
        }
    }
    );
    auto t4 = std::async(
        std::launch::async,
        [&q, &count, &wait]() {
        count++;
        while (wait) {};
        for (int i = 40; i < 50; ++i)
        {
            q.push(i);
            std::this_thread::yield();
        }
    }
    );
    // all tasks on mark
    while (count < 4);
    // all tasks go
    wait = false;
    t1.wait();
    t2.wait();
    t3.wait();
    t4.wait();

    // verify
    std::list<int> expected;
    for (int i = 10; i < 50; ++i)
    {
        expected.emplace_back(i);
    }
    std::list<int> actual;
    while (!q.empty())
    {
        actual.push_back(q.pop());
    }
    actual.sort();  // needed for unordered_map
    ASSERT_M(actual == expected, "concurrent push");
}

void test_concurrent4x_push_pop()
{
    utils::queue_mt<int> q;

    std::atomic<bool> wait{ true };
    std::atomic<unsigned int> concurrency{ 0 };

    cout << "\nPlease wait a min ...";

    int count = 1024;

    auto do_poppush = (
            [&q, &concurrency, &wait, count]() {
            concurrency++;
            while (wait) {};
            std::vector<int> vi;
            for (int e = 1; e < count; ++e)
            {
                vi.emplace_back(q.pop());
            }
            for (auto e : vi)
            {
                q.push(e);
            }
        }
    );

    auto do_push = (
        [&q, &concurrency, &wait, count]() {
            concurrency++;
            while (wait) {};
            for (int e = 1; e < count; ++e)
            {
                q.push(e);
                // the following ensures some pops go into wait
                if (! q.empty())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(q.size()/20));
                }
            }
        }
    );

    auto t1 = std::thread(do_poppush);
    auto t2 = std::thread(do_push);
    auto t3 = std::thread(do_poppush);
    auto t4 = std::thread(do_push);

    // wait until all threads are on mark.
    while (concurrency < 4);

    // all threads go
    wait = false;

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    // verify
    std::list<int> expected;
    for (int e = 1; e < count; ++e)
    {
        expected.emplace_back(e);
        expected.emplace_back(e);
    }
    std::list<int> actual;
    while (!q.empty())
    {
        actual.push_back(q.pop());
    }
    actual.sort();  // needed for unordered_map
    ASSERT_M(actual == expected, "concurrent 4x pop push");
}

int main(int, char **)
{

    test_interface();
    test_basic_functionality();
    test_concurrent_push();
    test_concurrent4x_push_pop();

    cout << "\ndone\n";
    getchar();
    return 0;
}
