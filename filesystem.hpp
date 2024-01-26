#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP
#include "microtar/src/microtar.h"

#include <span>
#include <compare>


class Filesystem {
    friend class FilesystemIterator;

    struct mtar_t tar;

public:
    Filesystem();

    std::span<const char> read_file(const char *path);

    class FilesystemIterator begin();
    class FilesystemIterator end();
};

class FilesystemIterator {
    friend class Filesystem;

    Filesystem& parent;

    unsigned seek_pos;
    mtar_header_t current;

    void clear() {
        current.name = nullptr;
    }
    void update() {
        if (mtar_get_header(&parent.tar, &current) != MTAR_ESUCCESS)
            clear();
    }

    FilesystemIterator(Filesystem& fs)
          : parent(fs) {
        // Rewind to the beginning
        mtar_rewind(&parent.tar);
        seek_pos = parent.tar.seek_pos;
    }

public:
    FilesystemIterator(const FilesystemIterator&) = delete;
    FilesystemIterator(FilesystemIterator&& o)
          : parent(o.parent), current(o.current), seek_pos(o.seek_pos) {}

    const mtar_header_t& operator *() const {
        return current;
    }
    const mtar_header_t *operator ->() const {
        return &current;
    }

    operator bool() const {
        return current.name != nullptr;
    }

    bool operator ==(const FilesystemIterator& o) const {
        return current.name == o->name;
    }
    FilesystemIterator& operator ++();
};
#endif // FILESYSTEM_HPP
