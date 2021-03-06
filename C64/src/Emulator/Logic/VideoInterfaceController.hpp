//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file VideoInterfaceController.hpp Video interface controller (VIC) chip
//
#pragma once

// includes
#include <vector>
#include "ICIAPortHandler.hpp"

namespace C64
{
class MemoryManager;
class Processor6510;
class IVideoOutputDevice;

/// VIC register names
/// \see http://www.cebix.net/VIC-Article.txt
enum VICRegisterNames
{
   vicRegD000 = 0x00,   ///< MxX: Sprite X position register start
   vicRegD001 = 0x01,   ///< MxY: Sprite Y position register start
   vicRegD010 = 0x10,   ///< MxX8: Sprite X position bit 8 for all sprites
   vicRegD011 = 0x11,   ///< Control register 1
   vicRegD012 = 0x12,   ///< RASTER: Raster counter
   vicRegD015 = 0x15,   ///< MxE: Sprite enable register
   vicRegD016 = 0x16,   ///< Control register 2
   vicRegD017 = 0x17,   ///< MxYE: Sprite Y expansion flags
   vicRegD018 = 0x18,   ///< VIC memory layout
   vicRegD019 = 0x19,   ///< Interrupt register
   vicRegD01A = 0x1a,   ///< Interrupt enabled
   vicRegD01B = 0x1b,   ///< MxDP: Sprite background priority
   vicRegD01C = 0x1c,   ///< MxMC: Sprite Multicolor flags
   vicRegD01D = 0x1d,   ///< MxXE: Sprite X expansion flags
   vicRegD020 = 0x20,   ///< EC: Border color
   vicRegD021 = 0x21,   ///< B0C: Background color 0
   vicRegD022 = 0x22,   ///< B1C: Background color 1
   vicRegD023 = 0x23,   ///< B2C: Background color 2
   vicRegD025 = 0x25,   ///< MM0: Sprite multicolor 0
   vicRegD026 = 0x26,   ///< MM1: Sprite multicolor 1
   vicRegD027 = 0x27,   ///< MxC: Sprite color start
};

enum VICInterruptType
{
   vicInterruptRaster = 1,                      ///< interrupt triggered by a raster line
   vicInterruptCollisionSpriteBackground = 2,   ///< interrupt triggered by a collision of a sprite and the background
   vicInterruptCollisionSpriteSprite = 4,       ///< interrupt triggered by a collision of two sprites
   vicInterruptLightpen = 8,                    ///< interrupt triggered by lightpen (unused)
};

/// \brief Video interface controller
/// \details Implementation of the VIC-II chip, PAL version (MOS 6569).
/// \see http://www.cebix.net/VIC-Article.txt
/// \see https://sh.scs-trc.net/vic/
class VideoInterfaceController: public ICIAPortHandler
{
public:
   /// ctor
   VideoInterfaceController(MemoryManager& memoryManager, Processor6510& processor);
   /// dtor
   ~VideoInterfaceController()
   {
   }

   /// sets flag "show debug info" to show more infos in columns 440-480
   void SetShowDebugInfo(bool showDebugInfo) throw()
   {
      m_showDebugInfo = showDebugInfo;
   }

   /// returns flag "show debug info"
   bool GetShowDebugInfo() const throw()
   {
      return m_showDebugInfo;
   }

   /// sets video output device for VIC render output
   void SetVideoOutputDevice(IVideoOutputDevice* pVideoOutputDevice)
   {
      m_pVideoOutputDevice = pVideoOutputDevice;
   }

   /// sets memory bank to use for VIC registers
   void SetMemoryBank(BYTE bBank);

   /// sets VIC register value
   void SetRegister(BYTE bRegister, BYTE bValue);

   /// reads VIC register value
   BYTE ReadRegister(BYTE bRegister) const;

   /// reads value of color RAM
   void SetColorRam(WORD wAddr, BYTE bValue)
   {
      ATLASSERT(static_cast<size_t>(wAddr) < m_vecColorRam.size());
      m_vecColorRam[wAddr] = bValue & 0x0f;
   }

   /// reads value from color RAM
   BYTE ReadColorRam(WORD wAddr) const
   {
      ATLASSERT(static_cast<size_t>(wAddr) < m_vecColorRam.size());
      return m_vecColorRam[wAddr] | 0; // TODO | with value from VIC
   }

   /// returns reference to color RAM
   std::vector<BYTE>& GetColorRam() { return m_vecColorRam; }

   /// performs one cycle step for VIC
   void Step();

private:
   /// called when a new rasterline is being reached
   void NextRasterline();

   /// renders a single scanline
   void RenderLine();

   /// renders a line in bitmap mode
   void RenderLineBitmapMode(WORD wYPos, BYTE abScanline[0x0200]);

   /// renders a line in character mode
   void RenderCharacterMode(WORD wYPos, BYTE abScanline[0x0200]);

   /// renders sprites for line
   void RenderSprites(BYTE abScanline[0x0200]);

   /// calculates screen columns for character or bitmap mode
   void CalcScreenColumns(WORD& numColumns, WORD& minColumn, WORD& maxColumn, WORD& startX);

   // virtual methods from ICIAPortHandler
   virtual void SetDataPort(BYTE portNumber, BYTE value) throw();

private:
   /// reference to memory manager to use
   const MemoryManager& m_memoryManager;

   /// reference to processor to use
   Processor6510& m_processor;

   /// all VIC registers
   BYTE m_abRegister[47];

   /// color ram
   std::vector<BYTE> m_vecColorRam;

   /// current rasterline
   WORD m_wRasterline;

   /// raster line where a rasterline IRQ is generated
   WORD m_wInterruptRasterline;

   /// cycle value of the next raster line
   unsigned int m_uiNextRasterlineCycle;

   /// indicates a bad line condition for the current rasterline
   bool m_bBadLine;

   /// start address of memory accessible to VIC
   WORD m_wMemoryStart;

   /// flip flop for vertical border switching; true means flip flop is set
   bool m_bVerticalBorderFlipFlop;

   /// set when DEN bit was set in raster line $30
   bool m_bDENSetInRaster30;

   /// indicates if VIC should draw debug infos in columns 440-480
   bool m_showDebugInfo;

   /// pointer to a video output device
   IVideoOutputDevice* m_pVideoOutputDevice;
};

} // namespace C64
