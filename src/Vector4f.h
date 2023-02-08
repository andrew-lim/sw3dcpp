#ifndef ANDREW_LIM_GRAPHICS_VECTOR4F_H
#define ANDREW_LIM_GRAPHICS_VECTOR4F_H

#include <cmath>
#include <cstdint>
#include <string>
namespace al { namespace graphics {

template <class T>
class Vector4g
{
public:

  T x, y, z, w;

  Vector4g(T x=0, T y=0, T z=0, T w=1)
  : x(x), y(y), z(z), w(w)
  {
  }

  template <class Arr>
  Vector4g(const Arr& arr)
  : x(arr[0])
  , y(arr[1])
  , z(arr[2])
  , w(arr[3])
  {
  }

  template <class Arr>
  Arr array() const
  {
    Arr arr;
    arr[0] = x;
    arr[1] = y;
    arr[2] = z;
    arr[3] = w;
    return arr;
  }

  static Vector4g zero()
  {
    return Vector4g(0,0,0,0);
  }

  T get(int index) const
  {
    switch (index)
    {
      case 0: return x;
      case 1: return y;
      case 2: return z;
      case 3: return w;
    }
    return 0;
  }

  T u() const
  {
    return x;
  }

  T v() const
  {
    return y;
  }

  Vector4g add(Vector4g r) const
  {
    return Vector4g(x + r.x, y + r.y, z + r.z, w + r.w);
  }

  Vector4g add(T r) const
  {
    return Vector4g(x + r, y + r, z + r, w + r);
  }

  Vector4g sub(Vector4g r) const
  {
      return Vector4g(x - r.x, y - r.y, z - r.z, w - r.w);
  }

  Vector4g sub(T r) const
  {
    return Vector4g(x - r, y - r, z - r, w - r);
  }

  Vector4g mul(Vector4g r) const
  {
      return Vector4g(x * r.x, y * r.y, z * r.z, w * r.w);
  }

  Vector4g mul(T r) const
  {
    return Vector4g(x * r, y * r, z * r, w * r);
  }

  Vector4g mul(T scaleX, T scaleY, T scaleZ, T scaleW=1)
  {
    return Vector4g(x * scaleX, y * scaleY, z * scaleZ, w * scaleW);
  }

  Vector4g div(Vector4g r) const
  {
      return Vector4g(x / r.x, y / r.y, z / r.z, w / r.w);
  }

  Vector4g div(T r) const
  {
    return Vector4g(x / r, y / r, z / r, w / r);
  }

  Vector4g abs() const
  {
    return Vector4g(std::abs(x), std::abs(y), std::abs(z), std::abs(w));
  }

  bool equals(const Vector4g& r) const
  {
    return x==r.x && y==r.y && z==r.z && w==r.w;
  }

  bool operator==(const Vector4g& r) const
  {
    return equals(r);
  }

  bool operator!=(const Vector4g& r) const
  {
    return !equals(r);
  }

  Vector4g lerp(const Vector4g& dest, T lerpFactor) const
  {
    return Vector4g(
      x+(dest.x-x)*lerpFactor,
      y+(dest.y-y)*lerpFactor,
      z+(dest.z-z)*lerpFactor,
      w+(dest.w-w)*lerpFactor
    );
//    return dest.sub(*this).mul(lerpFactor).add(*this);
  }

  std::string toString()
  {
    std::string s = "(";
    s += std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
    return s;
  }

}; // class Vector4g

typedef Vector4g<float> Vector4f;

} // namespace al
} // namespace graphics


#endif
