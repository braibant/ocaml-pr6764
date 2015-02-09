let (_ : Thread.t) = Thread.self () (* ensure Thread is linked *)

let ocaml_in_shared_lib x =
  Printf.printf "ocaml in shared lib %d\n%!" x

let () =
  Callback.register "ocaml_in_shared_lib" ocaml_in_shared_lib
