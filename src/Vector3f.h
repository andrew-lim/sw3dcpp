#ifndef ANDREW_LIM_GRAPHICS_VECTOR3F_H
#define ANDREW_LIM_GRAPHICS_VECTOR3F_H

#include <cmath>

namespace al { namespace graphics {

template <typename T>
class Vector3g
{
public:

  T x, y, z;

  Vector3g(T x=0, T y=0, T z=0)
  : x(x)
  , y(y)
  , z(z)
  {
  }

  template <class Arr>
  Vector3g(const Arr& arr)
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


  T get(int index) const
  {
    if (index==0) {
      return x;
    }
    else if (index==1) {
      return y;
    }
    return z;
  }

  T u() const
  {
    return x;
  }

  T v() const
  {
    return y;
  }

  Vector3g add(Vector3g r) const
  {
    return Vector3g(x + r.x, y + r.y, z + r.z);
  }

  Vector3g add(T r) const
  {
    return Vector3g(x + r, y + r, z + r);
  }

  Vector3g sub(Vector3g r) const
  {
      return Vector3g(x - r.x, y - r.y, z - r.z);
  }

  Vector3g sub(T r) const
  {
    return Vector3g(x - r, y - r, z - r);
  }

  Vector3g mul(Vector3g r) const
  {
      return Vector3g(x * r.x, y * r.y, z * r.z);
  }

  Vector3g mul(T r) const
  {
    return Vector3g(x * r, y * r, z * r);
  }

  Vector3g mul(T scaleX, T scaleY, T scaleZ)
  {
    return Vector3g(x * scaleX, y * scaleY, z * scaleZ);
  }

  Vector3g div(Vector3g r) const
  {
      return Vector3g(x / r.x, y / r.y, z / r.z);
  }

  Vector3g div(T r) const
  {
    return Vector3g(x / r, y / r, z / r);
  }

  Vector3g abs() const
  {
    return Vector3g(std::abs(x), std::abs(y), std::abs(z));
  }

  bool equals(const Vector3g& r) const
  {
    return x==r.x && y==r.y && z==r.z;
  }

  bool operator==(const Vector3g& r) const
  {
    return equals(r);
  }

  bool operator!=(const Vector3g& r) const
  {
    return !equals(r);
  }

  Vector3g lerp(const Vector3g& dest, T lerpFactor) const
  {
    return Vector3g(
      x+(dest.x-x)*lerpFactor,
      y+(dest.y-y)*lerpFactor,
      z+(dest.z-z)*lerpFactor
    );
  }

}; // class Vector3g

typedef Vector3g<float> Vector3f;

} // namespace al
} // namespace graphics


#endif
