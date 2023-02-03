#ifndef ANDREW_LIM_Vertex3f3F_H
#define ANDREW_LIM_Vertex3f3F_H

#include "Vector3f.h"
#include "Vector2f.h"

namespace al { namespace graphics {

class Vertex3f
{
public:
  Vector3f pos;
  Vector2f uv;

  static Vertex3f xyuv(float x, float y, float u, float v)
  {
    return {
      {x, y, 0},
      {u, v}
    };
  }

  static Vertex3f xyzuv(float x, float y, float z, float u, float v)
  {
    return {
      {x, y, z},
      {u, v}
    };
  }

  Vertex3f(float x, float y=0, float z=0)
  : pos(x, y, z)
  {

  }

  Vertex3f(const Vector3f& pos=Vector3f(),
           const Vector2f& uv=Vector2f())
  : pos(pos)
  , uv(uv)
  {
  }

  float get(int index) const
  {
    return pos.get(index);
  }

  float x() const
  {
    return pos.x;
  }

  float y() const
  {
    return pos.y;
  }

  float z() const
  {
    return pos.z;
  }

  float u() const
  {
    return uv.u();
  }

  float v() const
  {
    return uv.v();
  }

  Vertex3f changeX(float newX)
  {
    Vector3f newPos = pos;
    newPos.x= newX;
    return Vertex3f(
      newPos,
      uv
    );
  }

  Vertex3f lerp(const Vertex3f& other, float lerpFactor) const
  {
    const Vector3f& newPos = pos.lerp(other.pos, lerpFactor);
    const Vector2f& newTex = uv.lerp(other.uv, lerpFactor);
    return Vertex3f(newPos, newTex);
  }

  Vertex3f scaleXYZ(float f)
  {
    return Vertex3f(
      pos.mul(f, f, f),
      uv
    );
  }

  Vertex3f add(const Vertex3f& r) const
  {
    return Vertex3f(
      pos.add(r.pos),
      uv.add(r.uv)
    );
  }

  Vertex3f add(float r) const
  {
    return Vertex3f(
      pos.add(r),
      uv.add(r)
    );
  }

  Vertex3f sub(const Vertex3f& r) const
  {
    return Vertex3f(
      pos.sub(r.pos),
      uv.sub(r.uv)
    );
  }

  Vertex3f sub(float r) const
  {
    return Vertex3f(
      pos.sub(r),
      uv.sub(r)
    );
  }

  Vertex3f mul(const Vertex3f& r) const
  {
    return Vertex3f(
      pos.mul(r.pos),
      uv.mul(r.uv)
    );
  }

  Vertex3f mul(float r) const
  {
    return Vertex3f(
      pos.mul(r),
      uv.mul(r)
    );
  }

  Vertex3f div(const Vertex3f& r) const
  {
    return Vertex3f(
      pos.div(r.pos),
      uv.div(r.uv)
    );
  }

  Vertex3f div(float r) const
  {
    return Vertex3f(
      pos.div(r),
      uv.div(r)
    );
  }

  Vertex3f operator+(const Vertex3f& r) const
  {
    return add(r);
  }

  Vertex3f operator+(float r) const
  {
    return add(r);
  }

  Vertex3f operator-(const Vertex3f& r) const
  {
    return sub(r);
  }

  Vertex3f operator-(float r) const
  {
    return sub(r);
  }

  Vertex3f operator/(const Vertex3f& r) const
  {
    return div(r);
  }

  Vertex3f operator/(float r) const
  {
    return div(r);
  }

  Vertex3f operator*(const Vertex3f& r) const
  {
    return mul(r);
  }

  Vertex3f operator*(float r) const
  {
    return mul(r);
  }

}; // class Vector3f
} // namespace al
} // namespace graphics


#endif
