#pragma once

// includes

namespace Debug
{

/// \brief process memory reader/writer
/// note that for reading from/writing to a process the appropriate system privilige
/// is necessary.
class ProcessMemory
{
public:
   /// ctor; takes process id
   ProcessMemory(DWORD dwProcessId): m_dwProcessId(dwProcessId){}

   /// reads from process memory
   bool Read(LPCVOID pAddress, LPVOID pBuffer, SIZE_T nLength);

   /// reads from process memory
   bool Write(LPVOID pAddress, LPCVOID pBuffer, SIZE_T nLength);

private:
   /// process id
   DWORD m_dwProcessId;
};

} // namespace Debug
