What is this?
=============

This software produces [block crossing minimal drawings of Storylines](https://link.springer.com/chapter/10.1007/978-3-319-73915-1_29).
The key concept is to model the problem as a decision problem that can then be solved
by a modern SAT solver. The program is solver agnostic and can be executed with any SAT solver
that adheres to the interface given in [`include/csolver.h`](include/csolver.h).

Building
========

First make sure to initialize all submodules with `git submodule update --init`.
There are two compiletime dependencies:
- [GNU make](https://www.gnu.org/software/make/).
- [GNU Compiler Collection](https://www.gnu.org/software/gcc/).

Other compilers might work as well but have not been tested.
The software does not have any runtime dependecies.

The software provides a library and two binaries:
- build/lib/libstorylines.a
- build/bin/storylines-minisat
- build/bin/storylines-cadical

Run `make lib` or `make bin` to build these modules.
Alternatively `make all` builds everything.

Usage
=====

The library modules is the heart of this software. It serves as a middleware
between the user and the SAT solver. It therefore does not contain any solver.
It is therefore up to you to provide a compliant solver. A solver is compliant if
it implements the interface found in [`include/csolver.h`](include/csolver.h).
As of now there are two solvers that meet this criteria:
[CaDiCaL](https://github.com/acreter/cadical) and
[minisat](https://github.com/acreter/minisat). Both are included in this repository as
submodules. Check the [makefile](makefile) for an example.
The API of the library is provided in [`solver.h`](solver.h).

The binaries give an example on how to use the library. They also serve the impatient
or anyone who is not comfortable in C. The code for binaries at its core just
glues the library and the SAT solvers together while providing a parser to read JSON
input files. To use it run `.build/bin/storylines-minisat <your-json-file>`.
The [`examples`](examples) directory provides example input files.

JSON-Format
=======

This is the format expected by the binaries. All values are example values
and all times are inclusive:

```
{
	"meetings": [{        % an array of meetings
		"time_ending": 5    % the time this meeting is ending
		"members": [0,1,2]  % an array of members (see character id below)
		"time_starting": 0  % the time this meeting is starting
	}, {
		"time_ending": 10
		"members": [0,1]
		"time_starting": 6
	}, {
		"time_ending": 10
		"members": [2,3]
		"time_starting": 6
	}],

	"characters": [{    % an array of characters
		"births": [0],    % all births of the character
		"id": 0,          % unique id for the character
		"deaths": [10]    % all deaths of the character
	}, {
		"births": [0],
		"id": 1,
		"deaths": [10]
	}, {
		"births": [0],
		"id": 2,
		"deaths": [10]
	}, {
		"births": [6],
		"id": 3,
		"deaths": [10]
	}]
}
```

See [`examples`](examples) for more examples.
