// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_TESTING_PLATFORM_SUPPORT_WITH_MOCK_SCHEDULER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_TESTING_PLATFORM_SUPPORT_WITH_MOCK_SCHEDULER_H_

#include <memory>
#include "base/test/simple_test_tick_clock.h"
#include "third_party/blink/public/platform/web_thread.h"
#include "third_party/blink/renderer/platform/scheduler/base/test/sequence_manager_for_test.h"
#include "third_party/blink/renderer/platform/testing/testing_platform_support.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"

namespace cc {
class OrderedSimpleTaskRunner;
}

namespace blink {

namespace scheduler {
class MainThreadSchedulerImpl;
}

// This class adds scheduler and threading support to TestingPlatformSupport.
// See also ScopedTestingPlatformSupport to use this class correctly.
class TestingPlatformSupportWithMockScheduler : public TestingPlatformSupport {
  WTF_MAKE_NONCOPYABLE(TestingPlatformSupportWithMockScheduler);

 public:
  TestingPlatformSupportWithMockScheduler();
  ~TestingPlatformSupportWithMockScheduler() override;

  // Platform:
  std::unique_ptr<WebThread> CreateThread(
      const WebThreadCreationParams&) override;
  WebThread* CurrentThread() override;

  // Runs a single task.
  void RunSingleTask();

  // Runs all currently queued immediate tasks and delayed tasks whose delay has
  // expired plus any immediate tasks that are posted as a result of running
  // those tasks.
  //
  // This function ignores future delayed tasks when deciding if the system is
  // idle.  If you need to ensure delayed tasks run, try runForPeriodSeconds()
  // instead.
  void RunUntilIdle() override;

  // Runs for |seconds| the testing clock is advanced by |seconds|.  Note real
  // time elapsed will typically much less than |seconds| because delays between
  // timers are fast forwarded.
  void RunForPeriodSeconds(double seconds);

  // Advances |m_clock| by |seconds|.
  void AdvanceClockSeconds(double seconds);
  void AdvanceClock(base::TimeDelta duration);

  scheduler::MainThreadSchedulerImpl* GetMainThreadScheduler() const;

  // Controls the behavior of |m_mockTaskRunner| if true, then |m_clock| will
  // be advanced to the next timer when there's no more immediate work to do.
  void SetAutoAdvanceNowToPendingTasks(bool);

 protected:
  static double GetTestTime();

  base::SimpleTestTickClock clock_;
  scoped_refptr<cc::OrderedSimpleTaskRunner> mock_task_runner_;
  std::unique_ptr<scheduler::MainThreadSchedulerImpl> scheduler_;
  base::sequence_manager::SequenceManagerForTest*
      sequence_manager_;  // Owned by scheduler_.
  std::unique_ptr<WebThread> thread_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_TESTING_PLATFORM_SUPPORT_WITH_MOCK_SCHEDULER_H_
