#include "filesystem.hpp"
#include "context.hpp"
#include "microtar/src/microtar.h"
#include "incbin.h"

#include <etl/string.h>
#include <etl/vector.h>

INCBIN(fs_blob, SOURCE_DIR "/fs.tar");



Filesystem::Filesystem() {
    const auto err = mtar_open(&tar, reinterpret_cast<const void *>(incbin_fs_blob_start), "rb");
    if (err != MTAR_ESUCCESS)
        Context::get().panic("TAR init fail ");
}

etl::span<const char> Filesystem::read_file(const char *path) {
    // Find file
    mtar_header_t h;
    auto err = mtar_find(&tar, path, &h);
    if (err != MTAR_ESUCCESS)
        Context::get().panic("TAR find fail ");

    // Get data
    const void *ptr;
    err = mtar_get_data(&tar, &ptr);
    if (err != MTAR_ESUCCESS)
        Context::get().panic("TAR read fail ");

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
    if (!*this)
        Context::get().panic("TAR bad iter");

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
