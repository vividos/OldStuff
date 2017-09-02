//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file VideoInfo.hpp Video info class
//
#pragma once

class VideoInfo
{
public:
   VideoInfo() throw()
      :m_iId(-1),
      m_uiFadein(0),
      m_uiFadeout(0),
      m_bChanged(false)
   {
   }

   VideoInfo(const CString& cszName,
      const CString& cszAddress,
      unsigned int uiFadein,
      unsigned int uiFadeout) throw()
      :m_iId(-1),
      m_cszName(cszName),
      m_cszAddress(cszAddress),
      m_uiFadein(uiFadein),
      m_uiFadeout(uiFadeout),
      m_bChanged(true)
   {
   }

   // get methods

   int Id() const throw() { return m_iId; }
   CString Name() const throw() { return m_cszName; }
   CString Address() const throw() { return m_cszAddress; }
   unsigned int Fadein() const throw() { return m_uiFadein; }
   unsigned int Fadeout() const throw() { return m_uiFadeout; }

   // set methods

   void Id(int iId) throw() { m_iId = iId; }
   void Name(const CString& cszName) throw() { m_cszName = cszName; m_bChanged = true; }
   void Address(const CString& cszAddress) throw() { m_cszAddress = cszAddress; m_bChanged = true; }
   void Fadein(unsigned int uiFadein) throw() { m_uiFadein = uiFadein; m_bChanged = true; }
   void Fadeout(unsigned int uiFadeout) throw() { m_uiFadeout = uiFadeout; m_bChanged = true; }

private:
   bool m_bChanged;

   int m_iId;
   CString m_cszName;
   CString m_cszAddress;
   unsigned int m_uiFadein;
   unsigned int m_uiFadeout;
};
