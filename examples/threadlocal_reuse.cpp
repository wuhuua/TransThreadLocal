#include <iostream>
#include <vector>
#include "../TransThreadPool.h"

/*
*  This example shows how our TransLocal works with our TransThreadPool
*  The worker will sync with the master thread when task submitted
*/
int main(){
    int a=1,b=2;
    static TransLocal tl=TransLocal(a);
    TransThreadPool pool(4);
    std::vector< std::future<int> > results;
    tl.set<int>(b);
    for(int i=0;i<4;i++){
        results.push_back(pool.enqueue([i]{
            return tl.get<int>();
        }));
    }
    for(std::future<int> &result:results){
        std::cout<<result.get()<<std::endl;
    }
    return 0;
}