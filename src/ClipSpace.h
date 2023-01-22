#ifndef ANDREW_LIM_CLIP_SPACE_H
#define ANDREW_LIM_CLIP_SPACE_H
#include "Triangle.h"
#include <vector>

namespace al { namespace graphics {

class ClipSpace
{
public:
  static float findLerpFactor(const Vertex& src,
                              const Vertex& dst,
                              int ixyz,
                              int planeSign);

  static std::vector<Triangle> clipTriangle(Triangle triangle,
                                            int ixyz,
                                            int planeSign);
                                            
  static std::vector<Triangle> 
  clipTrianglesByPlane(std::vector<Triangle> triangles,
                       int ixyz, 
                       int planeSign);

  static std::vector<Triangle> 
  clipTrianglesByAllPlanes(std::vector<Triangle> triangles);

}; // class ClipSpace
} // namespace al
} // namespace graphics

#endif