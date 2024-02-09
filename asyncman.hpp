#ifndef ASYNCMAN_HPP
#define ASYNCMAN_HPP
#include <etl/vector.h>
#include <etl/delegate.h>
#include <etl/limits.h>


class AsyncMan {
public:
    using HandleID = uint8_t;
    using HandleCb = etl::delegate<void()>;

    struct Handle {
        HandleID id;
        bool active = true;
        HandleCb on_tick;
    };

private:
    constexpr static HandleID max_handle_id = etl::numeric_limits<HandleID>::max();

    etl::vector<Handle, 16> handles;

    HandleID find_free_id() const;

public:
    AsyncMan() {}
    AsyncMan(const AsyncMan&) = delete;
    AsyncMan(AsyncMan&&) = delete;

    Handle& new_handle() {
        Handle fres{find_free_id()};
        return handles.emplace_back(std::move(fres));
    }
    void delete_handle(HandleID id);

    Handle& get_handle(HandleID id);

    void tick();
};


class UniqueAsyncManHandle {
    AsyncMan& man;

    AsyncMan::HandleID id;

public:
    UniqueAsyncManHandle(AsyncMan& man) : man(man) {
        id = man.new_handle().id;
    }
    ~UniqueAsyncManHandle() {
        man.delete_handle(id);
    }

    AsyncMan::Handle& operator *() {
        return man.get_handle(id);
    }
    AsyncMan::Handle *operator ->() {
        return &man.get_handle(id);
    }

    const AsyncMan::Handle& operator *() const {
        return man.get_handle(id);
    }
    const AsyncMan::Handle *operator ->() const {
        return &man.get_handle(id);
    }
};
#endif // ASYNCMAN_HPP
