#include <caml/callback.h>
#include <caml/mlvalues.h>
#include <caml/threads.h>
#include <stdio.h>

static int initialized_runtime = 0;

value call_ocaml_in_shared_lib(char** argv)
{
  value r;
  value* cb;
  /* if (!initialized_runtime) { */
  /*   caml_startup(argv); */
  /*   caml_thread_initialize(); /\* removing this exhibits the segfault in PR#6764 *\/ */
  /*   initialized_runtime = 1; */
  /* } */
  caml_c_thread_register();
  caml_acquire_runtime_system();
  r = caml_callback(*caml_named_value("ocaml_in_shared_lib"), Val_int(42));
  caml_release_runtime_system();
  caml_c_thread_unregister();
  return r;
}

static void initialize_ocaml_runtime(){
  char *caml_argv[1] = { NULL };
  caml_startup(caml_argv);
  caml_thread_initialize();
}

static void finalize_ocaml_runtime(){
  value * at_exit = caml_named_value("Pervasives.do_at_exit");
  if (at_exit != NULL) caml_callback_exn(*at_exit, Val_unit);
}

__attribute__((constructor))
static void initialize_dll(){
  /* Dl_info dl_info; */
  /* dladdr((void *)initialize_ocaml_runtime, &dl_info); */
  /* char* name = malloc(strlen(dl_info.dli_fname)+1); */
  /* if(name) { */
  /*   strcpy(name,dl_info.dli_fname); */
  /*   caml_dll_path = name; */
  /* } */
  initialize_ocaml_runtime();
}

__attribute__((destructor))
static void finalize_dll(){
  finalize_ocaml_runtime();
}
