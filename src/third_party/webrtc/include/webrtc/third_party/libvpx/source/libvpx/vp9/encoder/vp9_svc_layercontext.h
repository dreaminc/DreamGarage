/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_ENCODER_VP9_SVC_LAYERCONTEXT_H_
#define VP9_ENCODER_VP9_SVC_LAYERCONTEXT_H_

#include "vpx/vpx_encoder.h"

#include "vp9/encoder/vp9_ratectrl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  // Inter-layer prediction is on on all frames.
  INTER_LAYER_PRED_ON,
  // Inter-layer prediction is off on all frames.
  INTER_LAYER_PRED_OFF,
  // Inter-layer prediction is off on non-key frames.
  INTER_LAYER_PRED_OFF_NONKEY,
  // Inter-layer prediction is on on all frames, but constrained such
  // that any layer S (> 0) can only predict from previous spatial
  // layer S-1, from the same superframe.
  INTER_LAYER_PRED_ON_CONSTRAINED
} INTER_LAYER_PRED;

typedef struct {
  RATE_CONTROL rc;
  int target_bandwidth;
  int spatial_layer_target_bandwidth;  // Target for the spatial layer.
  double framerate;
  int avg_frame_size;
  int max_q;
  int min_q;
  int scaling_factor_num;
  int scaling_factor_den;
  TWO_PASS twopass;
  vpx_fixed_buf_t rc_twopass_stats_in;
  unsigned int current_video_frame_in_layer;
  int is_key_frame;
  int frames_from_key_frame;
  FRAME_TYPE last_frame_type;
  struct lookahead_entry *alt_ref_source;
  int alt_ref_idx;
  int gold_ref_idx;
  int has_alt_frame;
  size_t layer_size;
  struct vpx_psnr_pkt psnr_pkt;
  // Cyclic refresh parameters (aq-mode=3), that need to be updated per-frame.
  int sb_index;
  signed char *map;
  uint8_t *last_coded_q_map;
  uint8_t *consec_zero_mv;
  uint8_t speed;
} LAYER_CONTEXT;

typedef struct SVC {
  int spatial_layer_id;
  int temporal_layer_id;
  int number_spatial_layers;
  int number_temporal_layers;

  int spatial_layer_to_encode;
  int first_spatial_layer_to_encode;

  // Workaround for multiple frame contexts
  enum { ENCODED = 0, ENCODING, NEED_TO_ENCODE } encode_empty_frame_state;
  struct lookahead_entry empty_frame;
  int encode_intra_empty_frame;

  // Store scaled source frames to be used for temporal filter to generate
  // a alt ref frame.
  YV12_BUFFER_CONFIG scaled_frames[MAX_LAG_BUFFERS];
  // Temp buffer used for 2-stage down-sampling, for real-time mode.
  YV12_BUFFER_CONFIG scaled_temp;
  int scaled_one_half;
  int scaled_temp_is_alloc;

  // Layer context used for rate control in one pass temporal CBR mode or
  // two pass spatial mode.
  LAYER_CONTEXT layer_context[VPX_MAX_LAYERS];
  // Indicates what sort of temporal layering is used.
  // Currently, this only works for CBR mode.
  VP9E_TEMPORAL_LAYERING_MODE temporal_layering_mode;
  // Frame flags and buffer indexes for each spatial layer, set by the
  // application (external settings).
  int ext_frame_flags[VPX_MAX_LAYERS];
  int lst_fb_idx[VPX_MAX_LAYERS];
  int gld_fb_idx[VPX_MAX_LAYERS];
  int alt_fb_idx[VPX_MAX_LAYERS];
  int force_zero_mode_spatial_ref;
  int current_superframe;
  int non_reference_frame;
  int use_base_mv;
  int use_partition_reuse;
  // Used to control the downscaling filter for source scaling, for 1 pass CBR.
  // downsample_filter_phase: = 0 will do sub-sampling (no weighted average),
  // = 8 will center the target pixel and get a symmetric averaging filter.
  // downsample_filter_type: 4 filters may be used: eighttap_regular,
  // eighttap_smooth, eighttap_sharp, and bilinear.
  INTERP_FILTER downsample_filter_type[VPX_SS_MAX_LAYERS];
  int downsample_filter_phase[VPX_SS_MAX_LAYERS];

  BLOCK_SIZE *prev_partition_svc;
  int mi_stride[VPX_MAX_LAYERS];

  int first_layer_denoise;

  int skip_enhancement_layer;

  int lower_layer_qindex;

  int last_layer_dropped[VPX_MAX_LAYERS];
  int drop_spatial_layer[VPX_MAX_LAYERS];
  int framedrop_thresh[VPX_MAX_LAYERS];
  SVC_LAYER_DROP_MODE framedrop_mode;

  INTER_LAYER_PRED disable_inter_layer_pred;

  // Flag to indicate scene change at current superframe, scene detection is
  // currently checked for each superframe prior to encoding, on the full
  // resolution source.
  int high_source_sad_superframe;

  // Flags used to get SVC pattern info.
  uint8_t update_last[VPX_SS_MAX_LAYERS];
  uint8_t update_golden[VPX_SS_MAX_LAYERS];
  uint8_t update_altref[VPX_SS_MAX_LAYERS];
  uint8_t reference_last[VPX_SS_MAX_LAYERS];
  uint8_t reference_golden[VPX_SS_MAX_LAYERS];
  uint8_t reference_altref[VPX_SS_MAX_LAYERS];

  // Keep track of the frame buffer index updated/refreshed on the base
  // temporal superframe.
  uint8_t fb_idx_upd_tl0[VPX_SS_MAX_LAYERS];

  // Keep track of the spatial and temporal layer id of the frame that last
  // updated the frame buffer index.
  uint8_t fb_idx_spatial_layer_id[REF_FRAMES];
  uint8_t fb_idx_temporal_layer_id[REF_FRAMES];
} SVC;

struct VP9_COMP;

// Initialize layer context data from init_config().
void vp9_init_layer_context(struct VP9_COMP *const cpi);

// Update the layer context from a change_config() call.
void vp9_update_layer_context_change_config(struct VP9_COMP *const cpi,
                                            const int target_bandwidth);

// Prior to encoding the frame, update framerate-related quantities
// for the current temporal layer.
void vp9_update_temporal_layer_framerate(struct VP9_COMP *const cpi);

// Update framerate-related quantities for the current spatial layer.
void vp9_update_spatial_layer_framerate(struct VP9_COMP *const cpi,
                                        double framerate);

// Prior to encoding the frame, set the layer context, for the current layer
// to be encoded, to the cpi struct.
void vp9_restore_layer_context(struct VP9_COMP *const cpi);

// Save the layer context after encoding the frame.
void vp9_save_layer_context(struct VP9_COMP *const cpi);

// Initialize second pass rc for spatial svc.
void vp9_init_second_pass_spatial_svc(struct VP9_COMP *cpi);

void get_layer_resolution(const int width_org, const int height_org,
                          const int num, const int den, int *width_out,
                          int *height_out);

// Increment number of video frames in layer
void vp9_inc_frame_in_layer(struct VP9_COMP *const cpi);

// Check if current layer is key frame in spatial upper layer
int vp9_is_upper_layer_key_frame(const struct VP9_COMP *const cpi);

// Get the next source buffer to encode
struct lookahead_entry *vp9_svc_lookahead_pop(struct VP9_COMP *const cpi,
                                              struct lookahead_ctx *ctx,
                                              int drain);

// Start a frame and initialize svc parameters
int vp9_svc_start_frame(struct VP9_COMP *const cpi);

void vp9_copy_flags_ref_update_idx(struct VP9_COMP *const cpi);

int vp9_one_pass_cbr_svc_start_layer(struct VP9_COMP *const cpi);

void vp9_free_svc_cyclic_refresh(struct VP9_COMP *const cpi);

void vp9_svc_reset_key_frame(struct VP9_COMP *const cpi);

void vp9_svc_check_reset_layer_rc_flag(struct VP9_COMP *const cpi);

void vp9_svc_constrain_inter_layer_pred(struct VP9_COMP *const cpi);

void vp9_svc_assert_constraints_pattern(struct VP9_COMP *const cpi);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // VP9_ENCODER_VP9_SVC_LAYERCONTEXT_
