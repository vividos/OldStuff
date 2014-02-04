#pragma once

// includes
#include <tlhelp32.h>
#include <vector>
#include <memory>

// define symbols that may not be available, e.g. under CE
#ifndef TH32CS_SNAPNOHEAPS
#define TH32CS_SNAPNOHEAPS 0
#endif

/// \brief Debugging Tools
namespace Debug
{

/// \brief contains all classes that have to do with toolhelp functions
namespace Toolhelp
{

/// \brief toolhelp snapshot
/*! captures a snapshot of the system's process, thread, module and/or heap lists, for later use
    in ProcessList, ThreadList, ModuleList and HeapList
*/
class Snapshot
{
public:
   /// \brief ctor; takes snapshot for toolhelp classes
   /*! \param dwFlags the following flags can be used (combine multiple flags with the or operator|)
        - TH32CS_SNAPHEAPLIST only create snapshot data for HeapList; th32ProcessID must be filled in
        - TH32CS_SNAPPROCESS  only create snapshot data for ProcessList
        - TH32CS_SNAPTHREAD   only create snapshot data for ThreadList
        - TH32CS_SNAPMODULE   only create snapshot data for ModuleList; th32ProcessID must be filled in
        - TH32CS_SNAPMODULE32 includes 32-bit module data when running on 64-bit OS
        - TH32CS_SNAPNOHEAPS  on Windows Mobile, exclude heap infos when using TH32CS_SNAPPROCESS
        - TH32CS_SNAPALL      all of the above, except TH32CS_SNAPMODULE32
        - TH32CS_INHERIT      the snapshot handle is inheritable, e.g. to child processes
       \param th32ProcessID optional process id
   */
   Snapshot(DWORD dwFlags, DWORD th32ProcessID = 0);

   /// convert to handle
   operator HANDLE() const { return m_spSnapshot.get(); }

private:
   /// snapshot data
   std::shared_ptr<void> m_spSnapshot;
};

/// list of all processes in the system
class ProcessList
{
public:
   ProcessList();
   ProcessList(const Snapshot& ts){ RefreshList(ts); }

   size_t GetCount() const { return m_processList.size(); }

   const PROCESSENTRY32& GetEntry(size_t uiIndex)
   {
      ATLASSERT(uiIndex < GetCount());
      return m_processList[uiIndex];
   }

   void RefreshList(const Snapshot& ts);

private:
   std::vector<PROCESSENTRY32> m_processList;
};

/// \brief list of all threads in the system
/*! to find threads of a specific process, get a process/thread snapshot and
    check the THREADENTRY32's th32ThreadID member
*/
class ThreadList
{
public:
   ThreadList();
   ThreadList(const Snapshot& ts, const PROCESSENTRY32& pe)
      :m_dwProcessID(pe.th32ProcessID)
   {
      RefreshList(ts);
   }
   ThreadList(const Snapshot& ts, DWORD dwProcessID)
      :m_dwProcessID(dwProcessID)
   {
      RefreshList(ts);
   }

   size_t GetCount() const { return m_threadList.size(); }

   const THREADENTRY32& GetEntry(size_t uiIndex)
   {
      ATLASSERT(uiIndex < GetCount());
      return m_threadList[uiIndex];
   }

   void RefreshList(const Snapshot& ts);

private:
   DWORD m_dwProcessID;
   std::vector<THREADENTRY32> m_threadList;
};

/// \brief list of all modules loaded for given process
class ModuleList
{
public:
   ModuleList(const Snapshot& ts, const PROCESSENTRY32& pe)
      :m_dwProcessID(pe.th32ProcessID)
   {
      RefreshList(ts);
   }
   ModuleList(const Snapshot& ts, DWORD dwProcessID)
      :m_dwProcessID(dwProcessID)
   {
      RefreshList(ts);
   }

   size_t GetCount() const { return m_moduleList.size(); }

   const MODULEENTRY32& GetEntry(size_t uiIndex)
   {
      ATLASSERT(uiIndex < GetCount());
      return m_moduleList[uiIndex];
   }

   void RefreshList(const Snapshot& ts);

private:
   DWORD m_dwProcessID;
   std::vector<MODULEENTRY32> m_moduleList;
};

/// \brief list of all heaps allocated for given process
class HeapList
{
public:
   HeapList(const Snapshot& ts, const PROCESSENTRY32& pe)
      :m_dwProcessID(pe.th32ProcessID)
   {
      RefreshList(ts);
   }
   HeapList(const Snapshot& ts, DWORD dwProcessID)
      :m_dwProcessID(dwProcessID)
   {
      RefreshList(ts);
   }

   size_t GetCount() const { return m_heapList.size(); }

   const HEAPLIST32& GetEntry(size_t uiIndex)
   {
      ATLASSERT(uiIndex < GetCount());
      return m_heapList[uiIndex];
   }

   void RefreshList(const Snapshot& ts);

private:
   DWORD m_dwProcessID;
   std::vector<HEAPLIST32> m_heapList;
};

/// \brief list of all heap entries for given heap list entry
class HeapEntryList
{
public:
   HeapEntryList(const Snapshot& ts, const HEAPLIST32& hl)
      :m_dwProcessID(hl.th32ProcessID), m_ulHeapID(hl.th32HeapID)
   {
      RefreshList(ts);
   }

   size_t GetCount() const { return m_heapEntryList.size(); }

   const HEAPENTRY32& GetEntry(size_t uiIndex)
   {
      ATLASSERT(uiIndex < GetCount());
      return m_heapEntryList[uiIndex];
   }

   void RefreshList(const Snapshot& ts);

private:
   DWORD m_dwProcessID;
   ULONG_PTR m_ulHeapID;
   std::vector<HEAPENTRY32> m_heapEntryList;
};

} // namespace Toolhelp

} // namespace Debug
