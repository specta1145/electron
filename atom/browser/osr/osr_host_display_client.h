// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_OSR_OSR_HOST_DISPLAY_CLIENT_H_
#define ATOM_BROWSER_OSR_OSR_HOST_DISPLAY_CLIENT_H_

#include <memory>

#include "base/callback.h"
#include "components/viz/host/host_display_client.h"
#include "services/viz/privileged/interfaces/compositing/layered_window_updater.mojom.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/native_widget_types.h"

namespace atom {

typedef base::Callback<void(const gfx::Rect&, const SkBitmap&)> OnPaintCallback;

class LayeredWindowUpdater : public viz::mojom::LayeredWindowUpdater {
 public:
  explicit LayeredWindowUpdater(viz::mojom::LayeredWindowUpdaterRequest request,
                                OnPaintCallback callback);
  ~LayeredWindowUpdater() override;

  // viz::mojom::LayeredWindowUpdater implementation.
  void OnAllocatedSharedMemory(
      const gfx::Size& pixel_size,
      mojo::ScopedSharedBufferHandle scoped_buffer_handle) override;
  void Draw(const gfx::Rect& damage_rect, DrawCallback draw_callback) override;

 private:
  OnPaintCallback callback_;
  mojo::Binding<viz::mojom::LayeredWindowUpdater> binding_;
  std::unique_ptr<SkCanvas> canvas_;

  DISALLOW_COPY_AND_ASSIGN(LayeredWindowUpdater);
};

class OffScreenHostDisplayClient : public viz::HostDisplayClient {
 public:
  explicit OffScreenHostDisplayClient(gfx::AcceleratedWidget widget,
                                      OnPaintCallback callback);
  ~OffScreenHostDisplayClient() override;

 private:
  void IsOffscreen(IsOffscreenCallback callback) override;

  OnPaintCallback callback_;

#if defined(OS_WIN)
  void CreateLayeredWindowUpdater(
      viz::mojom::LayeredWindowUpdaterRequest request) override;

  std::unique_ptr<LayeredWindowUpdater> layered_window_updater_;
#endif

  DISALLOW_COPY_AND_ASSIGN(OffScreenHostDisplayClient);
};

}  // namespace atom

#endif  // ATOM_BROWSER_OSR_OSR_HOST_DISPLAY_CLIENT_H_