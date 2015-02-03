#!/bin/bash

set -x -u -e

OCAML=`opam config var prefix`
INCLUDE=$OCAML/lib/ocaml

gcc -fPIC -g -c -I$INCLUDE shared_lib_c.c
$OCAML/bin/ocamlopt.opt -g -output-obj -o shared_lib_proto.so shared_lib.ml shared_lib_c.o threads/threads.cmxa unix.cmxa
gcc -g -shared -o shared_lib.so shared_lib_proto.so -L$OCAML/lib/ocaml -L$OCAML/lib/ocaml/threads -L$OCAML/lib/ocaml/unix -lthreads -lunix -lasmrun
gcc -g -o main -I$INCLUDE -ldl -lpthread main.c -lm
