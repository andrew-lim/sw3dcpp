#ifndef ANDREW_LIM_GRAPHICS_VECTOR3F_H
#define ANDREW_LIM_GRAPHICS_VECTOR3F_H

#include <cmath>

namespace al { namespace graphics {

class Vector3f
{
public:

  float x, y, z;

  Vector3f(float x=0, float y=0, float z=0)
  : x(x)
  , y(y)
  , z(z)
  {
  }

  template <class Arr>
  Vector3f(const Arr& arr)
  : x(arr[0])
  , y(arr[1])
  , z(arr[2])
  {
  }

  template <class Arr>
  Arr array() const
  {
    Arr arr;
    arr[0] = x;
    arr[1] = y;
    arr[2] = z;
    return arr;
  }


  float get(int index) const
  {
    if (index==0) {
      return x;
    }
    else if (index==1) {
      return y;
    }
    return z;
  }

  float u() const
  {
    return x;
  }

  float v() const
  {
    return y;
  }

  Vector3f add(Vector3f r) const
  {
    return Vector3f(x + r.x, y + r.y, z + r.z);
  }

  Vector3f add(float r) const
  {
    return Vector3f(x + r, y + r, z + r);
  }

  Vector3f sub(Vector3f r) const
  {
      return Vector3f(x - r.x, y - r.y, z - r.z);
  }

  Vector3f sub(float r) const
  {
    return Vector3f(x - r, y - r, z - r);
  }

  Vector3f mul(Vector3f r) const
  {
      return Vector3f(x * r.x, y * r.y, z * r.z);
  }

  Vector3f mul(float r) const
  {
    return Vector3f(x * r, y * r, z * r);
  }

  Vector3f mul(float scaleX, float scaleY, float scaleZ)
  {
    return Vector3f(x * scaleX, y * scaleY, z * scaleZ);
  }

  Vector3f div(Vector3f r) const
  {
      return Vector3f(x / r.x, y / r.y, z / r.z);
  }

  Vector3f div(float r) const
  {
    return Vector3f(x / r, y / r, z / r);
  }

  Vector3f abs() const
  {
    return Vector3f(std::abs(x), std::abs(y), std::abs(z));
  }

  bool equals(const Vector3f& r) const
  {
    return x==r.x && y==r.y && z==r.z;
  }

  bool operator==(const Vector3f& r) const
  {
    return equals(r);
  }

  bool operator!=(const Vector3f& r) const
  {
    return !equals(r);
  }

  Vector3f lerp(const Vector3f& dest, float lerpFactor) const
  {
    return Vector3f(
      x+(dest.x-x)*lerpFactor,
      y+(dest.y-y)*lerpFactor,
      z+(dest.z-z)*lerpFactor
    );
  }

}; // class Vector3f
} // namespace al
} // namespace graphics


#endif
