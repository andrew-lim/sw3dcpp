#ifndef ANDREW_LIM_GRAPHICS_VECTOR4F_H
#define ANDREW_LIM_GRAPHICS_VECTOR4F_H

#include <cmath>
#include <cstdint>
#include <string>
#include <glm/vec4.hpp> // glm::vec4
namespace al { namespace graphics {

class Vector4f
{
private:
  float parts[4];
  
public:
  
  Vector4f(float x=0, float y=0, float z=0, float w=1)
  {
    parts[0] = x;
    parts[1] = y;
    parts[2] = z;
    parts[3] = w;
  }

  Vector4f(const glm::vec4& v)
  {
    parts[0] = v[0];
    parts[1] = v[1];
    parts[2] = v[2];
    parts[3] = v[3];
  }
  
  glm::vec4 vec4()
  {
    return glm::vec4(parts[0], parts[1], parts[2], parts[3]);
  }
  
  float* getArray()
  {
    return parts;
  }

  static Vector4f zero()
  {
    return Vector4f(0,0,0,0);
  }

  float get(int index) const
  {
    return parts[index];
  }

  float x() const
  {
    return parts[0];
  }

  float y() const
  {
    return parts[1];
  }

  float z() const
  {
    return parts[2];
  }

  float w() const
  {
    return parts[3];
  }
  
  float u() const
  {
    return parts[0];
  }
  
  float v() const
  {
    return parts[1];
  }

  Vector4f add(Vector4f r) const
  {
    return Vector4f(x() + r.x(), y() + r.y(), z() + r.z(), w() + r.w());
  }
  
  Vector4f add(float r) const
  {
    return Vector4f(x() + r, y() + r, z() + r, w() + r);
  }

  Vector4f sub(Vector4f r) const
  {
      return Vector4f(x() - r.x(), y() - r.y(), z() - r.z(), w() - r.w());
  }
  
  Vector4f sub(float r) const
  {
    return Vector4f(x() - r, y() - r, z() - r, w() - r);
  }

  Vector4f mul(Vector4f r) const
  {
      return Vector4f(x() * r.x(), y() * r.y(), z() * r.z(), w() * r.w());
  }
  
  Vector4f mul(float r) const
  {
    return Vector4f(x() * r, y() * r, z() * r, w() * r);
  }

  Vector4f div(Vector4f r) const
  {
      return Vector4f(x() / r.x(), y() / r.y(), z() / r.z(), w() / r.w());
  }
  
  Vector4f div(float r) const
  {
    return Vector4f(x() / r, y() / r, z() / r, w() / r);
  }

  Vector4f abs() const
  {
    return Vector4f(std::abs(x()), std::abs(y()), std::abs(z()), std::abs(w()));
  }
  
  bool equals(const Vector4f& r) const
  {
    return x()==r.x() && y()==r.y() && z()==r.z() && w()==r.w();
  }
  
  bool operator==(const Vector4f& r) const
  {
    return equals(r);
  }
  
  bool operator!=(const Vector4f& r) const
  {
    return !equals(r);
  }

  Vector4f lerp(const Vector4f& dest, float lerpFactor) const
  {
    return Vector4f(
      x()+(dest.x()-x())*lerpFactor,
      y()+(dest.y()-y())*lerpFactor,
      z()+(dest.z()-z())*lerpFactor,
      w()+(dest.w()-w())*lerpFactor
    );
//    return dest.sub(*this).mul(lerpFactor).add(*this);
  }
  
  std::string toString()
  {
    std::string s = "(";
    s += std::to_string(x()) + ", " + std::to_string(y()) + ", " + std::to_string(z()) + ", " + std::to_string(w()) + ")";
    return s;
  }
  
}; // class Vector4f
} // namespace al
} // namespace graphics


#endif
