// MIT license; see LICENSE. Deadline contracts and real Windows timer pacing.
#define WIN32_LEAN_AND_MEAN
#include "../frame_limiter.h"
#include <cstdio>
int main()
{
    int failures = 0;
    auto check = [&](bool value, const char *name) {
        printf("%s: %s\n", value ? "PASS" : "FAIL", name); failures += !value;
    };
    pc2fix::FrameDeadline deadline;
    check(deadline.Configure(60, 10000000) && deadline.Interval() == 166667 && deadline.Remaining(100) == 0,
          "60 FPS rounds up and first frame has no delay");
    deadline.Started(1000000);
    check(deadline.Remaining(1100000) == 66667, "early frame waits only remaining budget");
    check(deadline.Remaining(2000000) == 0, "late frame is not delayed");
    deadline.Started(2000000);
    check(deadline.Remaining(2000001) == 166666, "late frame starts fresh budget without catch-up burst");
    check(deadline.Remaining(1) == 0, "backwards counter rejects wait");
    deadline.Reset(); check(deadline.Remaining(2000001) == 0, "activation reset permits immediate first frame");
    check(deadline.Configure(120, 10000000) && deadline.Interval() == 83334, "120 FPS has independent ceiling");
    check(deadline.Configure(0, 10000000) && deadline.Interval() == 0, "uncapped mode clears deadline");
    check(!deadline.Configure(-1, 10000000) && !deadline.Configure(1001, 10000000) &&
              !deadline.Configure(60, 0) && deadline.Interval() == 0, "invalid configuration disables pacing");
    pc2fix::FrameLimiter limiter;
    LARGE_INTEGER frequency{}, start{}, end{};
    QueryPerformanceFrequency(&frequency);
    check(limiter.Configure(60), "Windows timer backend configures");
    limiter.Wait(); QueryPerformanceCounter(&start);
    limiter.Wait(); limiter.Wait(); QueryPerformanceCounter(&end);
    check(end.QuadPart - start.QuadPart >= frequency.QuadPart / 31,
          "real timer enforces two 60 FPS intervals without busy wait");
    check(limiter.Configure(120) && limiter.Configure(0) && !limiter.Configure(-2),
          "timer reconfiguration and disable release resources safely");
    return failures ? 1 : 0;
}
