#ifndef ANDREW_LIM_GRAPHICS_VECTOR2F_H
#define ANDREW_LIM_GRAPHICS_VECTOR2F_H

#include <cmath>

namespace al { namespace graphics {

// Generic Vector2
template <class T>
class Vector2g
{
public:
  T values[2];
  Vector2g(T x=0, T y=0) {values[0]=x; values[1]=y;}
  const T& x() const { return values[0]; }
  const T& y() const { return values[1]; }
  const T& u() const { return values[0]; }
  const T& v() const { return values[1]; }
  T& x() { return values[0]; }
  T& y() { return values[1]; }
  T& u() { return values[0]; }
  T& v() { return values[1]; }
  Vector2g add(Vector2g r) const { return Vector2g(x()+r.x(), y()+r.y()); }
  Vector2g add(T r) const { return Vector2g(x() + r, y() + r); }
  Vector2g sub(Vector2g r) const { return Vector2g(x() - r.x(), y() - r.y()); }
  Vector2g sub(T r) const { return Vector2g(x() - r, y() - r); }
  Vector2g mul(Vector2g r) const { return Vector2g(x() * r.x(), y() * r.y()); }
  Vector2g mul(T r) const { return Vector2g(x() * r, y() * r); }
  Vector2g mul(T sx, T sy) const {return Vector2g(x()*sx, y()*sy);}
  Vector2g div(Vector2g r) const { return Vector2g(x() / r.x(), y() / r.y());}
  Vector2g div(T r) const { return Vector2g(x() / r, y() / r); }
  Vector2g abs() const { return Vector2g(std::abs(x()), std::abs(y())); }
  bool equals(const Vector2g& r) const { return x()==r.x() && y()==r.y(); }
  bool operator==(const Vector2g& r) const { return equals(r); }
  bool operator!=(const Vector2g& r) const { return !equals(r); }

  Vector2g lerp(const Vector2g& dest, T lerpFactor) const
  {
    return Vector2g(
      x()+(dest.x()-x())*lerpFactor,
      y()+(dest.y()-y())*lerpFactor
    );
  }

  Vector2g& operator/=(float f)
  {
    x() /= f;
    y() /= f;
    return *this;
  }

}; // class Vector2g

typedef Vector2g<float> Vector2f;

} // namespace al
} // namespace graphics


#endif
