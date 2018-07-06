; -------------------------------------------------------------	
;
; vividos' last intro
; (c) 2018 vividos
;
; -------------------------------------------------------------	

; this segment contains the title of the game or demo to present.

.segment "TITLE"

	; game text under the static "presents" text; must always be 20 chars long
	.byte "   hal - barbaric   "

; -------------------------------------------------------------	

; this segment contains the scroller text used in the intro.

.segment "SCROLLTEXT"

	.byte "vividos presents the demo 'barbaric' from hal. it doesn't have the most complicated "
	.byte "effects ever seen in a c64 demo, but it has a special visual appeal to me, and also "
	.byte "some great music from rob hubbard. enjoy watching!   "	
	.byte "intro credits: intro written by vividos; the music piece is 'blizzard' by JCH.    "	
	.byte "press space to start the demo...   "
	.byte 0
