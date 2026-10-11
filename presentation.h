// MIT license; see LICENSE.
#pragma once
#include "display_layout.h"
#include <ddraw.h>
#include <windows.h>
namespace pc2fix
{
inline WINDOWPLACEMENT FocusRestorePlacement(WINDOWPLACEMENT placement)
{
    if (placement.showCmd == SW_SHOWMINIMIZED || placement.showCmd == SW_MINIMIZE ||
        placement.showCmd == SW_SHOWMINNOACTIVE)
    {
        placement.showCmd = placement.flags & WPF_RESTORETOMAXIMIZED ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;
        placement.flags &= ~WPF_RESTORETOMAXIMIZED;
    }
    return placement;
}
inline RECT FitView(int width, int height, int sourceWidth, int sourceHeight)
{
    if (width <= 0 || height <= 0 || sourceWidth <= 0 || sourceHeight <= 0)
        return {};
    int w = width, h = MulDiv(width, sourceHeight, sourceWidth);
    if (h > height)
    {
        h = height;
        w = MulDiv(height, sourceWidth, sourceHeight);
    }
    return {(width - w) / 2, (height - h) / 2, (width + w) / 2, (height + h) / 2};
}
inline bool ViewPoint(RECT area, int width, int height, POINT input, POINT &output)
{
    if (area.right <= area.left || area.bottom <= area.top || width <= 0 || height <= 0 ||
        input.x < area.left || input.x >= area.right || input.y < area.top || input.y >= area.bottom)
        return false;
    output = {(input.x - area.left) * width / (area.right - area.left),
              (input.y - area.top) * height / (area.bottom - area.top)};
    return true;
}
} // namespace pc2fix
void ConfigurePresentation(const pc2fix::DisplayLayout &layout, bool enabled, bool windowed);
bool PresentationEnabled();
bool InstallPresentationHooks();
HRESULT PresentationCooperativeLevel(IDirectDraw *draw, HWND window);
HRESULT CreatePresentationSurface(IDirectDraw *draw, IDirectDrawSurface **output);
void ReleasePresentation();
bool IsNativeWorldFrame();
