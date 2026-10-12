// MIT license; see LICENSE. Addresses are for the verified German GOG image only.
#define WIN32_LEAN_AND_MEAN
#include "widescreen.h"
#include "camera_bounds.h"
#include "grid_corner.h"
#include "restaurant_background.h"
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
bool nativeWorldFrame = false;
SpriteFn spriteOriginal = nullptr;
using PaletteSpriteFn = void(__thiscall *)(void *, void *, void *, int, int, int);
PaletteSpriteFn paletteSpriteOriginal = nullptr;
BuffersFn buffersOriginal = nullptr;
using CameraFn = int(__thiscall *)(void *, int, int);
CameraFn cameraOriginal = nullptr;
using AxisFn = int(__thiscall *)(void *, int);
AxisFn cameraXOriginal = nullptr;
AxisFn cameraYOriginal = nullptr;
using SceneFn = void(__thiscall *)(void *, void *);
SceneFn sceneOriginal = nullptr;
using PlacementFn = int(__thiscall *)(void *, int, int, int, int, int, int);
PlacementFn placementOriginal = nullptr;
using CornerFn = void(__thiscall *)(void *, RECT *, int *, int *);
CornerFn bottomCornerOriginal = nullptr;
using CellFn = void *(__thiscall *)(void *, int, int, int, int);
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
bool NativeCity()
{
    auto scene = *reinterpret_cast<void **>(0x57c670);
    return Writable(scene, 0x294) && pc2fix::NativeCityScene(Field<int>(scene, 0x290));
}
bool NativeWorld()
{
    auto scene = *reinterpret_cast<void **>(0x57c670);
    return Writable(scene, 0x294) && pc2fix::NativeWorldScene(Field<int>(scene, 0x290));
}
void __fastcall BottomCorner(void *object, void *, RECT *rectangle, int *column, int *row)
{
    auto scene = *reinterpret_cast<void **>(0x57c670);
    if (active && Writable(scene, 0x294) && Field<int>(scene, 0x290) == 4 && Writable(object, 0x50) &&
        rectangle && column && row)
    {
        // Wide culling can end on a valid, empty node. The draw loop already
        // skips empty nodes, but the original corner converter dereferences
        // their absent payload to obtain the node's row-major index.
        auto lookup = reinterpret_cast<CellFn>(0x4ed5a0);
        auto cell = lookup(object, rectangle->right, rectangle->bottom, 1, 0);
        int x = 0, y = 0;
        if (Writable(cell, 12) && !Field<void *>(cell, 8) &&
            pc2fix::GridCellCoordinates(reinterpret_cast<uintptr_t>(Field<void *>(object, 0x4c)),
                                        reinterpret_cast<uintptr_t>(cell), Field<int>(object, 0x1c),
                                        Field<int>(object, 0x20), x, y))
        {
            *column = x;
            *row = y;
            static unsigned reported = 0;
            if (reported++ < 8)
                Log("wide: restaurant culling corner resolved from empty grid node");
            return;
        }
    }
    bottomCornerOriginal(object, rectangle, column, row);
}
int __fastcall Placement(void *object, void *, int x, int y, int commit, int mode, int cellX, int cellY)
{
    // The original mouse handler can run after the dispatcher clears +0x2fc.
    // Match the original rejected-placement result, without touching inventory.
    if (active && Writable(object, 0x35c) && !Field<void *>(object, 0x2fc))
    {
        Field<int>(object, 0x358) = 0;
        static unsigned reported = 0;
        if (reported++ < 8)
            Log("wide: rejected furniture hit-test without an active world");
        return 0;
    }
    if (active && mode == 0 && Writable(object, 0x35c) && Field<int>(object, 0x290) == 4 &&
        Field<int>(object, 0x344) != -1 && x >= 0 && y >= 0 && x <= *reinterpret_cast<int *>(0x571950) - 30 &&
        y <= *reinterpret_cast<int *>(0x571954))
    {
        auto world = Field<void *>(object, 0x2fc);
        if (Writable(world, 0x50))
        {
            auto lookup = reinterpret_cast<CellFn>(0x4ed5a0);
            auto cell = lookup(world, x, y, 0, 0);
            if (!Writable(cell, 12) || !Field<void *>(cell, 8))
            {
                Field<int>(object, 0x358) = 0;
                return 0;
            }
        }
    }
    return placementOriginal(object, x, y, commit, mode, cellX, cellY);
}
void __fastcall Scene(void *object, void *, void *window)
{
    if (active)
        BeginPresentationScene();
    if (active && Writable(object, 0x2da4))
    {
        int scene = Field<int>(object, 0x290);
        // The dispatcher has already destroyed the previous view. Change bases
        // at canvas creation, before original culling and buffer allocation.
        // Keep the previous view's dimensions until its teardown has completed.
        if (scene)
        {
            auto size = pc2fix::SceneCameraBase(display, scene);
            auto camera = reinterpret_cast<DWORD *>(0x571950);
            camera[2] = size.cx;
            camera[3] = size.cy;
            char line[128]{};
            sprintf_s(line, "wide: scene=%d camera base=%ldx%ld", scene, size.cx, size.cy);
            Log(line);
        }
    }
    sceneOriginal(object, window);
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
    if (active && NativeCity() && display.width == 1920 && display.height == 1080 &&
        Writable(object, 0x10178))
    {
        int lower = Field<int>(object, 0x2c) + Field<int>(object, 0x1016c);
        int upper = Field<int>(object, 0x34) - Field<int>(object, 0x10168);
        x = pc2fix::LimitOversizedCameraAxis(x, lower, upper, PhysicalUpper(object, false, upper));
    }
    return cameraXOriginal(object, x);
}
int __fastcall CameraY(void *object, void *, int y)
{
    if (active && NativeCity() && display.width == 1920 && display.height == 1080 &&
        Writable(object, 0x10178))
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
    const auto generation = PresentationSceneGeneration();
    bool worldFrame = false;
    // 0x523fe0 iterates the inline window pointer array at +0x14,
    // count +0x23c, and checks visibility at window +0x4c.
    if (Writable(object, 0x83e8))
    {
        auto count = Field<DWORD>(object, 0x23c);
        if (count <= (0x234 - 0x14) / 4)
            for (DWORD i = 0; i < count; ++i)
            {
                auto window = Field<void *>(object, 0x14 + i * 4);
                if (NativeWorld() && Writable(window, 0x50) && Field<DWORD>(window, 0x4c) &&
                    HasWorldCanvas(window))
                    worldFrame = true;
            }
        if (worldFrame != nativeWorldFrame)
            Field<DWORD>(object, 0x238) = 1; // Clear stale pixels on scene transitions.
    }
    nativeWorldFrame = worldFrame;
    frameOriginal(object, surface);
    CapturePresentationFrame(generation, worldFrame);
}
void __fastcall PaletteSprite(void *object, void *, void *surface, void *palette, int x, int y, int flags)
{
    // 0x4efc73 restores the room background before the original actor passes.
    // Clearing the entire buffer would erase objects outside the dirty region.
    auto scene = active ? *reinterpret_cast<void **>(0x57c670) : nullptr;
    if (Writable(scene, 0x300) && Field<int>(scene, 0x290) == 4)
    {
        auto world = Field<void *>(scene, 0x2fc);
        if (Writable(world, 0x10178) && Field<int>(world, 0xc) == 16 &&
            !Field<void *>(world, 0x70) && Field<void *>(world, 0x74) == object &&
            Field<void *>(world, 8) == surface && Writable(object, 0x3a) && Writable(surface, 0x74))
        {
            using SizeFn = void(__thiscall *)(void *, int *, int *);
            int width = 0, height = 0, ox = 0, oy = 0;
            reinterpret_cast<SizeFn>(0x53d090)(object, &width, &height);
            reinterpret_cast<SizeFn>(0x53d100)(object, &ox, &oy);
            const LONG bw = Field<WORD>(surface, 0x1a), bh = Field<WORD>(surface, 0x1c);
            // Reject unexpected geometry before additions or touching pixel memory.
            if (bw > 0 && bw <= 4096 && bh > 0 && bh <= 2160 &&
                width > 0 && width <= 16384 && height > 0 && height <= 16384 &&
                x >= -32768 && x <= 32767 && y >= -32768 && y <= 32767 &&
                Field<int>(surface, 0x20) == bw * 3 &&
                Field<void *>(surface, 0x28))
            {
                RECT artwork{x + ox, y + oy, x + ox + width, y + oy + height};
                RECT dirty{Field<LONG>(surface, 4), Field<LONG>(surface, 0xc),
                           Field<LONG>(surface, 8), Field<LONG>(surface, 0x10)};
                auto margins = pc2fix::RestaurantBackgroundMargins(dirty, artwork, {0, 0, bw, bh});
                static unsigned reported = 0;
                if (reported++ < 2)
                {
                    char message[320]{};
                    sprintf_s(message, "wide: restaurant restore buffer=%p pixels=%p size=%ldx%ld stride=%d dirty=%ld,%ld..%ld,%ld art=%ld,%ld..%ld,%ld margins=%u",
                              surface, Field<void *>(surface, 0x28), bw, bh, Field<int>(surface, 0x20),
                              dirty.left, dirty.top, dirty.right, dirty.bottom,
                              artwork.left, artwork.top, artwork.right, artwork.bottom, margins.count);
                    Log(message);
                }
                const int pitch = Field<int>(surface, 0x20);
                const size_t bytes = size_t(pitch) * bh;
                auto pixels = Field<BYTE *>(surface, 0x28);
                const BYTE tag = static_cast<BYTE>(flags == -1 ? *reinterpret_cast<int *>(0x585518) : flags);
                if (Writable(pixels, bytes))
                    pc2fix::ClearRestaurantMargins(pixels, bytes, pitch, bw, bh, margins, tag);
            }
        }
    }
    paletteSpriteOriginal(object, surface, palette, x, y, flags);
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
    sprintf_s(line, "wide: world allocator completed %dx%d (camera base %lux%lu)", width, height,
              *reinterpret_cast<DWORD *>(0x571958), *reinterpret_cast<DWORD *>(0x57195c));
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
    if (world && NativeWorld() && bounds.left == 0 && bounds.top == 0 && bounds.right == 800 &&
        bounds.bottom == 600)
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
        if (kind != pc2fix::SidebarWindow::None && moved != nativeWorldFrame)
        {
            int dx = nativeWorldFrame ? display.width - 800 : 800 - display.width;
            int dy = nativeWorldFrame ? display.height - 600 : 600 - display.height;
            OffsetRect(&bounds, dx, 0);
            OffsetRect(&Field<RECT>(object, 0x18), dx, 0);
            if (kind == pc2fix::SidebarWindow::Panel)
            {
                bounds.bottom = nativeWorldFrame ? display.height : 600;
                Field<RECT>(object, 0x18).bottom = bounds.bottom;
                for (DWORD i = 0; i < Field<DWORD>(object, 0xf0); ++i)
                {
                    auto child = *reinterpret_cast<void **>(entriesNow + i * 8);
                    if (!child)
                        continue;
                    auto &cr = Field<RECT>(child, 0x28);
                    // Keep building filter rows together; only the city-info toolbar
                    // is bottom anchored. Other panels keep their authored content.
                    if (footer && cr.top + bounds.top >= (nativeWorldFrame ? 551 : display.world.bottom))
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
    const bool nativeWorld = world && NativeWorld();
    background =
        (nativeWorld || (bounds.right == display.width && bounds.bottom == display.height && bounds.left > 0))
            ? Field<void *>(object, 0x94)
            : nullptr;
    worldBackground = nativeWorld;
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
        MH_CreateHook(reinterpret_cast<void *>(0x53d600), &PaletteSprite,
                      reinterpret_cast<void **>(&paletteSpriteOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x4ea230), &Buffers,
                      reinterpret_cast<void **>(&buffersOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x4ed440), &Camera,
                      reinterpret_cast<void **>(&cameraOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x4ed360), &CameraX,
                      reinterpret_cast<void **>(&cameraXOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x4ed3c0), &CameraY,
                      reinterpret_cast<void **>(&cameraYOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x4859c0), &Scene,
                      reinterpret_cast<void **>(&sceneOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x489180), &Placement,
                      reinterpret_cast<void **>(&placementOriginal)) != MH_OK ||
        MH_CreateHook(reinterpret_cast<void *>(0x4edaf0), &BottomCorner,
                      reinterpret_cast<void **>(&bottomCornerOriginal)) != MH_OK ||
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
bool IsNativeWorldFrame()
{
    return nativeWorldFrame;
}
bool RestaurantArtwork(RECT &bounds)
{
    if (!active || !nativeWorldFrame)
        return false;
    auto scene = *reinterpret_cast<void **>(0x57c670);
    if (!Writable(scene, 0x300) || Field<int>(scene, 0x290) != 4)
        return false;
    auto world = Field<void *>(scene, 0x2fc);
    if (!Writable(world, 0x10178) || Field<int>(world, 0xc) != 16 || Field<void *>(world, 0x70))
        return false;
    auto sprite = Field<void *>(world, 0x74);
    if (!Writable(sprite, 0x38))
        return false;
    using SizeFn = void(__thiscall *)(void *, int *, int *);
    int width = 0, height = 0, x = 0, y = 0;
    reinterpret_cast<SizeFn>(0x53d090)(sprite, &width, &height);
    reinterpret_cast<SizeFn>(0x53d100)(sprite, &x, &y);
    if (width <= 0 || height <= 0 || width > 16384 || height > 16384)
        return false;
    // Same background origin as 0x4efc2e..0x4efc73 (full-size world bitmap).
    x += Field<int>(world, 0x2c) - 16 * *reinterpret_cast<int *>(0x571960) - Field<int>(world, 0xe8);
    y += Field<int>(world, 0x30) - Field<int>(world, 0x10) * *reinterpret_cast<int *>(0x571964) -
         Field<int>(world, 0xec);
    if (x < 0 || y < 0 || x > display.world.right || y > display.world.bottom ||
        width > display.world.right - x || height > display.world.bottom - y)
        return false;
    bounds = {x, y, x + width, y + height};
    return true;
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
