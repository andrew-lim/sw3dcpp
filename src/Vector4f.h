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

  T values[4];

  Vector4g(T x=0, T y=0, T z=0, T w=1)
  {
    values[0] = x;
    values[1] = y;
    values[2] = z;
    values[3] = w;
  }

  template <class Arr>
  Vector4g(const Arr& arr)
  {
    values[0] = arr[0];
    values[1] = arr[1];
    values[2] = arr[2];
    values[3] = arr[3];
  }

  const T& x() const { return values[0]; }
  const T& y() const { return values[1]; }
  const T& z() const { return values[2]; }
  const T& w() const { return values[3]; }
  const T& u() const { return values[0]; }
  const T& v() const { return values[1]; }
  T& x() { return values[0]; }
  T& y() { return values[1]; }
  T& z() { return values[2]; }
  T& w() { return values[3]; }
  T& u() { return values[0]; }
  T& v() { return values[1]; }

  Vector4g add(Vector4g r) const
  {
    return Vector4g(x()+r.x(), y()+r.y(), z()+r.z(), w()+r.w());
  }

  Vector4g add(T r) const
  {
    return Vector4g(x() + r, y() + r, z() + r, w() + r);
  }

  Vector4g sub(Vector4g r) const
  {
    return Vector4g(x()-r.x(), y()-r.y(), z()-r.z(), w()-r.w());
  }

  Vector4g sub(T r) const
  {
    return Vector4g(x() - r, y() - r, z() - r, w() - r);
  }

  Vector4g mul(Vector4g r) const
  {
    return Vector4g(x()*r.x(), y()*r.y(), z()*r.z(), w()*r.w());
  }

  Vector4g mul(T r) const
  {
    return Vector4g(x() * r, y() * r, z() * r, w() * r);
  }

  Vector4g mul(T sx, T sy, T sz, T sw=1) const
  {
    return Vector4g(x()*sx,y()*sy,z()*sz,w()*sw);
  }

  Vector4g div(Vector4g r) const
  {
    return Vector4g(x()/r.x(), y()/r.y(), z()/r.z(), w()/r.w());
  }

  Vector4g div(T r) const
  {
    return Vector4g(x() / r, y() / r, z() / r, w() / r);
  }

  bool equals(const Vector4g& r) const
  {
    return x()==r.x() && y()==r.y() && z()==r.z() && w()==r.w();
  }
  bool operator==(const Vector4g& r) const { return equals(r); }
  bool operator!=(const Vector4g& r) const { return !equals(r); }

  Vector4g abs() const
  {
    return Vector4g(std::abs(x()), std::abs(y()), std::abs(z()), std::abs(w()));
  }

  template <class Arr>
  Arr array() const
  {
    Arr arr;
    arr[0] = values[0];
    arr[1] = values[1];
    arr[2] = values[2];
    arr[3] = values[3];
    return arr;
  }

  static Vector4g zero()
  {
    return Vector4g(0,0,0,0);
  }

  T& get(int index)
  {
    return values[index];
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

  Vector4g& operator/=(float f)
  {
    x() /= f;
    y() /= f;
    z() /= f;
    y() /= f;
    return *this;
  }

  Vector4g lerp(const Vector4g& dest, T lerpFactor) const
  {
    return Vector4g(
      x()+(dest.x()-x())*lerpFactor,
      y()+(dest.y()-y())*lerpFactor,
      z()+(dest.z()-z())*lerpFactor,
      w()+(dest.w()-w())*lerpFactor
    );
  }

  Vector4g rotateX(T rad) const
  {
    // Result.y = v.y * Cos - v.z * Sin;
    // Result.z = v.y * Sin + v.z * Cos;
    T cosTheta = std::cos(rad);
    T sinTheta = std::sin(rad);
    T newY = y() * cosTheta - z() * sinTheta;
    T newZ = y() * sinTheta + z() * cosTheta;
    return Vector4g(x(), newY, newZ, w());
  }

  Vector4g rotateY(T rad) const
  {
    // Result.x =  v.x * Cos + v.z * Sin;
    // Result.z = -v.x * Sin + v.z * Cos;
    T cosTheta = std::cos(rad);
    T sinTheta = std::sin(rad);
    T newX =  x() * cosTheta + z() * sinTheta;
    T newZ = -x() * sinTheta + z() * cosTheta;
    return Vector4g(newX, y(), newZ, w());
  }


}; // class Vector4g

typedef Vector4g<float> Vector4f;

} // namespace al
} // namespace graphics


#endif
