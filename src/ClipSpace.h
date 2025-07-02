#ifndef ANDREW_LIM_CLIP_SPACE_H
#define ANDREW_LIM_CLIP_SPACE_H
#include "Triangle.h"
#include <vector>

namespace al { namespace graphics {

/*
Contains methods to clip vertices in clip space.
Clip space is 1 step away from NDC (normalized device coordinates).
Perspective division by w hasn't happened yet.
*/
class ClipSpace
{
public:
  /*
  Finds the interpolation amount based off 2 homogeneous points,
  1 outside the clipping plane, and 1 inside the clipping plane.
  The clipping plane is based on their w components

  Uses the formula for ratio between 2 signed distances

    d1/(d1-d2)

	Where
    d1 is the signed distance between source point and the clipping plane
    d2 is the signed distance between destination point and the clipping plane

  This formula is based on the CLIPPING notes by Kenneth I. Joy which can be
  found on this page:
  https://fabiensanglard.net/polygon_codec/

  @param src source point to lerp from
  @param dst destination point
  @param ixyz Axis part - 0 or 1 or 2 for x/y/z axis respectively
  @param planeSign Either 1 or -1
  */
  static float signedDistanceRatio(const Vertex4f& src,
                                   const Vertex4f& dst,
                                   int ixyz,
                                   int planeSign)
  {
    const float d1 = (src.get(ixyz) - src.w()*planeSign);
    const float d2 = (dst.get(ixyz) - dst.w()*planeSign);
    return d1/(d1-d2);
  }

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