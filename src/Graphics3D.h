#ifndef ANDREW_LIM_Graphics3D_H
#define ANDREW_LIM_Graphics3D_H

#include "ImageData.h"
#include "Grid.h"
#include "Vertex.h"

namespace al { namespace graphics {

class Graphics3D
{
public:

  static float deg2rad(float deg) { return deg * (M_PI / 180.0); }
  static float rad2deg(float rad) { return rad * (180.0 / M_PI); }
  inline static int clampZero(int v) { return v<0 ? 0 : v; }
  inline static int clampSize(int v, int size) { return v>=size ? size-1 : v; }
  static bool clipByScreenLeft(int& x0, int& y0, int& x1, int& y1);
  static bool clipByScreenTop(int& x0, int& y0, int& x1, int& y1);

  // Gets the pixel from an ImageData bitmap by converting UV coordinates
  // to bitmap coordinates
  static u32& pixelAtUV(ImageData& imageData, float u, float v)
  {
    // UV coordinates start from the bottom-left of a texture
    // But ImageData coordinates start from the top-left
    // https://stackoverflow.com/a/33324409/1645045
    v = 1.0f-v;

    const int imageW = (int) imageData.width();
    const int imageH = (int) imageData.height();

    int x = (u*imageW);
    int y = (v*imageH);

    // Wrap out of bounds coordinates
    x = x % imageW;
    y = y % imageH;

    // Wrap negative coordinates
    if (x<0) {
      x = imageW - std::abs(x);
    }
    if (y<0) {
      y = imageH - std::abs(y);
    }

    return imageData.pixel(x,y);
  }

    /**
   * Calculates the cross-product from 3 vertices but ignores the Z component
   * Should be used in NDC space
   *
   * See https://stackoverflow.com/a/35280392/1645045
   *
   * Also see comment by "Arnon Marcus" in this video
   * https://www.youtube.com/watch?v=h_Aqol0oTs4&lc=UgxpwWe8s2eGiRiBh054AaABAg
   */
  template <class V>
  static float crossProduct2D(const V& a, const V& b, const V& c)
  {
    const float ax = b[0] - a[0];
    const float bx = c[0] - a[0];
    const float ay = b[1] - a[1];
    const float by = c[1] - a[1];
    // [ ax bx
    //   ay by ]
    return ax * by - ay * bx;
  }
  
  static void bline(ImageData& imageData, 
                    int x0, int y0, int x1, int y1, u32 rgba=0);

  static void triangleWire(ImageData& imageData,
                           int x1, int y1, int x2, int y2, int x3, int y3, 
                           u32 rgba);
                           
  static void fillTriangle(ImageData& imageData, 
                           int x1, int y1, int x2, int y2, int x3, int y3,
                           u32 rgba);

  static inline void affineScanLine(ImageData& imageData,
                                    int y,
                                    const Vertex3f& left,
                                    const Vertex3f& right,
                                    ImageData& textureImageData,
                                    Grid<float>* zbuffer=0);

  static void affineTriangle(ImageData& imageData, 
                             float x1, float y1, float u1, float v1,
                             float x2, float y2, float u2, float v2,
                             float x3, float y3, float u3, float v3,
                             ImageData& textureImageData);

  static inline void texturedScanLine(ImageData& imageData,
                                      int y,
                                      const Vertex3f& left,
                                      const Vertex3f& right,
                                      ImageData& textureImageData,
                                      Grid<float>* zbuffer=0);

  static void texturedTriangle(ImageData& imageData, 
                               float x1, float y1, float w1, float u1, float v1,
                               float x2, float y2, float w2, float u2, float v2,
                               float x3, float y3, float w3, float u3, float v3,
                               ImageData& textureImageData,
                               Grid<float>* zbuffer=0);


};

typedef Graphics3D G3D;

}}
#endif