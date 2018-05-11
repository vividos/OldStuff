; -------------------------------------------------------------
;
; vividos' last intro
; (c) 2018 vividos
;
; The intro is prepended to my favourite games and demos and
; should show off some C64 effects.
;
; The intro consists of the following effects:
; raster lines $30-$70: FLD side-scrolling logo
; raster line $c0: showing a "music beat" single raster line
; raster lines $cf-$e0: text scroller with raster bar
; raster lines $fd-$fe: hyperscreen; not visible in the intro
; raster lines $ff-$120?: music
;
; -------------------------------------------------------------

; constants
;DEBUG := 1

RASTERLINE_WORK := $120
RASTERLINE_FLD := $30
RASTERLINE_BEAT_LINE := $C0
RASTERLINE_SCROLLER := $cf
RASTERLINE_HYPERSCREEN_START := $f8    	; line 248
RASTERLINE_HYPERSCREEN_END := $fd  		; line 255
RASTERLINE_MUSIC := $ff

	; start of character for logo
LOGO_CHAR_START := $40
	; number of characters, starting from $40, used for logo
LOGO_CHAR_COUNT := 50

LOGO_WIDTH := 32

STATIC_TEXT_LINE := 6
STATIC_TEXT_COLUMN := (40-(static_text_end - static_text)) / 2

GAME_TEXT_LINE := 8
GAME_TEXT_MAX_COUNT := 20
SCREEN_SCROLL_LINE := 13

	; default delay counter for color rotation of static text
COLORS_STATIC_TEXT_DEFAULT_DELAY := 3

SID_REGISTER_VOLUME := $d418

; zero page register, containing pointer to current scroll text
; position
SCROLL_POS := $fb

; -------------------------------------------------------------
.segment "PRG_ADDRESS"

; hack to get start address as the .prg file position
.addr * + 2

; -------------------------------------------------------------

; this segment contains the "basic start", containing the
; following code: 2018 SYS2061
; this part is exactly $10 long and is overwritten with the
; charset when the start label is called
.segment "BASICSTART"

.word next_line + 1  ; link to next line
.word 2018           ; line number
.byte $9E            ; SYS token
.byte '2', '0', '6', '1'
next_line:
.byte 0, 0, 0        ; end of basic program

sys_start:
	jmp copy_charset

; -------------------------------------------------------------

; this segment contains the charset, starting at offset 16. the
; first 16 bytes of the charset are stored in the DATA
; segment and is restored at the start of the intro.
.segment "CHARSET"

charset:
.incbin "charset.bin", 16

; -------------------------------------------------------------

; this segment contains the charset for the logo.
.segment "LOGO_CHARSET"

logo_charset:
.incbin "logo-charset.bin"

; -------------------------------------------------------------

; this segment contains all the code used in the intro

.segment "CODE"

	; copy the charset bytes from charset_rest to $0800-$0810,
	; where the BASIC start code is located
copy_charset:
	ldx #$0f
copy_charset_loop:
	lda charset_rest,x
	sta charset - 16,x
	dex
	bpl copy_charset_loop

start:
	jsr setup_vic
	jsr setup_screen

	; initialize music
	lda #$00
	sta SID_REGISTER_VOLUME    ; volume off, for now

	lda #$00
	jsr music_init

	; set up IRQ
	sei

	; turn off CIA interrupts
	lda #$7f
	sta $dc0d   ; CIA 1
	sta $dd0d   ; CIA 2
	; cancel all CIA interrupts in queue/unprocessed
	lda $dc0d
	lda $dd0d

	; allow raster IRQs from VIC
	lda #$01
	sta $d01a

	; read from CIA 1/2 and VIC interrupt registers
	lda $dc0d
	lda $dd0d
	asl $d019

	; set up first IRQ routine
	lda #RASTERLINE_FLD
	ldx #<irq_fld
	ldy #>irq_fld
	jsr set_next_irq

	cli

	; wait for space key or joystick 2 button
wait:
	; check for space key
	lda #$7f
	sta $dc00
	lda $dc01
	cmp #$ef ; space key
	beq button_pressed

	; check for joystick 2 fire button
	lda $dc00 ; read game port 2
	and #$10  ; isolate button bit
	bne wait
	; fall-through

	; finish intro
button_pressed:

	; go to the next stage
	sei
	lda #127
	sta stage_counter
	lda #$02
	sta intro_stage
	cli

	lda intro_stage
wait_stage_3:
	cmp intro_stage
	beq wait_stage_3

	sei
	lda #$0b    ; switch off DEN register
	sta $d011

	lda #$00
	sta $d020
	sta $d021

	; prevent raster IRQs
	lda #$00
	sta $d01a

	; restore last page byte
	lda last_page_byte
	sta $3fff

	jsr $e544	; clear screen

	; reset charset to default
	lda #$15
	sta $d018

.ifdef EXIT_ACTION_SOFT_RESET
soft_reset:
	jmp $fce2
.endif

.ifdef EXIT_ACTION_START_PAYLOAD
start_payload:
	jmp payload
.endif

; -------------------------------------------------------------

	; routine: sets up VIC for intro
setup_vic:
	; black screen
	lda #$00
	sta $d020
	sta $d021

	; set charset at $0800, screen at $0400
	lda #(1* 16 + 1 * 2)
	sta $d018

	; set VIC control registers
	lda #$1b    ; scrolling: 3, 25 lines, VIC on, text mode
	sta $d011   ; RSEL=1, bit 3, needed for hyperscreen

	lda #$08    ; scrolling: 0, 40 columns, no multicolor
	sta $d016

	; multicolor colors for logo
	lda #$0e
	sta $d022

	lda #$06
	sta $d023

	; remember last page byte that would be visible in FLD and open borders
	lda $3fff
	sta last_page_byte

.ifdef DEBUG
	; use a visible pattern in debug mode
	lda #$aa
.else
	lda #$00
.endif
	sta $3fff

	rts

; -------------------------------------------------------------

	; routine: sets up screen for intro
setup_screen:

.ifndef DEBUG
	jsr $e544	; clear screen
.endif

	ldy #39
	lda #$09
logo_colorram_loop:
	sta $d800, y
	sta $d800 + 40, y
	sta $d800 + 80, y
	sta $d800 + 120, y
	dey
	bpl logo_colorram_loop

	; set up static text
	lda #COLORS_STATIC_TEXT_DEFAULT_DELAY
	sta colors_static_text_delay

	ldy #(static_text_end - static_text - 1)
static_text_char_loop:
	lda static_text,y
	and #$1f
	sta $0400 + STATIC_TEXT_LINE * 40 + STATIC_TEXT_COLUMN, y
	dey
	bpl static_text_char_loop

	; TODO create double-sized chars


	; set up scroll line
	lda #<scroll_text
	sta SCROLL_POS
	lda #>scroll_text
	sta SCROLL_POS + 1

	; set first character
	ldy #$00
	lda (SCROLL_POS),y
	and #$3f
	sta $0400 + 40 * SCREEN_SCROLL_LINE + 39

	; prepare color RAM
	ldy #39
	lda #$00
screen_colorram_loop:
	sta $d800 + 40 * SCREEN_SCROLL_LINE, y
	dey
	bpl screen_colorram_loop

	rts

; -------------------------------------------------------------

	; IRQ routine: work and screen initialisation
irq_work:
	inc $d019 ; clear VIC raster flag

	; init Y SCROLL for FLD
	lda #$1b    ; scrolling: 3, 25 lines, VIC on, text mode
	sta $d011   ; RSEL=1, bit 3, needed for hyperscreen

.ifdef DEBUG
	lda #$03	; cyan
	sta $d020
	sta $d021
.endif

	; stage handling: in stage 0 and 2, we count down the stage
	; counter, on which some effects are based; when it reaches
	; zero, switch to the next stage
	lda intro_stage
	cmp #$01
	beq no_next_stage

	; reduce stage 0 and 2 counter
	dec stage_counter
	bne no_next_stage

	inc intro_stage
no_next_stage:

	; update logo scrolling
	jsr update_logo_scroll_x
	jsr update_logo_scroll_y

	jsr update_static_text

	; update text scroller
	jsr update_scroll_pos

end_work_stages:

.ifdef DEBUG
	lda #$00
	sta $d020
	sta $d021
.endif

	; set up next IRQ
	lda #RASTERLINE_FLD
	ldx #<irq_fld
	ldy #>irq_fld
	jsr set_next_irq

	jmp $ea81

; -------------------------------------------------------------

	; routine: updates scroll X position of logo and when logo
	; has moved, copies logo to screen again.
update_logo_scroll_x:

	ldx intro_stage
	beq update_logo_scroll_x_stage0
	dex
	beq update_logo_scroll_x_stage1

update_logo_scroll_x_stage2:
	; stage 2: add last used x scroll position, which isn't
	; updated anymore, and add the stage counter to it
	lda stage_counter
	eor #$7f
	asl

	ldx logo_scroll_current_x_index
	clc
	adc logo_scroll_x_table, x
	jmp update_logo_scroll_x_calc_values

update_logo_scroll_x_stage0:
	; stage 0: stage counter goes from 127 to 0; use it to
	; scroll in the logo from the left
	lda stage_counter
	eor #$7f
	asl
	bpl update_logo_scroll_x_calc_values

update_logo_scroll_x_stage1:
	; increase index into x table
	inc logo_scroll_current_x_index
	lda logo_scroll_current_x_index
	and #$7f
	sta logo_scroll_current_x_index

	; get new value from table
	tax
	lda logo_scroll_x_table,x

update_logo_scroll_x_calc_values:
	tay       ; save value in y
	; calculate soft scroll x value
	and #$07
	eor #$07
	sta logo_scroll_x_softscroll_value

	; calc value of whole characters to move
	tya
	lsr      ; divide by 8
	lsr
	lsr

	; check if it has changed
	cmp logo_scroll_x_value
	sta logo_scroll_x_value		; and store; doesn't modify the Z flag

	beq logo_scroll_x_no_update	; value hasn't changed

	; screen update is needed
	; x contains the logo screen index; if negative or over LOGO_WIDTH,
	; no char has to be copied
	lda #0
	sec
	sbc logo_scroll_x_value
	tax

	; y contains the screen column, 0 to 39
	ldy #$00

logo_char_loop:
	; check if outside of the logo
	cpx #$00
	bmi logo_outside
	cpx #LOGO_WIDTH
	bmi logo_inside

logo_outside:
	lda #($40 + $20)
	sta $0400 + 0 * 40,y
	sta $0400 + 1 * 40,y
	sta $0400 + 2 * 40,y
	sta $0400 + 3 * 40,y

	jmp logo_copy_loop_end
logo_inside:
	lda logo_screen,x
	ora #$40
	sta $0400,y

	lda logo_screen + 1 * $20,x
	ora #$40
	sta $0400 + 1 * 40,y

	lda logo_screen + 2 * $20,x
	ora #$40
	sta $0400 + 2 * 40,y

	lda logo_screen + 3 * $20,x
	ora #$40
	sta $0400 + 3 * 40,y

logo_copy_loop_end:
	inx
	; loop for all screen columns
	iny
	cpy #40
	bmi logo_char_loop

logo_scroll_x_no_update:
	rts

; -------------------------------------------------------------

	; routine: updates scroll Y position of logo
	; in stage 0 and 2, the logo is scrolled in from the left,
	; or scrolled out to the right, and no Y position change
	; occurs; in stage 1, the logo is moved according to the Y
	; table
update_logo_scroll_y:

	ldx intro_stage
	dex
	bne update_logo_scroll_y_exit

	; stage 1
	; calc next number of FLD lines
	inc logo_scroll_current_y
	lda logo_scroll_current_y
	and #$7f
	sta logo_scroll_current_y

	; copy over to fld_num_lines
	ldx logo_scroll_current_y
	lda logo_scroll_y_table,x
	sta fld_num_lines

update_logo_scroll_y_exit:
	rts

; -------------------------------------------------------------

	; routine: updates static text color RAM

update_static_text:
	; move static text color ram
	ldy #(static_text_end - static_text - 1)
static_text_color_loop:
	lda colors_static_text, y
	sta $d800 + STATIC_TEXT_LINE * 40 + STATIC_TEXT_COLUMN, y
	dey
	bpl static_text_color_loop

	; in stage 0, set first characters to black, in order to fade in
	ldx intro_stage
	beq update_static_text_stage0
	cpx #$02
	bne update_static_text_check_rotate

update_static_text_stage2:
	; stage 2: overwrite first characters, based on the stage counter
	lda stage_counter
	eor #$7f
	lsr
	lsr
	lsr
	lsr
	tay
	lda #$00
update_static_text_stage2_loop:
	sta $d800 + STATIC_TEXT_LINE * 40 + STATIC_TEXT_COLUMN, y
	dey
	bpl update_static_text_stage2_loop
	bmi update_static_text_check_rotate

update_static_text_stage0:
	; stage 0: overwrite last characters, based on the stage counter
	lda stage_counter
	eor #$7f
	lsr
	lsr
	lsr
	lsr
	tay
	lda #$00
update_static_text_stage0_loop:
	sta $d800 + STATIC_TEXT_LINE * 40 + STATIC_TEXT_COLUMN, y
	iny
	cpy #(static_text_end - static_text)
	bmi update_static_text_stage0_loop

	; check if colors should be rotated, when delay count has reached zero
update_static_text_check_rotate:
	dec colors_static_text_delay
	bne colors_static_text_no_rotate

	lda #COLORS_STATIC_TEXT_DEFAULT_DELAY
	sta colors_static_text_delay

	ldx colors_static_text_end - 1
	ldy #(colors_static_text_end - colors_static_text - 2)
static_text_color_rotate_loop:
	lda colors_static_text, y
	sta colors_static_text + 1, y
	dey
	bpl static_text_color_rotate_loop

	stx colors_static_text

colors_static_text_no_rotate:
	rts

; -------------------------------------------------------------

	; IRQ routine: FLD for logo
	; FLD: Flexible Line Distance
	; see http://www.0xc64.com/2015/11/17/simple-fld-effect/
irq_fld:
	inc $d019 ; clear VIC raster flag

.ifdef DEBUG
	lda #$02
	sta $d020
	sta $d021
.endif

	; modify scroll register
	lda $d016
	sta saved_scroll_x

	and #$f8
	ora logo_scroll_x_softscroll_value   ; set soft scroll
	eor #$07	; negate
	ora #$10    ; turn on multicolor
	sta $d016

	; we have $1b in $d011, that means Y SCROLL is 3
	; bad lines occur when Y SCROLL == lower 3 bits of $d012
	; so wait for appropriate raster line

	lda #RASTERLINE_FLD + 1
fld_wait_line:
	cmp $d012
	bne fld_wait_line

	; loop, for number of FLD lines
	ldx	fld_num_lines
	beq fld1_finished

	jsr fld

fld1_finished:

	; calculate number of FLD lines below the logo
	lda #$40
	sec
	sbc fld_num_lines
	tax

	; calculate next rasterline
	lda $d012
	clc
	adc #$20

	; wait for it
fld_wait_after_logo:
	cmp $d012
	bne fld_wait_after_logo

	; now do some more FLD lines, number of lines is already in x
	jsr fld

.ifdef DEBUG
	lda #$00
	sta $d020
	sta $d021
.endif

	; restore scroll X and turn off multicolor from logo
	lda saved_scroll_x
	sta $d016

	; set up next IRQ
	lda #RASTERLINE_BEAT_LINE
	ldx #<irq_beat_line
	ldy #>irq_beat_line
	jsr set_next_irq

	jmp $ea81

; -------------------------------------------------------------

	; routine: FLD
	; called with the X register containing the number of
	; lines to prevent the bad line condition

fld:
fld_next_line:
	; wait for current line to finish
	lda $d012
fld_wait_line_finish:
	cmp $d012
	beq fld_wait_line_finish

	; do FLD effect: prevent a bad line by incrementing
	; Y SCROLL by 1 every raster line
	lda $d011
	clc
	adc #1
    and #7
    ora #$18
    sta $d011

	dex
	bne fld_next_line
	rts

; -------------------------------------------------------------

	; IRQ routine: show music beat line
irq_beat_line:
	inc $d019

	lda intro_stage
	cmp #$02
	beq no_beat_line

	; get beat value
	lda $d412    ; actually control register voice 3

	; wait for some raster lines
	ldy #RASTERLINE_BEAT_LINE + 4
	jsr waitline2

	; change color to beat value
	sta $d020
	sta $d021

	; wait for some cycles
	ldx #$09
beat_delay:
	dex
	bne beat_delay

	; set color back to black
	stx $d021
	stx $d020
no_beat_line:

	; set up next IRQ
	lda #RASTERLINE_SCROLLER
	ldx #<irq_text_scroller
	ldy #>irq_text_scroller
	jsr set_next_irq

	jmp $ea81

; -------------------------------------------------------------

	; IRQ routine: text scroller; moves the VIC X Scroll
	; register and updates the scroll text when necessary
	; also displays raster lines
irq_text_scroller:
	inc $d019

	; set x scroll register
	lda $d016
	sta saved_scroll_x

	and #$f8       ; clear bits 0-2
	ora scroll_x
	eor #$07       ; invert to get correct X scroll value
	sta $d016

	; wait for exact line
	ldy #RASTERLINE_SCROLLER + 1
	jsr waitline2

	ldx #$00
scroll_rasterbar_loop:
	lda rasterbar_scroll,x
	sta $d020
	sta $d021

	; delay by timing table
	; when timing = 8, we delay by 4 + 7 * (2 + 5) + (2 + 2) = 43 cycles
	ldy rasterbar_timing,x		  ; 4 cycles
	beq scroll_nowait
scroll_rasterbar_delay:
	dey                           ; 2 cycles
	bne scroll_rasterbar_delay    ; 2 cycles, 3 when y=0
	;nop                           ; 2 cycles

scroll_nowait:
	bit $ea                       ; 3 cycles
	inx                           ; 2 cycles
	cpx #rasterbar_scroll_end - rasterbar_scroll    ; 2 cycles
	bne scroll_rasterbar_loop     ; 2 cycles, 3 when exiting loop


	; restore scroll register
	lda saved_scroll_x
	sta $d016

	; set up next IRQ
	lda #RASTERLINE_HYPERSCREEN_START
	ldx #<irq_hyperscreen
	ldy #>irq_hyperscreen
	jsr set_next_irq

	jmp $ea81

; -------------------------------------------------------------

	; sub routine; updates scroll_x, and scroll_pos when a new
	; character is needed. the scroll speed is dynamic.
update_scroll_pos:
	; update scroll_x, scroll text when necessary
	lda scroll_x
	clc
	adc scroll_speed
	cmp #$08
	bpl scroll_update
	sta scroll_x
	rts

scroll_update:
	tax           ; save new scroll_x value to x
	and #$07      ; limit to 0-7
	sta scroll_x

	txa           ; get new scroll_x value
	clc
	lsr           ; divide by 8
	lsr
	lsr
	tax           ; save "chars to scroll x"

	; move current text to the left
	ldy #00
scroll_copy_loop:
	lda $0400 + 40 * SCREEN_SCROLL_LINE + 1, y
	sta $0400 + 40 * SCREEN_SCROLL_LINE, y
	iny
	cpy #40
	bmi scroll_copy_loop

	; update scroll position pointer
	txa             ; get "chars to scroll" value
	clc
	adc SCROLL_POS  ; add to scroll pos
	sta SCROLL_POS
	lda #$00
	adc SCROLL_POS + 1 ; add carry to high word
	sta SCROLL_POS + 1

	; get new character; ugly self-modifying-code hack
scroll_get_char:
	ldy #$00
	lda (SCROLL_POS),y
	and #$3f

	; was 0 character ?
	bne no_scroller_reset

	; reset scroller text to start
	lda #<scroll_text
	sta SCROLL_POS
	lda #>scroll_text
	sta SCROLL_POS + 1
	jmp scroll_get_char

no_scroller_reset:
	; store as next character
	sta $0400 + 40 * SCREEN_SCROLL_LINE + 39

	lda intro_stage
	cmp #$02
	bne update_scroll_text_no_fadeout

	; stage 2: fade out scroll text by overwriting color RAM
	lda stage_counter
	eor #$7f
	lsr
	tay

	lda #$01    ; color white
update_scroll_text_color_stage2_loop:
	sta $d800 + 40 * SCREEN_SCROLL_LINE, y
	dey
	bpl update_scroll_text_color_stage2_loop

update_scroll_text_no_fadeout:
	rts

; -------------------------------------------------------------

	; IRQ routine: hyperscreen; opens upper and lower border
	; see VIC article chapter 3.14.1. HYPERSCREEN
irq_hyperscreen:
	inc $d019

.ifdef DEBUG
	lda #$0b
	sta $d020
	sta $d021
.endif

	; clear RSEL bit
	lda $d011
	and #$f7
	sta $d011

	lda #RASTERLINE_HYPERSCREEN_END
wait_rasterline_hyperscreen_end:
	cmp $d012
	bne wait_rasterline_hyperscreen_end

	; set RSEL bit again
	lda $d011
	ora #$08
	sta $d011

.ifdef DEBUG
	lda #$0b
	sta $d020
	sta $d021
.endif

	; set up next IRQ
	lda #RASTERLINE_MUSIC
	ldx #<irq_music
	ldy #>irq_music
	jsr set_next_irq

	jmp $ea81

; -------------------------------------------------------------

	; IRQ routine: play music
irq_music:
	inc $d019

.ifndef DEBUG
	lda #$00
	jsr music_init + 3
.endif

	lda intro_stage
	cmp #$02
	bne irq_music_no_music_fadeout

	; use stage counter as fadeout
	lda stage_counter
	lsr    ; divide by 8, to be in range 0..15
	lsr
	lsr
	lsr
	sta SID_REGISTER_VOLUME

irq_music_no_music_fadeout:

	; set up next IRQ
	lda #<RASTERLINE_WORK
	ldx #<irq_work
	ldy #>irq_work
	jsr set_next_irq
	lda $d011
	ora #$80
	sta $d011

	jmp $ea81

; -------------------------------------------------------------

	; sub routine; sets up next IRQ
	; note: set interrupt flag before calling!
	; register A: next rasterline; only raster lines $00-$ff are supported
	; register X: low byte of IRQ routine
	; register Y: high byte of IRQ routine
set_next_irq:
	sta $d012

	; clear 8th bit
	lda $d011
	and #$7f
	sta $d011

	stx $0314 ; new IRQ handler
	sty $0315
	rts

; -------------------------------------------------------------

.if 0
	; sub routine; waits for rasterline given in Y register
	; the routine is cycle-exact and needs 2 additional raster lines
	; the A register is not modified and can be used
waitline:
	; wait for given raster line
	cpy $d012		; 4 cycles
	bne waitline    ; 2 cycles, 3 when branch taken

	; now we have a jitter of up to 5 cycles, since $d012 was read in
	; cycle 3 of the cpy, and the bne adds 2 cycles

	; wait for 2 + 9 * (2 + 3) + (2 + 2) = 51 cycles
	ldx #10			; 2 cycles
delay1:
	dex				; 2 cycles
	bne delay1      ; 2 cycles, 3 when branch taken

	; wait for next raster line
	iny             ; 2 cycles
	cpy $d012       ; 4 cycles
	beq skip1       ; 2 cycles, 3 when branch taken

	; skip 5 cycles
	bit $ea
	nop

skip1:
	; wait for 2 + 8 * (2 + 3) + (2 + 2) + 2 + 2 = 50 cycles
	ldx #9			; 2 cycles
delay2:
	dex				; 2 cycles
	bne delay2		; 2 cycles, 3 when branch taken
	nop				; 2 cycles
	nop				; 2 cycles

	; check for next raster line, 2 + 4 + (2 or 3) + 3 = 11 or 12 cycles
	iny				; 2 cycles
	cpy $d012		; 4 cycles
	nop				; 2 cycles
	beq skip2		; 2 cycles, 3 when branch taken
	cmp $00			; 3 cycles

skip2:
	; wait for 2 + 9 * (2 + 3) + (2 + 2) = 51 cycles
	ldx #10			; 2 cycles
delay3:
	dex				; 2 cycles
	bne delay3		; 2 cycles, 3 when branch taken

	; check for next raster line, 2 + 4 + (2 or 3) = 8 or 9 cycles
	iny				; 2 cycles
	cpy $d012		; 4 cycles
	bne skip3		; 2 cycles, 3 when branch taken
skip3:
	rts				; 6 cycles
.endif

; -------------------------------------------------------------

	; sub routine; waits for rasterline given in Y register
	; the routine is cycle-exact and needs 2 additional raster lines
	; the A register is not modified and can be used
waitline2:
	cpy $d012
	bne waitline2
	jsr cycles
	bit $ea
	nop
	cpy $d012
	beq wl_skip1
	nop
	nop
wl_skip1:
	jsr cycles
	bit $ea
	nop
	cpy $d012
	beq wl_skip2
	bit $ea
wl_skip2:
	jsr cycles
	nop
	nop
	nop
	cpy $d012
	bne onecycle
onecycle:
	rts

cycles:
         ldx #$06
lp2:     dex
         bne lp2
         iny
         nop
         nop
         rts
; -------------------------------------------------------------

.segment "MUSIC"
music_init:
.incbin "music.bin"

; -------------------------------------------------------------

; this segment contains all the data for the logo

.segment "LOGO_DATA"

logo_scroll_x_table:
	.incbin "logo-scroll-x.bin"

logo_scroll_y_table:
	.incbin "logo-scroll-y.bin"

	; this contains the screen bytes for the logo. Together
	; with the logo it forms the logo graphic used. The logo is 32
	; chars wide and 4 lines high, totalling in $80 bytes. As the
	; charset starts at $0e00, not $0800, add #$c0 to the screen
	; bytes to get the correct logo.
logo_screen:
	.incbin "logo-screen.bin"

; -------------------------------------------------------------

; this segment contains the payload of the intro that is to be
; started after finishing.

.segment "PAYLOAD"

payload:

; -------------------------------------------------------------

; this segment contains all the data used in the intro

.segment "DATA"

; data values needed for intro
data:

	; the stages of the intro
	; 0: the intro is starting, the logo is moved in and the texts
	;    are faded in
	; 1: the intro is running normally
	; 2: space or joystick 2 button was pressed, the intro is fading out
intro_stage:
	.byte 0

stage_counter:
	.byte 127

fld_num_lines:
	.byte 0   	; number of lines to move down in FLD effect

logo_scroll_x_value:
	.byte 0
logo_scroll_x_softscroll_value:
	.byte 0

scroll_speed:
	.byte 1   ; speed in 1/8th per character; can't be higher than 8!

scroll_x:
	.byte 0

saved_scroll_x:
	.byte 0

static_text:
	.byte "presents"
static_text_end:

colors_static_text:
	.byte 6, 4, 14, 3, 7, 1, 7, 10, 8, 2, 9
colors_static_text_end:

colors_static_text_delay:
	.byte 0

	; game text under the static "presents" text; must always be 20 chars long
game_text:
	.byte "GIANA SISTERS"
	.byte 0, 0, 0, 0, 0, 0, 0

rasterbar_scroll:
	.byte 0, 6, 4, 14, 13, 7           ; black, blue, purple, l.blue, l.green, yellow
	.byte 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ; white
	.byte 7, 10, 8, 2, 9, 0            ; yellow, l.red, orange, red, brown, black
rasterbar_scroll_end:

	; timing table for rasterbar_scroll
rasterbar_timing:
	.byte 7, 7, 7, 7, 7, 7, 7, 0
	.byte 7, 7, 7, 7, 7, 7, 7, 0
	.byte 7, 7, 7, 7, 7, 7

last_page_byte:
	.byte 0

start_game:
	.word $2000

logo_scroll_current_x_index:
	.byte 0

logo_scroll_current_y:
	.byte 0
charset_rest:
	.incbin "charset.bin", 0, 16

	; scroll text must be the last symbol in the file; the linker puts
	; the scroll text next to this symbol
scroll_text:
;	.byte 0
