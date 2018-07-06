; -------------------------------------------------------------	
;
; vividos' last intro
; (c) 2018 vividos
;
; -------------------------------------------------------------	

; this segment contains the title of the game or demo to present.

.segment "TITLE"

	; game text under the static "presents" text; must always be 20 chars long
	.byte "   giana sisters    "

; -------------------------------------------------------------	

; this segment contains the scroller text used in the intro.

.segment "SCROLLTEXT"

scroll_text:
	.byte "vividos proudly presents: the great giana sisters!   "
	.byte "intro credits: intro written by vividos; the music piece is 'blizzard' by JCH.    "	
	.byte "press space or joystick 2 button to start the game...   "
	.byte 0
