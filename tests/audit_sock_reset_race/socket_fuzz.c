// autogenerated by syzkaller (http://github.com/google/syzkaller)

#ifndef __NR_mmap
#define __NR_mmap 9
#endif
#ifndef __NR_socket
#define __NR_socket 41
#endif
#ifndef __NR_write
#define __NR_write 1
#endif

#define SYZ_SANDBOX_NONE 1
#define SYZ_REPEAT 1

#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <linux/capability.h>
#include <linux/sched.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int kFailStatus = 67;
const int kErrorStatus = 68;
const int kRetryStatus = 69;

__attribute__((noreturn)) void fail(const char* msg, ...)
{
  int e = errno;
  fflush(stdout);
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  fprintf(stderr, " (errno %d)\n", e);
  exit(kFailStatus);
}

__attribute__((noreturn)) void exitf(const char* msg, ...)
{
  int e = errno;
  fflush(stdout);
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  fprintf(stderr, " (errno %d)\n", e);
  exit(kRetryStatus);
}

static int flag_debug;

void debug(const char* msg, ...)
{
  if (!flag_debug)
    return;
  va_list args;
  va_start(args, msg);
  vfprintf(stdout, msg, args);
  va_end(args);
  fflush(stdout);
}

__thread int skip_segv;
__thread jmp_buf segv_env;

static void segv_handler(int sig, siginfo_t* info, void* uctx)
{
  if (__atomic_load_n(&skip_segv, __ATOMIC_RELAXED))
    _longjmp(segv_env, 1);
  exit(sig);
}

static void install_segv_handler()
{
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = segv_handler;
  sa.sa_flags = SA_NODEFER | SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGBUS, &sa, NULL);
}

#define NONFAILING(...)                                                \
  {                                                                    \
    __atomic_fetch_add(&skip_segv, 1, __ATOMIC_SEQ_CST);               \
    if (_setjmp(segv_env) == 0) {                                      \
      __VA_ARGS__;                                                     \
    }                                                                  \
    __atomic_fetch_sub(&skip_segv, 1, __ATOMIC_SEQ_CST);               \
  }

static uintptr_t execute_syscall(int nr, uintptr_t a0, uintptr_t a1,
                                 uintptr_t a2, uintptr_t a3,
                                 uintptr_t a4, uintptr_t a5,
                                 uintptr_t a6, uintptr_t a7,
                                 uintptr_t a8)
{
  switch (nr) {
  default:
    return syscall(nr, a0, a1, a2, a3, a4, a5);
  }
}

static void setup_main_process()
{
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = SIG_IGN;
  syscall(SYS_rt_sigaction, 0x20, &sa, NULL, 8);
  syscall(SYS_rt_sigaction, 0x21, &sa, NULL, 8);
  install_segv_handler();

  char tmpdir_template[] = "./syzkaller.XXXXXX";
  char* tmpdir = mkdtemp(tmpdir_template);
  if (!tmpdir)
    fail("failed to mkdtemp");
  if (chmod(tmpdir, 0777))
    fail("failed to chmod");
  if (chdir(tmpdir))
    fail("failed to chdir");
}

static void loop();

static void sandbox_common()
{
  prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
  setpgrp();
  setsid();

  struct rlimit rlim;
  rlim.rlim_cur = rlim.rlim_max = 128 << 20;
  setrlimit(RLIMIT_AS, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 1 << 20;
  setrlimit(RLIMIT_FSIZE, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 1 << 20;
  setrlimit(RLIMIT_STACK, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 0;
  setrlimit(RLIMIT_CORE, &rlim);

  unshare(CLONE_NEWNS);
  unshare(CLONE_NEWIPC);
  unshare(CLONE_IO);
}

static int do_sandbox_none()
{
  int pid = fork();
  if (pid)
    return pid;
  sandbox_common();
  loop();
  exit(1);
}

static void remove_dir(const char* dir)
{
  DIR* dp;
  struct dirent* ep;
  int iter = 0;
retry:
  dp = opendir(dir);
  if (dp == NULL) {
    if (errno == EMFILE) {
      exitf("opendir(%s) failed due to NOFILE, exiting");
    }
    exitf("opendir(%s) failed", dir);
  }
  while ((ep = readdir(dp))) {
    if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
      continue;
    char filename[FILENAME_MAX];
    snprintf(filename, sizeof(filename), "%s/%s", dir, ep->d_name);
    struct stat st;
    if (lstat(filename, &st))
      exitf("lstat(%s) failed", filename);
    if (S_ISDIR(st.st_mode)) {
      remove_dir(filename);
      continue;
    }
    int i;
    for (i = 0;; i++) {
      debug("unlink(%s)\n", filename);
      if (unlink(filename) == 0)
        break;
      if (errno == EROFS) {
        debug("ignoring EROFS\n");
        break;
      }
      if (errno != EBUSY || i > 100)
        exitf("unlink(%s) failed", filename);
      debug("umount(%s)\n", filename);
      if (umount2(filename, MNT_DETACH))
        exitf("umount(%s) failed", filename);
    }
  }
  closedir(dp);
  int i;
  for (i = 0;; i++) {
    debug("rmdir(%s)\n", dir);
    if (rmdir(dir) == 0)
      break;
    if (i < 100) {
      if (errno == EROFS) {
        debug("ignoring EROFS\n");
        break;
      }
      if (errno == EBUSY) {
        debug("umount(%s)\n", dir);
        if (umount2(dir, MNT_DETACH))
          exitf("umount(%s) failed", dir);
        continue;
      }
      if (errno == ENOTEMPTY) {
        if (iter < 100) {
          iter++;
          goto retry;
        }
      }
    }
    exitf("rmdir(%s) failed", dir);
  }
}

static uint64_t current_time_ms()
{
  struct timespec ts;

  if (clock_gettime(CLOCK_MONOTONIC, &ts))
    fail("clock_gettime failed");
  return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static void test();

void loop()
{
  int iter;
  for (iter = 0;; iter++) {
    char cwdbuf[256];
    sprintf(cwdbuf, "./%d", iter);
    if (mkdir(cwdbuf, 0777))
      fail("failed to mkdir");
    int pid = fork();
    if (pid < 0)
      fail("clone failed");
    if (pid == 0) {
      prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
      setpgrp();
      if (chdir(cwdbuf))
        fail("failed to chdir");
      test();
      exit(0);
    }
    int status = 0;
    uint64_t start = current_time_ms();
    for (;;) {
      int res = waitpid(pid, &status, __WALL | WNOHANG);
      int errno0 = errno;
      if (res == pid)
        break;
      usleep(1000);
      if (current_time_ms() - start > 5 * 1000) {
        kill(-pid, SIGKILL);
        kill(pid, SIGKILL);
        waitpid(pid, &status, __WALL);
        break;
      }
    }
    remove_dir(cwdbuf);
  }
}

long r[4];
void test()
{
  memset(r, -1, sizeof(r));
  r[0] = execute_syscall(__NR_mmap, 0x20000000ul, 0x15e000ul, 0x3ul,
                         0x32ul, 0xfffffffffffffffful, 0x0ul, 0, 0, 0);
  r[1] = execute_syscall(__NR_socket, 0x10ul, 0x3ul, 0x9ul, 0, 0, 0, 0,
                         0, 0);
  NONFAILING(memcpy((void*)0x2015b000,
                    "\x1f\x00\x00\x00\xe9\x03\xff\xca\x37\x06\x00\x00"
                    "\x07\x20\x31\x1f\xe7\x22\x00\x00\x01\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x09",
                    31));
  r[3] = execute_syscall(__NR_write, r[1], 0x2015b000ul, 0x1ful, 0, 0,
                         0, 0, 0, 0);
}
int main()
{
  int i;
  for (i = 0; i < 4; i++) {
    if (fork() == 0) {
      setup_main_process();
      do_sandbox_none();
    }
  }
  sleep(1000000);
  return 0;
}