# basic-coro - c++ coroutine library
Library that implements helper types for using c++ coroutines. Please be aware that this is a training project for me - I wanted to learn more about CMake, gtest and git submodules.

## Usage
### Prerequisites
* g++-10

### Installing
```
mkdir build && cd build
cmake -D CMAKE_CXX_COMPILER=g++-10 ..
make install
```
This will install appropriate headers into `./include/` and static linked library into `./lib/`.

### Classes
Library includes following classes:
* `SingleEvent<T>` which models `co_await` enabled event that can be set,
* `AwaitableTask<T>` which models `co_await` enabled task.

Please note that these classes are not multithreading enabled. There is no synchronization or any kind of protection form race conditions. If you need to use coroutines with multithreading, just use [CppCoro](https://github.com/lewissbaker/cppcoro). This library is mostly thought for use with simple GUI programming.

### Example
```c++
#include <iostream>

#include <basiccoro/AwaitableTask.hpp>
#include <basiccoro/SingleEvent.hpp>

basiccoro::AwaitableTask<void> consumer(basiccoro::SingleEvent<int>& event)
{
    std::cout << "consumer: start waiting" << std::endl;

    while (true)
    {
        const auto i = co_await event;
        std::cout << "consumer: received: " << i << std::endl;
    }
}

int main()
{
    basiccoro::SingleEvent<int> event;
    consumer(event);

    while (true)
    {
        int i = 0;

        std::cout << "Enter no.(1-9): ";
        std::cin >> i;

        if (i == 0)
        {
            break;
        }
        else if (1 <= i && i <= 9)
        {
            event.set(i);
        }
    }
}
```
Simple example highlighting use of coroutines in producer-consumer problem.

## Acknowledgments
* [CMake C++ Project Template](https://github.com/kigster/cmake-project-template) as this project is based on this template
* Lewis Baker has excellent [articles](https://lewissbaker.github.io/) on topic of coroutines and assymetric transfer. This project is mostly based on information (and code snippets) contained in those articles.
