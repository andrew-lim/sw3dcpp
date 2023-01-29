#ifndef ANDREW_LIM_GRAPHICS2D_H
#define ANDREW_LIM_GRAPHICS2D_H

#include "ImageData.h"
#include "Grid.h"

namespace al { namespace graphics {
  
class Graphics2D
{
public:
  static float clampUV(float val);
  static u32& pixelAtUV(ImageData& imageData, float u, float v);
  
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
  
  static void affineTriangle(ImageData& imageData, 
                             int x1, int y1, float u1, float v1,
                             int x2, int y2, float u2, float v2,
                             int x3, int y3, float u3, float v3, 
                             ImageData& textureImageData);                      

  static void texturedTriangle(ImageData& imageData, 
                               float x1, float y1, float w1, float u1, float v1,
                               float x2, float y2, float w2, float u2, float v2,
                               float x3, float y3, float w3, float u3, float v3,
                               ImageData& textureImageData,
                               Grid<float>* zbuffer=0);


};

}}
#endif