#include "../performance_probe.h"
#include <cstdio>
#include <limits>
int main()
{
    int failures = 0;
    auto check = [&](bool pass, const char *name) {
        printf("%s: %s\n", pass ? "PASS" : "FAIL", name);
        failures += pass ? 0 : 1;
    };
    check(pc2fix::TickMilliseconds(1000, 2500, 1000) == 1500,
          "performance ticks use frequency rather than assumed timer units");
    check(pc2fix::TickMilliseconds(2000, 1000, 1000) == 0 &&
              pc2fix::TickMilliseconds(1000, 2000, 0) == 0,
          "invalid clock intervals reject division by zero and negative duration");
    pc2fix::PhaseStats phase;
    phase.Add(4);
    phase.Add(10);
    phase.Add(-2);
    phase.Add((std::numeric_limits<double>::quiet_NaN)());
    check(phase.count == 2 && phase.Average() == 7 && phase.peak == 10,
          "phase averages exclude invalid durations and retain worst spike");
    pc2fix::FrameStats stats;
    stats.Add({1, 2, 3, 4, 10, true});
    stats.Add({2, 0, 0, 0, 60, false});
    check(stats.frames == 2 && stats.completed == 1 && stats.over50ms == 1 &&
              stats.copy.Average() == 1.5 && stats.total.peak == 60,
          "failed frames and slow frontend calls remain visible in summary");
    stats = {};
    check(stats.frames == 0 && stats.total.Average() == 0 && stats.total.peak == 0,
          "new reporting window does not retain old spikes or divide empty samples");
    return failures ? 1 : 0;
}
