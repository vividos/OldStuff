; -------------------------------------------------------------
;
; vividos' last intro
; (c) 2018 vividos
;
; -------------------------------------------------------------

; this segment contains the title of the game or demo to present.

.segment "TITLE"

	; game text under the static "presents" text; must always be 20 chars long
	.byte "   the last intro   "

; -------------------------------------------------------------	

; this segment contains the scroller text used in the intro.

.segment "SCROLLTEXT"

	.byte "hello and welcome to vividos' first and last intro! "
	.byte "i created this intro mainly to prove myself that i can still code in 6210 assembler, "
	.byte "and to check out if the effects that are used in all those intros and demos are really that "
	.byte "hard to do. guess what - they're not, if you have the infos about the effects, which were "
	.byte "not available at the time when i was first coding in assembler on the c64. "
	.byte "this intro features some raster bars, this scroller, and of course a wobbling logo. "
	.byte "intro credits: intro written by vividos; the music piece is 'blizzard' by jch.    "	
	.byte "press space or joystick 2 button to return to c64 basic...   "
	.byte 0
