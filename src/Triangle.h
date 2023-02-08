#ifndef ANDREW_LIM_GRAPHICS_TRIANGLE_H
#define ANDREW_LIM_GRAPHICS_TRIANGLE_H
#include "Vertex.h"
#include <glm/vec4.hpp>

namespace al { namespace graphics {

class Triangle
{
private:
  Vertex4f _vertices[3];
  std::uint32_t _color;
public:
  int textureID; // -1 if not set
  Triangle();
  Triangle(const Vertex4f& a, const Vertex4f& b, const Vertex4f& c);
  Triangle(float x1, float y1, float z1,
           float x2, float y2, float z2,
           float x3, float y3, float z3,
           std::uint32_t color);

  Vertex4f* getVertices()
  {
    return _vertices;
  }

  Vertex4f& vertex(int i)
  {
    return _vertices[i];
  }

  const Vertex4f& vertex(int i) const
  {
    return _vertices[i];
  }

  Vector2f& uv(int i)
  {
    return _vertices[i].uv;
  }

  std::uint32_t& color()
  {
    return _color;
  }

  float getTexU(int ptindex)
  {
    return _vertices[ptindex].uv.x;
  }

  float getTexV(int ptindex)
  {
    return _vertices[ptindex].uv.y;
  }

  float getW(int ptindex)
  {
    return _vertices[ptindex].pos.w;
  }

  void setTexUVs(float u1, float v1, float u2, float v2, float u3, float v3);

  void rotateX(float rad);
  void rotateY(float rad);

  void scale(float f);

}; // class Triangle
} // namespace al
} // namespace graphics

#endif