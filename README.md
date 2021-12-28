# Collections

Collections is a generic data structures library for the C language with low memory overhead built using only C standard libraries for the final project of the [CS50x](https://cs50.harvard.edu/x/) curse. See some more about this project in this [YouTube video](https://www.youtube.com).


## Features

Every collection in this library has the CRUD operations, attribute getters and some specific functions like sorting or arithmetic/logic operations.

- [x] List
- [x] Stack
- [x] Queue
- [x] Deque
- [x] Vector
- [x] Bit Array
- [ ] Set
- [ ] Map
- [ ] Trie
- [ ] Priority Queue


## Errors

Because the C language does not have exceptions, the return values are used to notify errors and the global variable errno is set to give some more information.

| Return Type | Error Value |
| ----------- | ----------- |
| bool        | false       |
| size_t      | 0           |
| pointer     | NULL        |

| Errno  | Description             |
| ------ | ----------------------- |
| EINVAL | Invalid argument        |
| ENOMEM | Not enough memory       |
| ENOSPC | No space left on device |

> **Warning:** Sometimes the `0` or `false` returns can be valid, in this case the best way to test if the operation resulted in errors is to set errno to `0` before the call and test if it's value changed.

> **Warning:** The ENOSPC errno value is normally used in a different context, but for this library it signifies that there is no space left in the given collection, meaning it is full.


## Desing

Although C is not an OOP language, the object oriented style was emulated by declaring the structures in `.c` files and using static functions to ensure encapsulation while also preceding "method" names with the "class" name and making its first argument a "this" reference to access the data required.


## Dependencies

Despite depending on the C standard libraries, all collections are independent from each other to make it possible to use certain collections without needing to include everything in the executable.

- errno.h
- stdlib.h
- stdint.h
- string.h
- stdbool.h


## Installation

This project does **not** have the proper static or dynamic library files yet. To add the library to your code download the [ZIP file](https://github.com/pedrolmcastro/collections/blob/main/collections.zip) or the desired collection files and compile it with the rest of the project. An example with all the library files inside a _collections_ directory can be seen in the [Makefile example](https://github.com/pedrolmcastro/collections/blob/main/examples/Makefile).

```
gcc main.c collections/*.c -o main -I collections
```
