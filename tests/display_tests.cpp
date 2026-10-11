#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include "../display_layout.h"
int main()
{
    unsigned failures = 0;
    auto check = [&](bool ok, const char* name) {
        printf("%s: %s\n", ok ? "PASS" : "FAIL", name); if (!ok) ++failures;
    };
    pc2fix::DisplayLayout layout{};
    check(pc2fix::MakeLayout(800,600,layout) && layout.world.right == 596 &&
        layout.world.bottom == 551, "original viewport recovered");
    check(pc2fix::MakeLayout(1920,1080,layout) && layout.world.right == 1716 &&
        layout.world.bottom == 1031, "1080p proposed world area");
    check(layout.sidebar.right-layout.sidebar.left == 204 &&
        layout.status.bottom-layout.status.top == 49, "UI dimensions independent of aspect ratio");
    POINT mapped{};
    check(pc2fix::SidebarToOriginal(layout,{1716,0},mapped) && mapped.x == 596 && mapped.y == 0,
        "right-anchored UI origin mapping");
    check(pc2fix::SidebarToOriginal(layout,{1919,550},mapped) && mapped.x == 799 && mapped.y == 550,
        "last authored UI pixel mapping");
    check(!pc2fix::SidebarToOriginal(layout,{1715,100},mapped), "world input does not map into UI");
    check(!pc2fix::SidebarToOriginal(layout,{1920,100},mapped), "exclusive right edge rejected");
    check(!pc2fix::SidebarToOriginal(layout,{1800,551},mapped), "extended padding does not activate controls");
    check(!pc2fix::SidebarToOriginal(layout,{1800,-1},mapped), "negative coordinates rejected");
    check(pc2fix::MakeLayout(2560,1440,layout) && layout.world.right == 2356 &&
        layout.world.bottom == 1391, "1440p proposed world area (geometry only)");
    check(!pc2fix::MakeLayout(799,600,layout) && !pc2fix::MakeLayout(800,599,layout), "undersized mode rejected");
    check(!pc2fix::MakeLayout(4097,1080,layout) && !pc2fix::MakeLayout(1920,2161,layout), "oversized mode rejected");
    check(layout.width == 2560 && layout.height == 1440, "invalid input leaves prior layout unchanged");
    return failures ? 1 : 0;
}
