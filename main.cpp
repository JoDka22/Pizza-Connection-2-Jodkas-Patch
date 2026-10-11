#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <shellapi.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <MinHook.h>
#include "main.h"
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "shell32.lib")
namespace {
using CreateWindowFn = decltype(&CreateWindowExA);
CreateWindowFn originalCreateWindow = nullptr;
constexpr char fileDigest[] = "18cfd32e0ac5d6d98162ec016e058a2b51808d9ca49ebee933ce3166ff9922fd";
constexpr char textDigest[] = "1fe8d65180eb537ad2bfe89b3b40c1c541399f0e464aa0d6c24b274fc74b1d1a";
bool DigestMatches(const BYTE* bytes, ULONG length, const char* expected)
{
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    BYTE digest[32]{};
    char hex[65]{};
    bool ok = false;
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) return false;
    if (BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0) >= 0 &&
        BCryptHashData(hash, const_cast<BYTE*>(bytes), length, 0) >= 0 &&
        BCryptFinishHash(hash, digest, sizeof(digest), 0) >= 0) {
        for (unsigned i = 0; i < sizeof(digest); ++i) sprintf_s(hex + i * 2, 3, "%02x", digest[i]);
        ok = std::strcmp(hex, expected) == 0;
    }
    if (hash) BCryptDestroyHash(hash);
    BCryptCloseAlgorithmProvider(algorithm, 0);
    return ok;
}
bool KnownFile(const wchar_t* path)
{
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size{};
    bool known = false;
    if (GetFileSizeEx(file, &size) && size.QuadPart == 1568768) {
        HANDLE mapping = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (mapping) {
            auto bytes = static_cast<const BYTE*>(MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0));
            if (bytes) {
                known = DigestMatches(bytes, static_cast<ULONG>(size.QuadPart), fileDigest);
                UnmapViewOfFile(bytes);
            }
            CloseHandle(mapping);
        }
    }
    CloseHandle(file);
    return known;
}
bool ReadableRange(const void* pointer, SIZE_T length)
{
    auto address = reinterpret_cast<ULONG_PTR>(pointer);
    if (!address || length > (std::numeric_limits<ULONG_PTR>::max)() - address) return false;
    const auto end = address + length;
    while (address < end) {
        MEMORY_BASIC_INFORMATION region{};
        if (!VirtualQuery(reinterpret_cast<const void*>(address), &region, sizeof(region)) ||
            region.State != MEM_COMMIT || (region.Protect & (PAGE_GUARD | PAGE_NOACCESS))) return false;
        const auto access = region.Protect & 0xff;
        if (access != PAGE_READONLY && access != PAGE_READWRITE && access != PAGE_WRITECOPY &&
            access != PAGE_EXECUTE_READ && access != PAGE_EXECUTE_READWRITE && access != PAGE_EXECUTE_WRITECOPY)
            return false;
        const auto start = reinterpret_cast<ULONG_PTR>(region.BaseAddress);
        if (region.RegionSize > (std::numeric_limits<ULONG_PTR>::max)() - start) return false;
        const auto next = start + region.RegionSize;
        if (next <= address) return false;
        address = next;
    }
    return true;
}
bool KnownImage(const BYTE* base)
{
    // This executable has stripped relocations. Reject any other layout or modified code.
    if (!ReadableRange(base, 0x1000)) return false;
    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0 || dos->e_lfanew > 0x800) return false;
    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 ||
        nt->FileHeader.TimeDateStamp != 0x3a39d466 || nt->FileHeader.NumberOfSections != 4 ||
        nt->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER32) ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC ||
        nt->OptionalHeader.ImageBase != 0x400000 ||
        nt->OptionalHeader.SizeOfImage != 0x18e000) return false;
    auto section = IMAGE_FIRST_SECTION(nt);
    if (std::memcmp(section->Name, ".text\0\0\0", 8) || section->VirtualAddress != 0x1000 ||
        section->Misc.VirtualSize != 0x15aabc || !(section->Characteristics & IMAGE_SCN_MEM_EXECUTE)) return false;
    const auto code = base + section->VirtualAddress;
    return ReadableRange(code, section->Misc.VirtualSize) &&
        DigestMatches(code, section->Misc.VirtualSize, textDigest);
}
bool KnownExecutable()
{
    wchar_t path[32768]{};
    DWORD length = GetModuleFileNameW(nullptr, path, _countof(path));
    return length && length < _countof(path) && KnownFile(path) &&
        reinterpret_cast<ULONG_PTR>(GetModuleHandleW(nullptr)) == 0x400000 &&
        KnownImage(reinterpret_cast<const BYTE*>(GetModuleHandleW(nullptr)));
}
bool WantsWindowed()
{
    int count = 0;
    auto args = CommandLineToArgvW(GetCommandLineW(), &count);
    if (!args) return false;
    bool requested = false;
    for (int i = 1; i < count; ++i) if (lstrcmpiW(args[i], L"-windowed") == 0) requested = true;
    LocalFree(args);
    return requested;
}
[[maybe_unused]] HWND WINAPI CreateGameWindow(DWORD exStyle, LPCSTR className, LPCSTR title, DWORD style,
    int x, int y, int width, int height, HWND parent, HMENU menu, HINSTANCE instance, LPVOID parameter)
{
    if (HIWORD(reinterpret_cast<ULONG_PTR>(className)) &&
        std::strcmp(className, "Pizza Connection 2") == 0 && !parent &&
        width == 800 && height == 600 && (style & WS_CAPTION) == WS_CAPTION) {
        RECT frame{0, 0, 800, 600};
        if (AdjustWindowRectEx(&frame, style, menu != nullptr, exStyle)) {
            width = frame.right - frame.left;
            height = frame.bottom - frame.top;
            MONITORINFO monitor{sizeof(monitor)};
            if (GetMonitorInfoW(MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY), &monitor)) {
                x = monitor.rcWork.left + (monitor.rcWork.right - monitor.rcWork.left - width) / 2;
                y = monitor.rcWork.top + (monitor.rcWork.bottom - monitor.rcWork.top - height) / 2;
            }
        }
    }
    return originalCreateWindow(exStyle, className, title, style, x, y, width, height,
        parent, menu, instance, parameter);
}
}
void Log(const char* message)
{
    OutputDebugStringA("pc2fix: "); OutputDebugStringA(message); OutputDebugStringA("\n");
    wchar_t path[32768]{};
    DWORD length = GetModuleFileNameW(nullptr, path, _countof(path));
    if (!length || length >= _countof(path)) return;
    auto slash = wcsrchr(path, L'\\');
    if (!slash || wcscpy_s(slash + 1, _countof(path) - (slash + 1 - path), L"pc2fix.log")) return;
    HANDLE file = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    DWORD written = 0;
    WriteFile(file, message, static_cast<DWORD>(std::strlen(message)), &written, nullptr);
    WriteFile(file, "\r\n", 2, &written, nullptr);
    CloseHandle(file);
}
void Main()
{
    Log("bootstrap: Bink audio initialisation; no simulation/timer patches");
    if (!KnownExecutable()) {
        Log("unsupported executable or modified code: all game hooks and writes disabled"); return;
    }
    Log("recognised: German GOG fastfood2.exe, SHA-256 and loaded .text verified");
    if (!WantsWindowed()) {
        Log("Bink bootstrap: no display hooks; optional ddraw proxy owns display diagnostics"); return;
    }
    // A zero HWND does not prove that the game's window thread has not started.
    // Until an early, synchronised entry point is established, do not race that thread.
    Log("windowed activation deferred: Bink bootstrap cannot prove pre-window synchronisation");
}
bool IsSupportedGame()
{
    return KnownExecutable();
}
