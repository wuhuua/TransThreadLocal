#include <iostream>
#include <vector>
#include "../TransThreadPool.h"

/*
*  This example shows how our TransLocal works with our TransThreadPool
*  If you want the worker share the same value with matser, pass pointer
*  otherwise there will be a new copy
*/
int main(){
    int a=1,b=2;
    int* ptr=&a;
    static TransLocal tl=TransLocal(ptr);
    TransThreadPool pool(4);
    std::vector< std::future<int> > results;
    a=2;
    for(int i=0;i<4;i++){
        results.push_back(pool.enqueue([i]{
            return *tl.get<int*>();
        }));
    }
    for(std::future<int> &result:results){
        std::cout<<result.get()<<std::endl;
    }
    return 0;
}