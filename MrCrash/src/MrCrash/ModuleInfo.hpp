//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file ModuleInfo.hpp Module info
//
#pragma once

// includes
#include <memory>
#include <ulib/win32/SystemTime.hpp>

namespace Debug
{

/// \brief module infos
/// data is shared between copies of this object
class ModuleInfo
{
public:
   ModuleInfo(LPVOID pBaseAddress, const CString& cszModuleName)
      :m_spData(new ModuleInfoData(pBaseAddress, cszModuleName))
   {
   }

   // get/set methods

   LPVOID BaseAddress() const throw() { return m_spData->m_pBaseAddress; }
   DWORD64 Size() const throw() { return m_spData->m_dwSize; }
   const CString& ModuleName() const throw() { return m_spData->m_cszModuleName; }
   SystemTime LoadTime() const throw() { return m_spData->m_stLoadTime; }
   SystemTime UnloadTime() const throw() { return m_spData->m_stUnloadTime; }
   const CString& FileVersion() const throw() { return m_spData->m_cszFileVersion; }

   void Size(DWORD64 dwSize) throw() { m_spData->m_dwSize = dwSize; }
   void LoadTime(SystemTime stLoadTime) throw() { m_spData->m_stLoadTime = stLoadTime; }
   void UnloadTime(SystemTime stUnloadTime) throw() { m_spData->m_stUnloadTime = stUnloadTime; }
   void FileVersion(const CString& cszFileVersion) throw() { m_spData->m_cszFileVersion = cszFileVersion; }

   /// less functor to sort ModuleInfo's by load time in an ordered container
   class CSortLibraryInfoByLoadTime: public std::less<ModuleInfo>
   {
   public:
      bool operator()(const ModuleInfo& li1, const ModuleInfo& li2) const throw()
      {
         return li1.m_spData->m_stLoadTime < li2.m_spData->m_stLoadTime;
      }
   };

private:
   /// module info struct
   struct ModuleInfoData
   {
      ModuleInfoData(LPVOID pBaseAddress, const CString& cszModuleName)
         :m_pBaseAddress(pBaseAddress),
          m_cszModuleName(cszModuleName),
          m_dwSize(0)
      {
      }

      LPVOID m_pBaseAddress;
      DWORD64 m_dwSize;
      CString m_cszModuleName;

      SystemTime m_stLoadTime;
      SystemTime m_stUnloadTime;

      CString m_cszFileVersion;
   };

   /// module info
   std::shared_ptr<ModuleInfoData> m_spData;
};

} // namespace Debug
