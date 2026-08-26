;; BDOS entry-point
;;
BDOS    equ     0005h


;;
;; System call IDs.
;;
BDOS_C_WRITE         equ     2
BDOS_A_WRITE         equ     4
BDOS_C_RAWIO         equ     6
BDOS_GET_IOBYTE      equ     7
BDOS_SET_IOBYTE      equ     8
BDOS_C_WRITESTR      equ     9
BDOS_GET_VERSION     equ     12
BDOS_DRV_ALLRESET    equ     13
BDOS_DRV_SET         equ     14
BDOS_FILE_OPEN       equ     15
BDOS_FILE_CLOSE      equ     16
BDOS_SEARCH_FIRST    equ     17
BDOS_SEARCH_NEXT     equ     18
BDOS_FILE_DELETE     equ     19
BDOS_FILE_READ       equ     20
BDOS_FILE_WRITE      equ     21
BDOS_FILE_CREATE     equ     22
BDOS_FILE_RENAME     equ     23
BDOS_DRV_LOGVEC      equ     24
BDOS_DRV_GET         equ     25
BDOS_SET_DMA         equ     26
BDOS_DRV_SETRO       equ     28
BDOS_DRV_GETRO       equ     29
BDOS_GET_SET_USER    equ     32
BDOS_RANDOM_READ     equ     33
BDOS_RANDOM_WRITE    equ     34
BDOS_COMPUTE_FILESZ  equ     35
BDOS_RANDREC         equ     36
BDOS_DRV_RESET       equ     37
BDOS_WRITEZF         equ     40
