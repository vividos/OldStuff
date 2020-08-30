// keyboard.cpp - keyboard access
// (c) 1999 Michael Fink
// https://github.com/vividos/OldStuff/

#include "defs.h"
#include "keyboard.h"

void send_kb()
{
  asm {
    push cx;
    push bx;
    mov bl, 3
  };
skb_1:
  asm {
    xor cx, cx
  };
skb_2:
  asm {
    in al, 64h;
    test al, 2;
    loopne skb_2;
    mov al, ah;
    out 60h, al
  };
skb_3:
  asm {
    in al, 64h;
    test al, 1;
    loope skb_3;
    in al, 60h;
    cmp al, 0fah;
    je skb_end;
    dec bl;
    jne skb_2;
    or bl, 1
  };
skb_end:
  asm {
    pop bx;
    pop cx
  };
};

bool set_typm(byte trate)
{
  asm {
    xor dx, dx;
    mov ah, 0f3h;
    cli
  }
  send_kb();
  asm {
    jne error;
    mov ah, trate
  }
  send_kb();
  asm {
    jne error;
    inc dl
  };
error:
  asm sti;
  return (bool)_DX;
};

word waitk()
{
  _AX = 0;
  asm int 16h;
  if (_AL != 0)
    asm xor ah,ah;
  return _AX;
};

bool keyboard::getbreak()
{
  asm {
    mov ax, 3300h;
    int 21h;
    xor dh, dh;
  };
  return (bool)_DX;
}

void keyboard::setbreak(bool state)
{
  asm {
    mov ax, 3301h;
    mov dl, state;
    int 21h
  };
}

bool keyboard::set_typem(byte delay, byte speed)
{
  return (bool)set_typm((byte)((delay << 5) + speed));
};

keycode keyboard::wait_key()
{
  return (keycode)waitk();
}

bool keyboard::key_pressed()
{
  asm {
    mov ah, 1
    int 16h
    mov ax, 1
    jnz empty:
    mov ax, 0
  };
empty:
  return (bool)_AL;
}

bool keyboard::is_special_key(specialKeys sk)
{
  byte c = *((byte far*)farpointer(0x40, 0x17));
  return (bool)((c & sk) != 0);
}

keycode keyboard::upcase(keycode kc)
{
  return (keycode)(kc >= kba && kc <= kbz ? kc & ~0x20 : kc);
}
