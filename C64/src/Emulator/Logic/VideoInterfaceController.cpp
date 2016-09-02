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
 m_wMemoryStart(0),
 m_bVerticalBorderFlipFlop(true),
 m_pVideoOutputDevice(NULL)
{
   memset(m_abRegister, 0, sizeof(m_abRegister));
   m_abRegister[vicRegD011] = 27;
   m_abRegister[vicRegD016] = 200;
   m_abRegister[vicRegD018] = 21;
   m_abRegister[vicRegD01A] = 0;   // no VIC IRQs
   m_abRegister[vicRegD020] = 254; // border color
   m_abRegister[vicRegD021] = 246; // background color

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

   switch(bRegister)
   {
   case 0x11:
      // note: bit 7 is bit8 of the interrupt raster line
      if (m_wInterruptRasterline == 0xffff)
         m_wInterruptRasterline = 0;

      m_wInterruptRasterline &= 0xff;
      if ((bValue & 0x80) != 0)
         m_wInterruptRasterline |= 1 << 8;
      ATLTRACE(_T("VIC: request for IRQ at line $%04x\n"), m_wInterruptRasterline);
      break;

   case 0x12: // bits 0 to 7 of the interrupt raster line
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
   case 0x11:

      // read bit 7 to bit 8 of the current raster line
      bValue &= 0x7f;
      if ((m_wRasterline & 0x0100) != 0)
         bValue |= 0x80;
      break;

   case 0x12: // read bits 0 to 7 of the interrupt raster line
      bValue = static_cast<BYTE>(m_wRasterline & 0xff);
      break;

   case 0x16:
      bValue |= 0xc0; // bits 6 and 7 always set
      break;

   case 0x18:
      // bit 0 is always set
      bValue |= 1;
      break;

   case 0x19:
      bValue |= 0x70; // bits 4..6 always set
      break;

   case 0x1a:
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

      if (m_wRasterline == c_wMaxRasterline)
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
/*
   static bool s_bTrace = false;

   if (m_wRasterline == 0)
      ATLTRACE(_T("VIC: reached next screen\n"));
   else
//      if (s_bTrace)
         ATLTRACE(_T("VIC: reached next raster line at %u\n"), m_wRasterline);
*/


   // DEN is bit 4
   bool bDEN = (m_abRegister[vicRegD011] & (1 << 4)) != 0;

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
      bool bRSEL = (m_abRegister[vicRegD011] & (1 << 3)) != 0;
      WORD wTopLine = bRSEL ? c_wVerticalBorderTopRSEL1 : c_wVerticalBorderTopRSEL0;
      WORD wBottomLine = bRSEL ? c_wVerticalBorderBottomRSEL1 : c_wVerticalBorderBottomRSEL0;

      if (m_wRasterline == wBottomLine)
         m_bVerticalBorderFlipFlop = true;
      else
      if (m_wRasterline == wTopLine && bDEN)
         m_bVerticalBorderFlipFlop = false;
   }

   // calculate next cycle value
   m_uiNextRasterlineCycle += c_uiNumCyclesPerRasterline;

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
   // fill with background color for now
   memset(abScanline, m_abRegister[vicRegD020] & 0xf, sizeof(abScanline)/sizeof(*abScanline));

   // 1. check if vertical border flipflop is set
   if (m_bVerticalBorderFlipFlop)
   {
      m_pVideoOutputDevice->OutputLine(m_wRasterline, abScanline);
      return; // when yes, leave now, since the line consists of the border color only
   }

   // 2. check if character or graphics mode, bit 5
   bool bBMM = (m_abRegister[vicRegD011] & (1 << 5)) != 0;

/*
   for(unsigned int i=0; i<0x0200; i++)
      abScanline[i] = m_bBadLine ? 0 : (i+m_wRasterline) & 15;
*/
   WORD wXStart = 40;
   WORD wYPos = m_wRasterline-0x30-4;

   if (bBMM)
      RenderLineBitmapMode(wXStart, wYPos, abScanline);
   else
      RenderCharacterMode(wXStart, wYPos, abScanline);

   RenderSprites(abScanline);

   m_pVideoOutputDevice->OutputLine(m_wRasterline, abScanline);

   // TODO steal some cycles from the processor if needed
   // steal 40 cycles for character data reads, e.g. when a bad line condition is true
}

void VideoInterfaceController::RenderLineBitmapMode(WORD wXStart, WORD wYPos, BYTE abScanline[0x0200])
{
   // calculate start of bitmap memory

   // bit CB13 decides if lower lor upper half of memory is used for bitmap
   bool bCB13 = (m_abRegister[vicRegD018] & (1 << 3)) != 0;

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
//   WORD wVideoMem = m_wMemoryStart + ((m_abRegister[vicRegD018] & 0xf0) >> 4) * 0x0400; // TODO remove
   wVideoMem += (wYPos >> 3) * 40;

   const BYTE* pVideoMem = &m_memoryManager.GetRAM()[wVideoMem];

   // calculate color memory location for this line
   WORD wColorMem = (wYPos >> 3) * 40;

   // decide if monochrome or multicolor mode, bit 4
   bool bMCM = (m_abRegister[vicRegD016] & (1 << 4)) != 0;

   BYTE abColors[4];
   if (bMCM)
      abColors[0] = m_abRegister[vicRegD021] & 0x0f;

   // render 40 columns, 8 pixels per column (40*8 = 320)
   for(WORD w = 0; w < 40; w++)
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

      BYTE bBitLine = pLine[w*8];

      if (bMCM)
      {
         for (unsigned int uiBit=0; uiBit<8; uiBit+=2)
         {
            // set pixels at position
            abScanline[w*8 + uiBit + wXStart + 0] =
            abScanline[w*8 + uiBit + wXStart + 1] = abColors[(bBitLine >> 6) & 3];

            bBitLine <<= 2;
         }
      }
      else
      {
         for (unsigned int uiBit=0; uiBit<8; uiBit++)
         {
            // set pixel at position
            abScanline[w*8 + uiBit + wXStart] = abColors[(bBitLine >> 7) & 1];

            bBitLine <<= 1;
         }
      }
   }
}

void VideoInterfaceController::RenderCharacterMode(WORD wXStart, WORD wYPos, BYTE abScanline[0x0200])
{
   ATLASSERT(wXStart + 320 < 0x0200);

   // set background color, just in case
   memset(&abScanline[wXStart], m_abRegister[vicRegD021] & 0x0f, 320);

   // calculate charset address, bits 1 to 3
   WORD wCharsetMem = m_wMemoryStart + (static_cast<WORD>(m_abRegister[vicRegD018] & 6) << 10);
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
   bool bMCM = (m_abRegister[vicRegD016] & (1 << 4)) != 0;

   // check if extended color mode, bit 6
   bool bECM = (m_abRegister[vicRegD011] & (1 << 6)) != 0;

   BYTE abColors[4] = {0};

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
      bool bMCM_ColorBit3 = bMCM && (m_vecColorRam[wColorMem + w] & (1 << 3)) != 0;

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
         for (unsigned int uiBit=0; uiBit<8; uiBit+=2)
         {
            // set pixels at position
            abScanline[w*8 + uiBit + wXStart + 0] =
            abScanline[w*8 + uiBit + wXStart + 1] = abColors[(bBitLine >> 6) & 3];

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

         for (unsigned int uiBit=0; uiBit<8; uiBit++)
         {
            // set pixel at position
            if ((bBitLine & 0x80) != 0)
               abScanline[w*8 + uiBit + wXStart] = abColors[0];

            bBitLine <<= 1;
         }
      }
   }
}

void VideoInterfaceController::RenderSprites(BYTE abScanline[0x0200])
{
   abScanline;
   // TODO
   // TODO steal 2 cycles from the processor for every sprite shown in this line
}

void VideoInterfaceController::SetDataPort(BYTE bValue)
{
   // first two bits serve as memory bank to use
   // note: bits are inverted from their meaning
   // e.g. 11 means $0000 - $3fff
   SetMemoryBank((bValue & 3) ^ 3);
}
