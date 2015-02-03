let ocaml_in_shared_lib x =
  Printf.printf "ocaml in shared lib %d" x

let () =
  Callback.register "ocaml_in_shared_lib" ocaml_in_shared_lib
