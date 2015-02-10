# ocaml-pr6764, ocaml-pr6776

This code is a follow up of the issue described here
http://caml.inria.fr/mantis/view.php?id=6764. The program segfaults
with OCaml 4.02.1 and the latest version of trunk. This issue has been
reported here http://caml.inria.fr/mantis/view.php?id=6776

# Summary

# Description

I am looking at embedding the OCaml runtime in a shared library. This
shared library might be loaded and unloaded several time by an
application that makes heavy use of threads.

The code hosted here (which was originally written by Mark Shinwell
during the discussion of the aforementioned related issue) illustrates
what happens when the shared object is loaded and unloaded several
time in a row.

Debugging the issue with this example, it appear that the "tick"
thread might still be alive after the dlclose function returns. The
tick thread should have been cleaned up by the `caml_thread_cleanup`
function, which is registered in the `at_exit` calls. However, the
implementation of `st_thread_kill` in
`otherlibs/systhreads/st_posix.c` is

```
static void st_thread_kill(st_thread_id thr)
{
  pthread_cancel(thr);
}
```

From http://man7.org/linux/man-pages/man3/pthread_cancel.3.html [^]
the thread cancellation happens asynchronously, and "the return status
of pthread_cancel() merely informs the caller whether the cancellation
request was successfully queued."

A suitable fix would be to use `pthread_join` after the
`pthread_cancel`. This would still not have the same behavior as the
win32 implementation (which uses `TerminateThread` and thus cannot
execute any user-mode code).

Another fix would be to replace the `st_thread_kill` in
`caml_thread_cleanup` with something a bit more agressive about
termination (a new `st_thread_real_kill`?)

# Running the example

```
$ make
$ make run
```
