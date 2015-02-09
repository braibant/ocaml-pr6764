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

  /* printf ("call_ocaml_in_shared_lib %d\n", tid); */
  int reg = caml_c_thread_register();
  caml_acquire_runtime_system();
  r = caml_callback(*caml_named_value("ocaml_in_shared_lib"), Val_int(42));
  caml_release_runtime_system();
  caml_c_thread_unregister();
  return r;
}

static void initialize_ocaml_runtime(){

  pid_t tid = syscall(SYS_gettid);

  printf ("initialize_ocaml_runtime (%d)...\n", tid);
  fflush(stdout);
  char *caml_argv[1] = { NULL };
  caml_startup(caml_argv);
  caml_release_runtime_system();
  fflush(stdout);
}

static void finalize_ocaml_runtime(){
  pid_t tid = syscall(SYS_gettid);

  printf ("finalize_ocaml_runtime (%d)...\n", tid);
  fflush(stdout);

  /* Option 0: do nothing. Yield an ocaml thread related segfault. */

  /* Option 1: call at exit. segfault in libgcc. */
  /* value * at_exit = caml_named_value("Pervasives.do_at_exit"); */
  /* if (at_exit != NULL) caml_callback_exn(*at_exit, Val_unit); */

  /* Option 2: call at exit, with the runtime lock. same as before. */
  caml_acquire_runtime_system();
  value * at_exit = caml_named_value("Pervasives.do_at_exit");
  if (at_exit != NULL) caml_callback_exn(*at_exit, Val_unit);

  printf ("finalize_ocaml_runtime: done\n");
  fflush(stdout);

  /* Option 3: call at exit, with the runtime lock. same as before. */
  /* caml_c_thread_register(); */
  /* caml_acquire_runtime_system(); */
  /* value * at_exit = caml_named_value("Pervasives.do_at_exit"); */
  /* if (at_exit != NULL) caml_callback_exn(*at_exit, Val_unit); */

  /* option 4 */
}

__attribute__((constructor))
static void initialize_dll(){
  initialize_ocaml_runtime();
}

__attribute__((destructor))
static void finalize_dll(){
  finalize_ocaml_runtime();
}
