#ifndef ANDREW_LIM_GRAPHICS_VECTOR3F_H
#define ANDREW_LIM_GRAPHICS_VECTOR3F_H

#include <cmath>

namespace al { namespace graphics {

template <typename T>
class Vector3g
{
public:

  T values[3];
  Vector3g(T x=0, T y=0, T z=0)
  {
    values[0] = x;
    values[1] = y;
    values[2] = z;
  }
  const T& x() const { return values[0]; }
  const T& y() const { return values[1]; }
  const T& z() const { return values[2]; }
  const T& u() const { return values[0]; }
  const T& v() const { return values[1]; }
  T& x() { return values[0]; }
  T& y() { return values[1]; }
  T& z() { return values[2]; }
  T& u() { return values[0]; }
  T& v() { return values[1]; }

  Vector3g add(Vector3g r) const
  {
    return Vector3g(x()+r.x(), y()+r.y(), z()+r.z());
  }

  Vector3g add(T r) const { return Vector3g(x()+r, y()+r, z()+r); }

  Vector3g sub(Vector3g r) const
  {
    return Vector3g(x()-r.x(), y()-r.y(), z()-r.z());
  }

  Vector3g sub(T r) const { return Vector3g(x() - r, y() - r, z() - r); }

  Vector3g mul(Vector3g r) const
  {
    return Vector3g(x() * r.x(), y() * r.y(), z() * r.z());
  }

  Vector3g mul(T r) const
  {
    return Vector3g(x() * r, y() * r, z() * r);
  }

  Vector3g mul(T sx(), T sy(), T sz())
  {
    return Vector3g(x() * sx(), y() * sy(), z() * sz());
  }

  Vector3g div(Vector3g r) const
  {
    return Vector3g(x() / r.x(), y() / r.y(), z() / r.z());
  }

  Vector3g div(T r) const
  {
    return Vector3g(x() / r, y() / r, z() / r);
  }

  Vector3g abs() const
  {
    return Vector3g(std::abs(x()), std::abs(y()), std::abs(z()));
  }

  bool equals(const Vector3g& r) const
  {
    return x()==r.x() && y()==r.y() && z()==r.z();
  }
  bool operator==(const Vector3g& r) const { return equals(r); }
  bool operator!=(const Vector3g& r) const { return !equals(r); }

  template <class Arr>
  Vector3g(const Arr& arr)
  {
    values[0] = arr[0];
    values[1] = arr[1];
    values[2] = arr[2];
  }

  template <class Arr>
  Arr array() const
  {
    Arr arr;
    arr[0] = values[0];
    arr[1] = values[1];
    arr[2] = values[2];
    return arr;
  }

  const T& get(int index) const
  {
    return values[index];
  }

  T& operator[](int index)
  {
    return values[index];
  }

  const T& operator[](int index) const
  {
    return values[index];
  }

  Vector3g lerp(const Vector3g& dest, T lerpFactor) const
  {
    return Vector3g(
      x()+(dest.x()-x())*lerpFactor,
      y()+(dest.y()-y())*lerpFactor,
      z()+(dest.z()-z())*lerpFactor
    );
  }

  Vector3g& operator/=(float f)
  {
    x() /= f;
    y() /= f;
    z() /= f;
    return *this;
  }

}; // class Vector3g

typedef Vector3g<float> Vector3f;

} // namespace al
} // namespace graphics


#endif
