; -------------------------------------------------------------	
;
; vividos' last intro
; (c) 2018 vividos
;
; -------------------------------------------------------------	

; constants
; PRG_START_ADDR := $0812
; NUM_BLOCKS := 58

; -------------------------------------------------------------	

; this segment contains code to copy the attached .prg file to
; the basic start at $0801 and then runs it

.segment "PAYLOAD"

copy_prg_and_start:
	sei
	lda #$00
	sta $d011   ; switch off screen

	; clear color ram, in case some decruncher puts data on the screen
	ldx #$00
	txa
clear_colorram_loop:
	sta $d800, x
	sta $d900, x
	sta $da00, x
	sta $db00, x
	dex
	bne clear_colorram_loop
	
	lda #$38
	sta $01
	
	ldx #(copy_end-copy_code)
copy_copy_code:
	lda copy_code,x
	sta $0100,x
	dex
	bpl copy_copy_code

	lda #<(copy_end + 2)
	sta $fb
	lda #>(copy_end + 2)
	sta $fc
	
	lda #$01
	sta $fd
	lda #$08
	sta $fe
	
	ldx #NUM_BLOCKS
	ldy #$00
	jmp $0100

copy_code:
stack_copy_loop:
	lda ($fb),y
	sta ($fd),y
	dey
	bne stack_copy_loop
	
	; show color bars
	dec $01
	dec $d020
	inc $01
	
	inc $fc
	inc $fe
	dex
	bne stack_copy_loop

	lda #$37
	sta $01

	lda #$00
	sta $d020
	
	lda #$1b
	sta $d011

	cli
	jmp PRG_START_ADDR
copy_end:
