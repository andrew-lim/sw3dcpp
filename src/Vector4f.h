#ifndef ANDREW_LIM_VECTORS
#define ANDREW_LIM_VECTORS

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
  
  glm::vec4 vec4()
  {
    return glm::vec4(parts[0], parts[1], parts[2], parts[3]);
  }
  
  float* getArray()
  {
    return this->parts;
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
    return Vector4f(this->x() + r.x(), this->y() + r.y(), this->z() + r.z(), this->w() + r.w());
  }
  
  Vector4f add(float r) const
  {
    return Vector4f(this->x() + r, this->y() + r, this->z() + r, this->w() + r);
  }

  Vector4f sub(Vector4f r) const
  {
      return Vector4f(this->x() - r.x(), this->y() - r.y(), this->z() - r.z(), this->w() - r.w());
  }
  
  Vector4f sub(float r) const
  {
    return Vector4f(this->x() - r, this->y() - r, this->z() - r, this->w() - r);
  }

  Vector4f mul(Vector4f r) const
  {
      return Vector4f(this->x() * r.x(), this->y() * r.y(), this->z() * r.z(), this->w() * r.w());
  }
  
  Vector4f mul(float r) const
  {
    return Vector4f(this->x() * r, this->y() * r, this->z() * r, this->w() * r);
  }

  Vector4f div(Vector4f r) const
  {
      return Vector4f(this->x() / r.x(), this->y() / r.y(), this->z() / r.z(), this->w() / r.w());
  }
  
  Vector4f div(float r) const
  {
    return Vector4f(this->x() / r, this->y() / r, this->z() / r, this->w() / r);
  }

  Vector4f abs() const
  {
    return Vector4f(std::abs(this->x()), std::abs(this->y()), std::abs(this->z()), std::abs(this->w()));
  }
  
  bool equals(const Vector4f& r) const
  {
    return this->x()==r.x() && this->y()==r.y() && this->z()==r.z() && this->w()==r.w();
  }
  
  bool operator==(const Vector4f& r) const
  {
    return this->equals(r);
  }
  
  bool operator!=(const Vector4f& r) const
  {
    return !this->equals(r);
  }

  Vector4f lerp(const Vector4f& dest, float lerpFactor) const
  {
    return dest.sub(*this).mul(lerpFactor).add(*this);
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
