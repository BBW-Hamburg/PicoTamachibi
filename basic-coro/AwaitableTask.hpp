#pragma once
#include <concepts>
#include <coroutine>
#include <exception>
#include <stdexcept>
#include <utility>

namespace basiccoro
{
namespace detail
{

template<class Derived>
struct PromiseBase
{
    auto get_return_object() { return std::coroutine_handle<Derived>::from_promise(static_cast<Derived&>(*this)); }
    void unhandled_exception() { std::terminate(); }
};

template<class Derived, class T> requires std::movable<T> || std::same_as<T, void>
struct ValuePromise : public PromiseBase<Derived>
{
    using value_type = T;
    T val;
    void return_value(T t) { val = std::move(t); }
};

template<class Derived>
struct ValuePromise<Derived, void> : public PromiseBase<Derived>
{
    using value_type = void;
    void return_void() {}
};

template<class T>
class AwaitablePromise : public ValuePromise<AwaitablePromise<T>, T>
{
public:
    auto initial_suspend() { return std::suspend_never(); }

    auto final_suspend() noexcept
    {
        if (waiting_)
        {
            waiting_.resume();
            if (waiting_.done())
            {
                waiting_.destroy();
            }
            waiting_ = nullptr;
        }

        return std::suspend_always();
    }

    void storeWaiting(std::coroutine_handle<> handle)
    {
        waiting_ = handle;
    }

    ~AwaitablePromise()
    {
        if (waiting_)
        {
            waiting_.destroy();
        }
    }

private:
    std::coroutine_handle<> waiting_ = nullptr;
};

template<class Promise>
class TaskBase
{
public:
    using promise_type = Promise;

    TaskBase();
    TaskBase(std::coroutine_handle<promise_type> handle);
    TaskBase(const TaskBase&) = delete;
    TaskBase(TaskBase&&);
    TaskBase& operator=(const TaskBase&) = delete;
    TaskBase& operator=(TaskBase&&);
    ~TaskBase();

    bool done() const { return handle_.done(); }

protected:
    std::coroutine_handle<promise_type> handle_;
    bool handleShouldBeDestroyed_;
};

template<class Promise>
TaskBase<Promise>::TaskBase()
    : handle_(nullptr), handleShouldBeDestroyed_(false)
{}

template<class Promise>
TaskBase<Promise>::TaskBase(std::coroutine_handle<promise_type> handle)
    : handle_(handle)
{
    // TODO: this whole system needs revamping with something like UniqueCoroutineHandle
    // and custom static interface to awaiter types - so await_suspend method would take in UniqueCoroutineHandle

    if (handle.done())
    {
        // it is resonable to expect that if the coroutine is done before
        // the task creation, then the original stack is continued without suspending,
        // and coroutine needs to be destroyed with TaskBase object
        handleShouldBeDestroyed_ = true;
    }
    else
    {
        // otherwise the coroutine should be managed by object that it is awaiting
        handleShouldBeDestroyed_ = false;
    }
}

template<class Promise>
TaskBase<Promise>::TaskBase(TaskBase&& other)
    : handle_(other.handle_), handleShouldBeDestroyed_(std::exchange(other.handleShouldBeDestroyed_, false))
{
}

template<class Promise>
TaskBase<Promise>& TaskBase<Promise>::operator=(TaskBase&& other)
{
    handle_ = other.handle_;
    handleShouldBeDestroyed_ = std::exchange(other.handleShouldBeDestroyed_, false);
    return *this;
}

template<class Promise>
TaskBase<Promise>::~TaskBase()
{
    if (handleShouldBeDestroyed_)
    {
        handle_.destroy();
    }
}

}  // namespace detail

template<class T>
class AwaitableTask : public detail::TaskBase<detail::AwaitablePromise<T>>
{
    using Base = detail::TaskBase<detail::AwaitablePromise<T>>;

public:
   using Base::Base;

    class awaiter;
    friend class awaiter;
    awaiter operator co_await() const;
};

template<class T>
struct AwaitableTask<T>::awaiter
{
    bool await_ready()
    {
        return task_.done();
    }

    template<class Promise>
    void await_suspend(std::coroutine_handle<Promise> handle)
    {
        task_.handle_.promise().storeWaiting(handle);
    }

    T await_resume()
    {
        if constexpr (!std::is_same_v<void, T>)
        {
            return std::move(task_.handle_.promise().val);
        }
    }

    const AwaitableTask& task_;
};

template<class T>
typename AwaitableTask<T>::awaiter AwaitableTask<T>::operator co_await() const
{
    return awaiter{*this};
}

}  // namespace basiccoro
