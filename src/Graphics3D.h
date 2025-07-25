#ifndef ANDREW_LIM_Graphics3D_H
#define ANDREW_LIM_Graphics3D_H

#include "ImageData.h"
#include "Vertex.h"
#include <cassert>

namespace al { namespace graphics {

class Graphics3D
{
public:

  static bool closeEnough(float a, float b)
  {
    return (fabs(a-b) < 0.000001 && fabs(b-a) < 0.000001);
  }

  static constexpr auto PI = 3.14159265358979323846;

  static float deg2rad(float deg) { return deg * (PI / 180.0); }
  static float rad2deg(float rad) { return rad * (180.0 / PI); }
  inline static int clampZero(int v) { return v<0 ? 0 : v; }
  inline static int clampSize(int v, int size) { return v>=size ? size-1 : v; }
  static bool clipLineToRect(int a[2], int b[2],
                             int xmin, int ymin, int xmax, int ymax);


  // Gets the pixel from an ImageData bitmap by converting UV coordinates
  // to bitmap coordinates
  static u32 pixelAtUV(ImageData& imageData, float u, float v)
  {
    // UV coordinates start from the bottom-left of a texture
    // But ImageData coordinates start from the top-left
    // https://stackoverflow.com/a/33324409/1645045
    v = 1.0-v;

    const int imageW = (int) imageData.width();
    const int imageH = (int) imageData.height();

    // TODO: Profile and see if using floor() would be faster than modulus
    // based on comment by tusken-raynor here:
    // https://gist.github.com/andrew-lim/f35861868f83dbae8ad2656c4fc82660
    // u = u - std::floor(u)
    // v = v - std::floor(v)
    int x = u * imageW;
    int y = v * imageH;

    // Wrap out of bounds coordinates
    x = x % imageW;
    y = y % imageH;

    // Wrap negative coordinates
    if (x<0) {
      x = imageW + x;
    }
    if (y<0) {
      y = imageH + y;
    }

    return imageData.getPixel(x,y);
  }

  static u32 lightPixel(u32 pixel, float lightStrength)
  {
      u8 rgba[4];
      memcpy(rgba, &pixel, 4);
      const u8 r = rgba[0] * lightStrength;
      const u8 g = rgba[1] * lightStrength;
      const u8 b = rgba[2] * lightStrength;
      const u8 a = rgba[3];
      return ImageData::makePixel(r, g, b, a);
  }

  static u32 colorForRGB(int r, int g, int b)
  {
    r = r % 256;
    g = g % 256;
    b = b % 256;
    return ImageData::makePixel(r, g, b, 255);
  }

  /**
   * Calculates the cross-product of 2 vectors but ignores the Z component
   * Should be used in NDC space
   *
   * The 2 vectors are identified by 3 vertices a, b and c where a is the
   * common point. First vector is (b-a) and second vector is (c-a)
   *
   * See
   * https://stackoverflow.com/a/35280392/1645045
   * https://stackoverflow.com/q/1560492/1645045
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

  /**
   * Another variant of crossProduct2D but directly takes in 2 vectors instead
   * of 3 vertices
   * Also see
   * https://stackoverflow.com/questions/243945/calculating-a-2d-vectors-cross-product
   */
  template <class V>
  static float crossProduct2D(const V& a, const V& b)
  {
    return a[0] * b[1] - a[1] * b[0];
  }

  template <class V>
  static V crossProduct(const V& a, const V& b)
  {
    const float x = a[1] * b[2] - a[2] * b[1];
    const float y = a[2] * b[0] - a[0] * b[2];
    const float z = a[0] * b[1] - a[1] * b[0];

    V out;
    out[0] = x;
    out[1] = y;
    out[2] = z;
    return out;
  }

  template <class V>
  static float dot(const V& a, const V& b)
  {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
  }

  template <class V>
  static float length(const V& a)
  {
    return sqrt(Graphics3D::dot(a,a));
  }

  template <class V>
  static V normalize(const V& in)
  {
    const float len = Graphics3D::length(in);
    V a(in);
    if (len) {
      a[0] = a[0] / len;
      a[1] = a[1] / len;
      a[2] = a[2] / len;
    }
    return a;
  }

  template <class V>
  static V triangleNormal(const V& v1, const V& v2, const V& v3)
  {
    V side1 = v2 - v1;
    V side2 = v3 - v1;
    return Graphics3D::normalize(Graphics3D::crossProduct(side1, side2));
  }

  template <class Arr>
  static Arr& perspectiveDivide(Arr& clipCoords)
  {
    const float w = clipCoords[3];
    if (w) {
      clipCoords[0] = clipCoords[0] / w;
      clipCoords[1] = clipCoords[1] / w;
      clipCoords[2] = clipCoords[2] / w;
    }
    return clipCoords;
  }

  template <class Arr>
  static Arr& ndcToWindow(Arr& ndc, float windowWidth, float windowHeight)
  {
    ndc[0] = (windowWidth*0.5*ndc[0] + windowWidth*0.5);
    ndc[1] = ((-ndc[1])*windowHeight*0.5 + windowHeight*0.5);
    return ndc;
  }

};

typedef Graphics3D G3D;

}}
#endif