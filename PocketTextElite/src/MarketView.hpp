//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file MarketView.hpp Market view
//
#pragma once

// forward references
class IMainFrame;

/// market view
class MarketView :
   public CDialogImpl<MarketView>,
   public CDialogResize<MarketView>,
   public CWinDataExchange<MarketView>
{
public:
   MarketView(IMainFrame& mainFrame)
      :m_mainFrame(mainFrame)
   {
   }

   enum { IDD = IDD_MARKET_FORM };

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      return CWindow::IsDialogMessage(pMsg);
   }

   void SetSelectedPlanet(unsigned int uiSelectedPlanet){ m_uiSelectedPlanet = uiSelectedPlanet; }

   void UpdateView();
   void UpdateBuySellButton();

private:
   friend class CDialogResize<MarketView>;

   BEGIN_DDX_MAP(MarketView)
      DDX_CONTROL_HANDLE(IDC_STATIC_TITLE, m_scTitle)
      DDX_CONTROL_HANDLE(IDC_STATIC_CASH_CARGO_INFO, m_scCashCargoInfo)
      DDX_CONTROL_HANDLE(IDC_BUTTON_MARKET_BUY, m_btnBuy)
      DDX_CONTROL_HANDLE(IDC_BUTTON_MARKET_SELL, m_btnSell)
      DDX_CONTROL_HANDLE(IDC_LIST_TRADEGOODS, m_lcTradegoods)
   END_DDX_MAP()

   BEGIN_DLGRESIZE_MAP(MarketView)
      DLGRESIZE_CONTROL(IDC_LIST_TRADEGOODS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
      DLGRESIZE_CONTROL(IDC_STATIC_FUEL, DLSZ_MOVE_Y)
      DLGRESIZE_CONTROL(IDC_BUTTON_BUY_FUEL_TENTH, DLSZ_MOVE_Y)
      DLGRESIZE_CONTROL(IDC_BUTTON_BUY_FUEL_ONE, DLSZ_MOVE_Y)
      DLGRESIZE_CONTROL(IDC_BUTTON_BUY_FUEL_FULL, DLSZ_MOVE_Y)
   END_DLGRESIZE_MAP()

   BEGIN_MSG_MAP(MarketView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_HANDLER(IDC_BUTTON_MARKET_BUY, BN_CLICKED, OnClickedButtonMarketBuy)
      COMMAND_HANDLER(IDC_BUTTON_MARKET_SELL, BN_CLICKED, OnClickedButtonMarketSell)
      COMMAND_HANDLER(IDC_BUTTON_BUY_FUEL_ONE, BN_CLICKED, OnClickedButtonBuyFuelOne)
      COMMAND_HANDLER(IDC_BUTTON_BUY_FUEL_TENTH, BN_CLICKED, OnClickedButtonBuyFuelTenth)
      COMMAND_HANDLER(IDC_BUTTON_BUY_FUEL_FULL, BN_CLICKED, OnClickedButtonBuyFuelFull)
      NOTIFY_HANDLER(IDC_LIST_TRADEGOODS, LVN_ITEMCHANGED, OnListTradegoodsSelChanged)
      CHAIN_MSG_MAP(CDialogResize<MarketView>)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnClickedButtonMarketBuy(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnClickedButtonMarketSell(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnClickedButtonBuyFuelOne(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnClickedButtonBuyFuelTenth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnClickedButtonBuyFuelFull(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnListTradegoodsSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

private:
   IMainFrame& m_mainFrame;

   /// currently selected planet; may differ from current
   unsigned int m_uiSelectedPlanet;

   CStatic m_scTitle;
   CStatic m_scCashCargoInfo;
   CButton m_btnBuy;
   CButton m_btnSell;
   CListViewCtrl m_lcTradegoods;
};
