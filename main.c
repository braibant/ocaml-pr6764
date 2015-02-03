#include <dlfcn.h>
#include <caml/mlvalues.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

extern void caml_main(char* argv[]);

static volatile int num_threads = 0;
static char** argv_global;

static void* thread_body(void* lib)
{
  pid_t tid;
  useconds_t sleep_before, sleep_after;
  void* fn;
  tid = syscall(SYS_gettid);
  sleep_before = rand() % 10000; /* 10ms */
  sleep_after = rand() % 10000; /* 10ms */
  printf("thread_body tid %d, sleep before %ld us, sleep after %ld us, ",
         tid, sleep_before, sleep_after);
  printf("num active threads %d\n", ++num_threads);
  fflush(stdout);
  usleep(sleep_before);
  fn = dlsym(lib, "call_ocaml_in_shared_lib");
  if (!fn) abort();
  if (((value (*)(char**)) fn)(argv_global) != Val_unit) abort();
  usleep(sleep_after);
  printf("tid %d exiting\n", tid);
  fflush(stdout);
  --num_threads;
  return NULL;
}

int main(int argc, char* argv[])
{
  void* lib;

  argv_global = argv;

  lib = dlopen("shared_lib.so", RTLD_NOW);
  if (!lib) {
    printf("%s\n", dlerror());
    fflush(stdout);
    abort();
  }

  printf("starting thread creation loop\n");
  fflush(stdout);
  while (1) {
    pthread_t handle;
    pthread_create(&handle, NULL, &thread_body, lib);
    usleep(100000); /* 100ms */
  }

  return 0;
}
