// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NGInlineItemResult_h
#define NGInlineItemResult_h

#include "third_party/blink/renderer/core/layout/ng/geometry/ng_box_strut.h"
#include "third_party/blink/renderer/core/layout/ng/inline/ng_physical_text_fragment.h"
#include "third_party/blink/renderer/core/layout/ng/inline/ng_text_end_effect.h"
#include "third_party/blink/renderer/core/layout/ng/ng_layout_result.h"
#include "third_party/blink/renderer/platform/fonts/shaping/shape_result.h"
#include "third_party/blink/renderer/platform/layout_unit.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class NGConstraintSpace;
class NGInlineItem;
class NGInlineNode;

struct NGInlineItemsData;

// The result of measuring NGInlineItem.
//
// This is a transient context object only while building line boxes.
// Produced while determining the line break points, but these data are needed
// to create line boxes.
//
// NGLineBreaker produces, and NGInlineLayoutAlgorithm consumes.
struct CORE_EXPORT NGInlineItemResult {
  DISALLOW_NEW_EXCEPT_PLACEMENT_NEW();

 public:
  // The NGInlineItem and its index.
  const NGInlineItem* item;
  unsigned item_index;

  // The range of text content for this item.
  unsigned start_offset;
  unsigned end_offset;

  // Inline size of this item.
  LayoutUnit inline_size;

  // ShapeResult for text items. Maybe different from NGInlineItem if re-shape
  // is needed in the line breaker.
  scoped_refptr<const ShapeResult> shape_result;

  // NGLayoutResult for atomic inline items.
  scoped_refptr<NGLayoutResult> layout_result;

  // Margins and padding for atomic inline items and open/close tags.
  NGLineBoxStrut margins;
  NGLineBoxStrut padding;

  // Borders/padding for open tags.
  LayoutUnit borders_paddings_line_over;
  LayoutUnit borders_paddings_line_under;

  // Has start/end edge for open/close tags.
  bool has_edge = false;

  // Inside of this may be breakable. False means there are no break
  // opportunities, or has CSS properties that prohibit breaking.
  // Used only during line breaking.
  bool may_break_inside = false;

  // Lines can break after this item. Set for all items.
  // Used only during line breaking.
  bool can_break_after = false;

  // True if this item contains only trailing spaces.
  // Trailing spaces are measured differently that they are split from other
  // text items.
  // Used only when 'white-space: pre-wrap', because collapsible spaces are
  // removed, and if 'pre', trailing spaces are not different from other
  // characters.
  bool has_only_trailing_spaces = false;

  // End effects for text items.
  // The effects are included in |shape_result|, but not in text content.
  NGTextEndEffect text_end_effect = NGTextEndEffect::kNone;

  NGInlineItemResult();
  NGInlineItemResult(const NGInlineItem*,
                     unsigned index,
                     unsigned start,
                     unsigned end);

#if DCHECK_IS_ON()
  void CheckConsistency(bool during_line_break = false) const;
#endif
};

// Represents a set of NGInlineItemResult that form a line box.
using NGInlineItemResults = Vector<NGInlineItemResult, 32>;

// Represents a line to build.
//
// This is a transient context object only while building line boxes.
//
// NGLineBreaker produces, and NGInlineLayoutAlgorithm consumes.
class CORE_EXPORT NGLineInfo {
  DISALLOW_NEW_EXCEPT_PLACEMENT_NEW();

 public:
  const NGInlineItemsData& ItemsData() const {
    DCHECK(items_data_);
    return *items_data_;
  }

  // The style to use for the line.
  const ComputedStyle& LineStyle() const {
    DCHECK(line_style_);
    return *line_style_;
  }
  void SetLineStyle(const NGInlineNode&,
                    const NGInlineItemsData&,
                    const NGConstraintSpace&,
                    bool is_first_formatted_line,
                    bool use_first_line_style,
                    bool is_after_forced_break);

  // Use ::first-line style if true.
  // https://drafts.csswg.org/css-pseudo/#selectordef-first-line
  // This is false for the "first formatted line" if '::first-line' rule is not
  // used in the document.
  // https://www.w3.org/TR/CSS22/selector.html#first-formatted-line
  bool UseFirstLineStyle() const { return use_first_line_style_; }

  // The last line of a block, or the line ends with a forced line break.
  // https://drafts.csswg.org/css-text-3/#propdef-text-align-last
  bool IsLastLine() const { return is_last_line_; }
  void SetIsLastLine(bool is_last_line) { is_last_line_ = is_last_line; }

  // If the line is marked as empty, it means that there's no content that
  // requires it to be present at all, e.g. when there are only close tags with
  // no margin/border/padding.
  bool IsEmptyLine() const { return is_empty_line_; }
  void SetIsEmptyLine() { is_empty_line_ = true; }

  // NGInlineItemResults for this line.
  NGInlineItemResults& Results() { return results_; }
  const NGInlineItemResults& Results() const { return results_; }

  LayoutUnit TextIndent() const { return text_indent_; }

  NGBfcOffset LineBfcOffset() const { return line_bfc_offset_; }
  LayoutUnit AvailableWidth() const { return available_width_; }
  LayoutUnit Width() const { return width_; }
  LayoutUnit ComputeWidth() const;
  void SetLineBfcOffset(NGBfcOffset line_bfc_offset,
                        LayoutUnit available_width,
                        LayoutUnit width);

  // Start text offset of this line.
  unsigned StartOffset() const { return start_offset_; }
  void SetStartOffset(unsigned offset) { start_offset_ = offset; }

  // The base direction of this line for the bidi algorithm.
  TextDirection BaseDirection() const { return base_direction_; }
  void SetBaseDirection(TextDirection direction) {
    base_direction_ = direction;
  }

  // Fragment to append to the line end. Used by 'text-overflow: ellipsis'.
  scoped_refptr<NGPhysicalTextFragment>& LineEndFragment() {
    return line_end_fragment_;
  }
  void SetLineEndFragment(scoped_refptr<NGPhysicalTextFragment>);

 private:
  const NGInlineItemsData* items_data_ = nullptr;
  const ComputedStyle* line_style_ = nullptr;
  NGInlineItemResults results_;
  scoped_refptr<NGPhysicalTextFragment> line_end_fragment_;

  NGBfcOffset line_bfc_offset_;
  LayoutUnit available_width_;
  LayoutUnit width_;
  LayoutUnit text_indent_;

  unsigned start_offset_;

  TextDirection base_direction_ = TextDirection::kLtr;

  bool use_first_line_style_ = false;
  bool is_last_line_ = false;
  bool is_empty_line_ = false;
};

}  // namespace blink

#endif  // NGInlineItemResult_h
