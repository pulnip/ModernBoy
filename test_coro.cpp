#include <coroutine>
#include <iostream>
#include <memory>
#include <chrono>

struct Data {
    void doSomething() {
        std::cout << "DO SOMETHING" << '\n';
    }
};

struct Task {
    struct promise_type {
        std::suspend_always initial_suspend() {
            return {};
        }

        std::suspend_never final_suspend() noexcept {
            return {};
        }

        void unhandled_exception() {}
        std::shared_ptr<Data> result;

        Task get_return_object() {
            return std::coroutine_handle<promise_type>::from_promise(*this);
        }

        void return_void() {}
    };

    std::coroutine_handle<promise_type> handle; // 코루틴 핸들을 보관

    Task(std::coroutine_handle<promise_type> h) : handle(h) {}

    ~Task() {
    }
};


struct Owner {
    std::shared_ptr<Data> data;

    std::shared_ptr<Data> get() {
        return data;
    }

    void set(std::shared_ptr<Data> newData, Task& task) {
        data = newData;
        task.handle.resume();
    }
};

struct Awaiter {
    Owner& owner;

    bool await_ready() {
        return !owner.get(); // Data가 준비되지 않았으면 false 반환
    }

    void await_suspend(std::coroutine_handle<> coro) {
        // Data가 준비되지 않았으면 일시 중단
        if (!owner.get())
            return;

        // Data가 준비되면 코루틴을 다시 재개
        coro.resume();
    }

    std::shared_ptr<Data> await_resume() {
        return owner.get();
    }
};

Awaiter operator co_await(Owner& owner) {
    return {owner};
}


Task func(Owner& owner) {
    co_await owner;
    owner.get()->doSomething();
}

int main() {
    Owner owner;

    // Task 객체를 생성하고 func 코루틴 실행
    Task task = func(owner);

    owner.set(std::make_shared<Data>(), task);

    return 0;
}
