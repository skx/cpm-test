;
; Does the file in the specified FCB (in DE) exist?
;
file_exists:
        ld      c, 15   ; open file
        call    0005

        push    af      ; save result

        ld      c, 16   ; close file
        call    0005

        pop     af

        ;; Did the open succeed.
        cp 00
        ret
