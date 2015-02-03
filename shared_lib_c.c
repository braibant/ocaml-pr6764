#include <caml/callback.h>
#include <caml/mlvalues.h>
#include <caml/threads.h>
#include <stdio.h>
#include <sys/syscall.h>

static int initialized_runtime = 0;

value call_ocaml_in_shared_lib(char** argv)
{
  value r;
  value* cb;
  pid_t tid = syscall(SYS_gettid);

  printf ("call_ocaml_in_shared_lib %d\n", tid);
  /* if (!initialized_runtime) { */
  /*   caml_startup(argv); */
  /*   caml_thread_initialize(); /\* removing this exhibits the segfault in PR#6764 *\/ */
  /*   initialized_runtime = 1; */
  /* } */
  int reg = caml_c_thread_register();
  caml_acquire_runtime_system();
  r = caml_callback(*caml_named_value("ocaml_in_shared_lib"), Val_int(42));
  caml_release_runtime_system();
  caml_c_thread_unregister();
  return r;
}

static void initialize_ocaml_runtime(){
  printf ("initialize_ocaml_runtime...\n");
  char *caml_argv[1] = { NULL };
  caml_startup(caml_argv);
  caml_release_runtime_system();
  fflush(stdout);
}

static void finalize_ocaml_runtime(){
  printf ("finalize_ocaml_runtime...\n");
  fflush(stdout);

  value * at_exit = caml_named_value("Pervasives.do_at_exit");
  if (at_exit != NULL) caml_callback_exn(*at_exit, Val_unit);
}

__attribute__((constructor))
static void initialize_dll(){
  initialize_ocaml_runtime();
}

__attribute__((destructor))
static void finalize_dll(){
  finalize_ocaml_runtime();
}
