// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains functions for launching subprocesses.

#ifndef BASE_PROCESS_LAUNCH_H_
#define BASE_PROCESS_LAUNCH_H_

#include <stddef.h>

#include <string>
#include <utility>
#include <vector>

#include "base/base_export.h"
#include "base/environment.h"
#include "base/macros.h"
#include "base/process/process.h"
#include "base/process/process_handle.h"
#include "base/strings/string_piece.h"
#include "build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_FUCHSIA)
#include <launchpad/launchpad.h>
#include <zircon/types.h>
#endif

#if defined(OS_POSIX) || defined(OS_FUCHSIA)
#include "base/posix/file_descriptor_shuffle.h"
#endif

namespace base {

class CommandLine;

#if defined(OS_WIN)
typedef std::vector<HANDLE> HandlesToInheritVector;
#elif defined(OS_FUCHSIA)
struct HandleToTransfer {
  uint32_t id;
  zx_handle_t handle;
};
typedef std::vector<HandleToTransfer> HandlesToTransferVector;
typedef std::vector<std::pair<int, int>> FileHandleMappingVector;
#elif defined(OS_POSIX)
typedef std::vector<std::pair<int, int>> FileHandleMappingVector;
#endif  // defined(OS_WIN)

// Options for launching a subprocess that are passed to LaunchProcess().
// The default constructor constructs the object with default options.
struct BASE_EXPORT LaunchOptions {
#if defined(OS_POSIX) || defined(OS_FUCHSIA)
  // Delegate to be run in between fork and exec in the subprocess (see
  // pre_exec_delegate below)
  class BASE_EXPORT PreExecDelegate {
   public:
    PreExecDelegate() = default;
    virtual ~PreExecDelegate() = default;

    // Since this is to be run between fork and exec, and fork may have happened
    // while multiple threads were running, this function needs to be async
    // safe.
    virtual void RunAsyncSafe() = 0;

   private:
    DISALLOW_COPY_AND_ASSIGN(PreExecDelegate);
  };
#endif  // defined(OS_POSIX)

  LaunchOptions();
  LaunchOptions(const LaunchOptions&);
  ~LaunchOptions();

  // If true, wait for the process to complete.
  bool wait = false;

  // If not empty, change to this directory before executing the new process.
  base::FilePath current_directory;

#if defined(OS_WIN)
  bool start_hidden = false;

  // Windows can inherit handles when it launches child processes.
  // See https://blogs.msdn.microsoft.com/oldnewthing/20111216-00/?p=8873
  // for a good overview of Windows handle inheritance.
  //
  // Implementation note: it might be nice to implement in terms of
  // base::Optional<>, but then the natural default state (vector not present)
  // would be "all inheritable handles" while we want "no inheritance."
  enum class Inherit {
    // Only those handles in |handles_to_inherit| vector are inherited. If the
    // vector is empty, no handles are inherited. The handles in the vector must
    // all be inheritable.
    kSpecific,

    // All handles in the current process which are inheritable are inherited.
    // In production code this flag should be used only when running
    // short-lived, trusted binaries, because open handles from other libraries
    // and subsystems will leak to the child process, causing errors such as
    // open socket hangs. There are also race conditions that can cause handle
    // over-sharing.
    //
    // |handles_to_inherit| must be null.
    //
    // DEPRECATED. THIS SHOULD NOT BE USED. Explicitly map all handles that
    // need to be shared in new code.
    // TODO(brettw) bug 748258: remove this.
    kAll
  };
  Inherit inherit_mode = Inherit::kSpecific;
  HandlesToInheritVector handles_to_inherit;

  // If non-null, runs as if the user represented by the token had launched it.
  // Whether the application is visible on the interactive desktop depends on
  // the token belonging to an interactive logon session.
  //
  // To avoid hard to diagnose problems, when specified this loads the
  // environment variables associated with the user and if this operation fails
  // the entire call fails as well.
  UserTokenHandle as_user = nullptr;

  // If true, use an empty string for the desktop name.
  bool empty_desktop_name = false;

  // If non-null, launches the application in that job object. The process will
  // be terminated immediately and LaunchProcess() will fail if assignment to
  // the job object fails.
  HANDLE job_handle = nullptr;

  // Handles for the redirection of stdin, stdout and stderr. The caller should
  // either set all three of them or none (i.e. there is no way to redirect
  // stderr without redirecting stdin).
  //
  // The handles must be inheritable. Pseudo handles are used when stdout and
  // stderr redirect to the console. In that case, GetFileType() will return
  // FILE_TYPE_CHAR and they're automatically inherited by child processes. See
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682075.aspx
  // Otherwise, the caller must ensure that the |inherit_mode| and/or
  // |handles_to_inherit| set so that the handles are inherited.
  HANDLE stdin_handle = nullptr;
  HANDLE stdout_handle = nullptr;
  HANDLE stderr_handle = nullptr;

  // If set to true, ensures that the child process is launched with the
  // CREATE_BREAKAWAY_FROM_JOB flag which allows it to breakout of the parent
  // job if any.
  bool force_breakaway_from_job_ = false;

  // If set to true, permission to bring windows to the foreground is passed to
  // the launched process if the current process has such permission.
  bool grant_foreground_privilege = false;
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
  // Set/unset environment variables. These are applied on top of the parent
  // process environment.  Empty (the default) means to inherit the same
  // environment. See AlterEnvironment().
  EnvironmentMap environ;

  // Clear the environment for the new process before processing changes from
  // |environ|.
  bool clear_environ = false;

  // Remap file descriptors according to the mapping of src_fd->dest_fd to
  // propagate FDs into the child process.
  FileHandleMappingVector fds_to_remap;
#endif  // defined(OS_WIN)

#if defined(OS_LINUX)
  // If non-zero, start the process using clone(), using flags as provided.
  // Unlike in clone, clone_flags may not contain a custom termination signal
  // that is sent to the parent when the child dies. The termination signal will
  // always be set to SIGCHLD.
  int clone_flags = 0;

  // By default, child processes will have the PR_SET_NO_NEW_PRIVS bit set. If
  // true, then this bit will not be set in the new child process.
  bool allow_new_privs = false;

  // Sets parent process death signal to SIGKILL.
  bool kill_on_parent_death = false;
#endif  // defined(OS_LINUX)

#if defined(OS_FUCHSIA)
  // If valid, launches the application in that job object.
  zx_handle_t job_handle = ZX_HANDLE_INVALID;

  // Specifies additional handles to transfer (not duplicate) to the child
  // process. The handles remain valid in this process if launch fails.
  // Each entry is an <id,handle> pair, with an |id| created using the PA_HND()
  // macro. The child retrieves the handle |zx_get_startup_handle(id)|.
  HandlesToTransferVector handles_to_transfer;

  // If set, specifies which capabilities should be granted (cloned) to the
  // child process.
  // A zero value indicates that the child process will receive
  // no capabilities.
  // By default the child will inherit the same capabilities, job, and CWD
  // from the parent process.
  uint32_t clone_flags =
      LP_CLONE_FDIO_NAMESPACE | LP_CLONE_DEFAULT_JOB | LP_CLONE_FDIO_STDIO;

  // Specifies the namespace paths which are to be cloned in the child process'
  // namespace. If left unset, the child process will be launched with an empty
  // namespace.
  // This flag allows the parent to pass only the bare minimum OS capabilities
  // to the child process, so that the potential attack surface is reduced in
  // case child process is compromised.
  // Cannot be combined with the clone flag LP_CLONE_FDIO_NAMESPACE, which is
  // equivalent to cloning every path.
  std::vector<FilePath> paths_to_map;
#endif  // defined(OS_FUCHSIA)

// TODO(crbug/836416): Remove OS_FUCHSIA here.
#if defined(OS_POSIX) && !defined(OS_FUCHSIA)
  // If not empty, launch the specified executable instead of
  // cmdline.GetProgram(). This is useful when it is necessary to pass a custom
  // argv[0].
  base::FilePath real_path;

  // If non-null, a delegate to be run immediately prior to executing the new
  // program in the child process.
  //
  // WARNING: If LaunchProcess is called in the presence of multiple threads,
  // code running in this delegate essentially needs to be async-signal safe
  // (see man 7 signal for a list of allowed functions).
  PreExecDelegate* pre_exec_delegate = nullptr;

  // Each element is an RLIMIT_* constant that should be raised to its
  // rlim_max.  This pointer is owned by the caller and must live through
  // the call to LaunchProcess().
  const std::vector<int>* maximize_rlimits = nullptr;

  // If true, start the process in a new process group, instead of
  // inheriting the parent's process group.  The pgid of the child process
  // will be the same as its pid.
  bool new_process_group = false;
#endif  // defined(OS_POSIX) && !defined(OS_FUCHSIA)

#if defined(OS_CHROMEOS)
  // If non-negative, the specified file descriptor will be set as the launched
  // process' controlling terminal.
  int ctrl_terminal_fd = -1;
#endif  // defined(OS_CHROMEOS)
};

// Launch a process via the command line |cmdline|.
// See the documentation of LaunchOptions for details on |options|.
//
// Returns a valid Process upon success.
//
// Unix-specific notes:
// - All file descriptors open in the parent process will be closed in the
//   child process except for any preserved by options::fds_to_remap, and
//   stdin, stdout, and stderr. If not remapped by options::fds_to_remap,
//   stdin is reopened as /dev/null, and the child is allowed to inherit its
//   parent's stdout and stderr.
// - If the first argument on the command line does not contain a slash,
//   PATH will be searched.  (See man execvp.)
BASE_EXPORT Process LaunchProcess(const CommandLine& cmdline,
                                  const LaunchOptions& options);

#if defined(OS_WIN)
// Windows-specific LaunchProcess that takes the command line as a
// string.  Useful for situations where you need to control the
// command line arguments directly, but prefer the CommandLine version
// if launching Chrome itself.
//
// The first command line argument should be the path to the process,
// and don't forget to quote it.
//
// Example (including literal quotes)
//  cmdline = "c:\windows\explorer.exe" -foo "c:\bar\"
BASE_EXPORT Process LaunchProcess(const string16& cmdline,
                                  const LaunchOptions& options);

// Launches a process with elevated privileges.  This does not behave exactly
// like LaunchProcess as it uses ShellExecuteEx instead of CreateProcess to
// create the process.  This means the process will have elevated privileges
// and thus some common operations like OpenProcess will fail. Currently the
// only supported LaunchOptions are |start_hidden| and |wait|.
BASE_EXPORT Process LaunchElevatedProcess(const CommandLine& cmdline,
                                          const LaunchOptions& options);

#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
// A POSIX-specific version of LaunchProcess that takes an argv array
// instead of a CommandLine.  Useful for situations where you need to
// control the command line arguments directly, but prefer the
// CommandLine version if launching Chrome itself.
BASE_EXPORT Process LaunchProcess(const std::vector<std::string>& argv,
                                  const LaunchOptions& options);

// Close all file descriptors, except those which are a destination in the
// given multimap. Only call this function in a child process where you know
// that there aren't any other threads.
BASE_EXPORT void CloseSuperfluousFds(const InjectiveMultimap& saved_map);
#endif  // defined(OS_WIN)

#if defined(OS_WIN)
// Set |job_object|'s JOBOBJECT_EXTENDED_LIMIT_INFORMATION
// BasicLimitInformation.LimitFlags to |limit_flags|.
BASE_EXPORT bool SetJobObjectLimitFlags(HANDLE job_object, DWORD limit_flags);

// Output multi-process printf, cout, cerr, etc to the cmd.exe console that ran
// chrome. This is not thread-safe: only call from main thread.
BASE_EXPORT void RouteStdioToConsole(bool create_console_if_not_found);
#endif  // defined(OS_WIN)

// Executes the application specified by |cl| and wait for it to exit. Stores
// the output (stdout) in |output|. Redirects stderr to /dev/null. Returns true
// on success (application launched and exited cleanly, with exit code
// indicating success).
BASE_EXPORT bool GetAppOutput(const CommandLine& cl, std::string* output);

// Like GetAppOutput, but also includes stderr.
BASE_EXPORT bool GetAppOutputAndError(const CommandLine& cl,
                                      std::string* output);

// A version of |GetAppOutput()| which also returns the exit code of the
// executed command. Returns true if the application runs and exits cleanly. If
// this is the case the exit code of the application is available in
// |*exit_code|.
BASE_EXPORT bool GetAppOutputWithExitCode(const CommandLine& cl,
                                          std::string* output, int* exit_code);

#if defined(OS_WIN)
// A Windows-specific version of GetAppOutput that takes a command line string
// instead of a CommandLine object. Useful for situations where you need to
// control the command line arguments directly.
BASE_EXPORT bool GetAppOutput(const StringPiece16& cl, std::string* output);
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
// A POSIX-specific version of GetAppOutput that takes an argv array
// instead of a CommandLine.  Useful for situations where you need to
// control the command line arguments directly.
BASE_EXPORT bool GetAppOutput(const std::vector<std::string>& argv,
                              std::string* output);

// Like the above POSIX-specific version of GetAppOutput, but also includes
// stderr.
BASE_EXPORT bool GetAppOutputAndError(const std::vector<std::string>& argv,
                                      std::string* output);
#endif  // defined(OS_WIN)

// If supported on the platform, and the user has sufficent rights, increase
// the current process's scheduling priority to a high priority.
BASE_EXPORT void RaiseProcessToHighPriority();

#if defined(OS_MACOSX)
// An implementation of LaunchProcess() that uses posix_spawn() instead of
// fork()+exec(). This does not support the |pre_exec_delegate| and
// |current_directory| options.
Process LaunchProcessPosixSpawn(const std::vector<std::string>& argv,
                                const LaunchOptions& options);

// Restore the default exception handler, setting it to Apple Crash Reporter
// (ReportCrash).  When forking and execing a new process, the child will
// inherit the parent's exception ports, which may be set to the Breakpad
// instance running inside the parent.  The parent's Breakpad instance should
// not handle the child's exceptions.  Calling RestoreDefaultExceptionHandler
// in the child after forking will restore the standard exception handler.
// See http://crbug.com/20371/ for more details.
void RestoreDefaultExceptionHandler();
#endif  // defined(OS_MACOSX)

// Creates a LaunchOptions object suitable for launching processes in a test
// binary. This should not be called in production/released code.
BASE_EXPORT LaunchOptions LaunchOptionsForTest();

#if defined(OS_LINUX) || defined(OS_NACL_NONSFI)
// A wrapper for clone with fork-like behavior, meaning that it returns the
// child's pid in the parent and 0 in the child. |flags|, |ptid|, and |ctid| are
// as in the clone system call (the CLONE_VM flag is not supported).
//
// This function uses the libc clone wrapper (which updates libc's pid cache)
// internally, so callers may expect things like getpid() to work correctly
// after in both the child and parent.
//
// As with fork(), callers should be extremely careful when calling this while
// multiple threads are running, since at the time the fork happened, the
// threads could have been in any state (potentially holding locks, etc.).
// Callers should most likely call execve() in the child soon after calling
// this.
//
// It is unsafe to use any pthread APIs after ForkWithFlags().
// However, performing an exec() will lift this restriction.
BASE_EXPORT pid_t ForkWithFlags(unsigned long flags, pid_t* ptid, pid_t* ctid);
#endif

}  // namespace base

#endif  // BASE_PROCESS_LAUNCH_H_
