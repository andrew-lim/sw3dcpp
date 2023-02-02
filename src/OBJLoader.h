#ifndef ANDREW_LIM_GRAPHICS_OBJ_LOADER_H
#define ANDREW_LIM_GRAPHICS_OBJ_LOADER_H
#include <string>
#include <vector>
#include <map>
#include "Triangle.h"
#include "ImageData.H"

namespace al { namespace graphics {

class OBJLoader
{
public:
  OBJLoader(bool debug=false)
  : debug(debug)
  {
  }

  bool debug;
  std::vector<Triangle> triangles;
  std::map<int,ImageData> textures;

  void d(const char *fmt, ...);

  void load(const std::string& path, float scale=1.0f);

  static std::string folder(const std::string& path);

  void loadPNG(const std::string& path, ImageData& imageData);
  void loadBMP(const std::string& path, ImageData& imageData);

}; // class OBJLoader
} // namespace al
} // namespace graphics

#endif