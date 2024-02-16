#include "SingleEvent.hpp"

#include <utility>

namespace basiccoro
{

detail::SingleEventBase::SingleEventBase(detail::SingleEventBase&& other)
    : waiting_(std::move(other.waiting_))
    , isSet_(std::exchange(other.isSet_, false))
{
}

detail::SingleEventBase& detail::SingleEventBase::operator=(detail::SingleEventBase&& other)
{
    waiting_ = std::move(other.waiting_);
    isSet_ = std::exchange(other.isSet_, false);
    return *this;
}

detail::SingleEventBase::~SingleEventBase()
{
    for (auto handle : waiting_)
    {
        handle.destroy();
    }
}

void detail::SingleEventBase::set_common()
{
    if (!isSet_)
    {
        if (waiting_.empty())
        {
            isSet_ = true;
        }
        else
        {
            // resuming coroutines can result in
            // consecutive co_awaits on this object
            auto temp = std::move(waiting_);

            for (auto handle : temp)
            {
                handle.resume();
                if (handle.done())
                {
                    handle.destroy();
                }
            }
        }
    }
}

SingleEvent<void>::awaiter SingleEvent<void>::operator co_await()
{
    return awaiter{*this};
}

}  // namespace basiccoro
