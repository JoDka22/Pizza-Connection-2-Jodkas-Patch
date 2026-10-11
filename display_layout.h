#pragma once
#include <windows.h>
namespace pc2fix {
struct DisplayLayout {
    int width, height;
    RECT world, sidebar, status;
};
// Original city viewport is 596x551. UI pixels retain their authored size.
inline bool MakeLayout(int width, int height, DisplayLayout& result)
{
    if (width < 800 || height < 600 || width > 4096 || height > 2160) return false;
    result = {width, height, {0, 0, width - 204, height - 49},
        {width - 204, 0, width, height - 49}, {0, height - 49, width, height}};
    return true;
}
// Only the ORIGINAL sized, top-aligned sidebar content has a known input mapping.
// Extended panel padding must not be mistaken for authored controls.
inline bool SidebarToOriginal(const DisplayLayout& layout, POINT point, POINT& result)
{
    if (point.x < layout.sidebar.left || point.x >= layout.sidebar.right ||
        point.y < 0 || point.y >= 551) return false;
    result = {point.x - layout.sidebar.left + 596, point.y};
    return true;
}
}
