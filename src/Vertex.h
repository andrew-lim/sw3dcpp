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
    return Vertex(
      Vector4f(x, y, 0, 0),
      Vector2f(u, v)
    );
  }
  
  static Vertex xywuv(float x, float y, float w, float u, float v)
  {
    return Vertex(
      Vector4f(x, y, 0, w),
      Vector2f(u, v)
    );
  }
  
  Vertex(float x, float y=0, float z=0, float w=1)
  : _pos(x, y, z, w)
  {
    
  }
  
  Vertex(const Vector4f& pos=Vector4f::zero(), 
         const Vector2f& texcoords=Vector2f::zero())
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

  bool isInsideViewFrustum() const
  {
    return isInsideXPlanes() && isInsideYPlanes() && isInsideZPlanes();
  }

  bool isInsideXPlanes() const
  {
    return fabs(_pos.x()) <= fabs(_pos.w());
  }

  bool isInsideYPlanes() const
  {
    return fabs(_pos.y()) <= fabs(_pos.w());
  }

  bool isInsideZPlanes() const
  {
    return fabs(_pos.z()) <= fabs(_pos.w());
  }

  Vertex lerp(const Vertex& other, float lerpFactor) const
  {
    const Vector4f& newPos = _pos.lerp(other._pos, lerpFactor);
    const Vector2f& newTex = _texcoords.lerp(other._texcoords, lerpFactor);
    return Vertex(newPos, newTex);
  }
  
}; // class Vector4f
} // namespace al
} // namespace graphics


#endif
