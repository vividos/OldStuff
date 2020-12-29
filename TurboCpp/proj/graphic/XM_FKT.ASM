; xm_fkt.asm - assembler routines for the xmode
;
; compile with memory model large
;

.286

  assume CS:_text, DS:DGROUP, ES:DGROUP, SS:DGROUP

IGROUP group _text                ;Zusammenfassung der Programm-Segmente
DGROUP group _BSS,  _DATA         ;Zusammenfassung der Daten-Segmente

_BSS   segment word public 'BSS'  ;dieses Segment nimmt alle nicht ini-
_BSS   ends                       ;tialisierten statischen Variablen auf

_DATA  segment word public 'DATA' ;alle initialisierten globalen und
				  ;statischen Variablen werden in diesem
				  ;Segment untergebracht

_DATA  ends


;== Konstanten =========================================================

SC_INDEX       = 3c4h             ;Indexregister des Sequencer-Ctrl.
SC_MAP_MASK    = 2                ;Nummer des Map-Mask-Registers
SC_MEM_MODE    = 4                ;Nummer des Memory-Mode-Registers

GC_INDEX       = 3ceh             ;Indexregister des Graphics-Ctrl.
GC_READ_MAP    = 4                ;Nummer des Read-Map-Registers
GC_GRAPH_MODE  = 5                ;Nummer des Graphics-Mode-Registers
GC_MISCELL     = 6                ;Nummer des Miscellaneous-Registers

CRTC_INDEX     = 3d4h             ;Indexregistes des CRT-Controllers
CC_MAX_SCAN    = 9                ;Nummer des Maximum-Scan-Line-Reg.
CC_START_HI    = 0Ch              ;Nummer des Hi-Start-Registers
CC_UNDERLINE   = 14h              ;Nummer des Underline-Registers
CC_MODE_CTRL   = 17h              ;Nummer des Mode-Control-Registers

DAC_WRITE_ADR  = 3C8h             ;DAC-Write-Adresse
DAC_READ_ADR   = 3C7h             ;DAC-Read-Adresse
DAC_DATA       = 3C9h             ;DAC-Datenregister

VERT_RETRACE   = 3DAh             ;Input Status-Register #1

PIXX           = 320              ;horizontale Auflîsung


;== Daten ==============================================================

_DATA  segment word public 'DATA'

public _vio_seg

_vio_seg    dw 0a000h              ;Video-Segment mit akt. Seite

_DATA  ends


;== Programm ===========================================================

_text  segment byte public 'CODE' ;das Programmsegment

;-- Public-Deklarationen -----------------------------------------------


public    _init320200             ;320*200-Punkte-Modus initialisieren
public    _exitvideo              ;stellt den Textmode wieder her
public    _wait_retrace           ;wartet auf vertikalen retrace
public    _setpage                ;Seite fÅr setpix und getpix festlegen
public    _showpage               ;zeigt eine Seite des Bildschirms an
public    _switchpage             ;schaltet zwischen Seite 0 und 1 um
public    _setpix                 ;Punkt setzen
public    _getpix                 ;Farbe eines Punktes ermitteln
public    _getfontptr             ;liefert Zeiger auf 8*8-Zeichensatz

public    _clearpage              ;lîscht angegebene page
public    _pagemove               ;moved page # auf aktuelle _vio_seg

public    _line_copy              ;kopiert ein byte-array auf den screen
public    _blockmove              ;bewegt blîcke


;-- INIT320200: initialisiert den 320*200 Punkte-Modus -----------------
;-- Deklaration : void init320200( void );

_init320200 proc far

	   ;-- zunÑchst ganz normal den Modus 13h einstellen, damit das
	   ;-- BIOS die meiste Arbeit der Initialisierung Åbernimmt.
	   ;-- Danach die Register verÑndern, die sich noch nicht in
	   ;-- der gewÅnschten Stellung befinden.

	   mov   ax,0013h         ;normalen Modus 13h einstellen
	   int   10h

	   mov   dx,GC_INDEX      ;Auftrennung der Speicheradressen

	   mov   al,GC_GRAPH_MODE ;im Graphics-Mode-Register des
	   out   dx,al            ;Gaphics-Controllers Åber das
	   inc   dx               ;Bit #4 abschalten
	   in    al,dx
	   and   al,11101111b
	   out   dx,al
	   dec   dx

	   mov   al,GC_MISCELL    ;und das Ganze noch einmal im
	   out   dx,al            ;Miscellanous-Register Åber das
	   inc   dx               ;Bit #1
	   in    al,dx
	   and   al,11111101b
	   out   dx,al

	   mov   dx,SC_INDEX      ;das Memory-Mode-Register im
	   mov   al,SC_MEM_MODE   ;Sequencer-Controlller dahingehend
	   out   dx,al            ;modifizieren, da· keine Auftrennung
	   inc   dx               ;der Adressen in verschiedene Bit-
	   in    al,dx            ;Planes mehr erfolgt und die aktuelle
	   and   al,11110111b     ;Plane Åber das Bit-Mask-Register
	   or    al,4             ;festgelegt wird
	   out   dx,al

	   mov   ax,_vio_seg       ;alle vier Bit-Planes komplett mit
	   mov   es,ax            ;dem Farbcode 00h fÅllen und damit
	   xor   di,di            ;den Bildschirm zunÑchst lîschen
	   mov   ax,di
	   mov   cx,8000h
	   rep   stosw

	   mov   dx,CRTC_INDEX    ;Doppelwort-Modus Åber das Bit 6
	   mov   al,CC_UNDERLINE  ;im Underline-Register des CRT-
	   out   dx,al            ;Controllers abstellen
	   inc   dx
	   in    al,dx
	   and   al,10111111b
	   out   dx,al
	   dec   dx

	   mov   al,CC_MODE_CTRL  ;Åber das Bit 6 im Mode-Control-
	   out   dx,al            ;Register des CRT-Controllers jetzt
	   inc   dx               ;vom Word- in den Byte-Modus wechseln
	   in    al,dx
	   or    al,01000000b
	   out   dx,al

	   retf                   ;zurÅck zum Aufrufer

endp


;-- EXITVIDEO: stellt Bildschirmmode 3 ein
;--

_exitvideo proc far

	   mov ax,0003h
	   int 10h

	   retf

endp


;-- WAIT_RETRACE: wartet auf den vertikalen retrace des Bildschirms
;--

_wait_retrace proc far
	   mov   dx,VERT_RETRACE
sp3:
	   in    al,dx
	   test  al,8
	   jne   sp3
sp4:
	   in    al,dx
	   test  al,8
	   je    sp4

	   retf
endp


;-- SETPAGE: WÑhlt die Seite aus, auf die bei Funktionsaufrufen von ----
;--          setpix und getpix zugegriffen werden soll
;-- Deklaration : void setpage( unsigend char page );

_setpage   proc far
ARG page1a:word

	   push  bp
	   mov   bp,sp
	   mov   cx,page1a

	   mov   al,4             ;Hi-Byte der Segadr = Seite * 4 + A0h
	   mul   cl
	   or    al,0A0h

	   mov   byte ptr _vio_seg + 1,al ;neue Segmentadresse merken

	   pop   bp
	   retf                   ;zurÅck zum Aufrufer

endp                   ;Ende der Prozedur


;-- SHOWPAGE: Bringt eine der beiden Bildschirmseiten zur Anzeige ------
;-- Deklaration : void showpage( unsigend char page );

_showpage  proc far
ARG page1b:word

	   push  bp
	   mov   bp,sp
	   mov   cx,page1b


	   mov   al,64            ;Hi-Byte des Offsets = Seite * 64
	   mul   cl
	   mov   ah,al            ;Hi-Byte des Offsets nach AH

	   ;-- jetzt die neue Startadresse laden -----------------------

	   mov   dx,CRTC_INDEX    ;CRT-Controller adressieren
	   mov   al,CC_START_HI   ;Registernummer nach AL
	   out   dx,ax            ;und ab gehts

	   ;-- wieder auf den Start eines Bildschirmaufbaus warten -----

	   call _wait_retrace

	   pop   bp
	   retf                   ;zurÅck zum Aufrufer

endp                   ;Ende der Prozedur


;-- SWITCHPAGE: Schaltet zwischen Seite 0 und 1 um
;--

_switchpage proc far
	   mov   ax,_vio_seg
	   push  ax
	   xor   ah,04h
	   mov   _vio_seg,ax
	   pop   ax
	   shl   ah,4

	   mov   dx,CRTC_INDEX    ;CRT-Controller adressieren
	   mov   al,CC_START_HI   ;Registernummer nach AL
	   out   dx,ax            ;und ab gehts

	   call  _wait_retrace
	   retf
endp


;-- SETPIX: verleiht einem Punkt eine bestimmte Farbe ------------------
;-- Deklaration : void setpix( int x, int y, unsigned char farbe );

_setpix    proc far

sframe     struc                  ;Struktur zum Zugriff auf den Stack
bp0        dw ?                   ;nimmt BP auf
ret_adr0   dw ?                   ;RÅcksprungadresse zum Aufrufer
ret_adr0b  dw ?
x0         dw ?                   ;X-Ordinate
y0         dw ?                   ;Y-Ordinate
farbe      dw ?                   ;Farbe
sframe     ends                   ;Ende der Struktur

frame      equ [ bp - bp0 ]       ;adressiert die Elemente der Struktur

	   push  bp               ;Adressierung der Parameter Åber
	   mov   bp,sp            ;das BP-Register vorbereiten

	   push  di               ;DI auf dem Stack sichern

	   mov   ax,PIXX / 4      ;Offset in den Video-RAM berechnen
	   mul   frame.y0         ;und nach DI laden
	   mov   cx,frame.x0
	   mov   bx,cx
	   shr   bx,1
	   shr   bx,1
	   add   ax,bx
	   mov   di,ax

	   and   cl,3             ;Bit-Maske fÅr die zu adressierende
	   mov   ah,1             ;Map in AH berechnen
	   shl   ah,cl
	   mov   al,SC_MAP_MASK   ;nach AL die Registernummer
	   mov   dx,SC_INDEX      ;Indexadresse des Sequencers laden
	   out   dx,ax            ;Bit-Mask-Register laden

	   mov   ax,_vio_seg       ;ES auf den Video-RAM setzen
	   mov   es,ax
	   mov   al,byte ptr frame.farbe  ;Farbe des Punktes laden
	   stosb                  ;und in die gewÑhlte Bit-Map schreiben

	   pop   di               ;Register wieder vom Stack holen
	   pop   bp

	   retf                   ;zurÅck zum Aufrufer

endp                   ;Ende der Prozedur


;-- GETPIX: liefert die Farbe eines Punktes ----------------------------
;-- Deklaration : unsigned char getpix( int x, int y );

_getpix    proc far

sframe1    struc                  ;Struktur zum Zugriff auf den Stack
bp1        dw ?                   ;nimmt BP auf
ret_adr1   dw ?                   ;RÅcksprungadresse zum Aufrufer
ret_adr1b  dw ?
x1         dw ?                   ;X-Ordinate
y1         dw ?                   ;Y-Ordinate
sframe1    ends                   ;Ende der Struktur

frame      equ [ bp - bp1 ]       ;adressiert die Elemente der Struktur

	   push  bp               ;Adressierung der Parameter Åber
	   mov   bp,sp            ;das BP-Register vorbereiten

	   push  si               ;SI auf dem Stack sichern

	   mov   ax,PIXX / 4      ;Offset in den Video-RAM berechnen
	   mul   frame.y1         ;und nach SI laden
	   mov   si,frame.x1
	   mov   cx,si
	   shr   si,1
	   shr   si,1
	   add   si,ax

	   and   cl,3             ;Bit-Maske fÅr die zu adressierende
	   mov   ah,cl            ;Map in AH berechnen
	   mov   al,GC_READ_MAP   ;nach AL die Registernummer
	   mov   dx,GC_INDEX      ;Indexadresse des Graphic-Ctrl laden
	   out   dx,ax            ;Read-Map-Register laden

	   mov   ax,_vio_seg       ;ES auf den Video-RAM setzen
	   mov   es,ax
	   mov   al,es:[si]       ;Farbe des Punktes laden

	   pop   si               ;Register wieder vom Stack holen
	   pop   bp

	   retf                   ;zurÅck zum Aufrufer

endp                   ;Ende der Prozedur


;-- GETFONTPTR: liefert FAR-Pointer auf 8*8 Zeichentabelle -------------
;-- Deklaration : void far * getfontptr( void )

_getfontptr proc far

          push  bp                ;BP sichern

	  mov   ax,1130h          ;Register fÅr Funktionsaufruf laden
	  mov   bh,3
	  int   10h               ;BIOS-Video-Interrupt aufrufen

	  mov   dx,es             ;Zeiger ES:BP in DX:AX zurÅckliefern
	  mov   ax,bp

	  pop   bp                ;BP vom Stack zurÅckholen
	  retf                    ;zurÅck zum Aufrufer

endp                  ;Ende der Prozedur


;-- CLEARPAGE: loescht angegebene Seite im Video-Ram
;-- Deklaration : void clearpage(unsigned char page);

_clearpage proc far
ARG page2a:word

	  push  bp
	  mov   bp,sp

	  push  di
	  cld                    ;bei Stringbefehlen aufwÑrts

	  mov   dx,GC_INDEX      ;aktuellen Write-Mode holen und
	  mov   al,GC_GRAPH_MODE ;Write-Mode 1 einstellen
	  out   dx,al
	  inc   dx
	  in    al,dx
	  push  ax               ;aktuellen Modus auf Stack merken
	  and   al,not 3
; Fehler! wenn write-mode 1 eingestellt wird, fÅhrt das zu streifen im bild
;	  or    al,0
	  out   dx,al

	  mov   dx,SC_INDEX      ;Zugriff auf alle Bit-Planes sichern
	  mov   ah,0Fh
	  mov   al,SC_MAP_MASK
	  out   dx,ax

	  mov   cx,page2a        ;Segmentadresse der Seite berechnen
	  shl   cx,10
	  or    ch,0a0h
	  mov   es,cx
	  xor   di,di

	  mov ax,di
	  xor ax,ax
	  mov cx,16000
	  rep stosb

	  mov   dx,GC_INDEX      ;wieder den alten Write-Mode ein-
	  pop   ax               ;stellen
	  mov   ah,al
	  mov   al,GC_GRAPH_MODE
	  out   dx,ax

	  pop di

	  pop bp
	  retf

endp


;-- PAGEMOVE: bewegt eine ganze Bildschirmseite auf eine andere
;-- Deklaration : void pagemove(unsigned char page);

_pagemove proc far
ARG page3a:word

	  push  bp
	  mov   bp,sp

	  push di          ;register di und si merken
	  push si

	  mov   dx,GC_INDEX      ;aktuellen Write-Mode holen und
	  mov   al,GC_GRAPH_MODE ;Write-Mode 1 einstellen
	  out   dx,al
	  inc   dx
	  in    al,dx

	  push ax          ;aktuellen Modus auf Stack merken

	  and   al,not 3   ;die ersten 2 Bits ausmaskieren
	  or    al,1       ;das erste setzen
	  out   dx,al      ;einstellen (=write-mode 1)

	  mov   dx,SC_INDEX      ;Zugriff auf alle Bit-Planes sichern
	  mov   ah,0Fh
	  mov   al,SC_MAP_MASK
	  out   dx,ax

	  mov   ax,_vio_seg   ;Segment in es:di laden (aktuelle page)
	  mov   es,ax
	  xor   di,di        ;di=0

	  mov   bx,ds        ;ds in bx merken

	  mov   cx,page3a
	  shl   cx,10        ;Segment der Zielpage in ds:si laden
	  or    ch,0a0h
	  mov   ds,cx
	  mov   si,di        ;si=di=0

	  cld
	  mov cx,16000
	  rep movsb           ;vga-mem vertrÑgt nur byte-moves

	  mov ds,bx           ;ds wieder herstellen

	  mov   dx,GC_INDEX      ;wieder den alten Write-Mode ein-

	  pop ax                 ;stellen

	  mov   ah,al
	  mov   al,GC_GRAPH_MODE
	  out   dx,ax

	  pop si
	  pop di

	  pop bp
	  retf

endp


;-- LINE_COPY: bewegt eine ganze Bildschirmseite auf eine andere
;-- Deklaration : void line_copy(int plane,byte *linebuffer,word offset,word bufsize);

_line_copy proc far
ARG plane:WORD,source:DWORD,offs:WORD,bsize:WORD       ;,clear:word

	  push bp
	  mov  bp,sp

	  push ds
	  push si
	  push di

; pointer laden
	  mov  ax,0a000h    ; 0xa000:xxxx laden
	  mov  es,ax

	  xor  dx,dx        ; wieviele planes schon?
	  mov  bx,plane     ; aktuelle plane
 @loop:

	  lds  si,source    ; source-pointer laden
	  mov  di,offs      ; offset in video-ram laden

	  add  si,dx        ; anzahl der planes dem offset dazu

; plane einstellen
	  push dx           ; dx merken
	  mov  cl,bl
	  mov  ah,1         ; (1<<plane)
	  shl  ah,cl
	  mov  al,2
	  mov  dx,3c4h      ; Indexadresse des Sequencers laden
	  out  dx,ax        ; Bit-Mask-Register laden
	  pop  dx

; daten Åbertragen
	  mov  cx,bsize     ; bufsize laden
	  shr  cx,2         ; durch 4 teilen

 @loop2:                    ; innere schleife
	  movsb             ; kopieren

	  add  si,3
	  loop @loop2

; plane erhîhen
	  inc  dx
	  inc  bx
	  cmp  bx,4      ; neue 0-plane?
	  jne  @cont1
	  inc  offs      ; dann offset erhîhen
 @cont1:
	  and  bx,3
	  cmp  bx,plane  ; schon 4 planes?
	  jne  @loop

; fertig
	  pop  di
	  pop  si
	  pop  ds

	  pop  bp
	  retf
endp


;-----------------------------------------------------------------------
;-- BLOCKMOVE: verschiebt eine Gruppe von Pixeln im Video-RAM
;-- Aufruf von TC: blockmove( int vp,vx,vy;
;--                           int np,nx,ny;
;--                           int breite, hoehe; );
_blockmove proc far

sframe4    struc                  ;Struktur zum Zugriff auf den Stack
bp4        dw ?                   ;nimmt BP auf
ret_adr4   dw ?                   ;RÅcksprungadresse zum Aufrufer
hoehe      dw ?                   ;Hîhe
breite     dw ?                   ;Breite
nachy      dw ?                   ;nach Y-Ordinate
nachx      dw ?                   ;nach X-Ordinate
nachpage   dw ?                   ;nach Seite
vony       dw ?                   ;von Y-Ordinate
vonx       dw ?                   ;von X-Ordinate
vonpage    dw ?                   ;von Seite
sframe4    ends                   ;Ende der Struktur

frame      equ [ bp - bp4 ]       ;adressiert die Elemente der Struktur

  push  bp               ;Adressierung der Parameter Åber
  mov   bp,sp            ;das BP-Register vorbereiten

  push  ds
  cld                    ;bei Stringbefehlen aufwÑrts

  mov   dx,GC_INDEX      ;aktuellen Write-Mode holen und
  mov   al,GC_GRAPH_MODE ;Write-Mode 1 einstellen
  out   dx,al
  inc   dx
  in    al,dx
  push  ax               ;aktuellen Modus auf Stack merken
  and   al,not 3
  or    al,1
  out   dx,al

  mov   al,4             ;DS auf Anfang der VON-Seite
  mov   cl,byte ptr frame.vonpage
  mul   cl
  or    al,0A0h
  xchg  ah,al
  mov   ds,ax

  mov   al,4             ;ES auf Anfang der NACH-Seite
  mov   cl,byte ptr frame.nachpage
  mul   cl
  or    al,0A0h
  xchg  ah,al
  mov   es,ax

  mov   ax,PIXX / 4      ;SI auf Startposition VON
  mul   frame.vony
  mov   si,frame.vonx
  shr   si,2
  add   si,ax

  mov   ax,PIXX / 4      ;DI auf Zielposition NACH
  mul   frame.nachy
  mov   di,frame.nachx
  shr   di,2
  add   di,ax

  mov   dh,byte ptr frame.hoehe   ;DH = Zeilen
  mov   dl,byte ptr frame.breite  ;DL = Bytes
  shr dl,2

  mov   bx,PIXX / 4      ;BX als Offset zur nÑchsten Zeile
  sub   bl,dl
  xor   ch,ch            ;Hi-Byte des Counters ist immer 0

  push  dx               ;DX auf dem Stack sichern
  mov   dx,SC_INDEX      ;Zugriff auf alle Bit-Planes sichern
  mov   ah,0Fh
  mov   al,SC_MAP_MASK
  out   dx,ax
  pop   dx               ;DX zurÅckholen

mt1:
  mov   cl,dl            ;Anzahl Bytes nach CL

  rep   movsb            ;Zeile kopieren
  add   di,bx            ;DI in nÑchste Zeile
  add   si,bx            ;und auch SI
  dec   dh               ;noch eine Zeile?
  jne   mt1              ;Ja ---> weiter

mtend:
  mov   dx,GC_INDEX      ;wieder den alten Write-Mode ein-
  pop   ax               ;stellen
  mov   ah,al
  mov   al,GC_GRAPH_MODE
  out   dx,ax

  pop   ds
  pop   bp

  retf
endp


;== Ende ===============================================================

_text    ends                     ;Ende des Programm-Segments
	 end                      ;Ende des Assembler-Source

