#ifndef ASYNCMAN_HPP
#define ASYNCMAN_HPP
#include <functional>
#include <etl/vector.h>
#include <etl/limits.h>


class AsyncMan {
public:
    using HandleID = uint8_t;
    using HandleCb = std::function<void()>;

    struct Handle {
        HandleID id = 0;
        bool active = true;
        class AsyncObject *object;

        bool is_valid() const {
            return id != 0 && object != nullptr;
        }
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
    UniqueAsyncManHandle(const UniqueAsyncManHandle& o) : man(o.man) {
        id = o.man.new_handle().id;
    }
    UniqueAsyncManHandle(UniqueAsyncManHandle&& o) : man(o.man), id(o.id) {
        o.id = 0;
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


class AsyncObject {
public:
    UniqueAsyncManHandle handle;

    AsyncObject(AsyncMan& man) : handle(man) {
        handle->object = this;
    }

    AsyncObject(const AsyncObject& o) : handle(o.handle) {
        handle->object = this;
    }
    AsyncObject(AsyncObject&&) = delete;

    virtual ~AsyncObject() {}

    virtual void on_tick() = 0;
};

#endif // ASYNCMAN_HPP
