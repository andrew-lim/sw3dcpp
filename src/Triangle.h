#ifndef ANDREW_LIM_GRAPHICS_TRIANGLE_H
#define ANDREW_LIM_GRAPHICS_TRIANGLE_H
#include "Vertex.h"

namespace al { namespace graphics {

class Triangle
{
public:
  Vertex4f vertices[3];
  std::uint32_t color;
  int textureID; // -1 if not set
  Triangle();
  Triangle(const Vertex4f& a, const Vertex4f& b, const Vertex4f& c);
  Triangle(float x1, float y1, float z1,
           float x2, float y2, float z2,
           float x3, float y3, float z3,
           std::uint32_t color = 0);

  Vertex4f& operator[](int i) { return vertices[i]; }
  const Vertex4f& operator[](int i) const { return vertices[i]; }

  Vertex4f& vertex(int i)
  {
    return vertices[i];
  }

  const Vertex4f& vertex(int i) const
  {
    return vertices[i];
  }

  Vector2f& uv(int i)
  {
    return vertices[i].uv;
  }

  float u(int ptindex) const
  {
    return vertices[ptindex].uv.x;
  }

  float v(int ptindex) const
  {
    return vertices[ptindex].uv.y;
  }

  float w(int ptindex) const
  {
    return vertices[ptindex].pos.w;
  }

  void setTexUVs(float u1, float v1, float u2, float v2, float u3, float v3);

  void rotateX(float rad);
  void rotateY(float rad);

  void scale(float f);

  void perspectiveDivide();


}; // class Triangle
} // namespace al
} // namespace graphics

#endif