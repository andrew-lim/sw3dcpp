#ifndef ANDREW_LIM_GRAPHICS_VECTOR2F_H
#define ANDREW_LIM_GRAPHICS_VECTOR2F_H

#include <cmath>
#include <cstdint>
#include <string>
#include <glm/vec2.hpp> // glm::vec2
namespace al { namespace graphics {

class Vector2f
{
private:
  float parts[2];

public:

  Vector2f(float x=0, float y=0)
  {
    parts[0] = x;
    parts[1] = y;
  }

  Vector2f(const glm::vec2& v)
  {
    parts[0] = v[0];
    parts[1] = v[1];
  }

  glm::vec2 vec2()
  {
    return glm::vec2(parts[0], parts[1]);
  }

  float* getArray()
  {
    return this->parts;
  }

  static Vector2f zero()
  {
    return Vector2f(0,0);
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

  float u() const
  {
    return parts[0];
  }

  float v() const
  {
    return parts[1];
  }

  Vector2f add(Vector2f r) const
  {
    return Vector2f(this->x() + r.x(), this->y() + r.y());
  }

  Vector2f add(float r) const
  {
    return Vector2f(this->x() + r, this->y() + r);
  }

  Vector2f sub(Vector2f r) const
  {
      return Vector2f(this->x() - r.x(), this->y() - r.y());
  }

  Vector2f sub(float r) const
  {
    return Vector2f(this->x() - r, this->y() - r);
  }

  Vector2f mul(Vector2f r) const
  {
      return Vector2f(this->x() * r.x(), this->y() * r.y());
  }

  Vector2f mul(float r) const
  {
    return Vector2f(this->x() * r, this->y() * r);
  }

  Vector2f mul(float scaleX, float scaleY)
  {
    return Vector2f(x() * scaleX, y() * scaleY);
  }

  Vector2f div(Vector2f r) const
  {
      return Vector2f(this->x() / r.x(), this->y() / r.y());
  }

  Vector2f div(float r) const
  {
    return Vector2f(this->x() / r, this->y() / r);
  }

  Vector2f abs() const
  {
    return Vector2f(std::abs(this->x()), std::abs(this->y()));
  }

  bool equals(const Vector2f& r) const
  {
    return this->x()==r.x() && this->y()==r.y();
  }

  bool operator==(const Vector2f& r) const
  {
    return this->equals(r);
  }

  bool operator!=(const Vector2f& r) const
  {
    return !this->equals(r);
  }

  Vector2f lerp(const Vector2f& dest, float lerpFactor) const
  {
    return Vector2f(
      x()+(dest.x()-x())*lerpFactor,
      y()+(dest.y()-y())*lerpFactor
    );
  }

  std::string toString()
  {
    std::string s = "(";
    s += std::to_string(x()) + ", " + std::to_string(y()) +  ")";
    return s;
  }

}; // class Vector2f
} // namespace al
} // namespace graphics


#endif
