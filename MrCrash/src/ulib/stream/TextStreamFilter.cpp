//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file TextStreamFilter.cpp text stream filter
//

// includes
#include "stdafx.h"
#include <ulib/stream/TextStreamFilter.hpp>
#include <ulib/Exception.hpp>
#include <vector>

/// character for carriage return
const TCHAR c_cCR = _T('\r');

/// character for line feed
const TCHAR c_cLF = _T('\n');

using Stream::TextStreamFilter;

TextStreamFilter::TextStreamFilter(Stream::IStream& stream,
   ETextEncoding textEncoding,
   ELineEndingMode lineEndingMode)
:ITextStream(textEncoding, lineEndingMode),
 m_stream(stream),
 m_bPutbackChar(false),
 m_cPutbackChar(0)
{
   if (textEncoding == textEncodingNative)
#if defined(_UNICODE) || defined(UNICODE)
      m_textEncoding = textEncodingUCS2;
#else
      m_textEncoding = textEncodingAnsi;
#endif

   if (lineEndingMode == lineEndingNative)
#ifdef WIN32
      m_lineEndingMode = lineEndingCRLF;
#elif defined(__ANDROID__)
      m_lineEndingMode = lineEndingLF;
#else
#  error define proper line ending mode for this platform!
#endif
}

TCHAR TextStreamFilter::ReadChar()
{
   if (m_bPutbackChar)
   {
      m_bPutbackChar = false;
      return m_cPutbackChar;
   }

   TCHAR ch = 0;

   // depending on the text encoding type, read next byte
   switch (m_textEncoding)
   {
   case textEncodingNative:
      // note: textEncodingNative should have been set to the appropriate encoding in ctor!
      ATLASSERT(false);
      break;

   case textEncodingAnsi:
      {
         char cch[2];
         cch[0] = m_stream.ReadByte();
         cch[1] = 0;

         // use current codepage to convert
         USES_CONVERSION;
         ch = A2CT(cch)[0];
      }
      break;

   case textEncodingUTF8:
      {
         // read first octet; determines how much further octets are needed; all
         // remaining octets start with bit 7 set and bit 6 cleared (10xx xxxx)
         BYTE bLeading = m_stream.ReadByte();
         if (bLeading <= 0x7f)
         {
            // easy case: char in ASCII zone
            ch = static_cast<char>(bLeading);
            break;
         }

         // count bits from bit 7 until a 0 occurs
         unsigned int uiCount = 0;
         while((bLeading & 0x80) != 0)
            uiCount++, bLeading <<= 1;

         // only one bit? or more than 6? illegal value for leading octet!
         if (uiCount <= 1 || uiCount > 6)
            throw Exception(_T("illegal utf8 lead byte encountered"), __FILE__, __LINE__);

         // move remaining bits in position; 
         bLeading >>= uiCount;

         // move remaining bits to bits 6..n, so that the next byte's 6 bits
         // can fill out the dword
         DWORD dwBits = bLeading;

         // read in remaining bytes, remove bits 6 and 7 and add it to the resulting byte
         for(unsigned int ui=1; ui<uiCount; ui++)
         {
            BYTE bNext = m_stream.ReadByte();
            if ((bNext & 0xc0) != 0x80)
               throw Exception(_T("illegal utf8 byte encountered"), __FILE__, __LINE__);
            dwBits <<= 6;
            dwBits |= bNext & 0x3f;
         }

         if (dwBits > 0xffff)
            throw Exception(_T("utf8 character out of range"), __FILE__, __LINE__);

#if defined(_UNICODE) || defined(UNICODE)
         ch = static_cast<TCHAR>(dwBits & 0xffff);
#else
         WCHAR acData[2];
         acData[0] = static_cast<WCHAR>(dwBits & 0xffff);
         acData[1] = 0;
         USES_CONVERSION;
         ch = W2CT(acData)[0];
#endif
      }
      break;

   case textEncodingUCS2:
      // assume UCS2-LE
      ch = m_stream.ReadByte();
      ch |= static_cast<TCHAR>(m_stream.ReadByte()) << 8;
      break;

   default:
      ATLASSERT(false);
   }

   return ch;
}

void TextStreamFilter::ReadLine(CString& cszLine)
{
   cszLine.Empty();

   TCHAR cLastCharacter = 0;
#ifdef _WIN32_WCE
   ULONGLONG ullPos = 0UL;
#else
   ULONGLONG ullPos = 0ULL;
#endif

   // depending on the line type, read in a line of characters
   for(;!m_stream.AtEndOfStream();)
   {
      TCHAR ch = ReadChar();

      // carriage return: can be CRLF, CR, and "any"
      if (ch == c_cCR)
      {
         if (m_lineEndingMode == lineEndingCR)
            break; // found CR; in ReadAny mode a LF might follow

         if (m_lineEndingMode == lineEndingReadAny)
         {
            // found CR; a a LF might follow; read more chars
            cLastCharacter = ch;
            ullPos = m_stream.Position();
            continue;
         }

         if (m_lineEndingMode == lineEndingCRLF)
         {
            // check if at end of stream
            if (m_stream.AtEndOfStream())
               break;

            // threre might be a LF, but we don't know for sure, so check
            ch = ReadChar();
            if (c_cLF == ch)
               break; // recognized CRLF

            // some other char, just do normal processing by appending CR
            cszLine += c_cCR;

            // put back character
            PutBackChar(ch);
         }
      }
      else
      // line feed: can be LF, and the LF part in CRLF when in "any" mode
      if (ch == c_cLF)
      {
         if (m_lineEndingMode == lineEndingLF)
            break; // found LF

         // in any mode, the last character must be a CR to start a new line
         if (m_lineEndingMode == lineEndingReadAny)
         {
            // last character may have been a CR, so we got CRLF;
            // or it was another character, then we simply got LF
            break;
         }
      }
      else
      // not CR nor LF, but previous was a CR
      if (m_lineEndingMode == lineEndingReadAny && cLastCharacter == c_cCR)
      {
         // recognized a CR line ending; try to put back current 
         if (m_stream.CanSeek())
         {
            // seek back to previous position
            m_stream.Seek(ullPos, Stream::IStream::seekBegin);
         }
         else
         {
            // can't seek, so put back character
            PutBackChar(ch);
         }
         break; // line is finished
      }

      cszLine += ch;

      if (m_lineEndingMode == lineEndingReadAny)
         cLastCharacter = ch;
   }
}

void TextStreamFilter::Write(const CString& cszText)
{
   // write text in proper encoding
   USES_CONVERSION;
   DWORD dwWriteBytes = 0;

   switch (m_textEncoding)
   {
   case textEncodingAnsi:
      {
         LPCSTR pszText = T2CA(cszText);
         m_stream.Write(pszText, static_cast<DWORD>(strlen(pszText) * sizeof(*pszText)), dwWriteBytes);
      }
      break;

   case textEncodingUTF8:
      {
#ifdef UNICODE
         LPCWSTR pszText = cszText;
#else
         LPCWSTR pszText = T2CW(cszText);
#endif
         int iSize = WideCharToMultiByte(CP_UTF8, 0, pszText, static_cast<int>(wcslen(pszText)),
            NULL, 0, NULL, NULL);
         std::vector<BYTE> vecBuffer(iSize);

         int iRet = WideCharToMultiByte(CP_UTF8, 0,
            pszText, static_cast<int>(wcslen(pszText)),
            reinterpret_cast<LPSTR>(&vecBuffer[0]), iSize,
            NULL, NULL);
         ATLVERIFY(iRet == iSize);

         m_stream.Write(&vecBuffer[0], iRet, dwWriteBytes);
      }
      break;

   case textEncodingUCS2:
      {
         LPCWSTR pszText = T2CW(cszText);
         m_stream.Write(pszText, static_cast<DWORD>(wcslen(pszText) * sizeof(*pszText)), dwWriteBytes);
      }
      break;

   default:
      ATLASSERT(false);
      break;
   }
}

void TextStreamFilter::WriteEndline()
{
   ATLASSERT(m_lineEndingMode != lineEndingReadAny);

   switch(m_lineEndingMode)
   {
   case lineEndingCRLF: Write(_T("\r\n")); break;
   case lineEndingLF:   Write(_T("\n")); break;
   case lineEndingCR:   Write(_T("\r")); break;

   default:
      ATLASSERT(false);
      break;
   }
}

void TextStreamFilter::PutBackChar(TCHAR ch)
{
   ATLASSERT(false == m_bPutbackChar);

   m_cPutbackChar = ch;
   m_bPutbackChar = true;
}
