#pragma once
#include <coroutine>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace basiccoro
{
namespace detail
{

template<class Event>
class AwaiterBase
{
public:
    AwaiterBase(Event& event)
        : event_(event)
    {}

    bool await_ready()
    {
        if (event_.isSet())
        {
            // unset already set event, then continue coroutine
            event_.isSet_ = false;
            return true;
        }

        return false;
    }

    void await_suspend(std::coroutine_handle<> handle)
    {
        event_.waiting_.push_back(handle);
    }

    typename Event::value_type await_resume()
    {
        if constexpr (!std::is_same_v<typename Event::value_type, void>)
        {
            return *event_.result;
        }
    }

private:
    Event& event_;
};

class SingleEventBase
{
public:
    SingleEventBase() = default;
    SingleEventBase(const SingleEventBase&) = delete;
    SingleEventBase(SingleEventBase&&);
    SingleEventBase& operator=(const SingleEventBase&) = delete;
    SingleEventBase& operator=(SingleEventBase&&);
    ~SingleEventBase();

    bool isSet() const {
        return isSet_;
    }
    void unset() {
        isSet_ = false;
    }

protected:
    void set_common();

private:
    template<class T>
    friend class AwaiterBase;
    std::vector<std::coroutine_handle<>> waiting_;
    bool isSet_ = false;
};

}  // namespace detail

template<class T>
class SingleEvent : public detail::SingleEventBase
{
public:
    using value_type = T;
    using awaiter = detail::AwaiterBase<SingleEvent<T>>;

    void set(T t) { result = std::move(t); set_common(); }
    awaiter operator co_await() { return awaiter{*this}; }

private:
    friend awaiter;
    std::optional<T> result;
};

template<>
class SingleEvent<void> : public detail::SingleEventBase
{
public:
    using value_type = void;
    using awaiter = detail::AwaiterBase<SingleEvent<void>>;

    void set() {
        set_common();
    }
    awaiter operator co_await();
};

}  // namespace basiccoro
