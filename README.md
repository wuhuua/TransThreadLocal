#  TransThreadPool

## :black_nib: Extension of ThreadPool:black_nib:

### ![brightgreen](https://img.shields.io/badge/-Header_only-brightgreen)![important](https://img.shields.io/badge/-All_platform-important)![blueviolet](https://img.shields.io/badge/-c++_17-blueviolet)![informational](https://img.shields.io/badge/-thread_pool-informational)![red](https://img.shields.io/badge/-English-red)

![Language](https://img.shields.io/badge/language-c++-brightgreen)

![Documentation](https://img.shields.io/badge/documentation-yes-brightgreen)

## :feet: Anouncement
This project is based on ThreadPool, which is based on C++11
[ThreadPool Github Link](https://github.com/progschj/ThreadPool.git)

## :whale:Download 

If you just want to use the TransThreadPool, just copy the header file.
Refer to examples in the `examples/` if you need any instruction
```shell
git clone ${url}
```

## :articulated_lorry: Environment

This project uses `std::any`, which has been supported since C++17. You must compile it with compiler supporting C++17 or later.


## :alarm_clock:Compile
```c++
#include "TransThreadPool"
```
No compile param is to be added



## :memo:Notes
>check how to use thread pool simply
- - [Simple example](./examples/simple_example.cpp)

>check how to use transmittable thread local with our tool
- - [TransThreadLocal set after pool init](./examples/threadlocal_reuse.cpp)
- - [TransThreadLocal pass pointer to share the same object](./examples/threadlocal_ptr.cpp)

>check how to add priority to your tasks
- - [Priority tasks](./examples/priority_example.cpp)



## :sparkling_heart:License

This repository uses the same COPYING Lisence as ThreadPool, feel free to use or distribute it on any purpose



Copyright :copyright:2023 wuhuua

