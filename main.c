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

#define COUNT 10
int main(int argc, char* argv[])
{
  void* lib;
  int count;
  pthread_t handles [COUNT];

  argv_global = argv;

  while(1) {

    lib = dlopen("shared_lib.so", RTLD_NOW | RTLD_LOCAL);
    if (!lib) {
      printf("%s\n", dlerror());
      fflush(stdout);
      abort();
    }

    printf("starting thread creation loop\n");
    fflush(stdout);

    for (count =0; count < COUNT; count ++) {
      pthread_t handle;
      pthread_create(&handles[count], NULL, &thread_body, lib);
      usleep(10000); /* 10ms */
    }
    printf("finishing thread creation loop\n");

    for (count =0; count < COUNT; count ++) {
      pthread_join(handles[count],NULL);
    };
    printf("all thread joined\n");

    dlclose(lib);
    printf("dlclose ok\n");
    fflush(stdout);
  }
  return 0;
}
