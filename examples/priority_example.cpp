#include <iostream>
#include <vector>
#include "../TransThreadPool.h"

/*
*  This example shows how our TransThreadPool works with priority
*  default priority is 0, and larger value means higher priority
*/
int main(){
    TransThreadPool pool(4);
    std::vector< std::future<std::pair<std::thread::id,int> > > results;
    for(int i=0;i<8;i++){
        results.push_back(pool.enqueue(i,[i]{
            std::thread::id threadId = std::this_thread::get_id();
            std::cout<<"Thread is "<<threadId<<", Number is "<<i<<std::endl;
            return std::pair<std::thread::id,int>(threadId,i);
        }));
    }
    for(std::future<std::pair<std::thread::id,int>> &result:results){
        std::pair<std::thread::id,int> tuple=result.get();
    }
    return 0;
}