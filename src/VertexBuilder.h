#ifndef ANDREW_LIM_VERTEX_BUILDER_H
#define ANDREW_LIM_VERTEX_BUILDER_H

#include "Vertex.h"

namespace al { namespace graphics {

class VertexBuilder
{
private:
  float _x, _y, _z, _w, _u, _v;

public:
  
  VertexBuilder()
  : _x(), _y(), _z(), _w(), _u(), _v()
  {
  }
  
  VertexBuilder& clear()
  {
    _x = _y = _z = _w = _u = _v = 0;
    return *this; 
  }
  
  VertexBuilder& xy(float x, float y)
  {
    _x = x; 
    _y = y;
    return *this; 
  }

  VertexBuilder& uv(float u, float v)
  {
    _u = u; 
    _v = v;
    return *this; 
  }

  VertexBuilder& xyuv(float x, float y, float u, float v)
  {
    _x = x; 
    _y = y;
    _u = u; 
    _v = v;
    return *this; 
  }

  VertexBuilder& x(float v)  {_x = v; return *this; }
  VertexBuilder& y(float v)  {_y = v; return *this; }
  VertexBuilder& z(float v)  {_z = v; return *this; }
  VertexBuilder& w(float v)  {_w = v; return *this; }
  VertexBuilder& u(float v)  {_u = v; return *this; }
  VertexBuilder& v(float v)  {_v = v; return *this; }
  
  Vertex build() const
  {
    return Vertex(
      Vector4f(_x, _y, _z, _w),
      Vector2f(_u, _v)
    );
  }
  
}; // class VertexBuilder
} // namespace al
} // namespace graphics


#endif
