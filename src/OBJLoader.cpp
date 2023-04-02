#include "OBJLoader.h"
#include "tiny_obj_loader.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdarg>
#include "OBJLoader.h"
#include "ImageData.h"
#include "Bitmap.h"
#include "lodepng.h"

using namespace al::graphics;
using namespace std;

void OBJLoader::d(const char *fmt, ...)
{
  if (debug) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
}

// https://stackoverflow.com/a/3071694/1645045
std::string OBJLoader::folder(const std::string& path)
{
  size_t found;
  found=path.find_last_of("/\\");
  return path.substr(0,found);
}

void OBJLoader::loadPNG(const std::string& path, ImageData& imageData)
{
  unsigned width=0, height=0;
  unsigned error;
  uint8_t* image = 0;
  error = lodepng_decode32_file(&image, &width, &height, path.c_str());
  if (error) {
    string msg = "lodepng_decode32_file failed to load file: ";
    msg += path;
    msg += " Error: ";
    msg += lodepng_error_text(error);
    throw runtime_error(msg);
  }
  if (image && width && height) {
    imageData.copy(image, width, height);
    imageData.swapEndianess();
    d("loadPNG success for %s\n", path.c_str());
  }
  if (image) {
    free(image);
  }
  else {
    throw runtime_error("Error loading image");
  }
}

void OBJLoader::loadBMP(const std::string& path, ImageData& imageData)
{
  Bitmap bmp;
  if (!bmp.loadFile(path.c_str())) {
    string error = "Error loading " + path;
    throw std::runtime_error(error.c_str());
  }

  uint8_t* imgbytes = new uint8_t[bmp.getWidth() * bmp.getHeight() * 4];
  uint32_t result = bmp.getBits(imgbytes);
  if (result) {
    imageData.copy(imgbytes, bmp.getWidth(), bmp.getHeight());
  }
  delete[] imgbytes;
  if (!result) {
    string error = "Error loading bitmap " + path;
    throw std::runtime_error(error.c_str());
  }
}


void OBJLoader::load(const std::string& path, float scale)
{
  triangles.clear();
  textures.clear();

  tinyobj::ObjReaderConfig reader_config;
  string materialFolder = folder(path);
  d("material folder=%s\n", materialFolder.c_str());
  reader_config.mtl_search_path = materialFolder; // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path, reader_config)) {
    if (!reader.Error().empty()) {
        std::string msg = "OBJLoader::load failed() ";
        msg += reader.Error();
        throw std::runtime_error(msg);
    }
  }

  if (!reader.Warning().empty()) {
    std::string msg = "OBJLoader::load warning:\n";
    msg += reader.Warning();
    d(msg.c_str());
  }

  auto& attrib = reader.GetAttrib();
  auto& shapes = reader.GetShapes();
  auto& materials = reader.GetMaterials();

  d("materials.size()=%d\n", materials.size());
  for (size_t i=0; i<materials.size(); i++) {
    const tinyobj::material_t& material = materials[i];
    if (material.diffuse_texname.size()) {
      ImageData textureData;
      string texturePath = materialFolder + string("\\") + material.diffuse_texname;
      d("material %d\ttexturePath=%s\n", (int)i, texturePath.c_str());

      bool isBMP = path.find(".bmp") != std::string::npos ||
                   path.find(".BMP") != std::string::npos;

      if (isBMP) {
        loadBMP(texturePath, textureData);
      }
      else {
        loadPNG(texturePath, textureData);
      }

      int key = (int)i;
      if (0==textures.count(key)) {
        textures[key] = textureData;
      }
    }
  }

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

      // Loop over faces(polygon)
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
      if (fv != 3) {
        string msg = "Only faces with exactly 3 vertices are supported. ";
        msg += "Found a face at index offset: " + std::to_string(index_offset);
        throw std::runtime_error(msg);
      }

      float u1=0, v1=0;
      float u2=0, v2=0;
      float u3=0, v3=0;

      tinyobj::index_t idx = shapes[s].mesh.indices[index_offset++];
      tinyobj::real_t x1 = attrib.vertices[3*size_t(idx.vertex_index)+0];
      tinyobj::real_t y1 = attrib.vertices[3*size_t(idx.vertex_index)+1];
      tinyobj::real_t z1 = attrib.vertices[3*size_t(idx.vertex_index)+2];
      if (idx.texcoord_index >= 0) {
        u1 = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
        v1 = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
      }

      idx = shapes[s].mesh.indices[index_offset++];
      tinyobj::real_t x2 = attrib.vertices[3*size_t(idx.vertex_index)+0];
      tinyobj::real_t y2 = attrib.vertices[3*size_t(idx.vertex_index)+1];
      tinyobj::real_t z2 = attrib.vertices[3*size_t(idx.vertex_index)+2];
      if (idx.texcoord_index >= 0) {
        u2 = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
        v2 = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
      }

      idx = shapes[s].mesh.indices[index_offset++];
      tinyobj::real_t x3 = attrib.vertices[3*size_t(idx.vertex_index)+0];
      tinyobj::real_t y3 = attrib.vertices[3*size_t(idx.vertex_index)+1];
      tinyobj::real_t z3 = attrib.vertices[3*size_t(idx.vertex_index)+2];
      if (idx.texcoord_index >= 0) {
        u3 = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
        v3 = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
      }

      Triangle t(
        x1*scale, y1*scale, z1*scale,
        x2*scale, y2*scale, z2*scale,
        x3*scale, y3*scale, z3*scale
      );
      t.setTexUVs(u1, v1, u2, v2, u3, v3);

      // per-face material
      const std::vector<int>& material_ids = shapes[s].mesh.material_ids;
      int material_id = (int)material_ids[f];
      if (material_id != -1) {
          t.textureID = material_id;
      }

      triangles.push_back(t);

    } // face loop


  } // shape loop
}