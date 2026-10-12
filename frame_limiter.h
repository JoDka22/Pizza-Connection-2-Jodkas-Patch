// MIT license; see LICENSE. Output pacing only; no engine timer replacement.
#pragma once
#include <windows.h>
#include <cstdint>
namespace pc2fix
{
class FrameDeadline
{
    int64_t interval = 0, previous = 0;
public:
    bool Configure(int fps, int64_t frequency)
    {
        previous = interval = 0;
        if (fps == 0) return true;
        if (fps < 1 || fps > 1000 || frequency < fps) return false;
        interval = (frequency + fps - 1) / fps;
        return true;
    }
    void Reset() { previous = 0; }
    int64_t Remaining(int64_t now) const
    {
        if (!interval || !previous || now < previous) return 0;
        const auto elapsed = now - previous;
        return elapsed < interval ? interval - elapsed : 0;
    }
    void Started(int64_t now) { previous = now; }
    int64_t Interval() const { return interval; }
};
class FrameLimiter
{
    FrameDeadline deadline;
    HANDLE timer = nullptr;
    int64_t frequency = 0;
public:
    ~FrameLimiter() { Configure(0); }
    bool Configure(int fps)
    {
        if (timer) { CloseHandle(timer); timer = nullptr; }
        LARGE_INTEGER value{};
        frequency = QueryPerformanceFrequency(&value) ? value.QuadPart : 0;
        if (!deadline.Configure(fps, frequency)) return false;
        if (fps)
        {
            // High-resolution timers are optional; fall back on older Windows.
            timer = CreateWaitableTimerExW(nullptr, nullptr, 0x2, TIMER_ALL_ACCESS);
            if (!timer) timer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
        }
        return true;
    }
    void Reset() { deadline.Reset(); }
    void Wait()
    {
        if (!deadline.Interval()) return;
        LARGE_INTEGER now{};
        if (!QueryPerformanceCounter(&now)) { Reset(); return; }
        const auto remaining = deadline.Remaining(now.QuadPart);
        if (remaining > 0)
        {
            // Round up to 100 ns. No busy wait or system timer-resolution change.
            LARGE_INTEGER due{};
            due.QuadPart = -((remaining * 10000000 + frequency - 1) / frequency);
            if (!timer || !SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE) ||
                WaitForSingleObject(timer, 1100) != WAIT_OBJECT_0)
                Sleep(static_cast<DWORD>((remaining * 1000 + frequency - 1) / frequency));
            if (!QueryPerformanceCounter(&now)) { Reset(); return; }
        }
        deadline.Started(now.QuadPart); // Late frames do not create a catch-up burst.
    }
};
} // namespace pc2fix
