// MIT license; see LICENSE. Addresses are for the verified German GOG image only.
#define WIN32_LEAN_AND_MEAN
#include "widescreen.h"
#include "camera_bounds.h"
#include "main.h"
#include "presentation.h"
#include <MinHook.h>
#include <cstdio>
#include <windows.h>
namespace
{
pc2fix::DisplayLayout display{};
bool active = false;
using RootFn = void *(__thiscall *)(void *, void *, HWND, void *, int, int, int);
using DrawFn = void(__thiscall *)(void *, void *);
using SpriteFn = void(__thiscall *)(void *, void *, int, int);
using BuffersFn = void(__thiscall *)(void *, void *, int, int, void *, int, int);
RootFn rootOriginal = nullptr;
DrawFn drawOriginal = nullptr;
DrawFn frameOriginal = nullptr;
bool cityFrame = false;
SpriteFn spriteOriginal = nullptr;
BuffersFn buffersOriginal = nullptr;
using CameraFn = int(__thiscall *)(void *, int, int);
CameraFn cameraOriginal = nullptr;
using AxisFn = int(__thiscall *)(void *, int);
AxisFn cameraXOriginal = nullptr;
AxisFn cameraYOriginal = nullptr;
thread_local void *background = nullptr;
thread_local bool worldBackground = false;
thread_local bool panelFooter = false;
thread_local RECT panelBounds{};
template <class T> T &Field(void *object, size_t offset)
{
    return *reinterpret_cast<T *>(static_cast<BYTE *>(object) + offset);
}
bool Writable(void *address, size_t size)
{
    MEMORY_BASIC_INFORMATION region{};
    if (!address || !VirtualQuery(address, &region, sizeof(region)) || region.State != MEM_COMMIT ||
        (region.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
        return false;
    auto protection = region.Protect & 0xff;
    if (protection != PAGE_READWRITE && protection != PAGE_WRITECOPY)
        return false;
    auto start = reinterpret_cast<ULONG_PTR>(address);
    auto end = reinterpret_cast<ULONG_PTR>(region.BaseAddress) + region.RegionSize;
    return start <= end && size <= end - start;
}
int Zoom()
{
    auto scene = *reinterpret_cast<void **>(0x57c670);
    return Writable(scene, 0x2da4) ? Field<int>(scene, 0x2da0) : -1;
}
int PhysicalUpper(void *object, bool vertical, int originalUpper)
{
    // 0x4ed0a0 stores the artwork edge before subtracting the viewport, then
    // collapses max<min to min. Recover that lost interval only for backgrounds.
    if (!Field<void *>(object, 0x70) && !Field<void *>(object, 0x74))
        return originalUpper;
    bool base = Field<int>(object, 0xc) == 16;
    auto dimension =
        *reinterpret_cast<int *>(base ? (vertical ? 0x57195c : 0x571958) : (vertical ? 0x571954 : 0x571950));
    int edge = Field<int>(object, vertical ? 0x10164 : 0x10160);
    int margin = Field<int>(object, vertical ? 0x10170 : 0x10168);
    if (dimension <= 0 || edge <= 0)
        return originalUpper;
    int upper = edge - dimension + (vertical ? margin : -margin);
    if (vertical && Zoom() == 2)
        upper -= 56;
    return min(originalUpper, upper);
}
int __fastcall CameraX(void *object, void *, int x)
{
    if (active && display.width == 1920 && display.height == 1080 && Writable(object, 0x10178))
    {
        int lower = Field<int>(object, 0x2c) + Field<int>(object, 0x1016c);
        int upper = Field<int>(object, 0x34) - Field<int>(object, 0x10168);
        x = pc2fix::LimitOversizedCameraAxis(x, lower, upper, PhysicalUpper(object, false, upper));
    }
    return cameraXOriginal(object, x);
}
int __fastcall CameraY(void *object, void *, int y)
{
    if (active && display.width == 1920 && display.height == 1080 && Writable(object, 0x10178))
    {
        int lower = Field<int>(object, 0x30) - Field<int>(object, 0x10174);
        int upper = Field<int>(object, 0x38) + Field<int>(object, 0x10170) - (Zoom() == 2 ? 56 : 0);
        y = pc2fix::LimitOversizedCameraAxis(y, lower, upper, PhysicalUpper(object, true, upper));
    }
    return cameraYOriginal(object, y);
}
int __fastcall Camera(void *object, void *, int x, int y)
{
    auto result = cameraOriginal(object, x, y);
    if (active && Writable(object, 0x10178))
    {
        // Dimensions were already subtracted by 0x4ed0a0. Never subtract them twice.
        int zoom = Zoom();
        int left = Field<int>(object, 0x2c) + Field<int>(object, 0x1016c);
        int right = PhysicalUpper(object, false, Field<int>(object, 0x34) - Field<int>(object, 0x10168));
        int top = Field<int>(object, 0x30) - Field<int>(object, 0x10174);
        int bottom = PhysicalUpper(
            object, true, Field<int>(object, 0x38) + Field<int>(object, 0x10170) - (zoom == 2 ? 56 : 0));
        static int lastZoom = -2;
        static unsigned lines = 0;
        if (lines < 48 && (result || zoom != lastZoom))
        {
            char line[256]{};
            sprintf_s(line,
                      "wide: camera zoom=%d request=%d,%d actual=%d,%d limits=%d,%d..%d,%d viewport=%lu,%lu",
                      zoom, x, y, Field<int>(object, 0xe8), Field<int>(object, 0xec), left, top, right,
                      bottom, *reinterpret_cast<DWORD *>(0x571950), *reinterpret_cast<DWORD *>(0x571954));
            Log(line);
            sprintf_s(line, "wide: camera raw=%d,%d..%d,%d margins=%d,%d,%d,%d artEdge=%d,%d",
                      Field<int>(object, 0x2c), Field<int>(object, 0x30), Field<int>(object, 0x34),
                      Field<int>(object, 0x38), Field<int>(object, 0x10168), Field<int>(object, 0x1016c),
                      Field<int>(object, 0x10170), Field<int>(object, 0x10174), Field<int>(object, 0x10160),
                      Field<int>(object, 0x10164));
            Log(line);
            ++lines;
            lastZoom = zoom;
        }
    }
    return result;
}
bool HasWorldCanvas(void *window)
{
    if (!Writable(window, 0xf4))
        return false;
    auto entries = Field<BYTE *>(window, 0xec);
    auto count = Field<DWORD>(window, 0xf0);
    if (count > 4096 || (count && !Writable(entries, static_cast<size_t>(count) * 8)))
        return false;
    for (DWORD i = 0; i < count; ++i)
    {
        auto child = *reinterpret_cast<void **>(entries + i * 8);
        if (Writable(child, 0x44) && Field<DWORD>(child, 4) == 42 && Field<DWORD>(child, 0x40) == 7)
            return true;
    }
    return false;
}
void __fastcall Frame(void *object, void *, void *surface)
{
    bool city = false;
    // 0x523fe0 iterates the inline window pointer array at +0x14,
    // count +0x23c, and checks visibility at window +0x4c.
    if (Writable(object, 0x83e8))
    {
        auto count = Field<DWORD>(object, 0x23c);
        if (count <= (0x234 - 0x14) / 4)
            for (DWORD i = 0; i < count; ++i)
            {
                auto window = Field<void *>(object, 0x14 + i * 4);
                if (Writable(window, 0x50) && Field<DWORD>(window, 0x4c) && HasWorldCanvas(window))
                    city = true;
            }
        if (city != cityFrame)
            Field<DWORD>(object, 0x238) = 1; // Clear stale pixels on scene transitions.
    }
    cityFrame = city;
    frameOriginal(object, surface);
}
void __fastcall Sprite(void *object, void *, void *surface, int x, int y)
{
    if (!active || object != background || !background || !Writable(surface, 0x74))
    {
        spriteOriginal(object, surface, x, y);
        return;
    }
    RECT clip{Field<LONG>(surface, 4), Field<LONG>(surface, 0xc), Field<LONG>(surface, 8),
              Field<LONG>(surface, 0x10)};
    auto part = [&](RECT area, int px, int py) {
        RECT intersect{};
        if (!IntersectRect(&intersect, &clip, &area))
            return;
        Field<LONG>(surface, 4) = intersect.left;
        Field<LONG>(surface, 8) = intersect.right;
        Field<LONG>(surface, 0xc) = intersect.top;
        Field<LONG>(surface, 0x10) = intersect.bottom;
        spriteOriginal(object, surface, px, py);
    };
    int dx = display.width - 800, dy = display.height - 600;
    if (worldBackground)
    {
        part({display.world.right, 0, display.width, 551}, x + dx, y);
        for (int top = 551; top < display.world.bottom; top += 200)
            part({display.world.right, top, display.width, min(top + 200, display.world.bottom)}, x + dx,
                 y + top - 300);
        part({0, display.world.bottom, 596, display.height}, x, y + dy);
        for (int left = 596; left < display.width; left += 200)
            part({left, display.world.bottom, min(left + 200, display.width), display.height}, x + left - 300,
                 y + dy);
    }
    else
    {
        int contentBottom = panelFooter ? 551 : 600;
        part({panelBounds.left, panelBounds.top, panelBounds.right, contentBottom}, x, y);
        for (int top = contentBottom; top < display.world.bottom; top += 100)
            part({panelBounds.left, top, panelBounds.right, min(top + 100, display.world.bottom)}, x,
                 y + top - 450);
        part({panelBounds.left, display.world.bottom, panelBounds.right, display.height}, x, y + dy);
    }
    Field<LONG>(surface, 4) = clip.left;
    Field<LONG>(surface, 8) = clip.right;
    Field<LONG>(surface, 0xc) = clip.top;
    Field<LONG>(surface, 0x10) = clip.bottom;
}
void *__fastcall Root(void *object, void *, void *draw, HWND window, void *pak, int width, int height,
                      int flags)
{
    if (active && width == 800 && height == 600)
    {
        width = display.width;
        height = display.height;
    }
    auto result = rootOriginal(object, draw, window, pak, width, height, flags);
    char line[160]{};
    sprintf_s(line, "wide: root %p size=%dx%d", object, width, height);
    Log(line);
    return result;
}
void __fastcall Buffers(void *object, void *, void *draw, int width, int height, void *canvas, int a5, int a6)
{
    // Keep original allocation, pitch and ownership. Camera bases feed this call
    // through 0x4859c0, including the game's existing zoom factors.
    buffersOriginal(object, draw, width, height, canvas, a5, a6);
    char line[160]{};
    sprintf_s(line, "wide: world allocator completed %dx%d (viewport %ldx%ld)", width, height,
              display.world.right, display.world.bottom);
    Log(line);
}
void __fastcall DrawWindow(void *object, void *, void *surface)
{
    if (!active || !Writable(object, 0x28c))
    {
        drawOriginal(object, surface);
        return;
    }
    auto entriesNow = Field<BYTE *>(object, 0xec);
    auto count = Field<DWORD>(object, 0xf0);
    if (count > 4096 || (count && !Writable(entriesNow, static_cast<size_t>(count) * 8)))
    {
        drawOriginal(object, surface);
        return;
    }
    bool world = false, footer = false;
    for (DWORD i = 0; i < Field<DWORD>(object, 0xf0); ++i)
    {
        auto child = *reinterpret_cast<void **>(entriesNow + i * 8);
        if (!child)
            continue;
        if (!Writable(child, 0x44))
        {
            drawOriginal(object, surface);
            return;
        }
        if (child && Field<DWORD>(child, 4) == 42 && Field<DWORD>(child, 0x40) == 7)
            world = true;
        if (Field<DWORD>(child, 4) == 1071 && Field<DWORD>(child, 0x40) == 5)
            footer = true;
    }
    auto &bounds = Field<RECT>(object, 8);
    bool changed = false;
    if (world && bounds.left == 0 && bounds.top == 0 && bounds.right == 800 && bounds.bottom == 600)
    {
        bounds.right = display.width;
        bounds.bottom = display.height;
        Field<RECT>(object, 0x18) = bounds;
        for (DWORD i = 0; i < Field<DWORD>(object, 0xf0); ++i)
        {
            auto child = *reinterpret_cast<void **>(entriesNow + i * 8);
            if (!child)
                continue;
            auto &cr = Field<RECT>(child, 0x28);
            auto offset =
                pc2fix::WorldControlOffset(display, Field<DWORD>(child, 4), Field<DWORD>(child, 0x40), cr);
            OffsetRect(&cr, offset.x, offset.y);
            Field<LONG>(child, 0x38) += offset.x;
            Field<LONG>(child, 0x3c) += offset.y;
        }
        Field<DWORD>(object, 0xe0) = 1;
        changed = true;
    }
    else
    {
        auto authored = bounds;
        bool moved = false;
        if (pc2fix::ClassifySidebarWindow(authored) == pc2fix::SidebarWindow::None)
        {
            OffsetRect(&authored, 800 - display.width, 0);
            if (authored.bottom == display.height)
                authored.bottom = 600;
            moved = pc2fix::ClassifySidebarWindow(authored) != pc2fix::SidebarWindow::None;
        }
        auto kind = pc2fix::ClassifySidebarWindow(authored);
        if (kind != pc2fix::SidebarWindow::None && moved != cityFrame)
        {
            int dx = cityFrame ? display.width - 800 : 800 - display.width;
            int dy = cityFrame ? display.height - 600 : 600 - display.height;
            OffsetRect(&bounds, dx, 0);
            OffsetRect(&Field<RECT>(object, 0x18), dx, 0);
            if (kind == pc2fix::SidebarWindow::Panel)
            {
                bounds.bottom = cityFrame ? display.height : 600;
                Field<RECT>(object, 0x18).bottom = bounds.bottom;
                for (DWORD i = 0; i < Field<DWORD>(object, 0xf0); ++i)
                {
                    auto child = *reinterpret_cast<void **>(entriesNow + i * 8);
                    if (!child)
                        continue;
                    auto &cr = Field<RECT>(child, 0x28);
                    // Keep building filter rows together; only the city-info toolbar
                    // is bottom anchored. Other panels keep their authored content.
                    if (footer && cr.top + bounds.top >= (cityFrame ? 551 : display.world.bottom))
                    {
                        OffsetRect(&cr, 0, dy);
                        Field<LONG>(child, 0x3c) += dy;
                    }
                }
            }
            Field<DWORD>(object, 0xe0) = 1;
            changed = true;
        }
    }
    if (changed)
    {
        auto r = Field<RECT>(object, 8);
        char line[180]{};
        sprintf_s(line, "wide: %s container %ld,%ld..%ld,%ld, %lu controls", world ? "world" : "sidebar",
                  r.left, r.top, r.right, r.bottom, count);
        Log(line);
    }
    auto previous = background;
    auto previousWorld = worldBackground;
    auto previousPanel = panelBounds;
    auto previousFooter = panelFooter;
    background =
        (world || (bounds.right == display.width && bounds.bottom == display.height && bounds.left > 0))
            ? Field<void *>(object, 0x94)
            : nullptr;
    worldBackground = world;
    panelBounds = bounds;
    panelFooter = footer;
    drawOriginal(object, surface);
    background = previous;
    worldBackground = previousWorld;
    panelBounds = previousPanel;
    panelFooter = previousFooter;
}
} // namespace
bool InstallWidescreen(const pc2fix::DisplayLayout &layout)
{
    auto camera = reinterpret_cast<DWORD *>(0x571950);
    if (!IsSupportedGame() || !Writable(camera, 16) || camera[0] != 596 || camera[1] != 551 ||
        camera[2] != 596 || camera[3] != 551)
    {
        Log("wide: image/camera preconditions rejected");
        return false;
    }
    display = layout;
    if (layout.width == 800 && layout.height == 600)
        return true;
    if (MH_Initialize() != MH_OK)
    {
        Log("wide: MinHook init failed");
        return false;
    }
    if (MH_CreateHook(reinterpret_cast<void *>(0x523100), &Root, reinterpret_cast<void **>(&rootOriginal)) !=
            MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x52f2e0), &DrawWindow,
                      reinterpret_cast<void **>(&drawOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x523fe0), &Frame,
                      reinterpret_cast<void **>(&frameOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x53d570), &Sprite,
                      reinterpret_cast<void **>(&spriteOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x4ea230), &Buffers,
                      reinterpret_cast<void **>(&buffersOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x4ed440), &Camera,
                      reinterpret_cast<void **>(&cameraOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x4ed360), &CameraX,
                      reinterpret_cast<void **>(&cameraXOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x4ed3c0), &CameraY,
                      reinterpret_cast<void **>(&cameraYOriginal)) != MH_OK ||
        !InstallPresentationHooks() || MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        Log("wide: hook setup failed");
        return false;
    }
    camera[0] = camera[2] = layout.world.right;
    camera[1] = camera[3] = layout.world.bottom;
    active = true;
    Log("wide: root/camera/container/background/allocator hooks active");
    return true;
}
bool IsCityFrame()
{
    return cityFrame;
}
void StopWidescreen()
{
    // Initial DirectDraw failure only, before root/world construction. Never
    // resize a running game's already allocated world buffers through this path.
    if (!active)
        return;
    active = false;
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    auto camera = reinterpret_cast<DWORD *>(0x571950);
    camera[0] = camera[2] = 596;
    camera[1] = camera[3] = 551;
}
