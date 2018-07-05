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
	dec $d020
	
	inc $fc
	inc $fe
	dex
	bne stack_copy_loop

	lda #$00
	sta $d020
	
	lda #$1b
	sta $d011

	cli
	jmp PRG_START_ADDR
copy_end:
