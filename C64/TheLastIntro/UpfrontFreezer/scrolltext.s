; -------------------------------------------------------------	
;
; vividos' last intro
; (c) 2018 vividos
;
; -------------------------------------------------------------	

; this segment contains the title of the game or demo to present.

.segment "TITLE"

	; game text under the static "presents" text; must always be 20 chars long
	.byte " upfront - freezer  "

; -------------------------------------------------------------	

; this segment contains the scroller text used in the intro.

.segment "SCROLLTEXT"

	.byte "welcome to another fine demo - freezer from upfront. when i first saw "
	.byte "the demo i was amazed, since it showed columns of raster bars that i thought "
	.byte "were impossible to do. also the guys have an eye for fade-ins and -outs. "
	.byte "enjoy watching the demo! "
	.byte "intro credits: intro written by vividos; the music piece is 'blizzard' by JCH.    "	
	.byte "press space to start the demo...   "
	.byte 0
