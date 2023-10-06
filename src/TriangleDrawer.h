#ifndef ANDREW_LIM_GRAPHICS_TRIANGLE_DRAWER_H
#define ANDREW_LIM_GRAPHICS_TRIANGLE_DRAWER_H
#include "Triangle.h"
#include "ImageData.h"
#include "Graphics3D.h"
#include "Grid.h"
#include "Vertex.h"
#include "Vector3f.h"

namespace al { namespace graphics {

class TriangleDrawer
{
public:
  enum DrawMode {
    DRAW_PERSPECTIVE, DRAW_SOLID, DRAW_WIREFRAME, DRAW_AFFINE,
    DRAW_RGB_SHADED
  };

  enum LightsStyle {
    LIGHTS_STYLE_NONE, LIGHTS_STYLE_FLAT, LIGHTS_STYLE_SMOOTH
  };

  DrawMode drawMode;
  LightsStyle lightsStyle;
  Vector3f lightDirection;
  Grid<float>* depthBuffer;
  std::uint32_t color;
  ImageData* textureImageData;

  TriangleDrawer(Grid<float>* depthBuffer = 0)
  : drawMode(TriangleDrawer::DRAW_SOLID)
  , lightsStyle(TriangleDrawer::LIGHTS_STYLE_SMOOTH)
  , lightDirection(1.0f, 0.0f, 1.0f)
  , depthBuffer(depthBuffer)
  , color(0)
  , textureImageData(0)
  {
    lightDirection = G3D::normalize(lightDirection);
  }

  void wireframeTriangle(ImageData& imageData,
                         const Triangle& triangle,
                         u32 rgba);

  void scanline(ImageData& imageData,
                const Triangle& triangle,
                const Vertex4f& left,
                const Vertex4f& right);

  void triangle(ImageData& imageData, const Triangle& triangle);

}; // class TriangleDrawer
} // namespace al
} // namespace graphics

#endif