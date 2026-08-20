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

Not all emulators care about user-numbers, because little real-world software seemed to use them, the same applies to the I/O byte.  Similarly a lot of emulators assume there is `A:` and nothing else so they don't need to handle keeping a persistant "active drive".

So remember a failure to run these examples does **not** mean a particular emulator is broken; it might be that they deliberately chose not to implement specific things.

Because I do want to keep things portable I neglected to run tests that read from the console, which would be harder to automate.   I also skipped all the disk-based I/O, using sectors, tracks, etc, because I don't implement those things in my own emulator.

The tests here, with the exception of [CONOUT.Z80](CONOUT.Z80), are all BDOS tests because that's the meat of CP/M.  It seems like there are no reasonable ways to exercise the BIOS.




# Usage

All the binaries can be executed, in turn, via:

    make test

This will default to executing them with my own emulator, [cpmulator](https://github.com/skx/cpmulator/), by using `cpmulator -input=stty $BINARY` as the execution method.

But by setting `DRIVER` you can execute the tests with any other emulator, for example:

* David Lee's [ntvcm](https://github.com/davidly/ntvcm/):
  * `make test DRIVER=ntvcm`
* Iván Izaguirre's [iz-cpm](https://github.com/ivanizag/iz-cpm/):
  * `make test DRIVER=iz-cpm`

You might need to make more effort for other emulators, as they prefer to run with the name of the binary to execute **without** the `.COM` suffix - just like the CP/M CCP.  An example of that is [cpm](https://github.com/jhallen/cpm).



## Test Results

Tests will change over time, and of course emulators might get updated to add/remove/alter their BIOS and BDOS implementations so these tests are just a point in time record.

That said you can run the tests yourself to receive current results.

* Run `./setup.sh` to clone each emulator from source.
* Run `./run.sh` to output a markdown table of results.


### Test Results 08/20/26

| Test | [cpm](https://github.com/jhallen/cpm) | [cpmulator](https://github.com/skx/cpmulator) | [iz-cpm](https://github.com/ivanizag/iz-cpm) | [ntvcm](https://github.com/davidly/ntvcm) | [tnyplo](https://gitlab.com/gbrein/tnylpo)
| ---- | -- | --  | -- | -- | -- |
| AWRITE.COM |  FAIL | ✔️ | ✔️ | ✔️ | FAIL |
| BDOSVER.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| CONOUT.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| CREATE.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| CWRITE.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| DELETE.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| DRIVE.COM |  FAIL | ✔️ | FAIL| FAIL | FAIL |
| FILESIZE.COM |  FAIL | ✔️ | ✔️ | ✔️ | ✔️ |
| IOPORT.COM |  ✔️ | ✔️ | FAIL| FAIL | ✔️ |
| RANDOMRW.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| RANDREC.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| READWRITE.COM |  FAIL | ✔️ | ✔️ | ✔️ | FAIL |
| RENAME.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| SEARCH.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| SETDMA.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| USERNUM.COM |  FAIL | ✔️ | ✔️ | FAIL | ✔️ |
| WSTRING.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |

#### Failures for cpm

===== AWRITE.COM =====

A>

A>

Unrecognized BDOS-Function 4:
AF=0044  BC=0004  DE=0050  HL=d659  SP=d471
Stack =  107 db69 d588 4f43  14d 2424 2024 2020


Unrecognized BDOS-Function 4:
AF=0044  BC=0004  DE=0041  HL=d659  SP=d471
Stack =  10e db69 d588 4f43  14d 2424 2024 2020


Unrecognized BDOS-Function 4:
AF=0044  BC=0004  DE=0053  HL=d659  SP=d471
Stack =  115 db69 d588 4f43  14d 2424 2024 2020


Unrecognized BDOS-Function 4:
AF=0044  BC=0004  DE=0053  HL=d659  SP=d471
Stack =  11c db69 d588 4f43  14d 2424 2024 2020


Unrecognized BDOS-Function 4:
AF=0044  BC=0004  DE=0045  HL=d659  SP=d471
Stack =  123 db69 d588 4f43  14d 2424 2024 2020


Unrecognized BDOS-Function 4:
AF=0044  BC=0004  DE=0044  HL=d659  SP=d471
Stack =  12a db69 d588 4f43  14d 2424 2024 2020


Unrecognized BDOS-Function 4:
AF=0044  BC=0004  DE=002e  HL=d659  SP=d471
Stack =  131 db69 d588 4f43  14d 2424 2024 2020


Unrecognized BDOS-Function 4:
AF=0044  BC=0004  DE=000a  HL=d659  SP=d471
Stack =  138 db69 d588 4f43  14d 2424 2024 2020


Unrecognized BDOS-Function 4:
AF=0044  BC=0004  DE=000d  HL=d659  SP=d471
Stack =  13f db69 d588 4f43  14d 2424 2024 2020


===== DRIVE.COM =====

A>
FAILED.

A>

===== FILESIZE.COM =====

A>
error: cannot find fp entry for FCB at 01c2 fctn 35, FCB named FILESIZEDAT
TMP     COM d49b

===== READWRITE.COM =====

A>
File already present.  Aborting

A>

===== USERNUM.COM =====

A>
FAILED.

A>

#### Failures for iz-cpm

===== DRIVE.COM =====

Bdos Err On I: Bad SectorFAILED.

===== IOPORT.COM =====
FAILED.

#### Failures for ntvcm

===== DRIVE.COM =====
FAILED.

===== IOPORT.COM =====
FAILED.

===== USERNUM.COM =====
FAILED.

#### Failures for tnylpo

===== AWRITE.COM =====

===== DRIVE.COM =====
tnylpo: access to invalid/unconfigured disk

===== READWRITE.COM =====
FAILED.
