; -------------------------------------------------------------	
;
; vividos' last intro
; (c) 2018 vividos
;
; -------------------------------------------------------------	

; this segment contains the title of the game or demo to present.

.segment "TITLE"

	; game text under the static "presents" text; must always be 20 chars long
	.byte "     sha-jongg      "

; -------------------------------------------------------------	

; this segment contains the scroller text used in the intro.

.segment "SCROLLTEXT"

	.byte "vividos proudly presents the game that i play the most on my raspberry pi that is running retropie... "
	.byte "it's a mah-jongg clone written by stephan hradek in 1991! i'm pretty sure i typed it in with the now "
	.byte "infamous mse editor from the 64er magazine. enjoy the game as i do it, even when it has no music...   "
	.byte "intro credits: intro written by vividos; the music piece is 'blizzard' by jch.    "	
	.byte "press space or joystick 2 button to start sha-jongg...   "
	.byte 0
