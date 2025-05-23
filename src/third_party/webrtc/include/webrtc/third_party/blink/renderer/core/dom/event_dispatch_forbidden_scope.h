// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_EVENT_DISPATCH_FORBIDDEN_SCOPE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_EVENT_DISPATCH_FORBIDDEN_SCOPE_H_

#include "base/auto_reset.h"
#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"
#include "third_party/blink/renderer/platform/wtf/wtf.h"

namespace blink {

#if DCHECK_IS_ON()

class EventDispatchForbiddenScope {
  STACK_ALLOCATED();
  DISALLOW_COPY_AND_ASSIGN(EventDispatchForbiddenScope);

 public:
  EventDispatchForbiddenScope() {
    DCHECK(IsMainThread());
    ++count_;
  }

  ~EventDispatchForbiddenScope() {
    DCHECK(IsMainThread());
    DCHECK(count_);
    --count_;
  }

  static bool IsEventDispatchForbidden() {
    if (!IsMainThread())
      return false;
    return count_;
  }

  class AllowUserAgentEvents {
    STACK_ALLOCATED();

   public:
    AllowUserAgentEvents() : change_(&count_, 0) { DCHECK(IsMainThread()); }

    ~AllowUserAgentEvents() { DCHECK(!count_); }

    base::AutoReset<unsigned> change_;
  };

 private:
  CORE_EXPORT static unsigned count_;
};

#else

class EventDispatchForbiddenScope {
  STACK_ALLOCATED();
  DISALLOW_COPY_AND_ASSIGN(EventDispatchForbiddenScope);

 public:
  EventDispatchForbiddenScope() {}

  class AllowUserAgentEvents {
   public:
    AllowUserAgentEvents() {}
  };
};

#endif  // DCHECK_IS_ON()

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_EVENT_DISPATCH_FORBIDDEN_SCOPE_H_
