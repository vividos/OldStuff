//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file MarketView.cpp Market view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "IMainFrame.hpp"
#include "MarketView.hpp"
#include "TextElite.hpp"

LRESULT MarketView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   // set up title font
   CFontHandle fh = m_scTitle.GetFont();

   LOGFONT lf = {0};
   fh.GetLogFont(lf);
   lf.lfWeight = FW_BOLD;
   if (lf.lfHeight < 0)
      lf.lfHeight -= 11;
   else
      lf.lfHeight += 11;

   CFont fontBig;
   ATLVERIFY(fontBig.CreateFontIndirect(&lf));

   m_scTitle.SetFont(fontBig);

   // create trade goods list
   {
      m_lcTradegoods.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

      m_lcTradegoods.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 70);
      m_lcTradegoods.InsertColumn(1, _T("Unit"), LVCFMT_LEFT, 34);
      m_lcTradegoods.InsertColumn(2, _T("Qty"), LVCFMT_LEFT, 35);
      m_lcTradegoods.InsertColumn(3, _T("Price"), LVCFMT_LEFT, 35);
      m_lcTradegoods.InsertColumn(4, _T("Cargo"), LVCFMT_LEFT, 45);

      std::vector<TradeGoodsInfo> vecTradegoods;
      TextElite::GetTradeGoodsInfo(vecTradegoods);

      for (size_t i=0, iMax = vecTradegoods.size(); i<iMax; i++)
      {
         const TradeGoodsInfo& info = vecTradegoods[i];

         int iItem = m_lcTradegoods.InsertItem(static_cast<int>(i), info.Name());
         m_lcTradegoods.SetItemText(iItem, 1, info.Unit());
      }
   }

   UpdateView();

   DlgResize_Init(false, true);

   return 1;
}

LRESULT MarketView::OnClickedButtonMarketBuy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   unsigned int uiIndex = m_lcTradegoods.GetSelectedIndex();
   if (uiIndex < 0)
      return 0; // none selected

   CString cszResponse;
   bool bRet = TextElite::Buy(uiIndex, 1, cszResponse);
   m_mainFrame.ShowResponse(cszResponse, bRet);

   UpdateView();
   m_lcTradegoods.SetFocus();
   return 0;
}

LRESULT MarketView::OnClickedButtonMarketSell(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   unsigned int uiIndex = m_lcTradegoods.GetSelectedIndex();
   if (uiIndex < 0)
      return 0; // none selected

   CString cszResponse;
   bool bRet = TextElite::Sell(uiIndex, 1, cszResponse);
   m_mainFrame.ShowResponse(cszResponse, bRet);
   UpdateView();
   m_lcTradegoods.SetFocus();
   return 0;
}

LRESULT MarketView::OnClickedButtonBuyFuelOne(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString cszResponse;
   bool bRet = TextElite::Fuel(10, cszResponse);
   m_mainFrame.ShowResponse(cszResponse, bRet);
   UpdateView();
   return 0;
}

LRESULT MarketView::OnClickedButtonBuyFuelTenth(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString cszResponse;
   bool bRet = TextElite::Fuel(1, cszResponse);
   m_mainFrame.ShowResponse(cszResponse, bRet);
   UpdateView();
   return 0;
}

LRESULT MarketView::OnClickedButtonBuyFuelFull(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (TextElite::GetFuelInTenth() >= 70)
      return 0;

   CString cszResponse;
   bool bRet = TextElite::Fuel(70-TextElite::GetFuelInTenth(), cszResponse);
   m_mainFrame.ShowResponse(cszResponse, bRet);
   UpdateView();
   return 0;
}

LRESULT MarketView::OnListTradegoodsSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
   UpdateBuySellButton();
   return 0;
}

void MarketView::UpdateBuySellButton()
{
   if (m_uiSelectedPlanet != TextElite::GetCurrentPlanet())
      return; // not local marketplace

   // get selection index
   int iIndex = m_lcTradegoods.GetSelectedIndex();
   if (iIndex < 0)
      return;

   unsigned int uiIndex = iIndex;

   // can only buy when market has item and it doesn't cost too much
   std::vector<MarketInfo> vecMarketInfos;
   TextElite::GetMarketInfo(m_uiSelectedPlanet, vecMarketInfos);
   const MarketInfo& info = vecMarketInfos[uiIndex];

   bool bBuy = info.Quantity() > 0 && info.PriceInTenths() < TextElite::GetCashInTenth();

   // can only sell when we have item in cargo
   std::vector<unsigned int> vecQuantities;
   TextElite::GetCargoQuantities(vecQuantities);

   ATLASSERT(uiIndex < vecQuantities.size());
   bool bSell = vecQuantities[uiIndex] > 0;

   // enable or disable buy/sell buttons based on selection
   m_btnBuy.EnableWindow(bBuy ? TRUE : FALSE);
   m_btnSell.EnableWindow(bSell ? TRUE : FALSE);
}

void MarketView::UpdateView()
{
   // disable buy/sell buttons; enabled by selection handler
   m_btnBuy.EnableWindow(FALSE);
   m_btnSell.EnableWindow(FALSE);

   // set new market name
   {
      CString cszTitle;

      PlanetInfo pi = TextElite::GetPlanetInfo(m_uiSelectedPlanet);

      cszTitle.Format(_T("%s Marketplace"), pi.Name().GetString());
      m_scTitle.SetWindowText(cszTitle);
   }

   // update cash and cargo info
   {
      CString cszText;

      unsigned int uiCash = TextElite::GetCashInTenth();

      cszText.Format(_T("Cash: %u.%01u CR - Cargo: %u t free"),
         uiCash / 10, uiCash % 10, TextElite::GetCargoSpace());
      m_scCashCargoInfo.SetWindowText(cszText);
   }

   // update trade goods list with quantity and prices
   {
      m_lcTradegoods.SetRedraw(FALSE);

      std::vector<MarketInfo> vecMarketInfos;
      TextElite::GetMarketInfo(m_uiSelectedPlanet, vecMarketInfos);

      std::vector<unsigned int> vecQuantities;
      TextElite::GetCargoQuantities(vecQuantities);

      CString cszText;
      for (int i=0, iMax = static_cast<int>(vecMarketInfos.size()); i<iMax; i++)
      {
         const MarketInfo& info = vecMarketInfos[i];

         cszText.Format(_T("%u"), info.Quantity());
         m_lcTradegoods.SetItemText(i, 2, cszText);

         cszText.Format(_T("%u.%01u"), info.PriceInTenths() / 10, info.PriceInTenths() % 10);
         m_lcTradegoods.SetItemText(i, 3, cszText);

         if (vecQuantities[i] > 0)
            cszText.Format(_T("%u"), vecQuantities[i]);
         else
            cszText.Empty();

         m_lcTradegoods.SetItemText(i, 4, cszText);
      }

      m_lcTradegoods.SetRedraw(TRUE);
   }

   // update fuel
   {
      unsigned int uiFuel = TextElite::GetFuelInTenth();

      CString cszFuel;
      cszFuel.Format(_T("Fuel: %u.%01u LY"), uiFuel / 10, uiFuel % 10);

      CWindow wndFuel = GetDlgItem(IDC_STATIC_FUEL);
      wndFuel.SetWindowText(cszFuel);
   }

   UpdateBuySellButton();
}
