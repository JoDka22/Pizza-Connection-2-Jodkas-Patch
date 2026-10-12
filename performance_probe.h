// MIT license; see LICENSE. Aggregate optional diagnostics; no gameplay timing changes.
#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace pc2fix
{
struct PhaseStats
{
    uint32_t count = 0;
    double sum = 0, peak = 0;
    void Add(double milliseconds)
    {
        if (!std::isfinite(milliseconds) || milliseconds < 0)
            return;
        ++count;
        sum += milliseconds;
        peak = (std::max)(peak, milliseconds);
    }
    double Average() const { return count ? sum / count : 0; }
};
struct FrameSample
{
    double copy = 0, lock = 0, compose = 0, publish = 0, total = 0;
    bool completed = false;
};
struct FrameStats
{
    uint32_t frames = 0, completed = 0, over50ms = 0;
    PhaseStats gap, copy, lock, compose, publish, total;
    void Add(const FrameSample &sample)
    {
        ++frames;
        completed += sample.completed ? 1 : 0;
        over50ms += sample.total > 50 ? 1 : 0;
        copy.Add(sample.copy);
        lock.Add(sample.lock);
        compose.Add(sample.compose);
        publish.Add(sample.publish);
        total.Add(sample.total);
    }
};
inline double TickMilliseconds(int64_t start, int64_t end, int64_t frequency)
{
    if (frequency <= 0 || end < start)
        return 0;
    return (static_cast<double>(end) - static_cast<double>(start)) * 1000.0 / frequency;
}
} // namespace pc2fix
