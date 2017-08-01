//
// ulib - a collection of useful classes
// Copyright (C) 2013-2016 Michael Fink
//
/// \file CString.hpp implementation of a platform independent CString class
//

// includes
#pragma once

// includes
#include <stdexcept>
#include <cstdarg>
#include <cstdio>
#include <atomic>
#include <iterator>
#include <algorithm>

// TODO remove
//#pragma warning(disable: 4100) // unreferenced formal parameter

#ifdef _MSC_VER
typedef int BOOL;
extern "C" __declspec(dllimport)
int
__stdcall
WideCharToMultiByte(
   UINT CodePage,
   DWORD dwFlags,
   LPCWSTR lpWideCharStr,
   int cchWideChar,
   LPSTR lpMultiByteStr,
   int cbMultiByte,
   LPCSTR lpDefaultChar,
   BOOL* lpUsedDefaultChar
);
extern "C" __declspec(dllimport)
int
__stdcall
MultiByteToWideChar(
   UINT CodePage,
   DWORD dwFlags,
   LPCSTR lpMultiByteStr,
   int cbMultiByte,
   LPWSTR lpWideCharStr,
   int cchWideChar
);

#elif defined(__ANDROID__)
// TODO
#endif

/// Character type traits, for single-byte char
template <typename T = CHAR>
struct CharTypeTraits
{
   typedef CHAR CharType;
   typedef WCHAR OtherCharType;

   typedef CharType* PXSTR;         ///< type of character string
   typedef const CharType* PCXSTR;  ///< type of character string; const version


   /// returns string length for narrow character
   static int StringLength(const CHAR* str)
   {
      if (str == nullptr)
         return 0;
      return strlen(str);
   }

   /// returns string length for narrow character, but only searches to a given max size
   static int StringLengthN(const CHAR* str, int maxSize)
   {
      if (str == nullptr)
         return 0;
      return strnlen(str, maxSize);
   }

   // narrow -> wide
   static int ConvertFromOther(LPWSTR strDestBuffer, int destLength, LPCSTR strSource)
   {
#ifdef _MSC_VER
#define CP_APC 0
      return ::MultiByteToWideChar(CP_ACP, 0, strSource, -1, strDestBuffer, destLength);
#elif defined(__ANDROID__)
      return 0; // TODO
#endif
   }

   static int FormatBuffer(PXSTR strBuffer, int bufferLength, PCXSTR strFormat, va_list args)
   {
#ifdef _MSC_VER
      if (strBuffer == 0 || bufferLength == 0)
         return _vscprintf(strFormat, args);

      return vsnprintf_s(strBuffer, bufferLength, bufferLength, strFormat, args);
#elif defined(__ANDROID__)
      if (strBuffer == 0 || bufferLength == 0)
         return vsnprintf(nullptr, 0, strFormat, args);

      return vsnprintf(strBuffer, bufferLength, strFormat, args);
#endif
   }

   static int StringCompare(PCXSTR lhs, PCXSTR rhs)
   {
      return strcmp(lhs, rhs);
   }

   static int StringCompareIgnoreCase(PCXSTR lhs, PCXSTR rhs)
   {
#ifdef _MSC_VER
      return strcmpi(lhs, rhs);
#elif defined(__ANDROID__)
      return strcasecmp(lhs, rhs);
#endif
   }

   static bool IsSpace(CharType ch)
   {
      return isspace(ch) != 0;
   }
};

/// Character type traits, for wchar_t, 16 bit, UCS
template <>
struct CharTypeTraits<WCHAR>
{
   typedef WCHAR CharType;
   typedef CHAR OtherCharType;

   typedef CharType* PXSTR;         ///< type of character string
   typedef const CharType* PCXSTR;  ///< type of character string; const version

   /// returns string length for wide character
   static int StringLength(const WCHAR* str)
   {
      if (str == nullptr)
         return 0;
      return wcslen(str);
   }

   /// returns string length for wide character, but only searches to a given max size
   static int StringLengthN(const WCHAR* str, int maxSize)
   {
#ifdef _MSC_VER
      if (str == nullptr)
         return 0;
      return wcsnlen(str, maxSize);
#elif defined(__ANDROID__)
      return -1; // TODO
#endif
   }

   // wide -> narrow
   static int ConvertFromOther(LPSTR strDestBuffer, int destLength, LPCWSTR strSource)
   {
#ifdef _MSC_VER
      return ::WideCharToMultiByte(CP_ACP, 0, strSource, -1, strDestBuffer, destLength, nullptr, nullptr);
#elif defined(__ANDROID__)
      return 0; // TODO
#endif
   }

   static int FormatBuffer(PXSTR strBuffer, int bufferLength, PCXSTR strFormat, va_list args)
   {
#ifdef _MSC_VER
      if (strBuffer == 0 || bufferLength == 0)
         return _vscwprintf(strFormat, args);

      return _vsnwprintf_s(strBuffer, bufferLength, bufferLength, strFormat, args);
#elif defined(__ANDROID__)
      return -1; // TODO
#endif
   }

   static int StringCompare(PCXSTR lhs, PCXSTR rhs)
   {
      return wcscmp(lhs, rhs);
   }

   static int StringCompareIgnoreCase(PCXSTR lhs, PCXSTR rhs)
   {
#ifdef _MSC_VER
      return _wcsicmp(lhs, rhs);
#elif defined(__ANDROID__)
      return -1; // TODO
#endif
   }

   static bool IsSpace(CharType ch)
   {
      return iswspace(ch) != 0;
   }
};

/// \brief A platform independent MFC CString template class
/// \details This class implements a platform independent string class
/// that is modeled after the CStringT class found in the MFC library.
template <typename T>
class CStringT
{
public:
   // typedefs

   /// Type of character managed in this string
   typedef typename CharTypeTraits<T>::CharType XCHAR;
   typedef XCHAR* PXSTR;         ///< Type of character string
   typedef const XCHAR* PCXSTR;  ///< Type of character string; const version

   /// Type of the "other" character type (char vs. wchar_t)
   typedef typename CharTypeTraits<T>::OtherCharType YCHAR;
   typedef YCHAR* PYSTR;         ///< Type of character string
   typedef const YCHAR* PCYSTR;  ///< Type of character string; const version

private:
   // nested structs

   /// Nested class: Actual string data
   // TODO make private?
   struct CStringData
   {
      int m_dataLength;         ///< actual length of string
      int m_allocLength;        ///< number of chars allocated in buffer
      std::atomic<int> m_numRefs;  ///< number of references
      // these fields are followed by the actual string buffer, and a zero terminator char

      void* GetData() throw()
      {
         return (this + 1);
      }

      void AddRef()
      {
         ++m_numRefs;
      }

      void Release()
      {
         ATLASSERT(m_numRefs != 0);
         if (--m_numRefs <= 0)
            Free();
      }

      bool IsShared() const throw()
      {
         return m_numRefs > 1;
      }

      static CStringData* Allocate(int length) throw()
      {
         ATLASSERT(length >= 0);
         if (length < 0)
            return nullptr;

         int dataBytes = (length + 1) * sizeof(XCHAR);
         // TODO align size
         int totalSize = sizeof(CStringData) + dataBytes;
         CStringData* data = (CStringData*)malloc(totalSize);

         if (data == nullptr)
            return nullptr;

         data->m_dataLength = 0;
         data->m_allocLength = length;
         data->m_numRefs = 1;

         return data;
      }

      /// Reallocates memory for string buffer; the buffer's content may be destroyed
      static CStringData* Reallocate(CStringData* data, int length) throw()
      {
         ATLASSERT(length >= 0);
         if (length < 0)
            return data;

         int dataBytes = (length + 1) * sizeof(XCHAR);
         int totalSize = sizeof(CStringData) + dataBytes;
         CStringData* newData = (CStringData*)realloc(data, totalSize);

         if (newData == nullptr)
         {
            free(data);
            return nullptr;
         }

         data = newData;

         data->m_dataLength = 0;
         data->m_allocLength = length;
         data->m_numRefs = 1;

         return data;
      }

      void Free()
      {
         free(this);
      }
   };

public:
   // ctors and dtor

   /// Default ctor
   CStringT()
   {
      CStringData* data = GetNilString();
      Attach(data);
   }

   /// Copy ctor
   CStringT(const CStringT& str)
   {
      CStringData* sourceData = str.GetData();
      CStringData* newData = CloneData(sourceData);
      Attach(newData);
   }

   /// Copy ctor; using other char type
   CStringT(const CStringT<YCHAR>& str)
   {
      Attach(GetNilString());

      int destLength = CharTypeTraits<YCHAR>::ConvertFromOther(nullptr, 0, str);
      if (destLength > 0)
      {
         PXSTR strBuffer = GetBuffer(destLength);

         CharTypeTraits<YCHAR>::ConvertFromOther(strBuffer, destLength, str);

         ReleaseBufferSetLength(destLength);
      }
      else
         Empty();
   }

   /// Ctor, taking a C style string, zero-terminated
   CStringT(PCXSTR str)
   {
      int length = CharTypeTraits<T>::StringLength(str);
      CStringData* data = Allocate(length);
      if (data == nullptr)
         throw std::runtime_error("CString: out of memory");

      Attach(data);
      SetLength(length);
      CopyChars(m_pszData, length, str, length);
   }

   /// Ctor, taking an other character type C style string, zero-terminated
   CStringT(PCYSTR str)
   {
      Attach(GetNilString());

      int destLength = CharTypeTraits<YCHAR>::ConvertFromOther(nullptr, 0, str);
      if (destLength > 0)
      {
         PXSTR strBuffer = GetBuffer(destLength);

         CharTypeTraits<YCHAR>::ConvertFromOther(strBuffer, destLength, str);

         ReleaseBufferSetLength(destLength);
      }
      else
         Empty();
   }

   /// Ctor, taking a C style string and a length
   CStringT(PCXSTR str, int length)
   {
      CStringData* data = Allocate(length);
      if (data == nullptr)
         throw std::runtime_error("CString: out of memory");

      Attach(data);
      SetLength(length);
      CopyChars(m_pszData, length, str, length);
   }

   /// Ctor, taking an other character type C style string and a length
   CStringT(PCYSTR str, int length)
   {
      Attach(GetNilString());

      CStringT<YCHAR> otherStr(str, length);

      SetString(otherStr);
   }

   /// Ctor, taking a char and a repeat count
   CStringT(XCHAR ch, int repeat = 1)
   {
      Attach(GetNilString());

      ATLASSERT(repeat >= 0);

      if (repeat <= 0)
         return;

      PXSTR pszBuffer = GetBuffer(repeat);

#ifdef _MSC_VER
      auto targetIter = stdext::checked_array_iterator<PXSTR>(pszBuffer, repeat);
#elif defined(__ANDROID__)
      auto targetIter = pszBuffer;
#endif
      std::fill_n(targetIter, repeat, ch);

      ReleaseBufferSetLength(repeat);
   }

   /// Ctor, taking an other char type char and a repeat count
   CStringT(YCHAR ch, int repeat = 1)
   {
      Attach(GetNilString());

      // TODO convert to other type, then flood characters
   }

   /// Dtor
   ~CStringT() throw()
   {
      CStringData* data = GetData();
      data->Release();
   }

   // operators

   /// Assign operator
   CStringT& operator=(const CStringT& str)
   {
      SetString(str);
      return *this;
   }

   /// Assign operator, C-style strings
   CStringT& operator=(PCXSTR str)
   {
      SetString(str);
      return *this;
   }

   /// In-place add operator
   CStringT& operator+=(const CStringT& str)
   {
      Append(str);
      return *this;
   }

   /// In-place add operator, C-style strings
   CStringT& operator+=(PCXSTR str)
   {
      Append(str);
      return *this;
   }

   /// In-place add operator, single character
   CStringT& operator+=(XCHAR str)
   {
      AppendChar(str);
      return *this;
   }

   /// Array operator
   XCHAR operator[](int iIndex) const
   {
      ATLASSERT(iIndex >= 0 && iIndex <= GetLength()); // indexing zero terminator is ok

      if (!(iIndex >= 0 && iIndex <= GetLength()))
         throw std::invalid_argument("CString: invalid index argument to array operator");

      return m_pszData[iIndex];
   }

   /// C-style string "cast" operator
   operator PCXSTR() const throw()
   {
      return m_pszData;
   }

   /// Appends a C-style string
   void Append(PCXSTR str)
   {
      Append(str, CharTypeTraits<XCHAR>::StringLength(str));
   }

   /// Appends a string
   void Append(const CStringT& str)
   {
      Append(str.GetString(), str.GetLength());
   }

   /// Appends a string, with given length
   void Append(PCXSTR str, int length)
   {
      ATLASSERT(length >= 0);
      if (length < 0)
         throw std::runtime_error("CString::Append: illegal parameter");

      // limit length to actual zero terminating char
      length = CharTypeTraits<T>::StringLengthN(str, length);

      int iOldLength = GetLength();
      ATLASSERT(iOldLength >= 0);

      int newLength = iOldLength + length;

      PXSTR pszBuffer = GetBuffer(newLength);

      CopyChars(pszBuffer + iOldLength, length, str, length);

      ReleaseBufferSetLength(newLength);
   }

   /// Appends a single character
   void AppendChar(XCHAR ch)
   {
      // TODO
   }

   /// Empties the string
   void Empty() throw()
   {
      if (GetLength() == 0)
         return;

      CStringData* oldData = GetData();
      oldData->Release();

      CStringData* newData = GetNilString();
      Attach(newData);
   }

   /// Returns character at given index
   XCHAR GetAt(int iIndex)
   {
      ATLASSERT(iIndex >= 0 && iIndex <= GetLength()); // indexing zero terminator is ok

      if (!(iIndex >= 0 && iIndex <= GetLength()))
         throw std::invalid_argument("CString: invalid index argument to GetAt()");

      return m_pszData[iIndex];
   }

   /// Returns a writable buffer of current string
   PXSTR GetBuffer()
   {
      CStringData* data = GetData();

      if (data->IsShared())
         Fork(data->m_dataLength);

      return m_pszData;
   }

   /// Returns a writable buffer of current string, with minimum given buffer length
   PXSTR GetBuffer(int bufferLength)
   {
      return PrepareWrite(bufferLength);
   }

   PXSTR GetBufferSetLength(int bufferLength)
   {
      PXSTR pszBuffer = GetBuffer(bufferLength);
      SetLength(bufferLength);

      return pszBuffer;
   }

   /// Releases a buffer retrieved by GetBuffer*() functions, and sets a new length;
   /// when -1 is passed, finds the new length by looking for zero terminating character
   void ReleaseBuffer(int newLength = -1)
   {
      if (newLength == -1)
      {
         int maxSize = GetData()->m_allocLength;
         newLength = CharTypeTraits<T>::StringLengthN(m_pszData, maxSize);
      }

      SetLength(newLength);
   }

   /// Releases a buffer retrieved by GetBuffer*() functions, and sets a new length
   void ReleaseBufferSetLength(int newLength)
   {
      ATLASSERT(newLength >= 0);

      SetLength(newLength);
   }

   /// Returns string length, without string-terminating zero character
   int GetLength() const throw()
   {
      return GetData()->m_dataLength;
   }

   /// Returns raw C string
   PCXSTR GetString() const throw()
   {
      return m_pszData;
   }

   /// Returns if string is empty
   bool IsEmpty() const throw()
   {
      return GetLength() == 0;
   }

   /// Sets character at given index
   void SetAt(int iIndex, XCHAR ch)
   {
      ATLASSERT(iIndex >= 0 && iIndex < GetLength()); // indexing zero terminator is NOT ok

      if (!(iIndex >= 0 && iIndex < GetLength()))
         throw std::invalid_argument("CString: invalid index argument to SetAt()");

      // TODO do differently?
      m_pszData[iIndex] = ch;
   }

   /// Sets new string
   void SetString(PCXSTR str)
   {
      SetString(str, CharTypeTraits<T>::StringLength(str));
   }

   /// Sets new string, with given length
   void SetString(PCXSTR str, int length)
   {
      if (length == 0)
      {
         Empty();
         return;
      }

      if (str == nullptr)
         throw std::runtime_error("CString::SetString: illegal parameter");

      // TODO remove?
      // check if the string to set is overlapped (in our own buffer)
      //ptrdiff_t iOffset = src - GetString();
      //int iOldLength = GetLength();

      //bool bIsOverlapped = iOffset < iOldLength;

      // copy
      PXSTR pszBuffer = GetBuffer(length);
      int iAllocLength = GetData()->m_allocLength;

      // TODO remove?
      //if (bIsOverlapped)
      //   CopyCharsOverlapped(pszBuffer, iAllocLength, str, length);
      //else
         CopyChars(pszBuffer, iAllocLength, str, length);

      ReleaseBufferSetLength(length);
   }

   // friend operators

   /// Add operator, with two CStringT's
   friend CStringT operator+(const CStringT& lhs, const CStringT& rhs)
   {
      CStringT result(lhs);
      result.Append(rhs);
      return result;
   }

   /// Add operator, with a C-style string and a CStringT
   friend CStringT operator+(PCXSTR lhs, const CStringT& rhs)
   {
      CStringT result(lhs);
      result.Append(rhs);
      return result;
   }

   /// Add operator, with a CStringT and a C-style string
   friend CStringT operator+(const CStringT& lhs, PCXSTR rhs)
   {
      CStringT result(lhs);
      result.Append(rhs);
      return result;
   }

   // comparison operators

   /// Equality operator, with two CStringT's
   friend bool operator==(const CStringT& lhs, const CStringT& rhs) throw()
   {
      return lhs.Compare(rhs) == 0;
   }

   /// Equality operator, with a C-style string and a CStringT
   friend bool operator==(PCXSTR lhs, const CStringT& rhs) throw()
   {
      return rhs.Compare(lhs) == 0;
   }

   /// Equality operator, with a CStringT and a C-style string
   friend bool operator==(const CStringT& lhs, PCXSTR rhs) throw()
   {
      return lhs.Compare(rhs) == 0;
   }

   /// Inequality operator, with two CStringT's
   friend bool operator!=(const CStringT& lhs, const CStringT& rhs) throw()
   {
      return lhs.Compare(rhs) != 0;
   }

   /// Inequality operator, with a C-style string and a CStringT
   friend bool operator!=(PCXSTR lhs, const CStringT& rhs) throw()
   {
      return rhs.Compare(lhs) != 0;
   }

   /// Inequality operator, with a CStringT and a C-style string
   friend bool operator!=(const CStringT& lhs, PCXSTR rhs) throw()
   {
      return lhs.Compare(rhs) != 0;
   }

   /// Less-than operator, with two CStringT's
   friend bool operator<(const CStringT& lhs, const CStringT& rhs) throw()
   {
      return lhs.Compare(rhs) < 0;
   }

   /// Less-than operator, with a C-style string and a CStringT
   friend bool operator<(PCXSTR lhs, const CStringT& rhs) throw()
   {
      return rhs.Compare(lhs) > 0;
   }

   /// Less-than operator, with a CStringT and a C-style string
   friend bool operator<(const CStringT& lhs, PCXSTR rhs) throw()
   {
      return lhs.Compare(rhs) < 0;
   }

   /// Greater-than operator, with two CStringT's
   friend bool operator>(const CStringT& lhs, const CStringT& rhs) throw()
   {
      return lhs.Compare(rhs) > 0;
   }

   /// Greater-than operator, with a C-style string and a CStringT
   friend bool operator>(PCXSTR lhs, const CStringT& rhs) throw()
   {
      return rhs.Compare(lhs) < 0;
   }

   /// Greater-than operator, with a CStringT and a C-style string
   friend bool operator>(const CStringT& lhs, PCXSTR rhs) throw()
   {
      return lhs.Compare(rhs) > 0;
   }

   /// Less-or-equal operator, with two CStringT's
   friend bool operator<=(const CStringT& lhs, const CStringT& rhs) throw()
   {
      return lhs.Compare(rhs) <= 0;
   }

   /// Less-or-equal operator, with a C-style string and a CStringT
   friend bool operator<=(PCXSTR lhs, const CStringT& rhs) throw()
   {
      return rhs.Compare(lhs) >= 0;
   }

   /// Less-or-equal operator, with a CStringT and a C-style string
   friend bool operator<=(const CStringT& lhs, PCXSTR rhs) throw()
   {
      return lhs.Compare(rhs) <= 0;
   }

   /// Greater-or-equal operator, with two CStringT's
   friend bool operator>=(const CStringT& lhs, const CStringT& rhs) throw()
   {
      return lhs.Compare(rhs) >= 0;
   }

   /// Greater-or-equal operator, with a C-style string and a CStringT
   friend bool operator>=(PCXSTR lhs, const CStringT& rhs) throw()
   {
      return rhs.Compare(lhs) <= 0;
   }

   /// Greater-or-equal operator, with a CStringT and a C-style string
   friend bool operator>=(const CStringT& lhs, PCXSTR rhs) throw()
   {
      return lhs.Compare(rhs) >= 0;
   }

   // higher-level functions

   /// Compares this string with another string, and returns order
   /// \retval -1 when this string is ordered earlier than str
   /// \retval 0 when both strings are ordered equal
   /// \retval 1 when this string is ordered later than str
   int Compare(PCXSTR str) const
   {
      return CharTypeTraits<T>::StringCompare(GetString(), str);
   }

   /// Compares this string with another string, and returns order, ignoring
   /// lowercase/uppercase characters
   /// \retval -1 when this string is ordered earlier than str
   /// \retval 0 when both strings are ordered equal
   /// \retval 1 when this string is ordered later than str
   int CompareNoCase(PCXSTR str) const
   {
      return CharTypeTraits<T>::StringCompareIgnoreCase(GetString(), str);
   }

   int Delete(int iIndex, int count = 1)
   {
      // TODO implement
      return 0;
   }

   int Insert(int iIndex, XCHAR ch)
   {
      // TODO implement
      return 0;
   }

   int Insert(int iIndex, PCXSTR str)
   {
      // TODO implement
      return 0;
   }

   int Replace(XCHAR chOld, XCHAR chNew)
   {
      // TODO implement
      return 0;
   }

   int Replace(PCXSTR strOld, PCXSTR strNew)
   {
      // TODO implement
      return 0;
   }

   int Remove(XCHAR chRemove)
   {
      // TODO implement
      return 0;
   }

   CStringT Tokenize(PCXSTR pszTokens, int& iStart) const
   {
      // TODO implement
      return CStringT();
   }

   int Find(XCHAR /*ch*/, int /*iStart*/ = 0) const throw()
   {
      // TODO implement
      return 0;
   }

   int Find(PCXSTR /*str*/, int /*iStart*/ = 0) const throw()
   {
      return 0;
   }

   int FindOneOf(PCXSTR /*strCharColl*/) const throw()
   {
      // TODO implement
      return 0;
   }

   int ReverseFind(XCHAR ch) const throw()
   {
      // TODO implement
      return 0;
   }

   CStringT& MakeUpper()
   {
      return *this;
   }

   CStringT& MakeLower()
   {
      return *this;
   }

   CStringT& MakeReverse()
   {
      // TODO implement
      return *this;
   }

   /// Remove all whitespace characters from start of string
   CStringT& TrimLeft()
   {
      // TODO implement
      return *this;
   }

   /// Remove a single character from start of string
   CStringT& TrimLeft(XCHAR ch)
   {
      // TODO implement
      return *this;
   }

   /// Removes all characters in the given string, from start of string
   CStringT& TrimLeft(PCXSTR str)
   {
      // TODO implement
      return *this;
   }

   /// Remove all whitespace characters from end of string
   CStringT& TrimRight()
   {
      // TODO implement
      return *this;
   }

   /// Remove a single character from end of string
   CStringT& TrimRight(XCHAR ch)
   {
      // TODO implement
      return *this;
   }

   /// Removes all characters in the given string, from end of string
   CStringT& TrimRight(PCXSTR str)
   {
      // TODO implement
      return *this;
   }

   /// Removes all whitespace characters from start and end of string
   CStringT& Trim()
   {
      return TrimRight().TrimLeft();
   }

   /// Removes single character from start and end of string
   CStringT& Trim(XCHAR ch)
   {
      return TrimRight(ch).TrimLeft(ch);
   }

   /// Removes all characters in the given string, from start and end of string
   CStringT& Trim(PCXSTR str)
   {
      return TrimRight(str).TrimLeft(str);
   }

   /// Returns a substring starting with the given index as first character
   CStringT Mid(int firstPos) const
   {
      return Mid(firstPos, GetLength() - firstPos);
   }

   /// Returns a substring starting with the given index as first character,
   /// and a given length
   CStringT Mid(int firstPos, int count) const
   {
      if (firstPos < 0)
         firstPos = 0;
      if (count < 0)
         count = 0;

      int length = GetLength();
      if (firstPos + count > length)
         count = length - firstPos;

      if (firstPos > length)
         count = 0; // return empty string

      if (firstPos == 0 && count >= length)
         return *this;

      return CStringT(GetString() + firstPos, count);
   }

   /// Returns a substring with number of characters from the right side of
   /// the string
   CStringT Right(int count) const
   {
      if (count < 0)
         count = 0;

      int length = GetLength();

      if (count >= length)
         return *this;

      return CStringT(GetString() + length - count, count);
   }

   /// Returns a substring with number of characters from the left side of the
   /// string
   CStringT Left(int count) const
   {
      if (count < 0)
         count = 0;

      int length = GetLength();

      if (count >= length)
         return *this;

      return CStringT(GetString(), count);
   }

   CStringT SpanIncluding(PCXSTR strCharColl) const
   {
      // TODO implement
      return CStringT();
   }

   CStringT SpanExcluding(PCXSTR strCharColl) const
   {
      // TODO implement
      return CStringT();
   }

   // formatting

   /// Formats text using format string and variable list of arguments and
   /// sets it as new string.
   void Format(PCXSTR strFormat, ...)
   {
      va_list args;
      va_start(args, strFormat);

      FormatV(strFormat, args);

      va_end(args);
   }

   /// Formats text using format string and variable list of arguments and
   /// appends it to the current string
   void AppendFormat(PCXSTR strFormat, ...)
   {
      va_list args;
      va_start(args, strFormat);

      AppendFormatV(strFormat, args);

      va_end(args);
   }

   /// Formats text using format string and variable list of arguments and
   /// sets it as new string; va_args version
   void FormatV(PCXSTR strFormat, va_list args)
   {
      int bufferLength = CharTypeTraits<T>::FormatBuffer(nullptr, 0, strFormat, args);

      PXSTR strBuffer = GetBufferSetLength(bufferLength);
      if (strBuffer == nullptr)
         throw std::runtime_error("CString: out of memory");

      int length = CharTypeTraits<T>::FormatBuffer(strBuffer, bufferLength + 1, strFormat, args);
      ATLASSERT(bufferLength == length);

      ReleaseBufferSetLength(length);
   }

   /// Formats text using format string and variable list of arguments and
   /// appends it to the current string; va_args version
   void AppendFormatV(PCXSTR strFormat, va_list args)
   {
      CStringT str;
      str.FormatV(strFormat, args);

      Append(str);
   }

private:
   /// Optimisation: empty string data that never gets collected by ref count
   struct CNilStringData : public CStringData
   {
      /// Ctor
      CNilStringData()
      {
         CStringData::m_numRefs = 2; // ensures that the nil string never gets freed
         CStringData::m_dataLength = 0;
         CStringData::m_allocLength = 0;

         extraBuffer[0] = extraBuffer[1] = 0;
      }

   private:
      /// Extra buffer for null terminating characters
      wchar_t extraBuffer[2];
   };

   /// Returns the string data struct
   CStringData* GetData() const throw()
   {
      // this class uses the same layout as the MFC CString class;
      // it allocates a space of two pointers, the first pointer being
      // a CStringData struct, and the second pointer points to the
      // buffer actually holding the string data.
      return reinterpret_cast<CStringData*>(m_pszData) - 1;
   }

   /// Allocates a new string data and string data
   CStringData* Allocate(int length) throw()
   {
      return CStringData::Allocate(length);
   }

   /// Frees string data
   void Free(CStringData* data)
   {
      data->Free();
   }

   /// Attaches a new string data
   void Attach(CStringData* data)
   {
      m_pszData = static_cast<PXSTR>(data->GetData());
   }

   /// Sets a new length for stored string; doesn't reallocate
   void SetLength(int length)
   {
      ATLASSERT(length >= 0);
      ATLASSERT(length <= GetData()->m_allocLength);

      if (length < 0 || length > GetData()->m_allocLength)
         throw std::invalid_argument("CString: invalid length argument to SetLength()");

      GetData()->m_dataLength = length;
      m_pszData[length] = 0;
   }

   /// Makes string non-shared by creating a unique copy
   void Fork(int newLength)
   {
      CStringData* oldData = GetData();
      int iOldLength = oldData->m_dataLength;

      CStringData* newData = Allocate(newLength);
      if (newData == nullptr)
         throw std::runtime_error("CString: out of memory");

      int iCharsToCopy = std::min(iOldLength, newLength) + 1;

      CopyChars(
         static_cast<PXSTR>(newData->GetData()), iCharsToCopy,
         static_cast<PXSTR>(oldData->GetData()), iCharsToCopy);

      newData->m_dataLength = newLength;

      oldData->Release();

      Attach(newData);
   }

   /// Prepares the string buffer for writing, to have at least given length;
   /// checks if buffer is shared or too small.
   PXSTR PrepareWrite(int length)
   {
      if (length < 0)
         throw std::runtime_error("CString::PrepareWrite(): invalid length");

      CStringData* oldData = GetData();

      bool isTooShort = oldData->m_allocLength < length;

      if (oldData->IsShared() || isTooShort)
         PrepareWrite2(length);

      return m_pszData;
   }

   /// Extends the buffer and makes it non-shared (unique)
   PXSTR PrepareWrite2(int length)
   {
      CStringData* oldData = GetData();

      // resize to whatever is bigger; current or given size
      length = std::max(oldData->m_dataLength, length);

      if (oldData->IsShared())
         Fork(length);
      else
         if (oldData->m_allocLength < length)
         {
            // unique, but buffer too small; increase buffer size
            ResizeBuffer(length);
         }

      return m_pszData;
   }

   /// Resizes the buffer to be at least given size; increases buffer size by factor
   void ResizeBuffer(int length)
   {
      CStringData* oldData = GetData();
      int newLength = oldData->m_allocLength;

      if (oldData->m_allocLength <= 1024 * 1024 * 1024) // 1 GB
      {
         // under 1 GB size: increase buffer by factor 1.5
         newLength = newLength + newLength / 2;
      }
      else
      {
         // over 1 GB size: add another 1 MB
         newLength += 1024 * 1024;
      }

      newLength = std::max(newLength, length);

      Reallocate(newLength);
   }

   /// Reallocates buffer to be exactly the given size; current string is copied over
   void Reallocate(int newLength)
   {
      CStringData* oldData = GetData();
      ATLASSERT(oldData->m_allocLength < newLength); // must only be called to enlarge the buffer

      if (newLength <= 0 || oldData->m_allocLength >= newLength)
         throw std::runtime_error("CString::Reallocate: illegal parameter");

      CStringData* newData = CStringData::Reallocate(oldData, newLength);
      if (newData == nullptr)
         throw std::runtime_error("CString::Reallocate: out of memory");

      Attach(newData);
   }

   /// Clones string data and contained string
   static CStringData* CloneData(CStringData* data)
   {
      CStringData* newData = CStringData::Allocate(data->m_dataLength);
      if (newData == nullptr)
         throw std::runtime_error("CString: out of memory");

      newData->m_dataLength = data->m_dataLength;

      CopyChars(
         static_cast<PXSTR>(newData->GetData()), newData->m_dataLength + 1,
         static_cast<PXSTR>(data->GetData()), data->m_dataLength + 1);

      return newData;
   }

   /// Copies characters from the source, including length, to target buffer;
   /// also handles overlapped cases
   static void CopyChars(PXSTR strTargetBuffer, int lenTargetBuffer,
      PCXSTR strSource, int lenSource)
   {
#ifdef _MSC_VER
      auto targetIter = stdext::checked_array_iterator<PXSTR>(strTargetBuffer, lenTargetBuffer);
#elif defined(__ANDROID__)
      auto targetIter = strTargetBuffer;
#endif

      // using std::copy automatically handles overlapped cases
      std::copy(strSource, strSource + lenSource, targetIter);
   }

   /// Returns a statically allocated CStringData struct that
   /// represents an empty string
   static CStringData* GetNilString()
   {
      // TODO not thread safe
      static CNilStringData s_nilData;

      s_nilData.AddRef();
      return &s_nilData;
   }

private:
   /// String data
   PXSTR m_pszData;
};

/// String for ANSI characters
typedef CStringT<CHAR> CStringA;

/// String for UCS, 16 bit characters
typedef CStringT<WCHAR> CStringW;

/// String for characters, type determined by Unicode setting
typedef CStringT<TCHAR> CString;
