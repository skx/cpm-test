/*
 * COMPLETE.C - Unified CP/M BDOS exerciser, for the Aztec C compiler.
 *
 * NOTE: Aztec C only treats the first 8 characters of an identifier
 * as significant, so every name in this file - constants, helpers,
 * and test functions alike - is kept to 8 characters or fewer and
 * chosen to be distinct from every other one.  Comments give the
 * matching BDOS_* name from header.inc for cross-reference.
 *
 * bdos() and bdoshl() are supplied by the Aztec C CP/M runtime
 * library:
 *
 *   bdos(func, arg)   - C=func, DE=arg, CALL 0005h, returns A.
 *   bdoshl(func, arg) - as above, but returns the full HL pair, for
 *                       calls whose result doesn't fit in a byte.
 */

/*
 * BDOS function numbers.
 */
#define CWRITE    2             /* BDOS_C_WRITE        */
#define AWRITE    4             /* BDOS_A_WRITE        */
#define RAWIO     6             /* BDOS_C_RAWIO        */
#define IOBGET    7             /* BDOS_GET_IOBYTE     */
#define IOBSET    8             /* BDOS_SET_IOBYTE     */
#define WRITESTR  9             /* BDOS_C_WRITESTR     */
#define BDOSVER   12            /* BDOS_GET_VERSION    */
#define DALLRST   13            /* BDOS_DRV_ALLRESET   */
#define DRVSET    14            /* BDOS_DRV_SET        */
#define FOPEN     15            /* BDOS_FILE_OPEN      */
#define FCLOSE    16            /* BDOS_FILE_CLOSE     */
#define SRCHFST   17            /* BDOS_SEARCH_FIRST   */
#define SRCHNXT   18            /* BDOS_SEARCH_NEXT    */
#define FDELETE   19            /* BDOS_FILE_DELETE    */
#define FREAD     20            /* BDOS_FILE_READ      */
#define FWRITE    21            /* BDOS_FILE_WRITE     */
#define FCREATE   22            /* BDOS_FILE_CREATE    */
#define FRENAME   23            /* BDOS_FILE_RENAME    */
#define DRVLOG    24            /* BDOS_DRV_LOGVEC     */
#define DRVGET    25            /* BDOS_DRV_GET        */
#define SETDMA    26            /* BDOS_SET_DMA        */
#define DRVSETRO  28            /* BDOS_DRV_SETRO      */
#define DRVGETRO  29            /* BDOS_DRV_GETRO      */
#define USERNUM   32            /* BDOS_GET_SET_USER   */
#define RREAD     33            /* BDOS_RANDOM_READ    */
#define RWRITE    34            /* BDOS_RANDOM_WRITE   */
#define FILESIZE  35            /* BDOS_COMPUTE_FILESZ */
#define RANDREC   36            /* BDOS_RANDREC        */
#define DRVRESET  37            /* BDOS_DRV_RESET      */
#define WRITEZF   40            /* BDOS_WRITEZF        */

#define DEFDMA    ((unsigned char *) 0x0080)
#define DMA_LEN   128

#define ERR       0xFF

/* Mask a bdos() result down to the single byte it represents. */
#define LOBYTE(x) ((x) & 0xFF)


/*
 * A CP/M 2.2 FCB - File Control Block - 36 bytes.
 */
struct fcb
{
    unsigned char dr;           /* drive: 0 = current           */
    char          f[8];         /* filename                     */
    char          t[3];         /* filetype                     */
    unsigned char ex;           /* extent, low byte             */
    unsigned char s1;           /* reserved                     */
    unsigned char s2;           /* extent, high byte            */
    unsigned char rc;           /* record count in this extent  */
    unsigned char al[16];       /* allocation map                */
    unsigned char cr;           /* current record               */
    unsigned char r0;           /* random record, low byte      */
    unsigned char r1;           /* random record, middle byte   */
    unsigned char r2;           /* random record, high byte     */
};

/*
 * The 32-byte structure BDOS_FILE_RENAME expects: the FCB of the
 * file to rename, immediately followed by the FCB it should be
 * renamed to.
 */
struct renfcb
{
    unsigned char dr1;
    char          f1[8];
    char          t1[3];
    unsigned char rsv1[4];
    unsigned char dr2;
    char          f2[8];
    char          t2[3];
    unsigned char rsv2[4];
};


/*
 * Running total of tests executed/failed, updated by run_test().
 */
int tests = 0;
int failures = 0;


/* ------------------------------------------------------------------
 * Generic helpers
 * ------------------------------------------------------------------
 */

/*
 * Zero an FCB and populate its name/type fields.  name and type must
 * be exactly 8 and 3 characters long, space-padded as required.
 */
initfcb(fcb, name, type)
struct fcb *fcb;
char *name, *type;
{
    char *p = (char *) fcb;
    int i;

    for (i = 0; i < sizeof(struct fcb); i++)
        p[i] = 0;

    for (i = 0; i < 8; i++)
        fcb->f[i] = name[i];
    for (i = 0; i < 3; i++)
        fcb->t[i] = type[i];
}

/*
 * Zero a rename-FCB and populate both halves of it.
 */
initrfcb(r, oldname, oldtype, newname, newtype)
struct renfcb *r;
char *oldname, *oldtype, *newname, *newtype;
{
    char *p = (char *) r;
    int i;

    for (i = 0; i < sizeof(struct renfcb); i++)
        p[i] = 0;

    for (i = 0; i < 8; i++)
        r->f1[i] = oldname[i];
    for (i = 0; i < 3; i++)
        r->t1[i] = oldtype[i];
    for (i = 0; i < 8; i++)
        r->f2[i] = newname[i];
    for (i = 0; i < 3; i++)
        r->t2[i] = newtype[i];
}

/*
 * Does the file described by the given FCB exist?  Opens it, closes
 * it again immediately, and reports whether the open succeeded.
 */
fexist(fcb)
struct fcb *fcb;
{
    int rc;

    rc = bdos(FOPEN, fcb);
    bdos(FCLOSE, fcb);

    return LOBYTE(rc) != ERR;
}

/* Clear EX/S1/S2 - used before re-opening a file. */
clrex(fcb)
struct fcb *fcb;
{
    fcb->ex = 0;
    fcb->s1 = 0;
    fcb->s2 = 0;
}

/* Clear EX/S1/S2/CR - used before asking BDOS to compute a file size. */
clrext(fcb)
struct fcb *fcb;
{
    clrex(fcb);
    fcb->cr = 0;
}

/* Set the random-record field (R0/R1/R2) of an FCB. */
setrr(fcb, rec)
struct fcb *fcb;
unsigned int rec;
{
    fcb->r0 = rec & 0xFF;
    fcb->r1 = (rec >> 8) & 0xFF;
    fcb->r2 = 0;
}

/* Fill `len` bytes at `buf` with `val`. */
fillmem(buf, len, val)
unsigned char *buf;
int len;
unsigned char val;
{
    int i;

    for (i = 0; i < len; i++)
        buf[i] = val;
}

/*
 * Randomly-read the record numbered `rec` into the DMA buffer, and
 * confirm every byte of it equals `rec`.  Used by both the random
 * read/write test, and as a building block elsewhere.
 */
rrchk(fcb, dma, rec)
struct fcb *fcb;
unsigned char *dma;
unsigned int rec;
{
    int i;

    setrr(fcb, rec);

    if (LOBYTE(bdos(RREAD, fcb)) != 0)
        return 0;

    for (i = 0; i < DMA_LEN; i++)
        if (dma[i] != (unsigned char) rec)
            return 0;

    return 1;
}

/* Print a NUL-terminated string, one character at a time, via BDOS. */
outstr(s)
char *s;
{
    while (*s)
        bdos(CWRITE, *s++);
}

/* Print a non-negative decimal integer via BDOS. */
outnum(n)
unsigned int n;
{
    char buf[6];
    int i = 0;

    if (n == 0)
    {
        bdos(CWRITE, '0');
        return;
    }

    while (n > 0)
    {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i > 0)
        bdos(CWRITE, buf[--i]);
}

/* Run one test, printing its name and PASSED/FAILED result. */
run_test(name, fn)
char *name;
int (*fn) ();
{
    int ok;

    outstr(name);
    outstr(" ... ");

    ok = (*fn) ();

    outstr(ok ? "PASSED.\r\n" : "FAILED.\r\n");

    tests++;
    if (!ok)
        failures++;
}


/* ------------------------------------------------------------------
 * Individual tests - one function per BDOS call under test.
 * ------------------------------------------------------------------
 */

/*
 * CWRITE (2) - console output.  There's no return value to check;
 * if the call doesn't wedge the emulator, that's a pass.  The
 * payload is deliberately not the word "PASSED" - run_test() already
 * prints that, and printing it twice is confusing.
 */
t_cwrite()
{
    char *p = "(via C_WRITE)\r\n";

    while (*p)
        bdos(CWRITE, *p++);
    return 1;
}

/*
 * AWRITE (4) - auxiliary (punch) output.  As above, no return value
 * to check.
 */
t_awrite()
{
    char *p = "(via A_WRITE)\r\n";

    while (*p)
        bdos(AWRITE, *p++);
    return 1;
}

/*
 * RAWIO (6) - direct console I/O, writing mode.  As above.
 */
t_rawio()
{
    char *p = "(via C_RAWIO)\r\n";

    while (*p)
        bdos(RAWIO, *p++);
    return 1;
}

/*
 * WRITESTR (9) - print a $-terminated string in one call.
 */
t_wrstr()
{
    static char msg[] = "(via C_WRITESTR)\r\n$";

    bdos(WRITESTR, msg);
    return 1;
}

/*
 * IOBGET (7) / IOBSET (8).
 */
tryiob(v)
unsigned char v;
{
    bdos(IOBSET, v);
    return LOBYTE(bdos(IOBGET, 0)) == v;
}

t_ioby()
{
    int original;

    original = LOBYTE(bdos(IOBGET, 0));

    if (!tryiob(0x00))
        goto fail;
    if (!tryiob(0xFF))
        goto fail;
    if (!tryiob(0x5A))
        goto fail;

    bdos(IOBSET, original);
    return 1;

fail:
    bdos(IOBSET, original);
    return 0;
}

/*
 * BDOSVER (12) - expect CP/M 2.2, i.e. H=0x00, L=0x22.
 */
t_ver()
{
    unsigned int v;

    v = bdoshl(BDOSVER, 0);

    if ((v >> 8) != 0x00)
        return 0;
    if ((v & 0xFF) != 0x22)
        return 0;

    return 1;
}

/*
 * DRVSET (14) / DRVGET (25) / DALLRST (13).
 */
t_drive()
{
    int original;

    original = LOBYTE(bdos(DRVGET, 0));

    bdos(DRVSET, 3);
    if (LOBYTE(bdos(DRVGET, 0)) != 3)
        goto fail;

    /* 200 (0xC8) should be masked down to its low 4 bits -> 8. */
    bdos(DRVSET, 200);
    if (LOBYTE(bdos(DRVGET, 0)) != 8)
        goto fail;

    bdos(DALLRST, 0);
    if (LOBYTE(bdos(DRVGET, 0)) != 0)
        goto fail;

    bdos(DRVSET, original);
    return 1;

fail:
    bdos(DRVSET, original);
    return 0;
}

/*
 * FCREATE (22) / FDELETE (19).
 */
t_creat()
{
    struct fcb f;

    initfcb(&f, "CREATE  ", "ME ");

    bdos(FDELETE, &f);          /* leftovers from a previous run */

    if (fexist(&f))
        return 0;

    bdos(FCREATE, &f);

    if (!fexist(&f))
        return 0;

    bdos(FDELETE, &f);
    return 1;
}

/*
 * FDELETE (19) via a wildcard FCB, removing several files in one
 * call.
 */
t_delete()
{
    struct fcb f1, f2, f3, wild;

    initfcb(&f1, "TEST1   ", "TXT");
    initfcb(&f2, "TEST2   ", "TXT");
    initfcb(&f3, "TEST3   ", "TXT");
    initfcb(&wild, "TEST?   ", "TXT");

    bdos(FDELETE, &f1);
    bdos(FDELETE, &f2);
    bdos(FDELETE, &f3);

    bdos(FCREATE, &f1);
    bdos(FCLOSE, &f1);
    bdos(FCREATE, &f2);
    bdos(FCLOSE, &f2);
    bdos(FCREATE, &f3);
    bdos(FCLOSE, &f3);

    bdos(FDELETE, &wild);

    if (fexist(&f1))
        return 0;
    if (fexist(&f2))
        return 0;
    if (fexist(&f3))
        return 0;

    return 1;
}

/*
 * SRCHFST (17) / SRCHNXT (18).
 */
t_search()
{
    struct fcb a, b, c, wild;
    int count;

    initfcb(&a, "SRCHA   ", "TS1");
    initfcb(&b, "SRCHB   ", "TS1");
    initfcb(&c, "SRCHC   ", "TS1");
    initfcb(&wild, "SRCH????", "TS1");

    bdos(FDELETE, &a);
    bdos(FDELETE, &b);
    bdos(FDELETE, &c);

    bdos(FCREATE, &a);
    bdos(FCLOSE, &a);
    bdos(FCREATE, &b);
    bdos(FCLOSE, &b);
    bdos(FCREATE, &c);
    bdos(FCLOSE, &c);

    count = 0;
    if (LOBYTE(bdos(SRCHFST, &wild)) != ERR)
    {
        count++;
        while (LOBYTE(bdos(SRCHNXT, &wild)) != ERR)
            count++;
    }

    if (count != 3)
        goto fail;

    bdos(FDELETE, &a);
    bdos(FDELETE, &b);
    bdos(FDELETE, &c);

    if (LOBYTE(bdos(SRCHFST, &wild)) != ERR)
        goto fail;

    return 1;

fail:
    bdos(FDELETE, &a);
    bdos(FDELETE, &b);
    bdos(FDELETE, &c);
    return 0;
}

/*
 * FRENAME (23).
 */
t_rename()
{
    struct fcb oldf, newf;
    struct renfcb r;

    initfcb(&oldf, "OLDNAME ", "DAT");
    initfcb(&newf, "NEWNAME ", "DAT");
    initrfcb(&r, "OLDNAME ", "DAT", "NEWNAME ", "DAT");

    bdos(FDELETE, &oldf);
    bdos(FDELETE, &newf);

    bdos(FCREATE, &oldf);
    bdos(FCLOSE, &oldf);

    if (!fexist(&oldf))
        goto fail;
    if (fexist(&newf))
        goto fail;

    bdos(FRENAME, &r);

    if (fexist(&oldf))
        goto fail;
    if (!fexist(&newf))
        goto fail;

    bdos(FDELETE, &newf);
    return 1;

fail:
    bdos(FDELETE, &oldf);
    bdos(FDELETE, &newf);
    return 0;
}

/*
 * DRVLOG (24).
 */
t_logvec()
{
    int original;
    unsigned int vec;

    original = LOBYTE(bdos(DRVGET, 0));

    bdos(DALLRST, 0);

    /* B: shouldn't be logged in yet. */
    vec = bdoshl(DRVLOG, 0);
    if (vec & 0x02)
        goto fail;

    /* Selecting B: should log it in. */
    bdos(DRVSET, 1);
    vec = bdoshl(DRVLOG, 0);
    if (!(vec & 0x02))
        goto fail;

    /* Resetting should clear it again. */
    bdos(DALLRST, 0);
    vec = bdoshl(DRVLOG, 0);
    if (vec & 0x02)
        goto fail;

    /* A: should still default to logged in. */
    if (!(vec & 0x01))
        goto fail;

    bdos(DRVSET, original);
    return 1;

fail:
    bdos(DRVSET, original);
    return 0;
}

/*
 * DRVSETRO (28) / DRVGETRO (29) / DRVRESET (37).
 */
t_ro1()
{
    int original;
    unsigned int vec;

    original = LOBYTE(bdos(DRVGET, 0));

    bdos(DALLRST, 0);

    vec = bdoshl(DRVGETRO, 0);
    if (vec != 0)
        goto fail;

    bdos(DRVSET, 1);            /* B: */
    bdos(DRVSETRO, 0);

    bdos(DRVSET, 2);            /* C: */
    bdos(DRVSETRO, 0);

    /* B and C should be read-only, and nothing else. */
    vec = bdoshl(DRVGETRO, 0);
    if ((vec & 0x06) != 0x06)
        goto fail;
    if (vec & 0xFFF9)
        goto fail;

    /* Selecting a different drive shouldn't clear the setting. */
    bdos(DRVSET, 0);
    vec = bdoshl(DRVGETRO, 0);
    if ((vec & 0x06) != 0x06)
        goto fail;

    /* Resetting all drives should clear all read-only flags. */
    bdos(DRVRESET, -1);         /* DE = 0xFFFF */
    vec = bdoshl(DRVGETRO, 0);
    if (vec != 0)
        goto fail;

    bdos(DRVSET, original);
    return 1;

fail:
    bdos(DALLRST, 0);
    bdos(DRVSET, original);
    return 0;
}

/*
 * Confirm that DRVSETRO actually prevents writing, creating,
 * deleting and renaming files on the read-only drive.
 */
ro2clean(f1, f2, f3, renamed, original)
struct fcb *f1, *f2, *f3, *renamed;
int original;
{
    bdos(DALLRST, 0);
    bdos(DRVSET, 1);
    bdos(FDELETE, f1);
    bdos(FDELETE, f2);
    bdos(FDELETE, f3);
    bdos(FDELETE, renamed);
    bdos(DRVSET, original);
}

t_ro2()
{
    int original;
    struct fcb f1, f2, f3, renamed;
    struct renfcb r;

    original = LOBYTE(bdos(DRVGET, 0));

    bdos(DALLRST, 0);
    bdos(DRVSET, 1);            /* B: */

    initfcb(&f1, "RO2FILE1", "DAT");
    initfcb(&f2, "RO2FILE2", "DAT");
    initfcb(&f3, "RO2FILE3", "DAT");
    initfcb(&renamed, "RO2FILE9", "DAT");
    initrfcb(&r, "RO2FILE1", "DAT", "RO2FILE9", "DAT");

    bdos(FDELETE, &f1);
    bdos(FDELETE, &f2);
    bdos(FDELETE, &f3);
    bdos(FDELETE, &renamed);

    if (LOBYTE(bdos(FCREATE, &f1)) == ERR)
        goto fail;
    bdos(FCLOSE, &f1);

    if (LOBYTE(bdos(FCREATE, &f2)) == ERR)
        goto fail;
    bdos(FCLOSE, &f2);

    if (!fexist(&f1) || !fexist(&f2))
        goto fail;

    /* Mark the (currently selected) drive B: read-only. */
    bdos(DRVSETRO, 0);

    /* Creating should now fail, and not create the file. */
    if (LOBYTE(bdos(FCREATE, &f3)) != ERR)
        goto fail;
    if (fexist(&f3))
        goto fail;

    /* Opening for read should still work ... */
    if (LOBYTE(bdos(FOPEN, &f1)) == ERR)
        goto fail;
    /* ... but writing to it should fail. */
    if (LOBYTE(bdos(FWRITE, &f1)) != ERR)
        goto fail;
    bdos(FCLOSE, &f1);

    /* Deleting should fail, and the file should remain. */
    if (LOBYTE(bdos(FDELETE, &f2)) != ERR)
        goto fail;
    if (!fexist(&f2))
        goto fail;

    /* Renaming should fail, and the old name should remain. */
    if (LOBYTE(bdos(FRENAME, &r)) != ERR)
        goto fail;
    if (!fexist(&f1))
        goto fail;
    if (fexist(&renamed))
        goto fail;

    ro2clean(&f1, &f2, &f3, &renamed, original);
    return 1;

fail:
    ro2clean(&f1, &f2, &f3, &renamed, original);
    return 0;
}

/*
 * SETDMA (26) - move the DMA target away from its default of 0x0080
 * and confirm reads land there instead.
 */
t_dma()
{
    struct fcb f;
    unsigned char *defdma = DEFDMA;
    static unsigned char ourdma[DMA_LEN];
    int i;

    initfcb(&f, "SETDMA  ", "DAT");

    bdos(FDELETE, &f);
    if (fexist(&f))
        return 0;

    bdos(FCREATE, &f);

    fillmem(defdma, DMA_LEN, 'A');
    bdos(FWRITE, &f);

    bdos(FCLOSE, &f);
    clrex(&f);
    bdos(FOPEN, &f);
    f.cr = 0;

    /* If the read still lands at the default DMA, we'll see 'Z'. */
    fillmem(defdma, DMA_LEN, 'Z');

    bdos(SETDMA, ourdma);

    if (LOBYTE(bdos(FREAD, &f)) != 0)
    {
        bdos(SETDMA, defdma);
        goto fail;
    }

    bdos(SETDMA, defdma);

    for (i = 0; i < DMA_LEN; i++)
        if (ourdma[i] != 'A')
            goto fail;

    for (i = 0; i < DMA_LEN; i++)
        if (defdma[i] != 'Z')
            goto fail;

    bdos(FCLOSE, &f);
    bdos(FDELETE, &f);
    return 1;

fail:
    bdos(FCLOSE, &f);
    bdos(FDELETE, &f);
    return 0;
}

/*
 * FWRITE (21) / FREAD (20) - write 256 sequential records, then read
 * them back and confirm the contents.
 *
 * As in READWRITE.Z80, the file is deleted on entry, not on exit, so
 * it can be examined afterwards - but unlike that original, a failure
 * here just returns 0 rather than halting the whole machine, since
 * other tests still need to run.
 */
t_rdwr()
{
    struct fcb f;
    unsigned char *dma = DEFDMA;
    unsigned int rec;
    int i;

    initfcb(&f, "SEQ_RDWT", "DAT");

    bdos(FDELETE, &f);
    if (fexist(&f))
        return 0;

    bdos(FCREATE, &f);

    for (rec = 0; rec < 256; rec++)
    {
        fillmem(dma, DMA_LEN, (unsigned char) rec);
        if (LOBYTE(bdos(FWRITE, &f)) != 0)
            goto fail;
    }
    bdos(FCLOSE, &f);

    clrex(&f);
    bdos(FOPEN, &f);
    f.cr = 0;

    for (rec = 0; rec < 256; rec++)
    {
        if (LOBYTE(bdos(FREAD, &f)) != 0)
            goto fail;

        for (i = 0; i < DMA_LEN; i++)
            if (dma[i] != (unsigned char) rec)
                goto fail;
    }

    bdos(FCLOSE, &f);
    return 1;

fail:
    bdos(FCLOSE, &f);
    return 0;
}

/*
 * RREAD (33) / RWRITE (34).
 */
t_rrw()
{
    struct fcb f;
    unsigned char *dma = DEFDMA;
    unsigned int rec;
    int i;

    initfcb(&f, "RANDMRW ", "DAT");

    bdos(FDELETE, &f);
    if (fexist(&f))
        return 0;

    bdos(FCREATE, &f);

    /* Write 10 records sequentially, each filled with its own number. */
    for (rec = 0; rec < 10; rec++)
    {
        fillmem(dma, DMA_LEN, (unsigned char) rec);
        if (LOBYTE(bdos(FWRITE, &f)) != 0)
            goto fail;
    }
    bdos(FCLOSE, &f);

    clrex(&f);
    bdos(FOPEN, &f);

    /* Read a handful back, out of sequence. */
    if (!rrchk(&f, dma, 7))
        goto fail;
    if (!rrchk(&f, dma, 2))
        goto fail;
    if (!rrchk(&f, dma, 9))
        goto fail;
    if (!rrchk(&f, dma, 0))
        goto fail;

    /* Overwrite record 3 in place, with a value that can't be
       confused with a record number. */
    setrr(&f, 3);
    fillmem(dma, DMA_LEN, 0xAA);
    if (LOBYTE(bdos(RWRITE, &f)) != 0)
        goto fail;

    /* Confirm record 3 now reads back as 0xAA throughout. */
    setrr(&f, 3);
    if (LOBYTE(bdos(RREAD, &f)) != 0)
        goto fail;
    for (i = 0; i < DMA_LEN; i++)
        if (dma[i] != 0xAA)
            goto fail;

    /* Confirm neighbouring records 2 and 4 are untouched. */
    if (!rrchk(&f, dma, 2))
        goto fail;
    if (!rrchk(&f, dma, 4))
        goto fail;

    bdos(FCLOSE, &f);
    bdos(FDELETE, &f);
    return 1;

fail:
    bdos(FCLOSE, &f);
    bdos(FDELETE, &f);
    return 0;
}

/*
 * WRITEZF (40) - write random with zero fill: writing a record
 * part-way through the first allocation block should zero-fill the
 * rest of that block.
 */
t_wzf()
{
    struct fcb f;
    unsigned char *dma = DEFDMA;
    int i;

    initfcb(&f, "WRITEZF ", "DAT");

    bdos(FDELETE, &f);
    if (fexist(&f))
        return 0;

    bdos(FCREATE, &f);

    fillmem(dma, DMA_LEN, 0x55);
    setrr(&f, 4);                /* shares block 0 with record 0 */
    if (LOBYTE(bdos(WRITEZF, &f)) != 0)
        goto fail;

    /* The record we wrote should read back with our fill byte. */
    setrr(&f, 4);
    if (LOBYTE(bdos(RREAD, &f)) != 0)
        goto fail;
    for (i = 0; i < DMA_LEN; i++)
        if (dma[i] != 0x55)
            goto fail;

    /* Record 0 - same block, never explicitly written - should be
       all zero bytes. */
    setrr(&f, 0);
    if (LOBYTE(bdos(RREAD, &f)) != 0)
        goto fail;
    for (i = 0; i < DMA_LEN; i++)
        if (dma[i] != 0)
            goto fail;

    bdos(FCLOSE, &f);
    bdos(FDELETE, &f);
    return 1;

fail:
    bdos(FCLOSE, &f);
    bdos(FDELETE, &f);
    return 0;
}

/*
 * FILESIZE (35) - write a file spanning more than one extent, and
 * confirm the computed size (in records) matches.
 */
t_fsize()
{
    struct fcb f;
    unsigned int remaining;
    unsigned int r;

    initfcb(&f, "FILESIZE", "DAT");

    bdos(FDELETE, &f);
    if (fexist(&f))
        return 0;

    bdos(FCREATE, &f);

    for (remaining = 130; remaining > 0; remaining--)
        if (LOBYTE(bdos(FWRITE, &f)) != 0)
            goto fail;

    bdos(FCLOSE, &f);
    bdos(FOPEN, &f);

    /* FILESIZE expects EX/S1/S2/CR to be zero on entry. */
    clrext(&f);

    bdos(FILESIZE, &f);

    if (f.r2 != 0)
        goto fail;
    r = (unsigned int) f.r0 + ((unsigned int) f.r1 << 8);
    if (r != 130)
        goto fail;

    bdos(FDELETE, &f);
    return 1;

fail:
    bdos(FDELETE, &f);
    return 0;
}

/*
 * RANDREC (36) - after a run of sequential reads, ask BDOS for the
 * equivalent random-record number and confirm it matches.
 */
t_rrec()
{
    struct fcb f;
    unsigned int remaining, rec;
    unsigned int r;

    initfcb(&f, "RANDREC ", "DAT");

    bdos(FDELETE, &f);
    if (fexist(&f))
        return 0;

    bdos(FCREATE, &f);

    /* Write 200 records - more than one 128-record extent. */
    for (remaining = 200; remaining > 0; remaining--)
        if (LOBYTE(bdos(FWRITE, &f)) != 0)
            goto fail;

    bdos(FCLOSE, &f);
    clrex(&f);
    bdos(FOPEN, &f);
    f.cr = 0;

    /* Read 130 of them sequentially - this crosses the extent
       boundary at record 128. */
    for (rec = 0; rec < 130; rec++)
        if (LOBYTE(bdos(FREAD, &f)) != 0)
            goto fail;

    bdos(RANDREC, &f);

    if (f.r2 != 0)
        goto fail;
    r = (unsigned int) f.r0 + ((unsigned int) f.r1 << 8);
    if (r != 130)
        goto fail;

    bdos(FCLOSE, &f);
    bdos(FDELETE, &f);
    return 1;

fail:
    bdos(FCLOSE, &f);
    bdos(FDELETE, &f);
    return 0;
}

/*
 * USERNUM (32).
 */
t_usrnum()
{
    int original, u;

    original = LOBYTE(bdos(USERNUM, 0xFF));

    for (u = 1; u < 16; u++)
    {
        bdos(USERNUM, u);
        if (LOBYTE(bdos(USERNUM, 0xFF)) != u)
            goto fail;
    }

    /* 200 (0xC8) should be masked down to its low 4 bits -> 8. */
    bdos(USERNUM, 200);
    if (LOBYTE(bdos(USERNUM, 0xFF)) != 8)
        goto fail;

    bdos(USERNUM, original);
    return 1;

fail:
    bdos(USERNUM, original);
    return 0;
}


/* ------------------------------------------------------------------
 * main() - run every test, report results.
 * ------------------------------------------------------------------
 */
main()
{
    outstr("cpm-test: unified BDOS test-suite\r\n");
    outstr("==================================\r\n\r\n");

    run_test("C_WRITE             ", t_cwrite);
    run_test("A_WRITE             ", t_awrite);
    run_test("C_RAWIO             ", t_rawio);
    run_test("C_WRITESTR          ", t_wrstr);
    run_test("GET_SET_IOBYTE      ", t_ioby);
    run_test("GET_VERSION         ", t_ver);
    run_test("DRV_SET/GET/ALLRESET", t_drive);
    run_test("FILE_CREATE/DELETE  ", t_creat);
    run_test("FILE_DELETE (wild)  ", t_delete);
    run_test("SEARCH_FIRST/NEXT   ", t_search);
    run_test("FILE_RENAME         ", t_rename);
    run_test("DRV_LOGVEC          ", t_logvec);
    run_test("DRV_SETRO/GETRO     ", t_ro1);
    run_test("Read-only drive     ", t_ro2);
    run_test("SET_DMA             ", t_dma);
    run_test("FILE_READ/WRITE     ", t_rdwr);
    run_test("RANDOM_READ/WRITE   ", t_rrw);
    run_test("WRITEZF             ", t_wzf);
    run_test("COMPUTE_FILESZ      ", t_fsize);
    run_test("RANDREC             ", t_rrec);
    run_test("GET_SET_USER        ", t_usrnum);

    outstr("\r\n");
    outstr("Results: ");
    outnum(tests - failures);
    outstr(" / ");
    outnum(tests);
    outstr(" tests passed.\r\n");

    return failures != 0;
}
