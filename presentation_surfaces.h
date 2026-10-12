// MIT license; see LICENSE. Own creation references independently of game references.
#pragma once
namespace pc2fix
{
template <class Surface> class PresentationSurfaces
{
  public:
    Surface *front = nullptr, *back = nullptr;
    PresentationSurfaces() = default;
    PresentationSurfaces(const PresentationSurfaces &) = delete;
    PresentationSurfaces &operator=(const PresentationSurfaces &) = delete;
    // Explicit release avoids COM teardown from loader-lock static destruction.
    void Reset()
    {
        auto oldFront = front;
        auto oldBack = back;
        front = back = nullptr; // Reentrant messages must not see releasing surfaces.
        if (oldBack)
            oldBack->Release();
        if (oldFront)
            oldFront->Release();
    }
    Surface *FrontForGame() const
    {
        if (front)
            front->AddRef();
        return front;
    }
};
} // namespace pc2fix
