#ifndef ANDREW_LIM_GRAPHICS_CAMERA_H
#define ANDREW_LIM_GRAPHICS_CAMERA_H
#include <cmath>

namespace al { namespace graphics {

class Camera
{
public:
  float x, y, z;
  float rot, vrot;
  float moveSpeed;
  float rotSpeed;

  Camera()
  : x(), y(), z()
  , rot(), vrot()
  , moveSpeed(), rotSpeed()
  {}

  void resetToOrigin()
  {
    x = y = z =0;
  }

  void zeroRotation()
  {
    rot = vrot = 0;
  }


  static glm::vec3 xyzToZXY(float x, float y, float z)
  {
    glm::vec3 pt;
    pt[0] = y;
    pt[1] = z;
    pt[2] = -x;
    return pt;
  }

private:

}; // class Camera
} // namespace al
} // namespace graphics

#endif
