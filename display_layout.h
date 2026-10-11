#pragma once
#include <windows.h>
namespace pc2fix
{
struct DisplayLayout
{
    int width, height;
    RECT world, sidebar, status;
};
// Original city viewport is 596x551. UI pixels retain their authored size.
inline bool MakeLayout(int width, int height, DisplayLayout &result)
{
    if (width < 800 || height < 600 || width > 4096 || height > 2160)
        return false;
    result = {width,
              height,
              {0, 0, width - 204, height - 49},
              {width - 204, 0, width, height - 49},
              {0, height - 49, width, height}};
    return true;
}
// Only the ORIGINAL sized, top-aligned sidebar content has a known input mapping.
// Extended panel padding must not be mistaken for authored controls.
inline bool SidebarToOriginal(const DisplayLayout &layout, POINT point, POINT &result)
{
    if (point.x < layout.sidebar.left || point.x >= layout.sidebar.right || point.y < 0 || point.y >= 551)
        return false;
    result = {point.x - layout.sidebar.left + 596, point.y};
    return true;
}
// UI coordinates and hit rectangles move together. World pixels retain identity.
inline POINT WorldControlOffset(const DisplayLayout &layout, DWORD id, DWORD type, const RECT &bounds)
{
    if (id == 42 && type == 7)
        return {0, 0};
    return {bounds.left >= 535 ? layout.width - 800 : 0,
            bounds.top >= 551 || id == 8014 || id == 8015 ? layout.height - 600 : 0};
}
enum class SidebarWindow
{
    None,
    Panel,
    Tabs,
    Minimap
};
inline SidebarWindow ClassifySidebarWindow(const RECT &bounds)
{
    if (bounds.left == 561 && bounds.top == 218 && bounds.right == 800 && bounds.bottom == 600)
        return SidebarWindow::Panel;
    if (bounds.left == 567 && bounds.top == 248 && bounds.right == 606 && bounds.bottom == 551)
        return SidebarWindow::Tabs;
    if (bounds.left == 635 && bounds.top == 33 && bounds.right == 763 && bounds.bottom == 161)
        return SidebarWindow::Minimap;
    return SidebarWindow::None;
}
} // namespace pc2fix
