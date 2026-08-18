# cpm-test

This repository contains some simple test-programs for CP/M emulators.

The intention here is that we can run regression tests, or test behaviour of various CP/M emulators.  If you're writing an emulator it would be ideal if you could compare your output to that produced by other emulators for example!




# Compilation

If you have the `pasmo` compiler you can build all the binaries via a simple `make`.

* `make all`
  * Build all example binaries.
* `make clean`
  * Remove all generated binaries.

To make things simpler for users I've commited binaries alongside the source.




# Limitations

Not all emulators care about user-numbers, because little real-world software seemed to use them.  Similarly a lot of emulators assume there is `A:` and nothing else.

In short a failure to run these examples does **not** mean a particular emulator is broken, it just means that it works differently to my own.

Because I do want to keep things portable I neglected to run tests that read from the console, which would be harder to automate.   I also skipped all the disk-based I/O, using sectors, tracks, etc, because I don't implement those things in my own emulator.

That said additional tests would be welcome, and adding instructions for other emulators too -  I guess RunCPM is another obvious one to compare against.
t




# Usage

All the binaries can be executed, in turn, via:

    make test

This will default to executing them with my own emulator, [cpmulator](https://github.com/skx/cpmulator/), by using `cpmulator -input=stty $BINARY` as the execution method.

But by setting `DRIVER` you can execute the tests with any other emulator, for example:

* David Lee's [ntvcm](https://github.com/davidly/ntvcm/):
  * `make test DRIVER=ntvcm`
* Iván Izaguirre's [iz-cpm](https://github.com/ivanizag/iz-cpm/):
  * `make test DRIVER=iz-cpm`
