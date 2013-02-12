libc.hiken is a C library which is intended to be easily portable to hobbyist
operating systems. An aspiring operating system programmer will most likely
struggle with the lack of the standard library and might get demotivated by the
complexity of programming one. The libc.hiken wants to help them concentrate on
the operating system core.


Design Goals
------------

* Ease of porting  
An OS developer should be able to integrate as much of libc.hiken as possible
without having to dig deep into this code.

* Readability  
Even though the code is written by someone else, libc.hiken should allow quick
insights into the program logic. It is explicitly allowed to sacrifice high
performance for readability. This is not necessarily negative because any
optimizations are left as a valuable exercise to the reader.


Contact
-------

Information and current code are available at the [Github page].
Comments, improvements, forks and pull requests are as always highly
appreciated!

[Github page]: <https://github.com/yayachiken/libc.hiken> 


License
-------

libc.hiken is licensed under the ISC license (see the LICENSE file for details).
If you are working on a operating system, you should have no problems with
incorporating this code (especially if your OS is open source).

If you feel unsure about licensing or need an exemption, feel free to contact me
over Github.

