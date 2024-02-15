#include "asyncman.hpp"
#include "context.hpp"



AsyncMan::HandleID AsyncMan::find_free_id() const {
    // Make sure handle list isn't going to overflow
    if (handles.full())
        Context::get().panic("Hndl OOB");

    // Try fast primitive max first
    {
        HandleID max_id = 0;
        for (const auto& handle : handles) {
            if (handle.id > max_id)
                max_id = handle.id;
        }
        if (max_id != max_handle_id)
            return max_id + 1;
    }

    // Attempt deeper search
    for (HandleID id = 1; id != max_handle_id; id++) {
        bool found = false;
        for (const auto& handle : handles) {
            if (handle.id == id) {
                found = true;
                break;
            }
        }
        if (!found)
            return id;
    }

    // Game over, no more handles left
    Context::get().panic("OO Hndl IDs");
}

void AsyncMan::delete_handle(HandleID id) {
    for (auto it = handles.begin(); it != handles.end(); it++)
        if (it->id == id) {
            handles.erase(it);
            return;
        }
}

AsyncMan::Handle &AsyncMan::get_handle(HandleID id) {
    for (auto it = handles.begin(); it != handles.end(); it++)
        if (it->id == id)
            return *it;

    Context::get().panic("Bd Hndl");
}

void AsyncMan::tick() {
    for (auto& handle : handles) {
        if (!handle.is_valid())
            continue;
        if (handle.active)
            handle.object->on_tick();
    }
}
