; -------------------------------------------------------------	
;
; vividos' last intro
; (c) 2018 vividos
;
; -------------------------------------------------------------

; this segment contains the title of the game or demo to present.

.segment "TITLE"

	; game text under the static "presents" text; must always be 20 chars long
	.byte "   bubble bobble    "

; -------------------------------------------------------------	

; this segment contains the scroller text used in the intro.

.segment "SCROLLTEXT"

	.byte "here is another favourite game from my c64 times... bubble bobble! "
	.byte "i remember playing through the complete 99 levels with a friend (wolfi!), "
	.byte "some bag of chips and a long saturday afternoon... those times! "
	.byte "intro credits: intro written by vividos; the music piece is 'blizzard' by JCH.    "	
	.byte "press space or joystick 2 button to start the game...   "
	.byte 0
