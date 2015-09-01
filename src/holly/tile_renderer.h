#ifndef TILE_RENDERER_H
#define TILE_RENDERER_H

#include <functional>
#include <Eigen/Dense>
#include "renderer/backend.h"

namespace dreavm {
namespace holly {

union TSP;
union TCW;
union PolyParam;
union VertexParam;
struct TileContext;

// The TextureCache interface provides an abstraction so the TileAccelerator /
// TraceViewer can provide raw texture and palette data on demand to the
// TileRenderer. While a static GetTextureKey is provided, each implementation
// is expected to manage their own cache internally.
typedef std::function<renderer::TextureHandle(const uint8_t *, const uint8_t *)>
    RegisterTextureCallback;

class TextureCache {
 public:
  static uint32_t GetTextureKey(const TSP &tsp, const TCW &tcw);

  virtual renderer::TextureHandle GetTexture(
      const TSP &tsp, const TCW &tcw, RegisterTextureCallback register_cb) = 0;
};

// The TileRenderer class is responsible for taking a particular TileContext,
// parsing it and ultimately rendering it out to the supplied backend. This
// is split out of the main TileAccelerator code so it can be re-used by
// TraceViewer.
enum { MAX_SURFACES = 0x10000, MAX_VERTICES = 0x10000 };

class TileRenderer {
 public:
  TileRenderer(TextureCache &texcache_);

  void RenderContext(const TileContext *tactx, renderer::Backend *rb);

 private:
  void Reset();

  renderer::Surface *AllocSurf();
  renderer::Vertex *AllocVert();
  void ParseColor(uint32_t base_color, uint32_t *color);
  void ParseColor(float r, float g, float b, float a, uint32_t *color);
  void ParseColor(float intensity, uint32_t *color);
  void ParseOffsetColor(uint32_t offset_color, uint32_t *color);
  void ParseOffsetColor(float r, float g, float b, float a, uint32_t *color);
  void ParseOffsetColor(float intensity, uint32_t *color);
  void ParseBackground(const TileContext *tactx);
  void ParsePolyParam(const TileContext *tactx, renderer::Backend *rb,
                      const PolyParam *param);
  void ParseVertexParam(const TileContext *tactx, renderer::Backend *rb,
                        const VertexParam *param);
  void ParseEndOfList(const TileContext *tactx);
  Eigen::Matrix4f GetProjectionMatrix();

  renderer::TextureHandle RegisterTexture(const TileContext *tactx,
                                          renderer::Backend *rb, const TSP &tsp,
                                          const TCW &tcw,
                                          const uint8_t *texture,
                                          const uint8_t *palette);
  renderer::TextureHandle GetTexture(const TileContext *tactx,
                                     renderer::Backend *rb, const TSP &tsp,
                                     const TCW &tcw);

  TextureCache &texcache_;

  // current global state
  const PolyParam *last_poly_;
  const VertexParam *last_vertex_;
  int list_type_;
  int vertex_type_;
  float face_color_[4];
  float face_offset_color_[4];

  // current render state
  renderer::Surface surfs_[MAX_SURFACES];
  renderer::Vertex verts_[MAX_VERTICES];
  int width_, height_;
  int num_surfs_;
  int num_verts_;
  int sorted_surfs_[MAX_SURFACES];
  int last_sorted_surf_;
};
}
}

#endif
