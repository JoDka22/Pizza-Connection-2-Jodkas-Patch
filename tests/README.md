# Local validation

The native tests include main.cpp to exercise the actual implementation. They use a hidden real Windows window, not a geometry mock. The optional image test requires the locally supplied GOG executable at ../pizza  connection 2/Pizza Connection 2/fastfood2.exe. This file is never executed or changed. SEC_IMAGE_NO_EXECUTE mapping and a private copy-on-write corruption test check loaded-code rejection.

After the Release build, in an x86 VS2022 Developer Command Prompt, from the repository root:

```bat
cl /nologo /std:c++17 /W4 /WX /MT /EHsc /I vendors\minhook-master\include tests\compatibility_tests.cpp /Foout\compatibility_tests.obj /Feout\compatibility_tests.exe /link user32.lib
out\compatibility_tests.exe extra -windowed another
cl /nologo /W4 /WX /MT /EHsc tests\proxy_smoke.cpp /Foout\proxy_smoke.obj /Feout\Release\proxy_smoke.exe
```

For the proxy smoke test ONLY, copy your original Bink DLL to out/Release/binkw32o.dll (never commit it), then run out/Release/proxy_smoke.exe. It calls BinkSetSoundSystem BEFORE resolving any forwarder, then checks real forwarding and repeated initialisation in an unsupported host. Also run it from another working directory. Inspect out/Release/pc2fix.log for exactly one bootstrap per process and rejection before hooks. Do not mistake this for a game playtest.

For the missing-original case, create a separate ignored folder containing ONLY the built binkw32.dll and proxy_smoke.exe. Run that executable with `--missing-original`. It must return success after verifying controlled rejection (return value 0, no loaded original). The expected log includes Windows error 126. Never rename/delete an existing original DLL to set up this test.

The native tests also exercise null images and a privately mapped NoAccess code page. All changes are confined to the test process; original bytes on disk remain untouched. Actual local gameplay observations are documented in docs/RUNTIME_VALIDATION_DE.md.

Python export audit: install pefile into an ignored local directory or your own environment and run tools/inspect_pe.py against your locally owned files. No game binary is supplied by the repository.
# Display prototype tests

From the repository root in an x86 Developer Command Prompt, after building the solution:

```bat
cl /nologo /std:c++17 /W4 /WX /MT /EHsc tests\display_tests.cpp /Foout\display_tests.obj /Feout\display_tests.exe /link user32.lib
out\display_tests.exe
cl /nologo /std:c++17 /W4 /WX /MT /EHsc tests\ddraw_smoke.cpp /Foout\ddraw_smoke.obj /Feout\Release\ddraw_smoke.exe
out\Release\ddraw_smoke.exe
```

The 36 layout/grid checks validate the release resolution gate, UI/world geometry, coordinate boundaries, anchored
control origins and bounded row-major grid indices. They do not replace gameplay tests. The smoke test exercises unknown-host
system forwarding and reads the current display mode without changing it. See
[actual game observations and remaining manual checks](../docs/WIDESCREEN_DE.md).

The additional safety test compiles the real widescreen implementation into an
unknown host and verifies rejection before fixed-address reads or MinHook init:

```bat
cl /nologo /std:c++17 /W4 /WX /MT /EHsc /I vendors\minhook-master\include tests\widescreen_safety_tests.cpp out\display-obj\Release\buffer.obj out\display-obj\Release\hde32.obj out\display-obj\Release\hook.obj out\display-obj\Release\trampoline.obj /Foout\widescreen_safety_tests.obj /Feout\widescreen_safety_tests.exe /link user32.lib gdi32.lib /LTCG
out\widescreen_safety_tests.exe
```

## Full-HD presentation tests

```bat
cl /nologo /std:c++17 /W4 /WX /MT /EHsc tests\presentation_tests.cpp /Foout\presentation_tests.obj /Feout\presentation_tests.exe /link user32.lib gdi32.lib
out\presentation_tests.exe
```

The 36 checks cover stale room input, offscreen composition, resize/failure/resource handling, restaurant geometry/picking,
restoring minimized normal/maximized focus snapshots,
aspect-preserving output, inverse mouse coordinates, bar/edge
rejection and actual RGB565 GDI transfer using a 1920-wide source stride with only
600 authored rows. First/last row and letterbox pixel checks guard against incorrect
DIB cropping. They do not test surface loss, game timing or every UI view. See
[local runtime observations](../docs/PRESENTATION_DE.md).

The CI script also builds `presentation_benchmark.cpp` and runs `--verify-only`:
three complete synthetic RGB image comparisons between the previous and clipped
restaurant compositor, including smaller and tall windows. Run
`out\presentation_benchmark.exe` without arguments for a local timing comparison.
Timing results are informational; CI never requires a speedup.
See [composition benchmark and limits](../docs/RESTAURANT_PERFORMANCE_DE.md).

`rgb565_benchmark.cpp` compares the complete buffered RGB32 path with the previous
RGB16 GDI output in 12 image cases: menus, city, restaurant and RGB555 fallback,
at three window sizes. The source covers every RGB565 color and padded rows.
Invalid conversion arguments are also checked. CI runs `--verify-only`; running
the executable without arguments additionally reports three-round median timings.

## Optional frontend timing probe

`performance_probe_tests.cpp` adds five checks for counter-frequency conversion,
invalid intervals, aggregate averages/peaks, incomplete frames and report reset.
It is included in `tools/ci-windows.cmd`. The default-disabled runtime probe and
CPU/GPU measurement limits are documented in [frame diagnosis](../docs/FRAME_SPIKES_DE.md).

## DirectDraw lifetime and ownership tests

`presentation_lifetime_tests.cpp` adds eleven regressions for independent game/
presenter references, either release order, partial construction and reentrant
teardown. It compiles the actual proxy against synthetic DirectDraw objects to
verify owner/non-owner forwarding and teardown. Included in `tools/ci-windows.cmd`.
See [deep code review](../docs/DEEP_REVIEW_LIFETIME_DE.md) for findings and limits.

## Frame/input transition tests

`frame_pipeline_tests.cpp` adds twelve transition regressions for independent
back/front/published geometry, scene generations, failed publication, native and
legacy input, equal room rectangles and client resize. It is included in the CI
script. See [frame/input synchronization](../docs/FRAME_INPUT_SYNC_DE.md).

## Camera bounds tests

```bat
cl /nologo /std:c++17 /W4 /WX /MT /EHsc tests\camera_bounds_tests.cpp /Foout\camera_bounds_tests.obj /Feout\camera_bounds_tests.exe
out\camera_bounds_tests.exe
```

Seven contract checks cover unchanged normal requests, anchoring an oversized view
inside the original renderer range, original inverted intervals and boundaries.
They do not exercise actual compressed game sprites. See
[camera analysis and rejected centering experiment](../docs/CAMERA_BOUNDS_DE.md).
