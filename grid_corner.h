// MIT license; see LICENSE. GOG world nodes are contiguous 12-byte records.
#pragma once
#include <cstdint>
namespace pc2fix
{
inline bool GridCellCoordinates(uintptr_t base, uintptr_t cell, int columns, int rows, int &x, int &y)
{
    if (!base || cell < base || columns <= 0 || rows <= 0)
        return false;
    const uint64_t count = static_cast<uint64_t>(columns) * rows;
    const uintptr_t offset = cell - base;
    if (count > 1048576 || offset % 12 || offset / 12 >= count)
        return false;
    x = static_cast<int>((offset / 12) % columns);
    y = static_cast<int>((offset / 12) / columns);
    return true;
}
} // namespace pc2fix
