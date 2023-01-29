#ifndef ANDREW_LIM_GRAPHICS_TRIANGLE_H
#define ANDREW_LIM_GRAPHICS_TRIANGLE_H
#include "Vertex.h"
#include <glm/vec4.hpp>

namespace al { namespace graphics {

class Triangle
{
private:
  Vertex _vertices[3];
  std::uint32_t _color;
public:
  int textureID; // -1 if not set
  Triangle();
  Triangle(const Vertex& a, const Vertex& b, const Vertex& c);
  Triangle(float x1, float y1, float z1, 
           float x2, float y2, float z2, 
           float x3, float y3, float z3, 
           std::uint32_t color);
  
  Vertex* getVertices()
  {
    return _vertices;  
  }

  Vertex& vertex(int i)
  {
    return _vertices[i];
  }

  const Vertex& vertex(int i) const
  {
    return _vertices[i];
  }

  Vector2f& texcoord(int i)
  {
    return _vertices[i].texcoords();
  }

  std::uint32_t& color()
  {
    return _color;
  }
  
  float getTexU(int ptindex)
  {
    return _vertices[ptindex].getTexCoords().u();
  }

  float getTexV(int ptindex)
  {
    return _vertices[ptindex].getTexCoords().v();
  }
  
  float getW(int ptindex)
  {
    return _vertices[ptindex].pos().w();
  }
  
  void setTexUVs(float u1, float v1, float u2, float v2, float u3, float v3);
  
  void rotateX(float rad);
  void rotateY(float rad);

  void scale(float f);

}; // class Triangle
} // namespace al
} // namespace graphics
  
#endif