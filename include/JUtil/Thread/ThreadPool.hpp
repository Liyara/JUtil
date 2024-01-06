#ifndef _JUTIL_THREAD_POOL_HPP_
#define _JUTIL_THREAD_POOL_HPP_

#include <JUtil/Thread/Thread.hpp>
#include <JUtil/Thread/Task.hpp>


namespace jutil  {

    template <
        size_t taskCount,
        typename = typename Enable<(taskCount > 0)>::Type
    >
    class __TaskQueue__ : public NonCopyable {
        public:
            __TaskQueue__() : _front(0), _rear(0) {
                _tasks.reserve(taskCount);
                _tasks.resize(taskCount);
                for (Task *task : _tasks) {
                    task = JUTIL_NULLPTR;
                }
            }

            void push(Task* task) {
                _mutex.lock();
                while (_size == taskCount) {
                    _notFull.wait(_mutex);
                }
                _tasks[_rear] = task;
                _rear = (_rear + 1) % taskCount;
                _size++;
                _notEmpty.signal();
                _mutex.unlock();
            }

            Task* pop() {
                _mutex.lock();
                while (_size == 0) {
                    _notEmpty.wait(_mutex);
                }
                Task* task = _tasks[_front];
                _front = (_front + 1) % taskCount;
                _size--;
                _notFull.signal();
                _mutex.unlock();
                return task;
            }


        private:
            Mutex _mutex;
            Condition _notFull, _notEmpty;
            Queue<Task*> _tasks;
            size_t _front, _rear, _size;
    };

    template <
        size_t taskCount,
        typename = typename Enable<(taskCount > 0)>::Type
    >
    class __WorkerThread__ : public Thread, public NonCopyable {
        public:
            __WorkerThread__(__TaskQueue__<taskCount>* tasks) : _tasks(tasks) {}
            __WorkerThread__() : _tasks(JUTIL_NULLPTR) {}

            void setTasks(__TaskQueue__<taskCount>* tasks) {
                _tasks = tasks;
            }

            void run() override {
                while (true) {
                    Task* task = _tasks->pop();
                    if (task) task->run();
                }
            }

        private:
            __TaskQueue__<taskCount>* _tasks;
    };

    template <
        size_t threadCount,
        size_t taskCount,
        typename = typename Enable<(threadCount > 0)>::Type,
        typename = typename Enable<(taskCount > 0)>::Type
    >
    class ThreadPool {
        public:
            ThreadPool() : _running(false) {
                
            }

            virtual ~ThreadPool() {
                for (size_t i = 0; i < _threads.size(); i++) {
                    //if (_threads[i]) delete _threads[i];
                }
            }

            void submit(Task* task) {
                if (!_running) return;
                _tasks.push(task);
            }

            void join() {
                if (!_running) return;
                for (size_t i = 0; i < threadCount; i++) {
                    _threads[i]->join();
                }
            }

            void start() {
                for (size_t i = 0; i < threadCount; i++) {
                    _threads.insert(new WorkerThread(&_tasks));
                    _threads[i]->start();
                }
                _running = true;
            }

            void stop() {
                if (!_running) return;
                for (size_t i = 0; i < threadCount; i++) {
                    _threads[i]->stop();
                }
                _running = false;
            }

        private:
            typedef __WorkerThread__<taskCount> WorkerThread;
            Queue<WorkerThread*> _threads;
            __TaskQueue__<taskCount> _tasks;
            bool _running;
    };
}


#endif // _JUTIL_THREAD_POOL_HPP_