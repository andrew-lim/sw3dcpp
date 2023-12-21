#ifndef ANDREW_LIM_GRAPHICS_TRIANGLE_DRAWER_H
#define ANDREW_LIM_GRAPHICS_TRIANGLE_DRAWER_H
#include "Triangle.h"
#include "ImageData.h"
#include "Graphics3D.h"
#include "Grid.h"
#include "Vertex.h"
#include "Vector3f.h"

typedef Grid<double> ZBuffer;

namespace al { namespace graphics {

class TriangleDrawer
{
public:
  enum DrawMode {
    DRAW_PERSPECTIVE, DRAW_SOLID, DRAW_WIREFRAME, DRAW_AFFINE,
    DRAW_RGB_SHADED
  };

  enum Algo {
    SCANLINE, BARYCENTRIC, BARYCENTRIC_O1, BARYCENTRIC_O2
  };

  enum LightsStyle {
    LIGHTS_STYLE_NONE, LIGHTS_STYLE_FLAT, LIGHTS_STYLE_SMOOTH
  };
  Algo algo = SCANLINE;
  DrawMode drawMode;
  LightsStyle lightsStyle;
  Vector3f lightDirection;
  ZBuffer* depthBuffer;
  std::uint32_t color;
  ImageData* textureImageData;

  TriangleDrawer(ZBuffer* depthBuffer = 0)
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

  // Delegates drawing to the other triangle drawing methods
  void triangle(ImageData& imageData, const Triangle& triangle);

  // Standard scanline approach that uses slope and top/bottom halves
  void scanlineTriangle(ImageData& imageData, const Triangle& triangle);

  // Simple unoptimized barycentric approach
  // Checks every pixel in the bounding box
  void barycentricTriangle(ImageData& imageData, const Triangle& triangle);

  // Optimized barycentric approach that uses interpolates using additions
  // Based on the following links:
  // https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/
  // https://github.com/gustavopezzi/triangle-rasterizer-float
  void barycentricO1(ImageData& imageData, const Triangle& triangle);

  // Similar to barycentricO1 but also reduces checks on pixels that lie on the
  // outside of the triangle.
  // First the triangle is broken into 2 smaller triangles - a flat-bottom and
  // flat-top. Then for each triangle 2 main things happen:
  // 1. While scanning a row horizontally starting from the left most vertex,
  //    the x-coordinate of the first pixel found inside the triangle becomes
  //    the starting x for checking the next row.
  // 2. While inside the triangle, stop scanning the current row once the first
  //    pixel outside the triangle is found.
  void barycentricO2(ImageData& imageData, const Triangle& triangle);

private:


    // Used by scanlineTriangle
  void scanline(ImageData& imageData,
                const Triangle& triangle,
                const Vertex4f& left,
                const Vertex4f& right,
                int y);

  // Used by all the barycentric drawing methods
  void barycentricPixel(ImageData& imageData,
                        int x, int y,
                        const Triangle& t,
                        float alp, float bet, float gam,
                        double w);

  // Used by barycentricO2
  int barycentricScanline(ImageData& imageData,
                          const Triangle& triangle,
                          int leftx, int rightx,
                          int y,
                          float area,
                          const float deltacols[3]);
}; // class TriangleDrawer
} // namespace al
} // namespace graphics

#endif