What is this?
=============

This software produces [block crossing minimal drawings of Storylines](https://link.springer.com/chapter/10.1007/978-3-319-73915-1_29).
The key concept is to model the problem as a decision problem that can then be solved
by a modern SAT solver. The program is solver agnostic and can be executed with any SAT solver
that adheres to the interface given in [`src/csolver.h`](src/csolver.h).

Building
========

If you're unfamiliar with compiling and linking C projects or just want a simple
binary, then skip ahead to `Example`.

The software provides two libraries:
- libslsolve.a
- libsldraw.a

The solver module itself only depends on libc.
The drawer module additionally depends on [plotutils](https://www.gnu.org/software/plotutils/).

Before building any module make sure to initialize the git submodule with `git submodule init && git submodule update`.
To build the solver module run `make build/libslsolve.a`. For the drawer module run `make build/libsldraw.a`.
Alternatively `make all` builds both.

Example
=======

The [`example`](example) directory provides an example on how to use the storylines
library. It implements a parser to read instances from input files, solves and
draws the storyline. It is therefore also suited for users that do not want
to integrate the library into their own code and focus on quick results.
You can build it with `make example`.
See [`example/README.md`](example/README.md) for details.

Note that the example code draws the storylines by default which makes [plotutils](https://www.gnu.org/software/plotutils/)
a requirement.

Usage
=====

Since this software does not provide a binary, it's only real use comes from
you linking it into your code. Remember to also link a solver that implements
the interface described in [`src/csolver.h`](src/csolver.h), for example
[CaDiCaL](https://github.com/acreter/cadical) or [minisat](https://github.com/acreter/minisat). For the drawer module you must also link `libplot.a`
and `lm.a`. Note that the order of linking is important. See [`example/makefile`](example/makefile) for an example.

The API for the provided libraries is described in [`slsolver.h`](slsolver.h) and [`sldrawer.h`](sldrawer.h) respectively.
