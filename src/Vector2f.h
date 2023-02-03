#ifndef ANDREW_LIM_GRAPHICS_VECTOR2F_H
#define ANDREW_LIM_GRAPHICS_VECTOR2F_H

#include <cmath>

namespace al { namespace graphics {

class Vector2f
{
public:

  float x, y;

  Vector2f(float x=0, float y=0)
  : x(x)
  , y(y)
  {
  }

  float get(int index) const
  {
    if (index==0) {
      return x;
    }
    return y;
  }

  float u() const
  {
    return x;
  }

  float v() const
  {
    return y;
  }

  Vector2f add(Vector2f r) const
  {
    return Vector2f(x + r.x, y + r.y);
  }

  Vector2f add(float r) const
  {
    return Vector2f(x + r, y + r);
  }

  Vector2f sub(Vector2f r) const
  {
      return Vector2f(x - r.x, y - r.y);
  }

  Vector2f sub(float r) const
  {
    return Vector2f(x - r, y - r);
  }

  Vector2f mul(Vector2f r) const
  {
      return Vector2f(x * r.x, y * r.y);
  }

  Vector2f mul(float r) const
  {
    return Vector2f(x * r, y * r);
  }

  Vector2f mul(float scaleX, float scaleY)
  {
    return Vector2f(x * scaleX, y * scaleY);
  }

  Vector2f div(Vector2f r) const
  {
      return Vector2f(x / r.x, y / r.y);
  }

  Vector2f div(float r) const
  {
    return Vector2f(x / r, y / r);
  }

  Vector2f abs() const
  {
    return Vector2f(std::abs(x), std::abs(y));
  }

  bool equals(const Vector2f& r) const
  {
    return x==r.x && y==r.y;
  }

  bool operator==(const Vector2f& r) const
  {
    return equals(r);
  }

  bool operator!=(const Vector2f& r) const
  {
    return !equals(r);
  }

  Vector2f lerp(const Vector2f& dest, float lerpFactor) const
  {
    return Vector2f(
      x+(dest.x-x)*lerpFactor,
      y+(dest.y-y)*lerpFactor
    );
  }

}; // class Vector2f
} // namespace al
} // namespace graphics


#endif
