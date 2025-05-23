// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_SURFACE_LAYER_BRIDGE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_SURFACE_LAYER_BRIDGE_H_

#include <memory>

#include "base/memory/scoped_refptr.h"
#include "components/viz/common/surfaces/parent_local_surface_id_allocator.h"
#include "components/viz/common/surfaces/surface_id.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "third_party/blink/public/platform/modules/frame_sinks/embedded_frame_sink.mojom-blink.h"
#include "third_party/blink/public/platform/web_surface_layer_bridge.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace cc {
class SolidColorLayer;
class SurfaceLayer;
}  // namespace cc

namespace viz {
class SurfaceInfo;
}  // namespace viz

namespace blink {

class WebLayerTreeView;

// The SurfaceLayerBridge facilitates communication about changes to a Surface
// between the Render and Browser processes.
class PLATFORM_EXPORT SurfaceLayerBridge
    : public blink::mojom::blink::EmbeddedFrameSinkClient,
      public WebSurfaceLayerBridge {
 public:
  SurfaceLayerBridge(WebLayerTreeView*, WebSurfaceLayerBridgeObserver*);
  ~SurfaceLayerBridge() override;

  void CreateSolidColorLayer();

  // Implementation of blink::mojom::blink::EmbeddedFrameSinkClient
  void OnFirstSurfaceActivation(const viz::SurfaceInfo&) override;

  // Implementation of WebSurfaceLayerBridge.
  void CreateSurfaceLayer() override;
  cc::Layer* GetCcLayer() const override;
  void ClearSurfaceId() override;
  void SetContentsOpaque(bool) override;

  const viz::FrameSinkId& GetFrameSinkId() const override {
    return frame_sink_id_;
  }

 private:
  scoped_refptr<cc::SurfaceLayer> surface_layer_;
  scoped_refptr<cc::SolidColorLayer> solid_color_layer_;

  // The |observer_| handles unregistering the contents layer on its own.
  WebSurfaceLayerBridgeObserver* observer_;
  viz::ParentLocalSurfaceIdAllocator parent_local_surface_id_allocator_;
  mojo::Binding<blink::mojom::blink::EmbeddedFrameSinkClient> binding_;

  const viz::FrameSinkId frame_sink_id_;
  viz::SurfaceId current_surface_id_;
  const viz::FrameSinkId parent_frame_sink_id_;
  bool opaque_ = false;
  bool surface_activated_ = false;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_SURFACE_LAYER_BRIDGE_H_
