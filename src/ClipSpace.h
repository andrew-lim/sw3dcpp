#ifndef ANDREW_LIM_CLIP_SPACE_H
#define ANDREW_LIM_CLIP_SPACE_H
#include "Triangle.h"
#include <vector>

namespace al { namespace graphics {

class ClipSpace
{
public:
  /*
  Finds the interpolation amount based off 2 homogeneous points,
  1 outside the clipping plane, and 1 inside the clipping plane.
  The clipping plane is based on their w components

  Uses the formula for ratio between 2 signed distances

    d1/(d1-d2)

  Let N be inside normal of clipping plane, then
    d1 is the signed distance between source point and N
    d2 is the signed distance between destination point and N

  Full explanation for this formula here:
  https://fabiensanglard.net/polygon_codec/clippingdocument/Clipping.pdf

  @param src source point to lerp from
  @param dst destination point
  @param ixyz Axis part - 0 or 1 or 2 for x/y/z axis respectively
  @param planeSign Either 1 or -1
  */
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