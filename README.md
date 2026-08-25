# cpm-test

This repository contains some simple programs to test CP/M 2.x emulators.  To ease usage the tests are supplied as both Z80 assembly source files, and compiled binaries.  The supplied `Makefile` can compile each example with the `pasmo` compiler if you wish to extend or rebuild them.

The following table shows the syscalls which are exercised, note that we exclude system calls relating to reading console/auxiliary input, and those that  refer to tracks, sectors and cylinders.  With the exception of [CONOUT.Z80](CONOUT.Z80) all the test programs are targeted at the CP/M BDOS interface.


|  # |   Hex | BDOS call                                                   | Test coverage? |
|---:|------:|-------------------------------------------------------------|----------------|
|  0 | `00h` | System Reset (`P_TERMCPM`)                                  |                |
|  1 | `01h` | Console Input (`C_READ`)                                    |                |
|  2 | `02h` | Console Output (`C_WRITE`)                                  | ✔️              |
|  3 | `03h` | Auxiliary Input (`A_READ`)                                  |                |
|  4 | `04h` | Auxiliary Output (`A_WRITE`)                                | ✔️              |
|  5 | `05h` | List Output (`L_WRITE`)                                     |                |
|  6 | `06h` | Direct Console I/O (`C_RAWIO`)                              |                |
|  7 | `07h` | Get I/O Byte (`GET_IOBYTE`)                                 | ✔️              |
|  8 | `08h` | Set I/O Byte (`SET_IOBYTE`)                                 | ✔️              |
|  9 | `09h` | Print String (`C_WRITESTR`)                                 | ✔️              |
| 10 | `0Ah` | Read Console Buffer (`C_READSTR`)                           |                |
| 11 | `0Bh` | Get Console Status (`C_STAT`)                               |                |
| 12 | `0Ch` | Return Version Number (`S_BDOSVER`)                         | ✔️              |
| 13 | `0Dh` | Reset Disk System (`DRV_ALLRESET`)                          | ✔️              |
| 14 | `0Eh` | Select Disk (`DRV_SET`)                                     | ✔️              |
| 15 | `0Fh` | Open File (`F_OPEN`)                                        | ✔️              |
| 16 | `10h` | Close File (`F_CLOSE`)                                      | ✔️              |
| 17 | `11h` | Search for First (`F_SFIRST`)                               | ✔️              |
| 18 | `12h` | Search for Next (`F_SNEXT`)                                 | ✔️              |
| 19 | `13h` | Delete File (`F_DELETE`)                                    | ✔️              |
| 20 | `14h` | Read Sequential (`F_READ`)                                  | ✔️              |
| 21 | `15h` | Write Sequential (`F_WRITE`)                                | ✔️              |
| 22 | `16h` | Make File (`F_MAKE`)                                        | ✔️              |
| 23 | `17h` | Rename File (`F_RENAME`)                                    | ✔️              |
| 24 | `18h` | Return Login Vector (`DRV_LOGIN`)                           | ✔️              |
| 25 | `19h` | Return Current Disk (`DRV_GET`)                             | ✔️              |
| 26 | `1Ah` | Set DMA Address (`F_DMAOFF`)                                | ✔️              |
| 27 | `1Bh` | Get Allocation Address / Allocation Vector (`DRV_GETALLOC`) |                |
| 28 | `1Ch` | Write Protect Disk (`DRV_SETRO`)                            |                |
| 29 | `1Dh` | Get Read-Only Vector (`DRV_GETRO`)                          |                |
| 30 | `1Eh` | Set File Attributes (`F_SETATT`)                            |                |
| 31 | `1Fh` | Get Disk Parameter Block Address (`DRV_GETDPB`)             |                |
| 32 | `20h` | Set/Get User Code (`GET_SET_USER`)                          | ✔️              |
| 33 | `21h` | Read Random (`F_RREAD`)                                     | ✔️              |
| 34 | `22h` | Write Random (`F_RWRITE`)                                   | ✔️              |
| 35 | `23h` | Compute File Size (`F_SIZE`)                                | ✔️              |
| 36 | `24h` | Set/Update Random Record (`F_RANDREC`)                      | ✔️              |
| 37 | `25h` | Reset Drive (`DRV_RESET`)                                   |                |
| 40 | `28h` | Write Random with Zero Fill (`F_WRITEZF`)                   |                |




# Limitations

Not all emulators care about user-numbers, because little real-world software seemed to use them, the same applies to the I/O byte.  Similarly a lot of emulators assume there is `A:` and nothing else so they don't need to handle keeping a persistant "active drive".

So a failure to run these examples does **not** mean a particular emulator is broken; it might be that they deliberately chose not to implement specific things.




# Usage

All the binaries can be executed, in turn, via:

    make test

This will default to executing them with my own emulator, [cpmulator](https://github.com/skx/cpmulator/), by using `cpmulator -input=stty $BINARY` as the execution method.  If you want to run the tests with another driver set `DRIVER` when you execute `make`.

For example:

* David Lee's [ntvcm](https://github.com/davidly/ntvcm/):
  * `make test DRIVER=ntvcm`
* Iván Izaguirre's [iz-cpm](https://github.com/ivanizag/iz-cpm/):
  * `make test DRIVER=iz-cpm`

You might need to make more effort for other emulators, as some emulators insist the name of the binary to execute is specified **without** the `.COM` suffix - just like the CP/M CCP.  An example of that is [cpm](https://github.com/jhallen/cpm).



## Test Results

Tests will change over time, and of course emulators might get updated to add/remove/alter their BIOS and BDOS implementations so these tests are just a point in time record.

That said you can run the tests yourself to receive current results.

* Run `./setup.sh` to clone each emulator from source.
  * You'll need a rust compiler, a golang toolchain, and a C and C++ compiler to compile all the emulators.
* Run `./run.sh` to output a markdown table of results.


### Test Results 08/25/26

| Test | [cpm](https://github.com/jhallen/cpm) | [cpmulator](https://github.com/skx/cpmulator) | [iz-cpm](https://github.com/ivanizag/iz-cpm) | [ntvcm](https://github.com/davidly/ntvcm) | [tnyplo](https://gitlab.com/gbrein/tnylpo)
| ---- | -- | --  | -- | -- | -- |
| AWRITE.COM |  FAIL | ✔️ | ✔️ | ✔️ | FAIL |
| BDOSVER.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| CONOUT.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| CREATE.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| CWRITE.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| DELETE.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| DRIVE.COM |  FAIL | ✔️ | FAIL| FAIL | FAIL |
| FILESIZE.COM |  ✔️ | ✔️ | ✔️ | ✔️ | ✔️ |
| IOPORT.COM |  ✔️ | ✔️ | FAIL| FAIL | ✔️ |
| LOGVEC.COM |  FAIL | FAIL | ✔️ | FAIL | FAIL |
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

===== LOGVEC.COM =====

A>
FAILED.

A>

===== READWRITE.COM =====

A>
File already present.  Aborting

A>

===== USERNUM.COM =====

A>
FAILED.

A>

#### Failures for cpmulator

===== LOGVEC.COM =====
FAILED.

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

===== LOGVEC.COM =====
FAILED.

===== USERNUM.COM =====
FAILED.

#### Failures for tnylpo

===== AWRITE.COM =====

===== DRIVE.COM =====
tnylpo: access to invalid/unconfigured disk

===== LOGVEC.COM =====
FAILED.

===== READWRITE.COM =====
FAILED.
