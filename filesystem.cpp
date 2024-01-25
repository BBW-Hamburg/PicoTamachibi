#include "filesystem.hpp"
#include "context.hpp"
#include "microtar/src/microtar.h"
#include "incbin.h"

#include <string>
#include <vector>

INCBIN(fs_blob, SOURCE_DIR "/fs.tar");



Filesystem::Filesystem() {
    const auto err = mtar_open(&tar, reinterpret_cast<const void *>(incbin_fs_blob_start), "rb");
    if (err != MTAR_ESUCCESS)
        Context::get().panic("TAR init fail "+std::to_string(err));
}

std::span<const char> Filesystem::read_file(const char *path) {
    mtar_header_t h;
    mtar_find(&tar, path, &h);
    const void *ptr;
    const auto err = mtar_get_data(&tar, &ptr);
    if (err != MTAR_ESUCCESS)
        Context::get().panic("TAR read fail "+std::to_string(err));
    return {reinterpret_cast<const char*>(ptr), h.size};
}
