// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_WORKER_NON_MAIN_THREAD_TASK_QUEUE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_WORKER_NON_MAIN_THREAD_TASK_QUEUE_H_

#include "base/task/sequence_manager/task_queue.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace blink {
namespace scheduler {

class NonMainThreadSchedulerImpl;

class PLATFORM_EXPORT NonMainThreadTaskQueue
    : public base::sequence_manager::TaskQueue {
 public:
  // TODO(kraynov): Consider options to remove TaskQueueImpl reference here.
  NonMainThreadTaskQueue(
      std::unique_ptr<base::sequence_manager::internal::TaskQueueImpl> impl,
      const Spec& spec,
      NonMainThreadSchedulerImpl* non_main_thread_scheduler);
  ~NonMainThreadTaskQueue() override;

  void OnTaskCompleted(const base::sequence_manager::TaskQueue::Task& task,
                       base::TimeTicks start,
                       base::TimeTicks end,
                       base::Optional<base::TimeDelta> thread_time);

 private:
  // Not owned.
  NonMainThreadSchedulerImpl* non_main_thread_scheduler_;
};

}  // namespace scheduler
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_WORKER_NON_MAIN_THREAD_TASK_QUEUE_H_
