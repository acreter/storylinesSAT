Example
=======

This example shows how to use the library.
It reads an instance from a json formated input
file and solves that instances. Since it provides a binary
it is useful for everyone who does not want to link the
library into their own code. You can build the example with
`make all`.

Binary
======

Two binaries will be linked: `build/sl-minisat` and `build/sl-cadical`,
using [CaDiCaL](https://github.com/acreter/cadical) and [minisat](https://github.com/acreter/minisat)
respectively. We recommend using the minisat version for better runtime.
Both binaries take exactly one argument: the path to the json formated input file.

JSON Format
===========

This is the format expected by the binaries. All values are example values
and all times are inclusive:

```
{
	"meta": {
		"start": 0			% the starting time; always has to be 0
		"end":   10			% the ending time
	},

	"meetings": [{			% an array of meetings
		"time_ending": 5	% the time this meeting is ending
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

	"characters": [{		% an array of characters
		"births": [0],		% all births of the character
		"id": 0,			% unique id for the character
		"deaths": [10]		% all deaths of the character
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

The files in [input](input) are input files for several movies.
Note that these might have redundant information. Only the fields specified above
are mandatory.
