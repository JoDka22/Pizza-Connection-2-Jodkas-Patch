// MIT license; see LICENSE. Reproduce transitions without game assets or hooks.
#define WIN32_LEAN_AND_MEAN
#include "../frame_pipeline.h"
#include <cstdio>
int main()
{
    int failures = 0;
    auto check = [&](bool pass, const char *name) {
        printf("%s: %s\n", pass ? "PASS" : "FAIL", name);
        failures += !pass;
    };
    pc2fix::DisplayLayout layout{};
    pc2fix::MakeLayout(1920, 1080, layout);
    pc2fix::FramePipeline frames;
    SIZE size{1920, 1080};
    POINT mapped{};
    check(!frames.Point(layout, size, true, {960, 540}, mapped),
          "input waits for the first successfully published image");
    frames.Rendered(frames.Generation(), true, layout, nullptr);
    auto city = frames.Back();
    frames.Copied(city);
    frames.Rendered(frames.Generation(), false, layout, nullptr);
    check(frames.Front().native && !frames.Back().native,
          "front retains its own geometry while a different back image is rendered");
    frames.Published(city, size, false);
    check(frames.Point(layout, size, true, {960, 540}, mapped) && mapped.x == 960 && mapped.y == 540,
          "published city image uses native input coordinates");
    frames.CopyFailed();
    check(!frames.Front().valid && frames.Point(layout, size, true, {960, 540}, mapped) && mapped.x == 960,
          "failed surface copy prevents repaint while preserving last published input");
    frames.Rendered(frames.Generation(), false, layout, nullptr);
    check(frames.Point(layout, size, true, {960, 540}, mapped) && mapped.x == 960,
          "rendering alone cannot switch visible input to legacy coordinates");
    auto menu = frames.Back();
    frames.Copied(menu);
    check(frames.Point(layout, size, true, {960, 540}, mapped) && mapped.x == 960,
          "failed publication preserves previous visible input mapping");
    frames.Published(menu, size, false);
    check(frames.Point(layout, size, true, {960, 540}, mapped) && mapped.x == 400 && mapped.y == 300,
          "successful menu publication switches to proportional 800x600 mapping");
    RECT art{26, 79, 826, 630};
    frames.Rendered(frames.Generation(), true, layout, &art);
    auto room = frames.Back();
    frames.Copied(room); frames.Published(room, size, true);
    frames.BeginScene();
    check(!frames.Point(layout, size, true, {900, 500}, mapped) &&
              !frames.Point(layout, size, true, {1900, 300}, mapped) && frames.Front().interior,
          "new scene rejects old-room input even when artwork rectangles are identical");
    frames.Rendered(frames.Generation(), true, layout, &art);
    room = frames.Back(); frames.Copied(room); frames.Published(room, size, true);
    auto corner = POINT{room.room.destination.left, room.room.destination.top};
    check(frames.Point(layout, size, true, corner, mapped) &&
              mapped.x == room.room.source.left && mapped.y == room.room.source.top,
          "matching scene publication restores accurate room picking");
    check(!frames.Point(layout, size, false, corner, mapped) &&
              frames.Point(layout, size, false, {1900, 300}, mapped) && mapped.x == 1900,
          "camera geometry mismatch rejects world picking but preserves same-scene sidebar");
    SIZE resized{1467, 825};
    bool beforeResize = frames.Point(layout, resized, true, {700, 400}, mapped);
    frames.Published(room, resized, true);
    check(!beforeResize && frames.Point(layout, resized, true, {1400, 300}, mapped),
          "resized client waits for publication at matching dimensions");
    auto obsolete = frames.Generation();
    frames.BeginScene();
    frames.Rendered(frames.Generation(), false, layout, nullptr);
    check(!frames.Rendered(obsolete, true, layout, &art) && !frames.Back().native,
          "nested scene switch cannot commit obsolete render metadata");
    frames.Reset();
    check(!frames.Back().valid && !frames.Front().valid &&
              !frames.Point(layout, size, true, {960, 540}, mapped),
          "teardown clears buffer metadata and published input");
    return failures ? 1 : 0;
}
