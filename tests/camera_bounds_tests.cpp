#include "../camera_bounds.h"
#include <climits>
#include <cstdio>
int main()
{
    int failures = 0;
    auto check = [&](bool result, const char *message) {
        printf("%s: %s\n", result ? "PASS" : "FAIL", message);
        if (!result) ++failures;
    };
    check(pc2fix::LimitOversizedCameraAxis(3500, 3041, 3703, 3703) == 3500,
          "valid city axis retains requested position");
    check(pc2fix::LimitOversizedCameraAxis(1534, 1474, 1534, 1456) == 1474,
          "observed oversized city height anchors inside renderer range");
    check(pc2fix::LimitOversizedCameraAxis(1400, 1474, 1534, 1456) == 1474,
          "oversized camera never moves below renderer lower bound");
    check(pc2fix::LimitOversizedCameraAxis(1600, 1474, 1534, 1456) == 1474,
          "oversized camera rejects scrolling toward empty lower area");
    check(pc2fix::LimitOversizedCameraAxis(1193, 1193, 1128, 1128) == 1193,
          "already inverted original interval is left for original implementation");
    check(pc2fix::LimitOversizedCameraAxis(1500, 1474, 1534, 1474) == 1500,
          "physical extent exactly at lower bound leaves original clamp in charge");
    check(pc2fix::LimitOversizedCameraAxis(INT_MAX, INT_MIN, INT_MAX, INT_MAX) == INT_MAX,
          "extreme signed requests do not cause arithmetic overflow");
    return failures ? 1 : 0;
}
