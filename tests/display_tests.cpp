#define WIN32_LEAN_AND_MEAN
#include "../display_layout.h"
#include <cstdio>
#include <windows.h>
int main()
{
    unsigned failures = 0;
    auto check = [&](bool ok, const char *name) {
        printf("%s: %s\n", ok ? "PASS" : "FAIL", name);
        if (!ok)
            ++failures;
    };
    pc2fix::DisplayLayout layout{};
    check(pc2fix::MakeLayout(800, 600, layout) && layout.world.right == 596 && layout.world.bottom == 551,
          "original viewport recovered");
    check(pc2fix::MakeLayout(1920, 1080, layout) && layout.world.right == 1716 && layout.world.bottom == 1031,
          "1080p proposed world area");
    check(layout.sidebar.right - layout.sidebar.left == 204 && layout.status.bottom - layout.status.top == 49,
          "UI dimensions independent of aspect ratio");
    POINT mapped{};
    check(pc2fix::SidebarToOriginal(layout, {1716, 0}, mapped) && mapped.x == 596 && mapped.y == 0,
          "right-anchored UI origin mapping");
    check(pc2fix::SidebarToOriginal(layout, {1919, 550}, mapped) && mapped.x == 799 && mapped.y == 550,
          "last authored UI pixel mapping");
    check(!pc2fix::SidebarToOriginal(layout, {1715, 100}, mapped), "world input does not map into UI");
    check(!pc2fix::SidebarToOriginal(layout, {1920, 100}, mapped), "exclusive right edge rejected");
    check(!pc2fix::SidebarToOriginal(layout, {1800, 551}, mapped),
          "extended padding does not activate controls");
    check(!pc2fix::SidebarToOriginal(layout, {1800, -1}, mapped), "negative coordinates rejected");
    check(pc2fix::MakeLayout(2560, 1440, layout) && layout.world.right == 2356 && layout.world.bottom == 1391,
          "1440p proposed world area (geometry only)");
    check(!pc2fix::MakeLayout(799, 600, layout) && !pc2fix::MakeLayout(800, 599, layout),
          "undersized mode rejected");
    check(!pc2fix::MakeLayout(4097, 1080, layout) && !pc2fix::MakeLayout(1920, 2161, layout),
          "oversized mode rejected");
    check(layout.width == 2560 && layout.height == 1440, "invalid input leaves prior layout unchanged");
    pc2fix::MakeLayout(1920, 1080, layout);
    auto worldOffset = pc2fix::WorldControlOffset(layout, 42, 7, {0, 0, 1716, 1031});
    check(worldOffset.x == 0 && worldOffset.y == 0, "world canvas keeps native input coordinates");
    auto titleOffset = pc2fix::WorldControlOffset(layout, 8002, 6, {535, 0, 597, 251});
    check(titleOffset.x == 1120 && titleOffset.y == 0, "overhanging sidebar decoration follows right anchor");
    auto statusOffset = pc2fix::WorldControlOffset(layout, 1040, 3, {10, 568, 60, 589});
    check(statusOffset.x == 0 && statusOffset.y == 480, "status content follows bottom anchor");
    auto noteOffset = pc2fix::WorldControlOffset(layout, 8015, 3, {1, 521, 596, 546});
    check(noteOffset.x == 0 && noteOffset.y == 480, "notification remains directly above status bar");
    RECT button{600, 178, 639, 217};
    POINT origin{599, 177};
    auto buttonOffset = pc2fix::WorldControlOffset(layout, 1018, 1, button);
    OffsetRect(&button, buttonOffset.x, buttonOffset.y);
    origin.x += buttonOffset.x;
    origin.y += buttonOffset.y;
    check(button.left == 1720 && button.right == 1759 && origin.x == 1719 && button.left - origin.x == 1,
          "draw origin and hit mask retain authored one-pixel inset");
    check(pc2fix::ClassifySidebarWindow({561, 218, 800, 600}) == pc2fix::SidebarWindow::Panel,
          "detail panel identified including overhanging tabs");
    check(pc2fix::ClassifySidebarWindow({567, 248, 606, 551}) == pc2fix::SidebarWindow::Tabs,
          "tab strip identified");
    check(pc2fix::ClassifySidebarWindow({635, 33, 763, 161}) == pc2fix::SidebarWindow::Minimap,
          "minimap window identified");
    check(pc2fix::ClassifySidebarWindow({0, 0, 800, 600}) == pc2fix::SidebarWindow::None &&
              pc2fix::ClassifySidebarWindow({600, 200, 800, 600}) == pc2fix::SidebarWindow::None,
          "unrecognised menu windows are not guessed into sidebar layout");
    check(pc2fix::ClassifySidebarWindow({1681, 218, 1920, 1080}) == pc2fix::SidebarWindow::None,
          "transformed panel is not shifted twice");
    return failures ? 1 : 0;
}
