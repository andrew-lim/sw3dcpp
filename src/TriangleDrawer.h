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
  static const int DRAW_SOLID = 0;
  static const int DRAW_WIREFRAME = 1;
  static const int DRAW_AFFINE = 2;
  static const int DRAW_PERSPECTIVE = 3;

  int drawMode;
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
                int y,
                const Vertex4f& left,
                const Vertex4f& right);

  void triangle(ImageData& imageData, const Triangle& triangle);

}; // class TriangleDrawer
} // namespace al
} // namespace graphics

#endif