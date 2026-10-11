// MIT license; see LICENSE. Keep oversized cameras inside the original safe range.
#pragma once
namespace pc2fix
{
inline int LimitOversizedCameraAxis(int requested, int lower, int upper, int physicalUpper)
{
    // Moving below the original lower bound can invalidate compressed sprite rows.
    return lower <= upper && physicalUpper < lower ? lower : requested;
}
} // namespace pc2fix
