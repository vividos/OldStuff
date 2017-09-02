#pragma once

#ifndef __LIST_VIEW_DRAG_REARRANGE_H__
#define __LIST_VIEW_DRAG_REARRANGE_H__

// Meant as a mix-in class to help implement
//  drag-n-drop rearranging of a SysListView32
// You can use this for a class that is superclassing
//  or subclassing a SysListView32.
//  The parent of the SysListView32 needs to reflect notifications.


class CListViewDragRearrange_MoveMarker;


//BUGBUG: With _UseDragImage as true, there's some visual artifacts
template <class T, bool _UseDragImage = false>
class CListViewDragRearrangeT
{
protected:
	HIMAGELIST m_hDragImageList;
	bool m_bDragging;
	int m_nNewIndex;
	int m_nContiguousSelectionStartingIndex;
	int m_nContiguousSelectionEndingIndex;
	HCURSOR m_hCursorMove;
	HCURSOR m_hCursorNoDrop;

	CListViewDragRearrange_MoveMarker* m_pWndMoveMarker;

public:
	CListViewDragRearrangeT() :
		m_hDragImageList(NULL),
		m_bDragging(false),
		m_nNewIndex(-1),
		m_nContiguousSelectionStartingIndex(-1),
		m_nContiguousSelectionEndingIndex(-1),
		m_hCursorMove(NULL),
		m_hCursorNoDrop(NULL)
	{
	}

	// We don't have a virtual destructor because this is a mix-in class
	~CListViewDragRearrangeT()
	{
		// NOTE: If this asserts, somehow
		//  CancelDragging didn't get called
		ATLASSERT(m_hCursorMove == NULL);
		ATLASSERT(m_hCursorNoDrop == NULL);
		ATLASSERT(m_hDragImageList == NULL);
	}

// Overrideables
public:
	BOOL MoveItem(int nFromIndex, int nToIndex)
	{
		T* pT = static_cast<T*>(this);

		BOOL bSuccess = TRUE;

		int nOldIndex = nFromIndex;
		// A derived class might want to completely or partially override this.
		// In this base class implementation, we do a shallow copy
		// of everything.

		int nColumnCount = 0;
		CHeaderCtrl ctrlHeader = pT->GetHeader();
		if(ctrlHeader && ctrlHeader.IsWindow())
		{
			nColumnCount = ctrlHeader.GetItemCount();
		}

		int nExtendedStyles = pT->GetExtendedListViewStyle();

		// Copy the item to the new index, then delete
		//  the old copy from the old index.
		// We need to InsertItem using the data for the
		// first columns, then SetItem for the other columns
		const size_t LIST_ITEM_TEXT_MAX_LENGTH = MAX_PATH;
		TCHAR sBuffer[LIST_ITEM_TEXT_MAX_LENGTH] = {0};

		LVITEM lvItem = {0};

 		// TODO: If there are more valid mask bits in the future,
		//  be sure to update this
		lvItem.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_NORECOMPUTE | LVIF_PARAM | LVIF_STATE | LVIF_TEXT;
#if (_WIN32_WINNT >= 0x501)
		lvItem.mask |= LVIF_GROUPID | LVIF_COLUMNS;
#endif
		lvItem.iItem = nFromIndex;
		lvItem.iSubItem = 0;
		lvItem.stateMask = (UINT)-1;
		lvItem.cchTextMax = LIST_ITEM_TEXT_MAX_LENGTH;
		lvItem.pszText = sBuffer;

		bSuccess = pT->GetItem(&lvItem);
		LPARAM lParamItem = lvItem.lParam;

		lvItem.iItem = nToIndex;
		lvItem.state &= ~LVIS_SELECTED;
		lvItem.stateMask = lvItem.state;

		// Insert the "main" item in its new position
		// (the "main" item is the first column with iSubItem==0)
		int nNewIndex = pT->InsertItem(&lvItem);

		if(nNewIndex <= nFromIndex)
		{
			nFromIndex++;
		}

		// Copy the SubItems
		for(int i = 1; i <= nColumnCount; i++)
		{
			::ZeroMemory(&lvItem, sizeof(lvItem));

			// IMPORTANT!!  Don't try to set the lParam when setting
			//  the SubItem.  The SetItem call will fail!
			//  The lParam "belongs" to the first column (the "item" column).
			//  MSDN also says not to set the "state" for a sub-item,
			//  but this works and seems to be a valid thing to want
			//  to do, so we'll leave it in.
 			// TODO: If there are more valid mask bits in the future
			//  that apply to subitems, be sure to update this
 			lvItem.mask = LVIF_TEXT | LVIF_STATE;
			if((nExtendedStyles & LVS_EX_SUBITEMIMAGES) == LVS_EX_SUBITEMIMAGES)
			{
				lvItem.mask |= LVIF_IMAGE;
			}
			lvItem.iItem = nFromIndex;
			lvItem.iSubItem = i;
			lvItem.stateMask = (UINT)-1;
			lvItem.cchTextMax = LIST_ITEM_TEXT_MAX_LENGTH;
			lvItem.pszText = sBuffer;

			bSuccess = pT->GetItem(&lvItem);

			lvItem.iItem = nNewIndex;
			lvItem.state &= ~LVIS_SELECTED;
			lvItem.stateMask = lvItem.state;

			bSuccess = pT->SetItem(&lvItem);
		}


		// Delete from original position
		bSuccess = pT->DeleteItem(nFromIndex);

		if(nNewIndex > nFromIndex)
		{
			nNewIndex--;
		}

		if(bSuccess)
		{
			// Let the deriving class know about the move
			//  in a nice packaged fasion
			//  (they could watch for insert and delete notifications,
			//  or we could make up a new notification or message,
			//  but we'll call a method instead)
			pT->ItemMoved(nOldIndex, nNewIndex, lParamItem);
		}

		return 0;
	}

	void ItemMoved(int nOldIndex, int nNewIndex, LPARAM lParamItem)
	{
		T* pT = static_cast<T*>(this);
		pT->Invalidate();
	}

public:
	BEGIN_MSG_MAP(thisClass)
	ALT_MSG_MAP(1)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_BEGINDRAG, OnListViewBeginDrag)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnListViewMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnListViewLButtonUp)
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMessage)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	LRESULT OnListViewBeginDrag(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);

#ifdef _DEBUG
		LONG dwStyle = pT->GetWindowLong(GWL_STYLE);
		// This class is only meant for lists in either LVS_REPORT or LVS_LIST mode
		ATLASSERT((dwStyle & (LVS_REPORT | LVS_LIST)) != 0);
#endif

		LPNMLISTVIEW pnhListView = (LPNMLISTVIEW)pnmh;

		// This call to DoDragDrop is just to ensure a dependency on OLE32.dll.
		// In the future, if we support true OLE drag and drop,
		// we'll really use DoDragDrop.
		::DoDragDrop(NULL, NULL, 0, 0);

		// To save on resources, we'll load the drag cursor
		// only when we need it, and destroy it when the drag is done
		HMODULE hOle32 = ::GetModuleHandle(_T("OLE32.dll"));
		if(hOle32 != NULL)
		{
			// Cursor ID identified using resource editor in Visual Studio
			int dragCursor = 2;
			int noDropCursor = 1;
			m_hCursorMove = ::LoadCursor(hOle32, MAKEINTRESOURCE(dragCursor));
			m_hCursorNoDrop = ::LoadCursor(hOle32, MAKEINTRESOURCE(noDropCursor));
		}

		// You can set your customized cursor here

		if(_UseDragImage)
		{
			POINT p = {0};
			IMAGEINFO imf = {0};
			HIMAGELIST hOneImageList = NULL, hTempImageList = NULL;
			BOOL bFirst = TRUE;
			int iHeight = 0;

			// Ok, now we create a drag-image for all selected items
			int iPos = pT->GetNextItem(-1, LVNI_SELECTED);
			while (iPos != -1)
			{
				if (bFirst)
				{
					// For the first selected item, we simply create a single-line drag image
					m_hDragImageList = pT->CreateDragImage(iPos, &p);
					ImageList_GetImageInfo(m_hDragImageList, 0, &imf);
					iHeight = imf.rcImage.bottom;
					bFirst = FALSE;
				}
				else
				{
					// For the rest selected items, we create a single-line drag image, then
					// append it to the bottom of the complete drag image
					hOneImageList = pT->CreateDragImage(iPos, &p);
					hTempImageList = ImageList_Merge(m_hDragImageList, 0, hOneImageList, 0, 0, iHeight);
					ImageList_Destroy(m_hDragImageList);
					ImageList_Destroy(hOneImageList);
					m_hDragImageList = hTempImageList;
					ImageList_GetImageInfo(m_hDragImageList, 0, &imf);
					iHeight = imf.rcImage.bottom;
				}
				iPos = pT->GetNextItem(iPos, LVNI_SELECTED);
			}

			// Now we can initialize then start the drag action
			POINT pt = pnhListView->ptAction;

			ImageList_BeginDrag(m_hDragImageList, 0, pt.x, 0);
			ImageList_DragEnter(pT->m_hWnd, pt.x, pt.y);
			// NOTE: We are doing a DragEnter with the ListView window.
			//  If you want the drag image to go outside of the ListView,
			//  use NULL here and in DragLeave, and use screen coordinates
			//  with the ImageList_* functions.
		}

		// If its a contiguous selection, we'll do the insertion mark
		// just a little smarter (we won't let it move to where it
		// already is)
		int iPos = pT->GetNextItem(-1, LVNI_SELECTED);
		m_nContiguousSelectionStartingIndex = iPos;
		m_nContiguousSelectionEndingIndex = iPos;
		int iPosPrev = iPos-1;
		while(iPos != -1 && m_nContiguousSelectionStartingIndex != -1)
		{
			if(iPos != iPosPrev+1)
			{
				m_nContiguousSelectionStartingIndex = -1;
				m_nContiguousSelectionEndingIndex = -1;
			}
			else
			{
				m_nContiguousSelectionEndingIndex = iPos;

				iPosPrev = iPos;
				iPos = pT->GetNextItem(iPos, LVNI_SELECTED);
			}
		}

		m_bDragging = true;

		pT->SetCapture();

		m_pWndMoveMarker = CListViewDragRearrange_MoveMarker::CreateInstance();
		if(m_pWndMoveMarker != NULL)
		{
			CRect rcClient;
			pT->GetClientRect(&rcClient);
			m_pWndMoveMarker->Create(rcClient.Width());
			// m_pWndMoveMarker will "delete this" after its destroyed
		}

		return 0;
	}

	LRESULT OnListViewMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if(!m_bDragging)
		{
			bHandled = FALSE;
			return 0;
		}

		CPoint ptCursor(lParam);

		T* pT = static_cast<T*>(this);


		BOOL bSuccess = TRUE;


		LVHITTESTINFO hti = {0};
		hti.pt = ptCursor;
		int nIndex = pT->HitTest(&hti);

		if(nIndex >= 0)
		{
			m_nNewIndex = nIndex;
		}
		else if(hti.flags == LVHT_NOWHERE)
		{
			// The cursor is in the client area, but not over an item.
			// We'll try two things, and otherwise leave
			// m_nNewIndex to what it was.
			//
			// 1. We'll try to figure out if the mouse is in the client area,
			//    but to the right of the columns.
			// 2. We'll try to figure out if the mouse is in the client area,
			//    but under the last item

			LVHITTESTINFO hti = {0};
			hti.pt.x = 0;
			hti.pt.y = ptCursor.y;
			int nIndexToLeft = pT->HitTest(&hti);
			if(nIndexToLeft >= 0)
			{
				m_nNewIndex = nIndexToLeft;
			}
			else
			{
				int nFirstVisibleIndex = pT->GetTopIndex();
				int nIndexLastVisibleItem = -1;
				int nCountPerPage = pT->GetCountPerPage();
				int nItemCount = pT->GetItemCount();
				int nIndexLastItem = nItemCount - 1;

				CRect rcFirstVisibleItem;
				bSuccess = pT->GetItemRect(nFirstVisibleIndex, &rcFirstVisibleItem, LVIR_BOUNDS);

				CRect rcLastVisibleItem;
				if((nFirstVisibleIndex + nCountPerPage) < nItemCount)
				{
					nIndexLastVisibleItem = nFirstVisibleIndex + nCountPerPage;
				}
				else
				{
					nIndexLastVisibleItem = nIndexLastItem;
				}
				bSuccess = pT->GetItemRect(nIndexLastVisibleItem, &rcLastVisibleItem, LVIR_BOUNDS);

				int nDragPastBottom = 1;
				if(ptCursor.y < rcFirstVisibleItem.top)
				{
					m_nNewIndex = nFirstVisibleIndex;
				}
				else if(ptCursor.y < (rcLastVisibleItem.bottom + nDragPastBottom*rcLastVisibleItem.Height()))
				{
					m_nNewIndex = nIndexLastVisibleItem;
				}
				else if(m_nNewIndex > nIndexLastVisibleItem)
				{
					// Later on, the
					//  "ptCursor.y > (rcItem.top + rcItem.Height()/2)"
					// check will increment m_nNewIndex so that it
					// will be put on the end
					m_nNewIndex = nIndexLastVisibleItem;
				}
				else
				{
					// Otherwise, leave m_nNewIndex what it was
					//  while the cursor is still in the client area
				}

			}
		}
		else
		{
			// The cursor is now outside of the client area
			m_nNewIndex = -1;
		}

		if(_UseDragImage)
		{
			ImageList_DragMove(ptCursor.x, ptCursor.y);
		}

		if(m_nNewIndex >= 0)
		{
			CRect rcItem;
			bSuccess = pT->GetItemRect(m_nNewIndex, &rcItem, LVIR_BOUNDS);
			if(bSuccess)
			{
				CPoint ptMarker;
				ptMarker.x = rcItem.left;

				if(ptCursor.y > (rcItem.top + rcItem.Height()/2))
				{
					m_nNewIndex++;
					ptMarker.y = rcItem.bottom;
				}
				else
				{
					ptMarker.y = rcItem.top;
				}

				if(	m_nContiguousSelectionStartingIndex >= 0 &&
					(m_nNewIndex >= m_nContiguousSelectionStartingIndex &&
					 m_nNewIndex <= m_nContiguousSelectionEndingIndex+1))
				{
					// Don't move if it'd go to the same place it already is
					m_nNewIndex = -1;
					m_pWndMoveMarker->ShowWindow(SW_HIDE);
				}
				else
				{
					pT->ClientToScreen(&ptMarker);
					ptMarker.y -= 1;
					m_pWndMoveMarker->Reposition(ptMarker.x, ptMarker.y);
				}
			}
			else
			{
				m_pWndMoveMarker->ShowWindow(SW_HIDE);
			}
		}
		else
		{
			m_pWndMoveMarker->ShowWindow(SW_HIDE);
		}

		CRect rcClient;
		pT->GetClientRect(&rcClient);
		if(::PtInRect(&rcClient, ptCursor))
		{
			::SetCursor(m_hCursorMove);
		}
		else
		{
			::SetCursor(m_hCursorNoDrop);
		}

		return 0;
	}

	LRESULT OnListViewLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(!m_bDragging)
		{
			bHandled = FALSE;
			return 0;
		}

		T* pT = static_cast<T*>(this);

		// End the drag-and-drop process
		this->CancelDragging();

		if(m_nNewIndex < 0)
		{
			return 0;
		}

		// Rearrange the items
		BOOL bSuccess = TRUE;

		int iPos = pT->GetNextItem(-1, LVNI_SELECTED);
		while(iPos != -1)
		{
			if(iPos == m_nNewIndex)
			{
				// If it's not moving, don't go through
				//  all the work of copying stuff
				m_nNewIndex++;
			}
			else
			{
				bSuccess = pT->MoveItem(iPos, m_nNewIndex);
				if(m_nNewIndex < iPos)
				{
					m_nNewIndex++;
				}
			}

			iPos = pT->GetNextItem(-1, LVNI_SELECTED);
		}

		return 0;
	}

	LRESULT OnForwardMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if(!m_bDragging)
		{
			bHandled = FALSE;
			return 0;
		}

		LPMSG lpMsg = (LPMSG) lParam;
		if(lpMsg->message == WM_KEYDOWN)
		{
			if(lpMsg->wParam == VK_ESCAPE)
			{
				this->CancelDragging();
			}

		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(!m_bDragging)
		{
			bHandled = FALSE;
			return 0;
		}

		if(wParam == VK_ESCAPE)
		{
			this->CancelDragging();
		}

		bHandled = FALSE;
		return 0;
	}


public:
	void CancelDragging(void)
	{
		T* pT = static_cast<T*>(this);

		if(m_bDragging)
		{
			m_bDragging = false;

			if(m_hCursorMove != NULL)
			{
				::DestroyCursor(m_hCursorMove);
				m_hCursorMove = NULL;
			}
			if(m_hCursorNoDrop != NULL)
			{
				::DestroyCursor(m_hCursorNoDrop);
				m_hCursorNoDrop = NULL;
			}

			if(m_pWndMoveMarker != NULL)
			{
				m_pWndMoveMarker->DestroyWindow();
				// m_pWndMoveMarker will "delete this" after its destroyed
				m_pWndMoveMarker = NULL;
			}

			if(_UseDragImage)
			{
				ImageList_DragLeave(pT->m_hWnd);
				ImageList_EndDrag();
				ImageList_Destroy(m_hDragImageList);
				m_hDragImageList = NULL;
			}

			ReleaseCapture();
		}

	}

};



typedef CWinTraits<WS_POPUP | WS_DISABLED, WS_EX_TOOLWINDOW> CListViewDragRearrange_MoveMarkerTraits;
class CListViewDragRearrange_MoveMarker : public CWindowImpl<CListViewDragRearrange_MoveMarker, CWindow, CListViewDragRearrange_MoveMarkerTraits>
{
protected:
	typedef CListViewDragRearrange_MoveMarker thisClass;
	typedef CWindowImpl<CListViewDragRearrange_MoveMarker, CWindow, CListViewDragRearrange_MoveMarkerTraits> baseClass;

public:
	enum MarkerStyle
	{
		eMarkerSingleLine = 0,
		eMarkerLineWithArrows = 1
	};

protected:
	CBrush m_brush;
	int m_nWidth;
	int m_nHeight;
	MarkerStyle m_eMarkerStyle;

// The constructors are protected so that you have to use CreateInstance
protected:
	CListViewDragRearrange_MoveMarker(MarkerStyle eMarkerStyle = eMarkerLineWithArrows, COLORREF crMarkerColor = RGB(255,0,0)) :
		m_nWidth(0),
		m_nHeight(1),
		m_eMarkerStyle(eMarkerStyle)
	{
		m_brush.CreateSolidBrush(crMarkerColor);

		switch(eMarkerStyle)
		{
		case eMarkerLineWithArrows:
			m_nHeight = 12;
			break;
		case eMarkerSingleLine:
		default:
			m_nHeight = 1;
			break;
		}
	}

public:
	static CListViewDragRearrange_MoveMarker* CreateInstance(MarkerStyle eMarkerStyle = eMarkerLineWithArrows, COLORREF crMarkerColor = RGB(255,0,0))
	{
		return new CListViewDragRearrange_MoveMarker(eMarkerStyle, crMarkerColor);
	}

public:
	// The -1 is so we'll end up with a NULL brush (the macro has it: (HBRUSH)(bkgnd + 1)).
	// (which is what we want, because we'll paint the background ourself)
	DECLARE_WND_CLASS_EX(_T("ListViewDragRearrange_MoveMarker"), CS_SAVEBITS, -1)

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
	END_MSG_MAP()

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(wParam != NULL)
		{
			CDCHandle dc((HDC)wParam);

			CRect rcClient;
			this->GetClientRect(&rcClient);

			dc.FillRect(&rcClient, m_brush);
		}
		return 1;
	}

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		// This class should be created with "new"
		delete this;
	}

	HWND Create(int nWidth)
	{
		m_nWidth = nWidth;
		RECT rcPos = {0,0,m_nWidth,m_nHeight};

		HWND hWnd = baseClass::Create(NULL, rcPos, _T("MoveMarker"),0,0,0U,NULL);

		if(m_eMarkerStyle == eMarkerLineWithArrows)
		{
			this->CreateLineWithArrowsRegion();
		}

		return hWnd;
	}

	void CreateLineWithArrowsRegion(void)
	{
		CRgn rgnArrowLeft, rgnArrowRight, rgnLine;
		POINT ptArrow[7] = {0};

		ptArrow[0].x = 2;	ptArrow[0].y = 0;
		ptArrow[1].x = 2;	ptArrow[1].y = 4;
		ptArrow[2].x = 0;	ptArrow[2].y = 4;
		ptArrow[3].x = 0;	ptArrow[3].y = 9;
		ptArrow[4].x = 2;	ptArrow[4].y = 9;
		ptArrow[5].x = 2;	ptArrow[5].y = 12;
		ptArrow[6].x = 8;	ptArrow[6].y = 6;
		rgnArrowLeft.CreatePolygonRgn(ptArrow, 7, ALTERNATE);


		for(int i=0;i<7;++i)
		{
			ptArrow[i].x = m_nWidth - ptArrow[i].x;
		}
		rgnArrowRight.CreatePolygonRgn(ptArrow, 7, ALTERNATE);

		rgnLine.CreateRectRgn(0, (m_nHeight/2), m_nWidth, (m_nHeight/2) + 1);

		CRgnHandle rgnMarker;
		rgnMarker.CreateRectRgn(0, 0, m_nWidth, m_nHeight);
		rgnMarker.CombineRgn(rgnArrowLeft, rgnArrowRight, RGN_OR);
		rgnMarker.CombineRgn(rgnLine, RGN_OR);

		this->SetWindowRgn(rgnMarker, FALSE);

		// The OS will take over the lifetime of rgnMarker
		// (see MSDN for SetWindowRgn)
	}

	void Reposition(int x, int y)
	{
		if(m_hWnd != NULL && ::IsWindow(m_hWnd))
		{
			this->SetWindowPos(
				HWND_TOP,
				x, y - (m_nHeight)/2,
				0, 0,
				SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
	}

};

#endif //__LIST_VIEW_DRAG_REARRANGE_H__
