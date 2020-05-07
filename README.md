What is this?
=============

This software produces [block crossing minimal drawings of Storylines](https://link.springer.com/chapter/10.1007/978-3-319-73915-1_29).
The key concept is to model the problem as a decision problem that can then be solved
by a modern SAT solver. The program is solver agnostic and can be executed with any SAT solver
that adheres to the interface given in [`src/csolver.h`](src/csolver.h).

Building
========

The software provides two libraries:
	- libstorylines.a
	- libstorylinesdrawer.a

The solver module itself only depends on libc.
The drawer module additionally depends on [plotutils](https://www.gnu.org/software/plotutils/).

Before building any module make sure to initialize the git submodule with `git submodule init && git submodule update`.
To build the solver module run `make storylines`. For the drawer module run `make draw`.
Alternatively `make all` builds both.

Usage
=====

Since this software does not provide a binary, it's only real use comes from
you linking it into your code. Remember to also link a solver that implements
the interface described in [`src/csolver.h`](src/csolver.h), for example
[CaDiCaL](https://github.com/acreter/cadical) or [minisat](https://github.com/acreter/minisat). For the drawer module you must also link `libplot.a`
and `lm.a`.

The API for the provided libraries is described in [`slsolver.h`](slsolver.h) and [`sldrawer.h`](sldrawer.h) respectively.
