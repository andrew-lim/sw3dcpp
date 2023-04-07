#ifndef ANDREW_LIM_GRAPHICS_TRIANGLE_DRAWER_H
#define ANDREW_LIM_GRAPHICS_TRIANGLE_DRAWER_H
#include "Triangle.h"
#include "ImageData.h"
#include "Grid.h"
#include "Vertex.h"

namespace al { namespace graphics {

class TriangleDrawer
{
public:
  enum DrawMode {
    DRAW_PERSPECTIVE, DRAW_SOLID, DRAW_WIREFRAME, DRAW_AFFINE,
    DRAW_RGB_SHADED
  };

  DrawMode drawMode;
  Grid<float>* depthBuffer;
  std::uint32_t color;
  ImageData* textureImageData;

  TriangleDrawer(Grid<float>* depthBuffer = 0)
  : drawMode(TriangleDrawer::DRAW_SOLID)
  , depthBuffer(depthBuffer)
  , color(0)
  , textureImageData(0)
  {
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