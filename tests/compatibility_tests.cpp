#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstring>
#include "../main.cpp"
int main()
{
    unsigned failures = 0;
    auto check = [&](bool condition, const char* name) {
        printf("%s: %s\n", condition ? "PASS" : "FAIL", name);
        if (!condition) ++failures;
    };
    const BYTE abc[] = {'a','b','c'};
    check(DigestMatches(abc, 3, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"), "SHA-256 known vector");
    check(!DigestMatches(abc, 3, fileDigest), "SHA-256 mismatch rejected");
    check(!KnownExecutable(), "unknown host rejected before fixed address reads");
    check(!KnownImage(nullptr), "null image rejected without dereference");
    const wchar_t* game = L"..\\pizza  connection 2\\Pizza Connection 2\\fastfood2.exe";
    check(KnownFile(game), "local GOG file fingerprint accepted (read only)");
    HANDLE file = CreateFileW(game, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    HANDLE mapping = file == INVALID_HANDLE_VALUE ? nullptr : CreateFileMappingW(file, nullptr,
        PAGE_READONLY | SEC_IMAGE_NO_EXECUTE, 0, 0, nullptr);
    auto image = mapping ? static_cast<BYTE*>(MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0)) : nullptr;
    check(image && KnownImage(image), "GOG image layout and code fingerprint accepted without executing game");
    if (image) {
        DWORD protection = 0;
        bool protectedPage = VirtualProtect(image + 0x1000, 0x1000, PAGE_NOACCESS, &protection) != FALSE;
        check(protectedPage, "test code page made inaccessible privately");
        if (protectedPage) {
            check(!KnownImage(image), "inaccessible loaded code rejected without crashing");
            DWORD discarded = 0;
            check(VirtualProtect(image + 0x1000, 0x1000, protection, &discarded) != FALSE,
                "test page protection restored");
        }
        bool writable = VirtualProtect(image + 0x1000, 1, PAGE_WRITECOPY, &protection) != FALSE;
        check(writable, "test mapping supports private copy for corruption test");
        if (writable) {
            image[0x1000] ^= 1;
            check(!KnownImage(image), "modified loaded code rejected");
        }
        UnmapViewOfFile(image);
    }
    if (mapping) CloseHandle(mapping);
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    check(WantsWindowed(), "windowed argument found among multiple arguments");
    Main(); // Unknown executable must return without installing hooks or writing fixed addresses.
    originalCreateWindow = &CreateWindowExA;
    WNDCLASSA wc{};
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = "Pizza Connection 2";
    check(RegisterClassA(&wc) != 0, "test window class registered");
    HWND window = CreateGameWindow(0, wc.lpszClassName, "hidden test", 0x80ca0000,
        0, 0, 800, 600, nullptr, nullptr, wc.hInstance, nullptr);
    RECT client{};
    check(window && GetClientRect(window, &client) && client.right == 800 && client.bottom == 600,
        "actual hidden window has 800x600 client area");
    if (window) DestroyWindow(window);
    window = CreateGameWindow(0, wc.lpszClassName, "hidden unrelated dimensions", WS_POPUP,
        0, 0, 320, 240, nullptr, nullptr, wc.hInstance, nullptr);
    check(window && GetClientRect(window, &client) && client.right == 320 && client.bottom == 240,
        "unrelated window geometry preserved");
    if (window) DestroyWindow(window);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);
    return failures ? 1 : 0;
}
