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

  static std::vector<Triangle> clip(const Triangle& triangle,
                                    int ixyz,
                                    int planeSign);

  static std::vector<Triangle>
  clip(const std::vector<Triangle>& triangles, int ixyz, int planeSign);

  static std::vector<Triangle>
  clipByNearPlane(const std::vector<Triangle>& triangles);

  static std::vector<Triangle>
  clipByNearFarPlanes(const std::vector<Triangle>& triangles);

  static std::vector<Triangle> 
  clipByAllPlanes(const std::vector<Triangle>& triangles);

}; // class ClipSpace
} // namespace al
} // namespace graphics

#endif