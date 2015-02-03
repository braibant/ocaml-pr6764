#include <caml/callback.h>
#include <caml/mlvalues.h>
#include <caml/threads.h>
#include <stdio.h>

static int initialized_runtime = 0;

value call_ocaml_in_shared_lib(char** argv)
{
  value r;
  value* cb;
  if (!initialized_runtime) {
    caml_startup(argv);
    caml_thread_initialize(); /* removing this exhibits the segfault in PR#6764 */
    initialized_runtime = 1;
  }
  caml_c_thread_register();
  caml_acquire_runtime_system();
  r = caml_callback(*caml_named_value("ocaml_in_shared_lib"), Val_int(42));
  caml_release_runtime_system();
  caml_c_thread_unregister();
  return r;
}
