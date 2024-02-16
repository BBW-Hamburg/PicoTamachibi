#include "filesystem.hpp"
#include "context.hpp"
#include "microtar/src/microtar.h"
#include "incbin.h"

#include <etl/string.h>
#include <etl/vector.h>

INCBIN(fs_blob, SOURCE_DIR "/fs.tar");



Filesystem::Filesystem() {
    const auto err = mtar_open(&tar, reinterpret_cast<const void *>(incbin_fs_blob_start), "rb");
    ASSERT_PANIC("TAR init fail", err == MTAR_ESUCCESS);
}

etl::span<const char> Filesystem::read_file(const char *path) {
    // Find file
    mtar_header_t h;
    auto err = mtar_find(&tar, path, &h);
    ASSERT_PANIC("TAR find fail", err == MTAR_ESUCCESS);

    // Get data
    const void *ptr;
    err = mtar_get_data(&tar, &ptr);
    ASSERT_PANIC("TAR read fail", err == MTAR_ESUCCESS);

    // Create span
    return {reinterpret_cast<const char*>(ptr), h.size};
}

FilesystemIterator Filesystem::begin() {
    FilesystemIterator fres(*this);
    fres.update(); // Valueful iterator
    return fres;
}

FilesystemIterator Filesystem::end() {
    FilesystemIterator fres(*this);
    fres.clear(); // Valueless iterator
    return fres;
}


FilesystemIterator& FilesystemIterator::operator ++() {
    // Panic if valueless
    ASSERT_PANIC("TAR bar iter", *this);

    // Restore our seek location
    mtar_seek(&parent.tar, seek_pos);

    // Go to next file
    mtar_next(&parent.tar);

    // Back up our new seek location
    seek_pos = parent.tar.seek_pos;

    // Update header
    update();

    return *this;
}
