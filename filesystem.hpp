#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP
#include "microtar/src/microtar.h"

#include <span>


class Filesystem {
    struct mtar_t tar;

public:
    Filesystem();

    std::span<const char> read_file(const char *path);
};
#endif // FILESYSTEM_HPP
