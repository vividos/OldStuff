//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file TextElite.hpp Text elite game wrapper
//
#pragma once

// includes
#include <vector>
#include <string>
#include "PlanetInfo.hpp"

/// info about trade goods
class TradeGoodsInfo
{
public:
   TradeGoodsInfo(const CString& cszName, const CString& cszUnit)
      :m_cszName(cszName), m_cszUnit(cszUnit)
   {
   }

   CString Name() const { return m_cszName; }
   CString Unit() const { return m_cszUnit; }

private:
   CString m_cszName;
   CString m_cszUnit;
};

/// info about market item
class MarketInfo
{
public:
   MarketInfo(unsigned int uiQuantity, unsigned int uiPrice)
      :m_uiQuantity(uiQuantity),
       m_uiPrice(uiPrice)
   {
   }
   unsigned int Quantity() const { return m_uiQuantity; }
   unsigned int PriceInTenths() const { return m_uiPrice; }

private:
   unsigned int m_uiQuantity;
   unsigned int m_uiPrice;
};

/// wrapper to the TXTELITE.c code
class TextElite  
{
public:
   /// inits game
   static void Init();

   static bool Buy(unsigned int uiTradegoodIndex, unsigned int uiAmount, CString& cszResponse);
   static bool Sell(unsigned int uiTradegoodIndex, unsigned int uiAmount, CString& cszResponse);

   static bool Fuel(unsigned int uiTenthOfFuel, CString& cszResponse);

   static bool Jump(unsigned int uiPlanetIndex, CString& cszResponse);
   static void GalaxyJump();

   /// returns cash in tenths of CR
   static unsigned int GetCashInTenth();
   static unsigned int GetFuelInTenth();

   static unsigned int GetCurrentPlanet();

   static unsigned int GetCargoSpace();

   static PlanetInfo GetPlanetInfo(unsigned int uiPlanetIndex);

   static void GetTradeGoodsInfo(std::vector<TradeGoodsInfo>& vecTradegoods);

   static void GetMarketInfo(unsigned int uiPlanetIndex, std::vector<MarketInfo>& vecMarketInfos);

   static void GetCargoQuantities(std::vector<unsigned int>& vecQuantities);

   static CString GetInfo(unsigned int uiPlanetIndex);

   static void GetLocalPlanets(std::vector<PlanetInfo>& vecInfos);

   /// returns distance of two planet systems, in tenths of light years
   static unsigned short CalcDistance(unsigned int uiPlanetIndex1, unsigned int uiPlanetIndex2);
};
