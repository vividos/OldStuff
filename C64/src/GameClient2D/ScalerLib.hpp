//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ScalerLib.hpp Scaler library loader
//
#pragma once

/// scaler
class ScalerLib
{
public:
   /// scale function typedef
   typedef void(*T_fnScale)(
      unsigned int bufferType, // one of 16, 555, 565, 32 or 888
      unsigned int scaleFactor, // scale factor, either 2, 3 or 4
      unsigned char* destBuffer,
      const unsigned char* sourceBuffer,
      unsigned int unscaledWidth,
      unsigned int unscaledHeight) throw();

   /// ctor; loads scaler library
   ScalerLib(LPCTSTR filename)
   {
      HMODULE module_ = LoadLibrary(filename);
      if (module_ != nullptr)
         module.reset(module_, FreeLibrary);
   }

   /// returns if scaler library was loaded correctly
   bool Valid() const { return module.get() != nullptr; }

   /// returns scaler function, when available in scaler library
   T_fnScale GetScalerFunc()
   {
      if (!Valid())
         return T_fnScale();

      T_fnScale fn =
         reinterpret_cast<T_fnScale>(
            GetProcAddress(
               static_cast<HMODULE>(module.get()),
               "Scale"));

      return fn;
   }

private:
   /// scaler library module
   std::shared_ptr<void> module;
};
