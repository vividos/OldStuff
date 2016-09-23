//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file VideoInterfaceController.cpp Video interface controller (VIC) chip
//

// includes
#include "StdAfx.h"
#include "VideoInterfaceController.hpp"
#include "MemoryManager.hpp"
#include "Processor6510.hpp"
#include "IVideoOutputDevice.hpp"

using C64::VideoInterfaceController;

/// maximum rasterline, excluding; note that on NTSC there are fewer rasterlines
const WORD c_wMaxRasterline = 312;

/// maximum raster column
const WORD c_maxRasterColumn = 403;

/// number of cycles per rasterline
const unsigned int c_uiNumCyclesPerRasterline = 63;

/// vertical border top rasterline when RSEL = 0
const WORD c_wVerticalBorderTopRSEL0 = 0x37;
/// vertical border top rasterline when RSEL = 1
const WORD c_wVerticalBorderTopRSEL1 = 0x33;

/// vertical border bottom rasterline when RSEL = 0
const WORD c_wVerticalBorderBottomRSEL0 = 0xf6;
/// vertical border bottom rasterline when RSEL = 1
const WORD c_wVerticalBorderBottomRSEL1 = 0xfa;

/// horizontal border left xpos when CSEL = 0
const WORD c_wHorizontalBorderLeftCSEL0 = 0x1f;
/// horizontal border left xpos when CSEL = 1
const WORD c_wHorizontalBorderLeftCSEL1 = 0x18;

/// horizontal border right xpos when CSEL = 0
const WORD c_wHorizontalBorderRightCSEL0 = 0x14e;
/// horizontal border right xpos when CSEL = 1
const WORD c_wHorizontalBorderRightCSEL1 = 0x157;


VideoInterfaceController::VideoInterfaceController(C64::MemoryManager& memoryManager, C64::Processor6510& processor)
   :m_memoryManager(memoryManager),
   m_processor(processor),
   m_wRasterline(0),
   m_wInterruptRasterline(0xffff), // -1 is for "none"
   m_vecColorRam(0x0400),
   m_uiNextRasterlineCycle(0),
   m_wMemoryStart(0), // bank 0, 0x0000-0x3fff
   m_bVerticalBorderFlipFlop(true),
   m_showDebugInfo(false),
   m_pVideoOutputDevice(NULL)
{
   memset(m_abRegister, 0, sizeof(m_abRegister));

   // default: 27, or 0001 1011
   // bits 0..2: soft scrolling
   // bit 3: 25/24 lines (1/0)
   // bit 4: VIC on/off (1/0)
   // bit 5: HiRes/text (1/0)
   // bit 6: background color/text (1/0)
   // bit 7: bit 8 of current raster line
   m_abRegister[vicRegD011] = 27;

   // current raster line: 0
   m_abRegister[vicRegD012] = 0;

   // default: 200, or 1100 1000
   // bits 0..2: soft scrolling
   // bit 3: 40/38 columns (1/0)
   // bit 4: multicolor/normal (1/0)
   // bit 5: always 0
   // bits 6+7: always 1
   m_abRegister[vicRegD016] = 200;

   // default: 21, or 0001 0101
   // bit 0: always 1
   // bits 1..3: 010 char rom at 0x1000
   // bits 4..7: 0001: screen ram at 0x0400
   m_abRegister[vicRegD018] = 21;

   m_abRegister[vicRegD019] = 0x70 | 0; // currently no IRQs active
   m_abRegister[vicRegD01A] = 0xF0 | 0; // no VIC IRQs enabled
   m_abRegister[vicRegD020] = 0xF0 | 14; // border color: light blue
   m_abRegister[vicRegD021] = 0xF0 | 6; // background color: blue

   m_uiNextRasterlineCycle = processor.GetProcessedCycles() + c_uiNumCyclesPerRasterline;
}

void VideoInterfaceController::SetMemoryBank(BYTE bBank)
{
   ATLASSERT(bBank < 4);
   m_wMemoryStart = (bBank & 3) << 14;
}

void VideoInterfaceController::SetRegister(BYTE bRegister, BYTE bValue)
{
   ATLASSERT(bRegister < sizeof(m_abRegister));

   ATLTRACE(_T("VIC register write at $d0%02x = $%02x, rasterline %u\n"), bRegister, bValue, m_wRasterline);

   switch (bRegister)
   {
   case vicRegD011:
      // note: bit 7 is bit8 of the interrupt raster line
      if (m_wInterruptRasterline == 0xffff)
         m_wInterruptRasterline = 0;

      m_wInterruptRasterline &= 0xff;
      if ((bValue & 0x80) != 0)
         m_wInterruptRasterline |= 1 << 8;
      ATLTRACE(_T("VIC: request for IRQ at line $%04x\n"), m_wInterruptRasterline);
      break;

   case vicRegD012: // bits 0 to 7 of the interrupt raster line
      if (m_wInterruptRasterline == 0xffff)
         m_wInterruptRasterline = 0;
      m_wInterruptRasterline &= 0x0100;
      m_wInterruptRasterline |= bValue;
      ATLTRACE(_T("VIC: request for IRQ at line $%04x\n"), m_wInterruptRasterline);
      break;

   default:
      break;
   }

   m_abRegister[bRegister] = bValue;
}

BYTE VideoInterfaceController::ReadRegister(BYTE bRegister) const
{
   ATLASSERT(bRegister < sizeof(m_abRegister));

   ATLTRACE(_T("VIC register read at $d0%02x\n"), bRegister);

   BYTE bValue = m_abRegister[bRegister];

   switch (bRegister)
   {
   case vicRegD011:
      // read bit 7 to bit 8 of the current raster line
      bValue &= 0x7f;
      if ((m_wRasterline & 0x0100) != 0)
         bValue |= 0x80;
      break;

   case vicRegD012: // read bits 0 to 7 of the interrupt raster line
      bValue = static_cast<BYTE>(m_wRasterline & 0xff);
      break;

   case vicRegD016:
      bValue |= 0xc0; // bits 6 and 7 always set
      break;

   case vicRegD018:
      // bit 0 is always set
      bValue |= 1;
      break;

   case vicRegD019:
      bValue |= 0x70; // bits 4..6 always set
      break;

   case vicRegD01A:
      bValue |= 0xf0; // bits 4..7 always set
      break;

   case 0x1e: // sprite-sprite collision
   case 0x1f: // sprite-background collision
      // automatically cleared at read
      const_cast<VideoInterfaceController&>(*this).m_abRegister[bRegister] = 0;
      break;

   default:
      break;
   }

   if (bRegister >= 0x20)
      bValue |= 0xf0; // always set high nibble

   return bValue;
}

void VideoInterfaceController::Step()
{
   // check if we have a new rasterline
   unsigned int uiCurrentCycles = m_processor.GetProcessedCycles();
   signed int iCyclesRemaining = m_uiNextRasterlineCycle - uiCurrentCycles;

   if (iCyclesRemaining <= 0)
   {
      // next rasterline
      m_wRasterline++;

      if (m_wRasterline >= c_wMaxRasterline)
      {
         m_wRasterline = 0;

         if (m_pVideoOutputDevice != NULL)
            m_pVideoOutputDevice->ScreenCompleted();
      }

      NextRasterline();
   }
}

void VideoInterfaceController::NextRasterline()
{
   // DEN is bit 4
   bool bDEN = GetBit(m_abRegister[vicRegD011], 4);

   if (m_wRasterline == 0x30)
   {
      m_bDENSetInRaster30 = bDEN;
   }

   // calculate bad line condition
   m_bBadLine = m_wRasterline >= 0x30 && m_wRasterline <= 0xf7 &&
      (m_wRasterline & 7) == (m_abRegister[vicRegD011] & 7) &&
      m_bDENSetInRaster30;

   RenderLine();

   // check if vertical border flipflop has to be set/reset
   {
      // RSEL is bit 3 of $D011
      bool bRSEL = GetBit(m_abRegister[vicRegD011], 3);
      WORD wTopLine = bRSEL ? c_wVerticalBorderTopRSEL1 : c_wVerticalBorderTopRSEL0;
      WORD wBottomLine = bRSEL ? c_wVerticalBorderBottomRSEL1 : c_wVerticalBorderBottomRSEL0;

      if (m_wRasterline == wBottomLine)
         m_bVerticalBorderFlipFlop = true;
      else
         if ((m_wRasterline == wTopLine) && bDEN)
            m_bVerticalBorderFlipFlop = false;
   }

   // calculate next cycle value
   m_uiNextRasterlineCycle += c_uiNumCyclesPerRasterline;

   // steal 40 cycles for character data reads when a bad line condition is true
   if (m_bBadLine)
      m_uiNextRasterlineCycle -= 40;

   // check for rasterline interrupt
   if ((m_abRegister[vicRegD01A] & vicInterruptRaster) != 0 && // raster IRQ bit must be set
      m_wInterruptRasterline == m_wRasterline &&   // rasterline must be reached
      !m_processor.GetFlag(C64::flagInterrupt)) // interrupt flag must be cleared
   {
      // set interrupt bit and bit 7
      m_abRegister[vicRegD019] |= vicInterruptRaster | 0x80;

      // generate an interrupt
      m_processor.Interrupt(C64::interruptTypeIRQ_VIC);
   }
}

void VideoInterfaceController::RenderLine()
{
   if (m_pVideoOutputDevice == NULL)
      return; // no output device? no work to do

   BYTE abScanline[0x0200];
   // fill with background color
   memset(abScanline, m_abRegister[vicRegD020] & 0xf, c_maxRasterColumn + 1);

   if (m_showDebugInfo)
   {
      // rest: fill with black
      memset(abScanline + c_maxRasterColumn + 1, 0, 0x0200 - c_maxRasterColumn - 1);
   }

   if (m_showDebugInfo &&
      m_wRasterline == m_wInterruptRasterline)
   {
      // columns 410-420: interrupt rasterline: white
      for (unsigned int i = 410; i < 420; i++)
         abScanline[i] = 1;
   }

   // 1. check if vertical border flipflop is set
   if (m_bVerticalBorderFlipFlop)
   {
      if (m_showDebugInfo)
      {
         // columns 420-430: vertical border: red
         for (unsigned int i = 420; i < 430; i++)
            abScanline[i] = 2;
      }

      m_pVideoOutputDevice->OutputLine(m_wRasterline, abScanline);
      return; // when yes, leave now, since the line consists of the border color only
   }

   if (m_showDebugInfo)
   {
      // columns 420-430: bad line condition black/lightblue
      for (unsigned int i = 420; i < 430; i++)
         abScanline[i] = m_bBadLine ? 0 : 14;
   }

   // 2. check if character or graphics mode, bit 5
   bool bBMM = GetBit(m_abRegister[vicRegD011], 5);

   WORD wXStart = 40;
   WORD wYPos = m_wRasterline - 0x30 - 4;

   if (bBMM)
      RenderLineBitmapMode(wXStart, wYPos, abScanline);
   else
      RenderCharacterMode(wXStart, wYPos, abScanline);

   RenderSprites(abScanline);

   m_pVideoOutputDevice->OutputLine(m_wRasterline, abScanline);
}

void VideoInterfaceController::RenderLineBitmapMode(WORD wXStart, WORD wYPos, BYTE abScanline[0x0200])
{
   // calculate start of bitmap memory

   // bit CB13 decides if lower lor upper half of memory is used for bitmap
   bool bCB13 = GetBit(m_abRegister[vicRegD018], 3);

   // calculate bitmap memory pos for this line
   WORD wBitmapMem = m_wMemoryStart + (bCB13 ? 0x2000 : 0);
   // add cell offset position; 320 bytes per 8 y-pos values
   wBitmapMem += (wYPos >> 3) * 320;
   // add in-cell offset position
   wBitmapMem += (wYPos & 7);

   const BYTE* pLine = &m_memoryManager.GetRAM()[wBitmapMem];

   // calculate video memory location for this line
   // video memory location: bits 4..7
   WORD wVideoMem = m_wMemoryStart + (static_cast<WORD>(m_abRegister[vicRegD018] & 0xf0) << 6);
   wVideoMem += (wYPos >> 3) * 40;

   const BYTE* pVideoMem = &m_memoryManager.GetRAM()[wVideoMem];

   // calculate color memory location for this line
   WORD wColorMem = (wYPos >> 3) * 40;

   // decide if monochrome or multicolor mode, bit 4
   bool bMCM = GetBit(m_abRegister[vicRegD016], 4);

   BYTE abColors[4];
   if (bMCM)
      abColors[0] = m_abRegister[vicRegD021] & 0x0f;

   // render 40 columns, 8 pixels per column (40*8 = 320)
   for (WORD w = 0; w < 40; w++)
   {
      if (bMCM)
      {
         // multicolor mode
         abColors[1] = pVideoMem[w] >> 4; // background: high nibble
         abColors[2] = pVideoMem[w] & 0x0f; // foreground: low nibble
         abColors[3] = m_vecColorRam[wColorMem + w] & 0x0f;
      }
      else
      {
         // monochrome mode
         abColors[0] = pVideoMem[w] & 0x0f; // background: low nibble
         abColors[1] = pVideoMem[w] >> 4; // foreground: high nibble
      }

      BYTE bBitLine = pLine[w * 8];

      if (bMCM)
      {
         for (unsigned int uiBit = 0; uiBit < 8; uiBit += 2)
         {
            // set pixels at position
            abScanline[w * 8 + uiBit + wXStart + 0] =
               abScanline[w * 8 + uiBit + wXStart + 1] = abColors[(bBitLine >> 6) & 3];

            bBitLine <<= 2;
         }
      }
      else
      {
         for (unsigned int uiBit = 0; uiBit < 8; uiBit++)
         {
            // set pixel at position
            abScanline[w * 8 + uiBit + wXStart] = abColors[(bBitLine >> 7) & 1];

            bBitLine <<= 1;
         }
      }
   }

   if (m_showDebugInfo)
   {
      // columns 430-440: bitmap mode, monochrome: cyan, multicolor: pink
      for (unsigned int i = 430; i < 440; i++)
         abScanline[i] = bMCM ? 3 : 4;
   }
}

void VideoInterfaceController::RenderCharacterMode(WORD wXStart, WORD wYPos, BYTE abScanline[0x0200])
{
   ATLASSERT(wXStart + 320 < 0x0200);

   // set background color, just in case
   memset(&abScanline[wXStart], m_abRegister[vicRegD021] & 0x0f, 320);

   // calculate charset address, bits 1 to 3
   WORD wCharsetMem = m_wMemoryStart + (static_cast<WORD>(m_abRegister[vicRegD018] & 14) << 10);
   // add in-cell offset position
   wCharsetMem += (wYPos & 7);

   const BYTE* pCharsetMem = &m_memoryManager.GetRAM()[wCharsetMem];

   // calculate video memory location for this line
   // video memory location: bits 4..7
   WORD wVideoMem = m_wMemoryStart + (static_cast<WORD>(m_abRegister[vicRegD018] & 0xf0) << 6);
   wVideoMem += (wYPos >> 3) * 40;

   const BYTE* pVideoMem = &m_memoryManager.GetRAM()[wVideoMem];

   // calculate color memory location for this line
   WORD wColorMem = (wYPos >> 3) * 40;

   // decide if monochrome or multicolor mode, bit 4
   bool bMCM = GetBit(m_abRegister[vicRegD016], 4);

   // check if extended color mode, bit 6
   bool bECM = GetBit(m_abRegister[vicRegD011], 6);

   BYTE abColors[4] = { 0 };

   if (bMCM)
   {
      if (bECM)
         __nop();
      else
      {
         abColors[0] = m_abRegister[vicRegD021];
         abColors[1] = m_abRegister[vicRegD022];
         abColors[2] = m_abRegister[vicRegD023];
      }
   }

   for (WORD w = 0; w < 40; w++)
   {
      if (bMCM)
      {
         if (bECM)
         {
            __nop();
         }
         else
         {
            abColors[3] = m_vecColorRam[wColorMem + w] & 0x07;
         }
      }
      else
         abColors[0] = m_vecColorRam[wColorMem + w] & 0x0f;

      // indicates in MCM mode if bit 3 of color ram is set
      bool bMCM_ColorBit3 = bMCM && GetBit(m_vecColorRam[wColorMem + w], 3);

      if (bMCM && !bMCM_ColorBit3)
         abColors[0] = abColors[3];
      else
         abColors[0] = m_abRegister[vicRegD021];

      // get char from video ram
      BYTE bChar = pVideoMem[w];
      // get position in charset
      BYTE bBitLine = pCharsetMem[bChar * 8];

      // in MCM mode, bit 3 decides if character should be rendered in 
      if (bMCM && !bECM && bMCM_ColorBit3)
      {
         for (unsigned int uiBit = 0; uiBit < 8; uiBit += 2)
         {
            // set pixels at position
            abScanline[w * 8 + uiBit + wXStart + 0] =
               abScanline[w * 8 + uiBit + wXStart + 1] = abColors[(bBitLine >> 6) & 3];

            bBitLine <<= 2;
         }
      }
      else
         if (bMCM && bECM)
         {
            __nop();
            // TODO ecm mode
         }
         else
         {
            // standard mode, or MCM with bit 3 of the color value being unset

            for (unsigned int uiBit = 0; uiBit < 8; uiBit++)
            {
               // set pixel at position
               if ((bBitLine & 0x80) != 0)
                  abScanline[w * 8 + uiBit + wXStart] = abColors[0];

               bBitLine <<= 1;
            }
         }
   }

   if (m_showDebugInfo)
   {
      // columns 430-440: character mode;
      // multicolor: blue, extended multicolor: green, monochrome: yellow
      for (unsigned int i = 430; i < 440; i++)
         abScanline[i] = bMCM ? (bECM ? 5 : 6) : 7;
   }
}

void VideoInterfaceController::RenderSprites(BYTE abScanline[0x0200])
{
   WORD wVideoMem = m_wMemoryStart + (static_cast<WORD>(m_abRegister[vicRegD018] & 0xf0) << 6);

   // array indicating if a sprite pixel was already set
   // used to implement sprite priority
   bool pixelAlreadySet[0x0200];
   std::uninitialized_fill(std::begin(pixelAlreadySet), std::end(pixelAlreadySet), false);

   for (BYTE spriteNr = 0; spriteNr < 8; spriteNr++)
   {
      bool enabled = GetBit(m_abRegister[vicRegD015], spriteNr);

      if (!enabled)
         continue;

      // X/Y interleaved
      unsigned int posX = m_abRegister[vicRegD000 + spriteNr * 2];
      unsigned int posY = m_abRegister[vicRegD001 + spriteNr * 2];

      if (GetBit(m_abRegister[vicRegD010], spriteNr))
         posX += 0x100;

      BYTE spriteHeight = 21;
      BYTE spriteWidth = 24;

      bool bitMxXE = GetBit(m_abRegister[vicRegD01D], spriteNr); // X expansion
      if (bitMxXE)
         spriteWidth *= 2;

      bool bitMxYE = GetBit(m_abRegister[vicRegD017], spriteNr); // Y expansion
      if (bitMxYE)
         spriteHeight *= 2;

      bool shownInThisLine =
         m_wRasterline >= posY && m_wRasterline < posY + spriteHeight;

      if (m_showDebugInfo)
      {
         // columns 460-476, 2 colums per sprite
         // sprite: on: white, off: black
         abScanline[460 + spriteNr * 2 + 0] = shownInThisLine ? 1 : 0;
         abScanline[460 + spriteNr * 2 + 1] = shownInThisLine ? 1 : 0;
      }

      if (!shownInThisLine)
         continue;

      bool bitMxMC = GetBit(m_abRegister[vicRegD01C], spriteNr);

      BYTE spriteColor = m_abRegister[vicRegD027 + spriteNr] & 0xF;

      BYTE spriteMultiColor0 = m_abRegister[vicRegD025] & 0xF;
      BYTE spriteMultiColor1 = m_abRegister[vicRegD026] & 0xF;

      WORD spriteIndexAddr = wVideoMem + 0x03F8 + spriteNr;

      BYTE spriteIndex = m_memoryManager.GetRAM()[spriteIndexAddr];

      WORD spriteDataBaseAddr = m_wMemoryStart + (spriteIndex << 6);

      BYTE spriteLine = static_cast<BYTE>((m_wRasterline - posY) & 0xFF);
      if (bitMxYE)
         spriteLine /= 2;
      ATLASSERT(spriteLine < 21);

      WORD spriteDataAddr = spriteDataBaseAddr + spriteLine * 3;

      const BYTE* spriteDataPtr = &m_memoryManager.GetRAM()[spriteDataAddr];
      DWORD spriteData =
         (DWORD(spriteDataPtr[0]) << 16) |
         (DWORD(spriteDataPtr[1]) << 8) |
         DWORD(spriteDataPtr[2]);

      if (spriteData == 0)
         continue; // shortcut: nothing to render

      // render sprite
      for (BYTE spritePosX = 0; spritePosX < spriteWidth; spritePosX++)
      {
         // calculate logical column in sprite
         BYTE spriteColumn = spritePosX;
         if (bitMxXE)
            spriteColumn /= 2;

         unsigned int currentPosX = posX + spritePosX;
         if (currentPosX > 403)
            break; // beyond VIC width; skip rest of line

         if (bitMxMC)
         {
            // multicolor
            BYTE spriteMultiColumn = spriteColumn - (spriteColumn & 1);
            BYTE colorIndex = (spriteData >> (24 - spriteMultiColumn - 2)) & 3;

            if (colorIndex > 0)
            {
               if (!pixelAlreadySet[currentPosX])
               {
                  BYTE color =
                     colorIndex == 1 ? spriteMultiColor0 :
                     colorIndex == 2 ? spriteColor : spriteMultiColor1;

                  abScanline[currentPosX] = color;
                  pixelAlreadySet[currentPosX] = true;
               }
            }
         }
         else
         {
            // monochrome
            bool colorBit = ((spriteData >> (24 - spriteColumn - 1)) & 1) != 0;

            if (colorBit &&
               !pixelAlreadySet[currentPosX])
            {
               abScanline[currentPosX] = spriteColor;
               pixelAlreadySet[currentPosX] = true;
            }
         }
      }

      // steal 2 cycles from the processor for every sprite shown in this line
      m_uiNextRasterlineCycle -= 2;
   }
}

void VideoInterfaceController::SetDataPort(BYTE portNumber, BYTE bValue)
{
   if (portNumber == 0)
   {
      // first two bits serve as memory bank to use
      // note: bits are inverted from their meaning
      // e.g. 11 means $0000 - $3fff
      SetMemoryBank((bValue & 3) ^ 3);
   }
}
