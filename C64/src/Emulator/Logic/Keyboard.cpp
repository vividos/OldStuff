//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Keyboard.cpp C64 keyboard implementation
//

// includes
#include "StdAfx.h"
#include "Keyboard.hpp"

using C64::Keyboard;

Keyboard::Keyboard()
   :m_currentPortA(0xFF),
   m_currentPortB(0xFF)
{
   memset(m_keyMatrix, 0, sizeof(m_keyMatrix));
}

void Keyboard::SetKeyState(BYTE keyCode, bool keyState, bool shiftState)
{
   ATLTRACE(_T("SetKeyState: keyCode=%02x keyState=%i shiftState=%i\n"), keyCode, keyState ? 1 : 0, shiftState ? 1 : 0);

   if (keyCode == VK_UP)
   {
      SetKeyState(VK_LSHIFT, keyState, shiftState);
      SetKeyState(VK_DOWN, keyState, !shiftState);
      return;
   }

   if (keyCode == VK_LEFT)
   {
      SetKeyState(VK_LSHIFT, keyState, shiftState);
      SetKeyState(VK_RIGHT, keyState, !shiftState);
      return;
   }

   BYTE pa = 0;
   BYTE pb = 0;
   if (!CalcPortBitIndices(keyCode, shiftState, pa, pb))
      return;

   BYTE value = m_keyMatrix[pa];

   if (keyState)
      value |= (1 << pb);
   else
      value &= ~(1 << pb);

   m_keyMatrix[pa] = value;
}

bool Keyboard::CalcPortBitIndices(BYTE keyCode, bool shiftState, BYTE& pa, BYTE& pb)
{
   switch (keyCode)
   {
   case VK_PAUSE: pa = 7; pb = 7; break;
      // special case '/': see key '7'
   case VK_OEM_COMMA: pa = 5; pb = 7; break;
   case 'N': pa = 4; pb = 7; break;
   case 'V': pa = 3; pb = 7; break;
   case 'X': pa = 2; pb = 7; break;
   case VK_LSHIFT: pa = 1; pb = 7; break;
   case VK_DOWN: pa = 0; pb = 7; break;

   case 'Q': pa = 7; pb = 6; break;
   case VK_OEM_5: pa = 6; pb = 6; break; // the ^ character
   case '@': pa = 5; pb = 6; break;
   case 'O': pa = 4; pb = 6; break;
   case 'U': pa = 3; pb = 6; break;
   case 'T': pa = 2; pb = 6; break;
   case 'E': pa = 1; pb = 6; break;
   case VK_F5: pa = 0; pb = 6; break;

   case VK_RWIN:
   case VK_LWIN: pa = 7; pb = 5; break; // C= key
   case '=': pa = 6; pb = 5; break;
   case ':': pa = 5; pb = 5; break;
   case 'K': pa = 4; pb = 5; break;
   case 'H': pa = 3; pb = 5; break;
   case 'F': pa = 2; pb = 5; break;
   case 'S': pa = 1; pb = 5; break;
   case VK_F3: pa = 0; pb = 5; break;

   case ' ': pa = 7; pb = 4; break;
   case VK_RSHIFT: pa = 6; pb = 4; break;
   case VK_OEM_PERIOD: pa = 5; pb = 4; break;
   case 'M': pa = 4; pb = 4; break;
   case 'B': pa = 3; pb = 4; break;
   case 'C': pa = 2; pb = 4; break;
   case 'Z': pa = 1; pb = 4; break;
   case VK_F1: pa = 0; pb = 4; break;

   case '2': pa = 7; pb = 3; break;
   case VK_HOME: pa = 6; pb = 3; break;
   case VK_OEM_MINUS: pa = 5; pb = 3; break;
   case '0': pa = 4; pb = 3; break;
   case '8': pa = 3; pb = 3; break;
   case '6': pa = 2; pb = 3; break;
   case '4': pa = 1; pb = 3; break;
   case VK_F7: pa = 0; pb = 3; break;

   case VK_CONTROL: pa = 7; pb = 2; break;
   case ';': pa = 6; pb = 2; break;
   case 'L': pa = 5; pb = 2; break;
   case 'J': pa = 4; pb = 2; break;
   case 'G': pa = 3; pb = 2; break;
   case 'D': pa = 2; pb = 2; break;
   case 'A': pa = 1; pb = 2; break;
   case VK_RIGHT: pa = 0; pb = 2; break;

      // special case: the '<-' key not found on keyboards
      //case '???': pa = 7; pb = 1; break;
   case '*': pa = 6; pb = 1; break;
   case 'P': pa = 5; pb = 1; break;
   case 'I': pa = 4; pb = 1; break;
   case 'Y': pa = 3; pb = 1; break;
   case 'R': pa = 2; pb = 1; break;
   case 'W': pa = 1; pb = 1; break;
   case VK_RETURN: pa = 0; pb = 1; break;

   case '1': pa = 7; pb = 0; break;
      // special case: pound sign not found on most keyboards
      //case '£': pa = 6; pb = 0; break;
   case VK_OEM_PLUS: pa = 5; pb = 0; break;
   case '9': pa = 4; pb = 0; break;
   case '7':
      // special case: '/' is a 7 with shift pressed
      if (shiftState)
      {
         pa = 6; pb = 7;
      }
      else
      {
         pa = 3; pb = 0;
      }
      break;
   case '5': pa = 2; pb = 0; break;
   case '3': pa = 1; pb = 0; break;
   case VK_DELETE: pa = 0; pb = 0; break;

   default:
      return false;
   }

   return true;
}

void Keyboard::SetDataPort(BYTE portNumber, BYTE bValue) throw()
{
   if (portNumber == 0)
      m_currentPortA = bValue;
   else
      if (portNumber == 1)
         m_currentPortB = bValue;
      else
         ATLASSERT(false);
}

void Keyboard::ReadDataPort(BYTE portNumber, BYTE& bValue) const throw()
{
   if (portNumber == 0)
   {
      // read from port A, mask with current port B
      BYTE mask = m_currentPortB ^ 0xFF;
      bValue = 0;
      for (BYTE pa = 0; pa < 8; pa++)
      {
         BYTE row = m_keyMatrix[pa];
         row &= mask; // only use the bits that are 1 in mask

         if (row > 0)
         {
            bValue |= (1 << pa);
         }
      }
   }
   else
   {
      // read from port B, mask with current port A
      BYTE mask = m_currentPortA ^ 0xFF;
      bValue = 0;
      for (BYTE pa = 0; pa < 8; pa++)
      {
         // only use the pa line when the port mask has the bit set
         if ((mask & (1 << pa)) != 0)
         {
            bValue |= m_keyMatrix[pa];
         }
      }
   }

   bValue = bValue ^ 0xff;
}
