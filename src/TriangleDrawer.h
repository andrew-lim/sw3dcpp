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
    SCANLINE, BARYCENTRIC, BARYCENTRIC_ADD
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

  // Triangle drawing methods

  // Standard scanline approach that uses slope and top/bottom halves
  void scanlineTriangle(ImageData& imageData, const Triangle& triangle);

  // Simple unoptimized barycentric approach
  void barycentricTriangle(ImageData& imageData, const Triangle& triangle);

  // Optimized barycentric approach that uses interpolates using additions
  void barycentricAddTriangle(ImageData& imageData, const Triangle& triangle);

private:


    // Used by scanlineTriangle
  void scanline(ImageData& imageData,
                const Triangle& triangle,
                const Vertex4f& left,
                const Vertex4f& right,
                int y);

  void barycentricPixel(ImageData& imageData,
                        int x, int y,
                        const Triangle& t,
                        float alp, float bet, float gam,
                        double w);

}; // class TriangleDrawer
} // namespace al
} // namespace graphics

#endif