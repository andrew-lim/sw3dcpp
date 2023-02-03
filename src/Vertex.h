#ifndef ANDREW_LIM_VERTEX_H
#define ANDREW_LIM_VERTEX_H

#include "Vector4f.h"
#include "Vector2f.h"

namespace al { namespace graphics {

class Vertex
{
private:

  Vector4f _pos;
  Vector2f _texcoords;
  
public:
  
  static Vertex xyuv(float x, float y, float u, float v)
  {
    return {
      {x, y, 0, 0},
      {u, v}
    };
  }
  
  static Vertex xywuv(float x, float y, float w, float u, float v)
  {
    return {
      {x, y, 0, w},
      {u, v}
    };
  }

  Vertex(float x, float y=0, float z=0, float w=1)
  : _pos(x, y, z, w)
  {

  }
  
  Vertex(const Vector4f& pos=Vector4f(),
         const Vector2f& texcoords=Vector2f())
  : _pos(pos)
  , _texcoords(texcoords)
  {
  }
  
  Vector4f& pos()
  {
    return _pos;
  }
  
  Vector2f& texcoords()
  {
    return _texcoords;
  }

  Vector4f getPos() const
  {
    return _pos;
  }

  Vector2f getTexCoords() const
  {
    return _texcoords;
  }

  void setTexCoords(const Vector2f& texcoords)
  {
    _texcoords = texcoords;
  }

  float get(int index) const
  {
    return _pos.get(index);
  }

  float x() const
  {
    return _pos.x();
  }

  float y() const
  {
    return _pos.y();
  }

  float z() const
  {
    return _pos.z();
  }

  float w() const
  {
    return _pos.w();
  }

  float u() const
  {
    return _texcoords.u();
  }
  
  float v() const
  {
    return _texcoords.v();
  }

  Vertex changeX(float newX)
  {
    return Vertex(
      _pos.changeX(newX),
      _texcoords
    );
  }

  Vertex lerp(const Vertex& other, float lerpFactor) const
  {
    const Vector4f& newPos = _pos.lerp(other._pos, lerpFactor);
    const Vector2f& newTex = _texcoords.lerp(other._texcoords, lerpFactor);
    return Vertex(newPos, newTex);
  }

  Vertex scaleXYZ(float f)
  {
    return Vertex(
      _pos.mul(f, f, f, 1),
      _texcoords
    );
  }

  Vertex add(const Vertex& r) const
  {
    return Vertex(
      _pos.add(r._pos),
      _texcoords.add(r._texcoords)
    );
  }

  Vertex add(float r) const
  {
    return Vertex(
      _pos.add(r),
      _texcoords.add(r)
    );
  }

  Vertex sub(const Vertex& r) const
  {
    return Vertex(
      _pos.sub(r._pos),
      _texcoords.sub(r._texcoords)
    );
  }

  Vertex sub(float r) const
  {
    return Vertex(
      _pos.sub(r),
      _texcoords.sub(r)
    );
  }

  Vertex mul(const Vertex& r) const
  {
    return Vertex(
      _pos.mul(r._pos),
      _texcoords.mul(r._texcoords)
    );
  }

  Vertex mul(float r) const
  {
    return Vertex(
      _pos.mul(r),
      _texcoords.mul(r)
    );
  }

  Vertex div(const Vertex& r) const
  {
    return Vertex(
      _pos.div(r._pos),
      _texcoords.div(r._texcoords)
    );
  }

  Vertex div(float r) const
  {
    return Vertex(
      _pos.div(r),
      _texcoords.div(r)
    );
  }

  Vertex operator+(const Vertex& r) const
  {
    return add(r);
  }

  Vertex operator+(float r) const
  {
    return add(r);
  }

  Vertex operator-(const Vertex& r) const
  {
    return sub(r);
  }

  Vertex operator-(float r) const
  {
    return sub(r);
  }

  Vertex operator/(const Vertex& r) const
  {
    return div(r);
  }

  Vertex operator/(float r) const
  {
    return div(r);
  }

  Vertex operator*(const Vertex& r) const
  {
    return mul(r);
  }

  Vertex operator*(float r) const
  {
    return mul(r);
  }
  
}; // class Vector4f
} // namespace al
} // namespace graphics


#endif
