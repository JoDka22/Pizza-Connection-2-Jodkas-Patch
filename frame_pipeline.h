// MIT license; see LICENSE. Geometry follows the rendered, copied and published pixels.
#pragma once
#include "restaurant_view.h"
#include <cstdint>
namespace pc2fix
{
struct FrameGeometry
{
    uint64_t generation = 0;
    bool valid = false, native = false, interior = false;
    RECT artwork{};
    RestaurantView room{};
};
class FramePipeline
{
    uint64_t generation = 1;
    FrameGeometry back{}, front{}, published{};
    SIZE publishedSize{};
    bool enlarged = false;
  public:
    uint64_t Generation() const { return generation; }
    void BeginScene() { ++generation; back = {}; }
    bool Rendered(uint64_t token, bool native, const DisplayLayout &layout, const RECT *artwork)
    {
        if (token != generation)
            return false; // A nested scene switch invalidates this render pass.
        back = {};
        back.generation = token;
        back.valid = true;
        back.native = native;
        if (native && artwork)
        {
            back.artwork = *artwork;
            back.interior = MakeRestaurantView(layout, *artwork, back.room);
        }
        return true;
    }
    FrameGeometry Back() const { return back; }
    FrameGeometry Front() const { return front; }
    void Copied(const FrameGeometry &image) { front = image; }
    void CopyFailed() { front = {}; } // Do not repaint unverified surface contents.
    void Published(const FrameGeometry &image, SIZE client, bool roomEnlarged)
    {
        published = image;
        publishedSize = client;
        enlarged = roomEnlarged;
    }
    bool Point(const DisplayLayout &layout, SIZE client, bool artworkMatches, POINT input, POINT &output) const
    {
        if (!published.valid || published.generation != generation ||
            client.cx != publishedSize.cx || client.cy != publishedSize.cy)
            return false;
        const int width = published.native ? layout.width : 800;
        const int height = published.native ? layout.height : 600;
        POINT mapped{};
        if (!ViewPoint(FitView(client.cx, client.cy, width, height), width, height, input, mapped))
            return false;
        if (enlarged)
            return RestaurantFramePoint(published.room, artworkMatches, mapped, output);
        output = mapped;
        return true;
    }
    bool ArtworkMatches(const RECT &artwork) const
    {
        return !enlarged || EqualRect(&published.artwork, &artwork);
    }
    bool Enlarged() const { return enlarged; }
    void Reset() { generation = 1; back = front = published = {}; publishedSize = {}; enlarged = false; }
};
} // namespace pc2fix
