#ifndef TRANS_THREAD_POOL_H
#define TRANS_THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <typeinfo>
#include <any>

class TransThreadPool;
class TransTask;
class TransLocal;

class TransLocal{
private:
    static thread_local std::unordered_map<TransLocal*,std::any> transLocals;
    static void setMap(std::unordered_map<TransLocal*,std::any>& transLocals_);
    static std::unordered_map<TransLocal*,std::any> getMap();
    class type;
public:
    friend TransTask;
    friend TransThreadPool;
    template<class T>
    TransLocal(T &value);
    ~TransLocal();
    template<class T>
    T get();
    template<class T>
    void set(T &value);
};
template<class T>
TransLocal::TransLocal(T &value){
    set(value);
}
void TransLocal::setMap(std::unordered_map<TransLocal*,std::any> &transLocals_){
    for (const auto& transLocal : transLocals_) {
        TransLocal* key = transLocal.first;
        std::any value = transLocal.second;
        transLocals[key] = value;
    }
}

thread_local std::unordered_map<TransLocal*,std::any> TransLocal::transLocals=std::unordered_map<TransLocal*,std::any>();

TransLocal::~TransLocal(){
    transLocals.erase(this);
}

std::unordered_map<TransLocal*,std::any> TransLocal::getMap(){
    std::unordered_map<TransLocal*,std::any> transLocals_=
    std::unordered_map<TransLocal*,std::any>(transLocals.size());
    for (const auto& transLocal : transLocals) {
        TransLocal* key = transLocal.first;
        std::any value = transLocal.second;
        transLocals_[key] = value;
    }
    return transLocals_;
}
template<class T>
T TransLocal::get(){
    return std::any_cast<T>(transLocals[this]);
}
template<class T>
void TransLocal::set(T &value){
    transLocals[this]=value;
}

class TransTask{
private:
    int priority_;
    std::function<void()> task_;
    std::unordered_map<TransLocal*,std::any> taskLocals_;
public:
    TransTask()=default;
    TransTask(int priority,std::function<void()> task);
    std::function<void()>& task();
    std::unordered_map<TransLocal*,std::any>& taskLocals();
    int priority();
};
TransTask::TransTask(int priority,std::function<void()> task){
    priority_=priority;
    task_=task;
    taskLocals_=TransLocal::getMap();  
}
std::function<void()>& TransTask::task(){
    return task_;
}
std::unordered_map<TransLocal*,std::any>& TransTask::taskLocals(){
    return taskLocals_;
}
int TransTask::priority(){
    return priority_;
}

class CompLess{
public:
    bool operator()(TransTask a,TransTask b){
        return a.priority()<b.priority();
    }
};


class TransThreadPool{
public:
    TransThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
        template<class F, class... Args>
    auto enqueue(int priority, F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
    ~TransThreadPool();
private:
    int defaultPriority=0;
    std::vector< std::thread > workers;
    std::priority_queue<TransTask,std::vector<TransTask>,CompLess> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

inline TransThreadPool::TransThreadPool(size_t threads)
    :   stop(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back(
            [this]
            {
                for(;;)
                {
                    TransTask task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.top());
                        this->tasks.pop();
                    }
                    std::unordered_map<TransLocal*,std::any> captured=TransLocal::getMap();
                    TransLocal::setMap(task.taskLocals());
                    (task.task())();
                    TransLocal::setMap(captured);
                }
            }
        );
}

template<class F, class... Args>
auto TransThreadPool::enqueue(int priority,F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    std::unordered_map<TransLocal*,std::any> taskLocals=TransLocal::getMap();
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace(TransTask(priority,[task](){ (*task)(); }));
    }
    condition.notify_one();
    return res;
}

template<class F, class... Args>
auto TransThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    return enqueue(defaultPriority,std::forward<F>(f), std::forward<Args>(args)...);
}

inline TransThreadPool::~TransThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}

#endif
