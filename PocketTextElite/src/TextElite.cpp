//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file TextElite.cpp Text elite game wrapper
//

// includes
#include "stdafx.h"
#include "TextElite.hpp"

char* g_pNull = "";

// //////////////////////////////////////

#define galsize (256)
#define lasttrade 16
#define maxlen (20) /* Length of strings */

#pragma pack(push)
#pragma pack(1)

typedef struct
{   unsigned short x;
   unsigned short y;       /* One byte unsigned */
   unsigned short economy; /* These two are actually only 0-7  */
   unsigned short govtype;   
   unsigned short techlev; /* 0-16 i think */
   unsigned short population;   /* One byte */
   unsigned short productivity; /* Two byte */
   unsigned short radius; /* Two byte (not used by game at all) */
   char _dummy[4];
   char name[12];
} plansys ;

typedef struct
{                         /* In 6502 version these were: */
   unsigned short baseprice;        /* one byte */
   signed short gradient;   /* five bits plus sign */
   unsigned short basequant;        /* one byte */
   unsigned short maskbyte;         /* one byte */
   unsigned short units;            /* two bits */
   char   name[20];         /* longest="Radioactives" */
  } tradegood ;


typedef struct
{ unsigned short quantity[lasttrade+1];
  unsigned short price[lasttrade+1];
} markettype ;

#pragma pack(pop)

extern "C"
plansys galaxy[galsize];

extern "C"
tradegood commodities[lasttrade];

// //////////////////////////////////////

#include <string>
#include <vector>

std::vector<char> g_vecPrintfText;

void textelite_clear()
{
   g_vecPrintfText.clear();
}

void textelite_get(CString& cszText)
{
   std::string str(g_vecPrintfText.begin(), g_vecPrintfText.end());
   cszText = str.c_str();
   cszText.TrimLeft(_T("\r\n"));
   cszText.TrimRight();
}

// //////////////////////////////////////

extern "C"
void init();

void TextElite::Init()
{
   init();
}

// //////////////////////////////////////

extern "C"
char tradnames[lasttrade][maxlen];

extern "C"
int dobuy(char*);

bool TextElite::Buy(unsigned int uiTradegoodIndex, unsigned int uiAmount, CString& cszResponse)
{
   char text[maxlen*2];
   _snprintf(text, sizeof(text), "%s %u", tradnames[uiTradegoodIndex], uiAmount);

   textelite_clear();

   int iRet = dobuy(text);
   textelite_get(cszResponse);
   return iRet != 0;
}

extern "C"
int dosell(char*);

bool TextElite::Sell(unsigned int uiTradegoodIndex, unsigned int uiAmount, CString& cszResponse)
{
   char text[maxlen*2];
   _snprintf(text, sizeof(text), "%s %u", tradnames[uiTradegoodIndex], uiAmount);

   textelite_clear();

   int iRet = dosell(text);
   textelite_get(cszResponse);
   return iRet != 0;
}

// //////////////////////////////////////

extern "C"
int dofuel(char*);

bool TextElite::Fuel(unsigned int uiTenthOfFuel, CString& cszResponse)
{
   char text[16];
   _snprintf(text, sizeof(text), "%u.%01u", uiTenthOfFuel / 10, uiTenthOfFuel % 10);

   textelite_clear();

   int iRet = dofuel(text);
   textelite_get(cszResponse);
   return iRet != 0;
}

// //////////////////////////////////////

extern "C"
int dojump(char *);

bool TextElite::Jump(unsigned int uiPlanetIndex, CString& cszResponse)
{
   ATLASSERT(uiPlanetIndex < galsize);

   textelite_clear();

   int iRet = dojump(galaxy[uiPlanetIndex].name);
   textelite_get(cszResponse);
   return iRet != 0;
}

// //////////////////////////////////////

extern "C" int dogalhyp(char* s);

void TextElite::GalaxyJump()
{
   dogalhyp(g_pNull);
}

// //////////////////////////////////////

extern "C"
extern signed long cash;

/// returns cash in tenth of CR
unsigned int TextElite::GetCashInTenth()
{
   return static_cast<unsigned int>(::cash);
}

// //////////////////////////////////////

extern "C"
extern unsigned short fuel;

unsigned int TextElite::GetFuelInTenth()
{
   return static_cast<unsigned int>(::fuel);
}

// //////////////////////////////////////

extern "C"
extern int currentplanet;

unsigned int TextElite::GetCurrentPlanet()
{
   return static_cast<unsigned int>(currentplanet);
}

// //////////////////////////////////////

extern "C"
extern unsigned short holdspace;

unsigned int TextElite::GetCargoSpace()
{
   return static_cast<unsigned int>(holdspace);
}

// //////////////////////////////////////

PlanetInfo TextElite::GetPlanetInfo(unsigned int uiPlanetIndex)
{
   ATLASSERT(uiPlanetIndex < galsize);

   PlanetInfo pi(uiPlanetIndex, galaxy[uiPlanetIndex].name,
      galaxy[uiPlanetIndex].x,
      galaxy[uiPlanetIndex].y / 2
      );

   // TODO do more fields

   return pi;
}

// //////////////////////////////////////

extern "C"
char unitnames[][5];

void TextElite::GetTradeGoodsInfo(std::vector<TradeGoodsInfo>& vecTradegoods)
{
   for(unsigned int ui=0; ui<lasttrade+1; ui++)
   {
      CString cszName(commodities[ui].name);
      cszName.TrimRight();
      vecTradegoods.push_back(
         TradeGoodsInfo(cszName, unitnames[commodities[ui].units % 3]));
   }
}


extern "C"
markettype localmarket;
extern "C"
markettype genmarket(unsigned short fluct, plansys p);

void TextElite::GetMarketInfo(unsigned int uiPlanetIndex, std::vector<MarketInfo>& vecMarketInfos)
{
   ATLASSERT(uiPlanetIndex < galsize);

   const markettype& market = uiPlanetIndex == GetCurrentPlanet() ? localmarket : genmarket(0x00,galaxy[uiPlanetIndex]);

   for(unsigned int ui=0; ui<lasttrade+1; ui++)
   {
      vecMarketInfos.push_back(MarketInfo(market.quantity[ui], market.price[ui]));
   }
}

// //////////////////////////////////////
extern "C" 
unsigned short shipshold[lasttrade+1];

void TextElite::GetCargoQuantities(std::vector<unsigned int>& vecQuantities)
{
   for(unsigned int i=0; i<lasttrade+1; i++)
      vecQuantities.push_back(shipshold[i]);
}

// //////////////////////////////////////

extern "C"
void prisys(plansys plsy, int compressed);

CString TextElite::GetInfo(unsigned int uiPlanetIndex)
{
   textelite_clear();

   prisys(galaxy[uiPlanetIndex], false);

   CString cszResponse;
   textelite_get(cszResponse);

   return cszResponse;
}

// //////////////////////////////////////

extern "C"
unsigned short distance(plansys a,plansys b);

const int maxfuel =70; /* 7.0 LY tank */

void TextElite::GetLocalPlanets(std::vector<PlanetInfo>& vecInfos)
{
   for (int i=0;i<galsize;++i)
   {
      unsigned short d=distance(galaxy[i],galaxy[currentplanet]);
      if(d <= maxfuel + 30) // 30: safe margin
         vecInfos.push_back(GetPlanetInfo(i));
   }
}

// //////////////////////////////////////

unsigned short TextElite::CalcDistance(unsigned int uiPlanetIndex1, unsigned int uiPlanetIndex2)
{
   ATLASSERT(uiPlanetIndex1 < galsize);
   ATLASSERT(uiPlanetIndex2 < galsize);

   return distance(galaxy[uiPlanetIndex1], galaxy[uiPlanetIndex2]);
}

// //////////////////////////////////////

#include <cstdarg>

extern "C"
int textelite_printf(const char* s, ...)
{
   va_list v;
   va_start(v,s);

   char buffer[256];

   int iRet = _vsnprintf(buffer, sizeof(buffer)/sizeof(*buffer), s, v);

   // add to global buffer
   if (iRet > 0)
      g_vecPrintfText.insert(g_vecPrintfText.end(), buffer, buffer + iRet);

   va_end(s);
   return iRet;
}
