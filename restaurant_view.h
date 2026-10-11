// MIT license; see LICENSE. Presentation-only restaurant enlargement.
#pragma once
#include "presentation.h"
namespace pc2fix
{
struct RestaurantView
{
    RECT source{}, destination{}, area{};
};
inline bool MakeRestaurantView(const DisplayLayout &layout, RECT artwork, RestaurantView &view)
{
    view = {};
    // The authored sidebar tabs protrude 35 pixels into the world canvas.
    RECT area{0, 0, layout.sidebar.left - 35, layout.status.top};
    if (artwork.right <= artwork.left || artwork.bottom <= artwork.top || artwork.left < 0 ||
        artwork.top < 0 || artwork.right > area.right || artwork.bottom > area.bottom)
        return false; // A partially visible/large room keeps the normal camera view.
    RECT source{max(0L, artwork.left - 16), max(0L, artwork.top - 16), min(area.right, artwork.right + 16),
                min(area.bottom, artwork.bottom + 16)};
    const int width = source.right - source.left, height = source.bottom - source.top;
    RECT available{16, 16, area.right - 16, area.bottom - 16};
    if (available.right <= available.left || available.bottom <= available.top)
        return false;
    // Keep pixel artwork legible; never enlarge beyond 2x or squash either axis.
    RECT destination = FitView(min(available.right - available.left, width * 2),
                               min(available.bottom - available.top, height * 2), width, height);
    const int w = destination.right - destination.left, h = destination.bottom - destination.top;
    destination = {(area.right - w) / 2, (area.bottom - h) / 2, (area.right - w) / 2 + w,
                   (area.bottom - h) / 2 + h};
    view = {source, destination, area};
    return true;
}
inline bool RestaurantPoint(const RestaurantView &view, POINT input, POINT &output)
{
    // All UI, including the protruding tabs and lower status bar, stays unchanged.
    if (!PtInRect(&view.area, input))
    {
        output = input;
        return true;
    }
    if (!ViewPoint(view.destination, view.source.right - view.source.left,
                   view.source.bottom - view.source.top, input, output))
        return false;
    output.x += view.source.left;
    output.y += view.source.top;
    return true;
}
} // namespace pc2fix
