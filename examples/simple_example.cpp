#include <iostream>
#include <vector>
#include "../TransThreadPool.h"

/*
*  This example shows how our TransThreadPool works
*  Use an integer to init the pool
*  Then use enqueue function to add task
*  The future returned from enqueue function can get the result
*/
int main(){
    TransThreadPool pool(4);
    std::vector< std::future<std::pair<std::thread::id,int> > > results;
    for(int i=0;i<8;i++){
        results.push_back(pool.enqueue([i]{
            std::thread::id threadId = std::this_thread::get_id();
            return std::pair<std::thread::id,int>(threadId,i);
        }));
    }
    for(std::future<std::pair<std::thread::id,int>> &result:results){
        std::pair<std::thread::id,int> tuple=result.get();
        std::cout<<"Thread is "<<tuple.first<<", Number is "<<tuple.second<<std::endl;
    }
    return 0;
}