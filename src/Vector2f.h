#ifndef ANDREW_LIM_GRAPHICS_VECTOR2F_H
#define ANDREW_LIM_GRAPHICS_VECTOR2F_H

#include <cmath>

namespace al { namespace graphics {

// Generic Vector2
template <class T>
class Vector2g
{
public:
  T x, y;
  Vector2g(T x=0, T y=0) : x(x) , y(y) {}
  T get(int i) const { return i==0?x:y;}
  T u() const { return x; }
  T v() const { return y; }
  Vector2g add(Vector2g r) const { return Vector2g(x+r.x, y+r.y); }
  Vector2g add(T r) const { return Vector2g(x + r, y + r); }
  Vector2g sub(Vector2g r) const { return Vector2g(x - r.x, y - r.y); }
  Vector2g sub(T r) const { return Vector2g(x - r, y - r); }
  Vector2g mul(Vector2g r) const { return Vector2g(x * r.x, y * r.y); }
  Vector2g mul(T r) const { return Vector2g(x * r, y * r); }
  Vector2g mul(T scaleX, T scaleY) { return Vector2g(x * scaleX, y * scaleY); }
  Vector2g div(Vector2g r) const { return Vector2g(x / r.x, y / r.y); }
  Vector2g div(T r) const { return Vector2g(x / r, y / r); }
  Vector2g abs() const { return Vector2g(std::abs(x), std::abs(y)); }
  bool equals(const Vector2g& r) const { return x==r.x && y==r.y; }
  bool operator==(const Vector2g& r) const { return equals(r); }
  bool operator!=(const Vector2g& r) const { return !equals(r); }

  Vector2g lerp(const Vector2g& dest, T lerpFactor) const
  {
    return Vector2g(
      x+(dest.x-x)*lerpFactor,
      y+(dest.y-y)*lerpFactor
    );
  }

}; // class Vector2g

typedef Vector2g<float> Vector2f;

} // namespace al
} // namespace graphics


#endif
