; -------------------------------------------------------------
;
; vividos' last intro
; (c) 2018 vividos
;
; -------------------------------------------------------------

; constants

WINDOW_LINE_START := 2
WINDOW_HEIGHT := 20

WINDOW_COL_START := 4
WINDOW_WIDTH := 30

; -------------------------------------------------------------
.segment "PRG_ADDRESS"

; hack to get start address as the .prg file position
.addr * + 2

; -------------------------------------------------------------

; this segment contains the "basic start", containing the
; following code: 2018 SYS2061
.segment "BASICSTART"

.word next_line + 1  ; link to next line
.word 2018           ; line number
.byte $9E            ; SYS token
.byte '2', '0', '6', '1'
next_line:
.byte 0, 0, 0        ; end of basic program

; -------------------------------------------------------------

; this segment contains all the code used in the intro
.segment "CODE"

start:
	lda #$0c
	sta $d020
	sta $d021

	lda #$0b
	sta $0286

	jsr $e544	; clear screen
	
	jsr draw_directory_window

	ldx #$08
	jsr get_directory
	sta max_dir_entry
	lda #$01
	sta start_dir_entry
	
	jsr print_directory
	rts


; -------------------------------------------------------------

	; routine: draws the directory window

draw_directory_window:
	lda #85		; left round top corner
	sta $0400 + WINDOW_LINE_START * 40 + WINDOW_COL_START
	lda #73		; right round top corner
	sta $0400 + WINDOW_LINE_START * 40 + WINDOW_COL_START + WINDOW_WIDTH

	lda #74		; left round bottom corner
	sta $0400 + (WINDOW_LINE_START + WINDOW_HEIGHT) * 40 + WINDOW_COL_START
	lda #75		; right round bottom corner
	sta $0400 + (WINDOW_LINE_START + WINDOW_HEIGHT) * 40 + WINDOW_COL_START + WINDOW_WIDTH

	lda #107	; left
	sta $0400 + (WINDOW_LINE_START + 2) * 40 + WINDOW_COL_START
	sta $0400 + (WINDOW_LINE_START + WINDOW_HEIGHT - 2) * 40 + WINDOW_COL_START
	lda #115	; right
	sta $0400 + (WINDOW_LINE_START + 2) * 40 + WINDOW_COL_START + WINDOW_WIDTH
	sta $0400 + (WINDOW_LINE_START + WINDOW_HEIGHT - 2) * 40 + WINDOW_COL_START + WINDOW_WIDTH

	ldx #WINDOW_WIDTH - 2
draw_window_x_loop:
	lda #64
	sta $0400 + WINDOW_LINE_START * 40 + WINDOW_COL_START + 1, x
	sta $0400 + (WINDOW_LINE_START + WINDOW_HEIGHT) * 40 + WINDOW_COL_START + 1, x
	sta $0400 + (WINDOW_LINE_START + 2) * 40 + WINDOW_COL_START + 1, x
	sta $0400 + (WINDOW_LINE_START + WINDOW_HEIGHT - 2) * 40 + WINDOW_COL_START + 1, x
	dex
	bpl draw_window_x_loop	

	lda #93
	sta $0400 + (WINDOW_LINE_START + 1) * 40 + WINDOW_COL_START
	sta $0400 + (WINDOW_LINE_START + WINDOW_HEIGHT - 1) * 40 + WINDOW_COL_START
	sta $0400 + (WINDOW_LINE_START + 1) * 40 + WINDOW_COL_START + WINDOW_WIDTH
	sta $0400 + (WINDOW_LINE_START + WINDOW_HEIGHT - 1) * 40 + WINDOW_COL_START + WINDOW_WIDTH

	ldx #<($0400 + (WINDOW_LINE_START + 3) * 40 + WINDOW_COL_START)
	ldy #>($0400 + (WINDOW_LINE_START + 3) * 40 + WINDOW_COL_START)
	stx $fb
	sty $fc

	ldx #WINDOW_HEIGHT - 5
draw_window_y_loop:
	lda #93
	ldy #0
	sta ($fb),y
	ldy #WINDOW_WIDTH
	sta ($fb),y
	
	lda $fb
	clc
	adc #40
	sta $fb
	lda $fc
	adc #0
	sta $fc
	
	dex
	bne draw_window_y_loop

	rts

; -------------------------------------------------------------

	; routine: prints directory in the drawn window
	
print_directory:
	ldx #$0f
print_label_loop:
	lda dirspace + 2,x
	;jsr pscheck
	sta $0400 + (WINDOW_LINE_START + 1) * 40 + WINDOW_COL_START + 1,x
	dex 
	bpl print_label_loop

	; calc start pointer into dirspace
	lda start_dir_entry
	sta $fb
	lda #$00
	sta $fc
	
	clc
	rol $fb
	rol $fc
	
	rol $fb
	rol $fc
	
	rol $fb
	rol $fc

	rol $fb
	rol $fc

	rol $fb
	rol $fc

	lda #<dirspace
	clc
	adc $fb
	sta $fb
	
	lda #>dirspace
	adc $fc
	sta $fc

	; TODO
	
	rts

; -------------------------------------------------------------

	; routine: gets directory from a floppy drive and stores it
	; in the memory, starting at label dirspace; first entry is
	; the disk label, the last entry is the number of free
	; blocks.
	; parameter: drive number in X register
	; return value: number of entries in the table, including
	; label and "blckcs free" entry.
get_directory:

TALK := $ffb4	; set serial bus to talk
TKSA := $ff96	; sends secondary address after TALK
BUSIN := $ffa5	; get byte from serial bus
OPENI := $f3d5	; send secondary address and filename
CLOSEI := $f642	; close serial bus

SECADR := $b9	; current secondary address
DEVICENR := $ba	; current device number
IOSTATUS := $90	; Kernal I/O status

	stx DEVICENR
	ldy #00
	sty SECADR

	lda #$01
	ldx #<dirname
	ldy #>dirname
	jsr $ffbd

	lda #<dirspace
	sta $fb
	lda #>dirspace
	sta $fc

	jsr OPENI

	jsr TALK
	lda $018c
	jsr TKSA
	ldy #$05
	.byte $2c	; bit opcode
l1:
	ldy #$03
l5:
	tax 
	jsr BUSIN
	dey 
	bpl l5
	lda IOSTATUS
	bne l4
 
	sta $62
	stx $63
	ldx #$90
	sec 
	jsr $bc49
	jsr $bddf
	ldx #00
l13a:
	lda $0100,x
	inx 
	beq l13
	jsr store
	jmp l13a
l13:
	jsr BUSIN
	ldx IOSTATUS
	bne l3
	cmp #$42	; "b"
	beq l14
	cmp #$22
	bne l13
l2:
	jsr BUSIN
	beq l3
	ldx IOSTATUS
	bne l3
l14:
	jsr store
	jmp l2
l3:
	lda $fb
	ora #$1f
	sta $fb
	lda #00
	jsr store
	jmp l1
l4:
	jsr CLOSEI
	lda $fb
	sec 
	sbc #<dirspace
	sta $fb
	lda $fc
	sbc #>dirspace
	sta $fc
 
	clc 
 
	ror $fc
	ror $fb
 
	ror $fc
	ror $fb
 
	ror $fc
	ror $fb
 
	ror $fc
	ror $fb
 
	ror $fc
	ror $fb
 
	lda $fb
	rts 
 
store:
	ldy #00
	sta ($fb),y
	inc $fb
	bne l12
	inc $fc
l12:
	rts 
	
dirname:
	.byte "$"

; -------------------------------------------------------------

; this segment contains all the data used in the intro

.segment "DATA"

data:

start_dir_entry:
	.byte 0

max_dir_entry:
	.byte 0

dirspace:
	.byte 0


