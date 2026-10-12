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
inline bool RestaurantCanvasView(const RestaurantView &view, RestaurantView &canvas)
{
    canvas = {};
    if (view.area.left < 0 || view.area.top < 0 || view.area.right > 4096 || view.area.bottom > 2160 ||
        view.area.right <= view.area.left || view.area.bottom <= view.area.top ||
        view.source.left < view.area.left || view.source.top < view.area.top ||
        view.source.right > view.area.right || view.source.bottom > view.area.bottom ||
        view.destination.left < view.area.left || view.destination.top < view.area.top ||
        view.destination.right > view.area.right || view.destination.bottom > view.area.bottom)
        return false;
    const int width = view.source.right - view.source.left;
    const int height = view.source.bottom - view.source.top;
    const int w = view.destination.right - view.destination.left;
    const int h = view.destination.bottom - view.destination.top;
    if (width <= 0 || height <= 0 || w <= 0 || h <= 0)
        return false;
    // The artwork crop defines the stable zoom and room anchor, not the limit
    // of moving world sprites. Extend that same transform across the world.
    const LONG left = view.destination.left - MulDiv(view.source.left - view.area.left, w, width);
    const LONG top = view.destination.top - MulDiv(view.source.top - view.area.top, h, height);
    RECT destination{left, top,
                     left + MulDiv(view.area.right - view.area.left, w, width),
                     top + MulDiv(view.area.bottom - view.area.top, h, height)};
    canvas = {view.area, destination, view.area};
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
    RestaurantView canvas{};
    if (!RestaurantCanvasView(view, canvas) ||
        !ViewPoint(canvas.destination, canvas.source.right - canvas.source.left,
                   canvas.source.bottom - canvas.source.top, input, output))
        return false;
    output.x += canvas.source.left;
    output.y += canvas.source.top;
    return true;
}
inline bool RestaurantFramePoint(const RestaurantView &view, bool geometryMatches, POINT input, POINT &output)
{
    if (!PtInRect(&view.area, input))
    {
        output = input; // Sidebar/status remain responsive during a room transition.
        return true;
    }
    return geometryMatches && RestaurantPoint(view, input, output);
}
} // namespace pc2fix
