#include "..\duipub.h"
#include "UITreeView.h"

#pragma warning( disable: 4251 )
namespace DuiLib
{
	CTreeNodeUI::CTreeNodeUI( CTreeNodeUI* _ParentNode /*= NULL*/, CPaintManagerUI* pManager /*= NULL*/ )
	{
		m_dwItemTextColor		= 0x00000000;
		m_dwItemHotTextColor	= 0;
		m_dwSelItemTextColor	= 0;
		m_dwSelItemHotTextColor	= 0;

		m_pTreeView		= NULL;
		m_bIsVisable	= false;
		m_bIsCheckBox	= false;
		m_bExpanded		= false;

		m_bExpanding = false;

		m_pParentTreeNode	= NULL;

		SetManager(pManager, NULL, true);

		m_pHoriz			= new CHorizontalLayoutUI();
		m_pHoriz->SetManager(pManager, this);
		m_pFolderButton	= new CCheckBoxUI();
		m_pFolderButton->SetManager(pManager, this);
		m_pDottedLine		= new CLabelUI();
		m_pDottedLine->SetManager(pManager, this);
		m_pCheckBox		= new CCheckBoxUI();
		m_pCheckBox->SetManager(pManager, this);
		m_pItemButton		= new COptionUI();
		m_pItemButton->SetManager(pManager, this);
		m_pOverFolderButton = new CLabelUI();
		m_pOverFolderButton->SetManager(pManager, this);		

		double S = 1.0;
		if (pManager)
		{
			S = pManager->GetDpiScale();
		}

		this->SetFixedHeight((int)(S * 18));
		//this->SetFixedWidth(250);
		m_pFolderButton->SetFixedWidth(GetFixedHeight());
		m_pOverFolderButton->SetFixedWidth(GetFixedHeight());
		m_pDottedLine->SetFixedWidth((int)(S * 2));
		m_pCheckBox->SetFixedWidth(GetFixedHeight());
		m_pItemButton->SetAttribute(_T("align"),_T("left"));
		m_pItemButton->SetAttribute(_T("valign"), _T("vcenter"));

		m_pDottedLine->SetVisible(false);
		m_pCheckBox->SetVisible(false);
		m_pItemButton->SetMouseEnabled(false);

		if(_ParentNode)
		{
			if (_tcsicmp(_ParentNode->GetClass(), _T("TreeNodeUI")) != 0)
				return;

			m_pDottedLine->SetVisible(_ParentNode->IsVisible());
			m_pDottedLine->SetFixedWidth((int)(_ParentNode->GetDottedLine()->GetFixedWidth() + S * 16));
			this->SetParentNode(_ParentNode);
		}

		m_pHoriz->Add(m_pDottedLine);
		m_pHoriz->Add(m_pFolderButton);
		m_pHoriz->Add(m_pOverFolderButton);
		m_pHoriz->Add(m_pCheckBox);
		m_pHoriz->Add(m_pItemButton);
		Add(m_pHoriz);

		if (pManager)
		{
			LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(_T("TreeNode"));
			if (pDefaultAttributes) {
				ApplyAttributeList(pDefaultAttributes);
			}
		}
	}
	
	CTreeNodeUI::~CTreeNodeUI( void )
	{

	}

	LPCTSTR CTreeNodeUI::GetClass() const
	{
		return _T("TreeNodeUI");
	}

	LPVOID CTreeNodeUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, _T("TreeNode")) == 0 )
			return static_cast<CTreeNodeUI*>(this);
		return CListContainerElementUI::GetInterface(pstrName);
	}

	void CTreeNodeUI::DoEvent( TEventUI& event )
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CContainerUI::DoEvent(event);
			return;
		}

		CListContainerElementUI::DoEvent(event);

		if( event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {
				m_pManager->SendNotify(this, _T("itemdbclick"));
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled()) {
				if((m_dwStatus & UISTATE_SELECTED) && GetSelItemHotTextColor())
					m_pItemButton->SetTextColor(GetSelItemHotTextColor());
				else
					m_pItemButton->SetTextColor(GetItemHotTextColor());
			}
			else 
				m_pItemButton->SetTextColor(m_pItemButton->GetDisabledTextColor());

			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled()) {
				if((m_dwStatus & UISTATE_SELECTED) && GetSelItemTextColor())
					m_pItemButton->SetTextColor(GetSelItemTextColor());
				else if(!(m_dwStatus & UISTATE_SELECTED))
					m_pItemButton->SetTextColor(GetItemTextColor());
			}
			else 
				m_pItemButton->SetTextColor(m_pItemButton->GetDisabledTextColor());

			return;
		}
	}

	void CTreeNodeUI::Invalidate()
	{
		if( !IsVisible() )
			return;

		if( GetParent() ) {
			CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(_T("Container")));
			if( pParentContainer ) {
				RECT rc = pParentContainer->GetPos();
				RECT rcInset = pParentContainer->GetInset();
				rc.left += rcInset.left;
				rc.top += rcInset.top;
				rc.right -= rcInset.right;
				rc.bottom -= rcInset.bottom;
				CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
				CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = m_rcItem;
				if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
					return;

				CControlUI* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while( pParent = pParent->GetParent() )
				{
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
						return;
				}

				if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
			}
			else {
				CContainerUI::Invalidate();
			}
		}
		else {
			CContainerUI::Invalidate();
		}
	}

	bool CTreeNodeUI::Select( bool bSelect /*= true*/ )
	{
		bool nRet = CListContainerElementUI::Select(bSelect);
		if(IsSelected())
			m_pItemButton->SetTextColor(GetSelItemTextColor());
		else 
			m_pItemButton->SetTextColor(GetItemTextColor());

		return nRet;
	}


	void CTreeNodeUI::SetVisibleTag( bool _IsVisible )
	{
		m_bIsVisable = _IsVisible;
	}

	bool CTreeNodeUI::GetVisibleTag()
	{
		return m_bIsVisable;
	}

	void CTreeNodeUI::SetItemText( LPCTSTR pstrValue )
	{
		m_pItemButton->SetText(pstrValue);
	}

	CDuiString CTreeNodeUI::GetItemText()
	{
		return m_pItemButton->GetText();
	}

	void CTreeNodeUI::CheckBoxSelected( bool _Selected )
	{
		m_pCheckBox->Selected(_Selected);
	}

	bool CTreeNodeUI::IsCheckBoxSelected() const
	{
		return m_pCheckBox->IsSelected();
	}

	bool CTreeNodeUI::IsHasChild() const
	{
		return !m_arTreeNodes.IsEmpty();
	}

	bool CTreeNodeUI::Add( CControlUI* _pTreeNodeUI )
	{
		if (_tcsicmp(_pTreeNodeUI->GetClass(), _T("TreeNodeUI")) == 0)
			return AddChildNode((CTreeNodeUI*)_pTreeNodeUI);

		return CListContainerElementUI::Add(_pTreeNodeUI);
	}

		//************************************
	// 函数名称: AddAt
	// 返回类型: bool
	// 参数信息: CControlUI * pControl
	// 参数信息: int iIndex				该参数仅针对当前节点下的兄弟索引，并非列表视图索引
	// 函数说明: TreeNode指定索引位置插入子节点(edit by joe 2014/7/28)
	//************************************
	bool CTreeNodeUI::AddAt( CControlUI* pControl, int iIndex )
	{
        if (!pControl)
            return false;

        if(_tcsicmp(pControl->GetClass(), _T("TreeNodeUI")) != 0)
            return false;

        //filter invalidate index
        int iDestIndex = iIndex;
        if (iDestIndex < 0)
        {
            iDestIndex = 0;
        }
        else if (iDestIndex > GetCountChild())
        {
            iDestIndex = GetCountChild();
        }
        if(iIndex != iDestIndex) iIndex = iDestIndex;

		if(NULL == static_cast<CTreeNodeUI*>(pControl->GetInterface(_T("TreeNode"))))
			return false;

		CTreeNodeUI* pIndexNode = static_cast<CTreeNodeUI*>(m_arTreeNodes.GetAt(iIndex));
		if(!pIndexNode){
			if(!m_arTreeNodes.Add(pControl))
				return false;
		}
		else if(pIndexNode && !m_arTreeNodes.InsertAt(iIndex,pControl))
			return false;

		if(!pIndexNode && m_pTreeView && m_pTreeView->GetItemAt(GetTreeIndex()+1))
			pIndexNode = static_cast<CTreeNodeUI*>(m_pTreeView->GetItemAt(GetTreeIndex()+1)->GetInterface(_T("TreeNode")));

		CalcLocation((CTreeNodeUI*)pControl);

		if(m_pTreeView && pIndexNode)
			return m_pTreeView->AddAt((CTreeNodeUI*)pControl,pIndexNode);
		else 
			return m_pTreeView->Add((CTreeNodeUI*)pControl);

		return true;
	}


	bool CTreeNodeUI::AddChildNode( CTreeNodeUI* _pTreeNodeUI )
	{
		if (!_pTreeNodeUI)
			return false;

		if (_tcsicmp(_pTreeNodeUI->GetClass(), _T("TreeNodeUI")) != 0)
			return false;


		CalcLocation(_pTreeNodeUI);

		bool nRet = true;

		if(m_pTreeView){
			CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(m_arTreeNodes.GetAt(m_arTreeNodes.GetSize()-1));
			if(!pNode || !pNode->GetLastNode())
				nRet = m_pTreeView->AddAt(_pTreeNodeUI,GetTreeIndex()+1) >= 0;
			else nRet = m_pTreeView->AddAt(_pTreeNodeUI,pNode->GetLastNode()->GetTreeIndex()+1) >= 0;
		}

		if(nRet)
		{
			_pTreeNodeUI->SetVisible(GetFolderButton()->IsSelected());    //add by redrain 2014.11.7
			m_arTreeNodes.Add(_pTreeNodeUI);
		}
		
		return nRet;
	}

	bool CTreeNodeUI::Remove( CControlUI* pControl )
	{
		return RemoveAt((CTreeNodeUI*)pControl);
	}

	bool CTreeNodeUI::IsExpanded() const
	{
		return m_bExpanded;
	}

	bool CTreeNodeUI::Expand(bool bExpand /*= true*/, bool bExpandChildren /*= false*/)
	{		
		if (m_bExpanding)
			return false;

		m_bExpanding = true;

		m_bExpanded = bExpand;

		if (GetCountChild() > 0)
		{
			int nCount = GetCountChild();
			for (int nIndex = 0; nIndex < nCount; nIndex++)
			{
				CTreeNodeUI* pItem = GetChildNode(nIndex);
				pItem->SetVisible(bExpand);
				if (!bExpand || bExpandChildren)
				{
					pItem->Expand(bExpand, bExpandChildren);
				}
			}
		}

		if (GetFolderButton()->IsVisible())
		{
			GetFolderButton()->Selected(bExpand);
		}

		m_bExpanding = false;

		return true;
	}

	bool CTreeNodeUI::RemoveAt(CTreeNodeUI* _pTreeNodeUI)
	{
		int nIndex = m_arTreeNodes.Find(_pTreeNodeUI);
		CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(m_arTreeNodes.GetAt(nIndex));
		if(pNode && pNode == _pTreeNodeUI)
		{
			while(pNode->IsHasChild())
				pNode->RemoveAt(static_cast<CTreeNodeUI*>(pNode->m_arTreeNodes.GetAt(0)));

			m_arTreeNodes.Remove(nIndex);

			if(m_pTreeView)
				m_pTreeView->Remove(_pTreeNodeUI);

			return true;
		}
		return false;
	}

	void CTreeNodeUI::SetParentNode( CTreeNodeUI* _pParentTreeNode )
	{
		m_pParentTreeNode = _pParentTreeNode;
	}

	CTreeNodeUI* CTreeNodeUI::GetParentNode() const
	{
		return m_pParentTreeNode;
	}

	long CTreeNodeUI::GetCountChild() const
	{
		return m_arTreeNodes.GetSize();
	}

	void CTreeNodeUI::SetTreeView( CTreeViewUI* _CTreeViewUI )
	{
		m_pTreeView = _CTreeViewUI;
	}

	CTreeViewUI* CTreeNodeUI::GetTreeView()
	{
		return m_pTreeView;
	}

	void CTreeNodeUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if(_tcsicmp(pstrName, _T("text")) == 0 )
			m_pItemButton->SetText(pstrValue);
		else if(_tcsicmp(pstrName, _T("horizattr")) == 0 )
			m_pHoriz->ApplyAttributeList(pstrValue);
		else if(_tcsicmp(pstrName, _T("dotlineattr")) == 0 )
			m_pDottedLine->ApplyAttributeList(pstrValue);
		else if(_tcsicmp(pstrName, _T("folderattr")) == 0 )
			m_pFolderButton->ApplyAttributeList(pstrValue);
		else if(_tcsicmp(pstrName, _T("checkboxattr")) == 0 )
			m_pCheckBox->ApplyAttributeList(pstrValue);
		else if(_tcsicmp(pstrName, _T("itemattr")) == 0 )
			m_pItemButton->ApplyAttributeList(pstrValue);
		else if(_tcsicmp(pstrName, _T("itemtextcolor")) == 0 ){
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemTextColor(clrColor);
		}
		else if(_tcsicmp(pstrName, _T("itemhottextcolor")) == 0 ){
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemHotTextColor(clrColor);
		}
		else if(_tcsicmp(pstrName, _T("selitemtextcolor")) == 0 ){
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelItemTextColor(clrColor);
		}
		else if(_tcsicmp(pstrName, _T("selitemhottextcolor")) == 0 ){
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelItemHotTextColor(clrColor);
		}
		else if(_tcsicmp(pstrName,_T("defaultexpand")) == 0)
			GetFolderButton()->Selected(_tcsicmp(pstrValue,_T("true")) == 0);
		else CListContainerElementUI::SetAttribute(pstrName,pstrValue);
	}

	void CTreeNodeUI::IsAllChildChecked()
	{	
		bool bIsAllChildChecked = true;
		bool bIsAllChildUncheck = true;
		int nCount = GetCountChild();
		if(nCount > 0)
		{			
			for(int nIndex = 0;nIndex < nCount;nIndex++)
			{
				CTreeNodeUI* pItem = GetChildNode(nIndex);
				if(!pItem->GetCheckBox()->IsSelected())
				{
					bIsAllChildChecked = false;
				}
				else
				{
					bIsAllChildUncheck = false;
				}
			}			
			if (bIsAllChildChecked && !GetCheckBox()->IsSelected())
			{
				GetCheckBox()->Selected(true);
				return;
			}
			else if (bIsAllChildUncheck && GetCheckBox()->IsSelected())
			{
				GetCheckBox()->Selected(false);
				return;
			}
			
		}

	}

	CStdPtrArray CTreeNodeUI::GetTreeNodes()
	{
		return m_arTreeNodes;
	}

	CTreeNodeUI* CTreeNodeUI::GetChildNode( int _nIndex ) const
	{
		return static_cast<CTreeNodeUI*>(m_arTreeNodes.GetAt(_nIndex));
	}

	void CTreeNodeUI::SetVisibleFolderBtn( bool _IsVisibled )
	{
		m_pFolderButton->SetVisible(_IsVisibled);
		m_pOverFolderButton->SetVisible(!_IsVisibled);
	}

	bool CTreeNodeUI::GetVisibleFolderBtn()
	{
		return m_pFolderButton->IsVisible();
	}

	void CTreeNodeUI::SetVisibleCheckBtn( bool _IsVisibled )
	{
		m_pCheckBox->SetVisible(_IsVisibled);
	}

	bool CTreeNodeUI::GetVisibleCheckBtn()
	{
		return m_pCheckBox->IsVisible();
	}

	int CTreeNodeUI::GetTreeIndex()
	{
		if(!m_pTreeView)
			return -1;

		for(int nIndex = 0;nIndex < m_pTreeView->GetCount();nIndex++){
			if(this == m_pTreeView->GetItemAt(nIndex))
				return nIndex;
		}

		return -1;
	}

	int CTreeNodeUI::GetNodeIndex()
	{
		if(!GetParentNode() && !m_pTreeView)
			return -1;

		if(!GetParentNode() && m_pTreeView)
			return GetTreeIndex();

		return GetParentNode()->GetTreeNodes().Find(this);
	}

	DuiLib::CDuiString CTreeNodeUI::GetToolTip() const
	{
		return m_pItemButton->GetToolTip();
	}

	void CTreeNodeUI::SetToolTip(LPCTSTR pstrText)
	{
		for (int i = 0; i < GetCount(); i++)
		{
			GetItemAt(i)->SetToolTip(pstrText);
		}
		m_pItemButton->SetToolTip(pstrText);
	}

	void CTreeNodeUI::SetToolTipWidth(int nWidth)
	{
		m_pItemButton->SetToolTipWidth(nWidth);
	}

	int CTreeNodeUI::GetToolTipWidth(void)
	{
		return m_pItemButton->GetToolTipWidth();
	}

	void CTreeNodeUI::SetEnabled(bool bEnable /*= true*/)
	{
		__super::SetEnabled(bEnable);
		if (bEnable)
		{
			if (m_dwStatus & UISTATE_SELECTED)
				m_pItemButton->SetTextColor(GetSelItemTextColor());
			else
				m_pItemButton->SetTextColor(GetItemTextColor());
		}
		else
			m_pItemButton->SetTextColor(m_pItemButton->GetDisabledTextColor());
	}

// 	DuiLib::CControlUI* CTreeNodeUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
// 	{
// 		if (CControlUI::FindControl(Proc, pData, uFlags))
// 		{
// 			if (pFolderButton->FindControl(Proc, pData, uFlags))
// 				return pFolderButton;
// 			if (pCheckBox->FindControl(Proc, pData, uFlags))
// 				return pCheckBox;
// 			
// 			return pItemButton;
// 		}
// 
// 		return CListContainerElementUI::FindControl(Proc, pData, uFlags);
// 	}

	CTreeNodeUI* CTreeNodeUI::GetLastNode()
	{
		if(!IsHasChild())
			return this;

		CTreeNodeUI* nRetNode = NULL;

		for(int nIndex = 0;nIndex < GetTreeNodes().GetSize();nIndex++){
			CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(GetTreeNodes().GetAt(nIndex));
			if(!pNode)
				continue;

			CDuiString aa = pNode->GetItemText();

			if(pNode->IsHasChild())
				nRetNode = pNode->GetLastNode();
			else 
				nRetNode = pNode;
		}
		
		return nRetNode;
	}

	void CTreeNodeUI::CalcLocation( CTreeNodeUI* _pTreeNodeUI )
	{
		double S = GetScaleDpi();
		_pTreeNodeUI->GetDottedLine()->SetVisible(true);
		_pTreeNodeUI->GetDottedLine()->SetFixedWidth((int)(m_pDottedLine->GetFixedWidth()+16*S));
		_pTreeNodeUI->SetParentNode(this);
		_pTreeNodeUI->GetItemButton()->SetGroup(m_pItemButton->GetGroup());
		_pTreeNodeUI->SetTreeView(m_pTreeView);
		_pTreeNodeUI->CalcAllChildren();
	}

	void CTreeNodeUI::CalcAllChildren()
	{
		auto arNode = GetTreeNodes();
		for (int i = 0; i < arNode.GetSize(); i++)
		{
			CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(arNode.GetAt(i));
			if (!pNode)
				continue;
			CalcLocation(pNode);
		}
	}

	void CTreeNodeUI::SetItemTextColor(DWORD _dwItemTextColor)
	{
		m_dwItemTextColor	= _dwItemTextColor;
		m_pItemButton->SetTextColor(m_dwItemTextColor);
	}

	DWORD CTreeNodeUI::GetItemTextColor() const
	{
		return m_dwItemTextColor;
	}

	void CTreeNodeUI::SetItemHotTextColor( DWORD _dwItemHotTextColor )
	{
		m_dwItemHotTextColor = _dwItemHotTextColor;
		Invalidate();
	}

	DWORD CTreeNodeUI::GetItemHotTextColor() const
	{
		return m_dwItemHotTextColor;
	}

	void CTreeNodeUI::SetSelItemTextColor( DWORD _dwSelItemTextColor )
	{
		m_dwSelItemTextColor = _dwSelItemTextColor;
		Invalidate();
	}

	DWORD CTreeNodeUI::GetSelItemTextColor() const
	{
		return m_dwSelItemTextColor;
	}

	void CTreeNodeUI::SetSelItemHotTextColor( DWORD _dwSelHotItemTextColor )
	{
		m_dwSelItemHotTextColor = _dwSelHotItemTextColor;
		Invalidate();
	}

	DWORD CTreeNodeUI::GetSelItemHotTextColor() const
	{
		return m_dwSelItemHotTextColor;
	}

	/*****************************************************************************/
	/*****************************************************************************/
	/*****************************************************************************/

	CTreeViewUI::CTreeViewUI( void ) : m_bVisibleFolderBtn(true),m_bVisibleCheckBtn(false),m_uItemMinWidth(0)
	{
		this->GetHeader()->SetVisible(false);
	}

	CTreeViewUI::~CTreeViewUI( void )
	{
		
	}

	LPCTSTR CTreeViewUI::GetClass() const
	{
		return _T("TreeViewUI");
	}

	LPVOID CTreeViewUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, _T("TreeView")) == 0 ) return static_cast<CTreeViewUI*>(this);
		return CListUI::GetInterface(pstrName);
	}

	bool CTreeViewUI::Add( CTreeNodeUI* pControl )
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("TreeNodeUI")) != 0)
			return false;

		pControl->OnNotify += MakeDelegate(this,&CTreeViewUI::OnDBClickItem);
		pControl->GetFolderButton()->OnNotify += MakeDelegate(this,&CTreeViewUI::OnFolderChanged);
		pControl->GetCheckBox()->OnNotify += MakeDelegate(this,&CTreeViewUI::OnCheckBoxChanged);

		pControl->SetVisibleFolderBtn(m_bVisibleFolderBtn);
		pControl->SetVisibleCheckBtn(m_bVisibleCheckBtn);
		if(m_uItemMinWidth > 0)
			pControl->SetMinWidth(m_uItemMinWidth);

		CListUI::Add(pControl);

		if(pControl->GetCountChild() > 0)
		{
			int nCount = pControl->GetCountChild();
			for(int nIndex = 0;nIndex < nCount;nIndex++)
			{
				CTreeNodeUI* pNode = pControl->GetChildNode(nIndex);
				if(pNode)
					Add(pNode);
			}
		}

		pControl->SetTreeView(this);
		return true;
	}

	//************************************
	// 函数名称: AddAt
	// 返回类型: long
	// 参数信息: CTreeNodeUI * pControl
	// 参数信息: int iIndex
	// 函数说明: 该方法不会将待插入的节点进行缩位处理，若打算插入的节点为非根节点，请使用AddAt(CTreeNodeUI* pControl,CTreeNodeUI* _IndexNode) 方法
	//************************************
	long CTreeViewUI::AddAt( CTreeNodeUI* pControl, int iIndex )
	{
		if (!pControl)
			return -1;

		if (_tcsicmp(pControl->GetClass(), _T("TreeNodeUI")) != 0)
			return -1;

		//CTreeNodeUI* pParent = static_cast<CTreeNodeUI*>(GetItemAt(iIndex));
		//if(!pParent)
		//	return -1;

		pControl->OnNotify += MakeDelegate(this,&CTreeViewUI::OnDBClickItem);
		pControl->GetFolderButton()->OnNotify += MakeDelegate(this,&CTreeViewUI::OnFolderChanged);
		pControl->GetCheckBox()->OnNotify += MakeDelegate(this,&CTreeViewUI::OnCheckBoxChanged);

		pControl->SetVisibleFolderBtn(m_bVisibleFolderBtn);
		pControl->SetVisibleCheckBtn(m_bVisibleCheckBtn);

		if(m_uItemMinWidth > 0)
			pControl->SetMinWidth(m_uItemMinWidth);

		CListUI::AddAt(pControl,iIndex);

		if(pControl->GetCountChild() > 0)
		{
			int nCount = pControl->GetCountChild();
			for(int nIndex = 0;nIndex < nCount;nIndex++)
			{
				CTreeNodeUI* pNode = pControl->GetChildNode(nIndex);
				if(pNode)
					return AddAt(pNode,iIndex+1);
			}
		}
		else
			return iIndex+1;

		return -1;
	}


	bool CTreeViewUI::AddAt( CTreeNodeUI* pControl,CTreeNodeUI* _IndexNode )
	{
		if(!_IndexNode && !pControl)
			return false;

		int nItemIndex = -1;

		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			if(_IndexNode == GetItemAt(nIndex)){
				nItemIndex = nIndex;
				break;
			}
		}

		if(nItemIndex == -1)
			return false;

		return AddAt(pControl,nItemIndex) >= 0;
	}

	//************************************
	// 函数名称: Remove
	// 返回类型: bool
	// 参数信息: CTreeNodeUI * pControl
	// 函数说明: pControl 对象以及下的所有节点将被一并移除
	//************************************
	bool CTreeViewUI::Remove( CTreeNodeUI* pControl )
	{
		if(pControl->GetCountChild() > 0)
		{
			int nCount = pControl->GetCountChild();
			for(int nIndex = 0;nIndex < nCount;nIndex++)
			{
				CTreeNodeUI* pNode = pControl->GetChildNode(nIndex);
				if(pNode){
					pControl->Remove(pNode);
				}
			}
		}
		CListUI::Remove(pControl);
		return true;
	}

	//************************************
	// 函数名称: RemoveAt
	// 返回类型: bool
	// 参数信息: int iIndex
	// 函数说明: iIndex 索引以及下的所有节点将被一并移除
	//************************************
	bool CTreeViewUI::RemoveAt( int iIndex )
	{
		CTreeNodeUI* pItem = (CTreeNodeUI*)GetItemAt(iIndex);
		if (!pItem)
		{
			return false;
		}
		//if(pItem->GetCountChild())
		Remove(pItem);
		return true;
	}

	void CTreeViewUI::RemoveAll()
	{
		CListUI::RemoveAll();
	}

	void CTreeViewUI::Notify( TNotifyUI& msg )
	{
		
	}

	bool CTreeViewUI::OnCheckBoxChanged( void* param )
	{
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if(pMsg->sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			CCheckBoxUI* pCheckBox = (CCheckBoxUI*)pMsg->pSender;
			CTreeNodeUI* pItem = (CTreeNodeUI*)pCheckBox->GetParent()->GetParent();
			SetItemCheckBox(pCheckBox->GetCheck(),pItem);
			m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED, (WPARAM)pItem);
			return true;
		}
		return true;
	}

	bool CTreeViewUI::OnFolderChanged( void* param )
	{
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if(pMsg->sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			CCheckBoxUI* pFolder = (CCheckBoxUI*)pMsg->pSender;
			CTreeNodeUI* pItem = (CTreeNodeUI*)pFolder->GetParent()->GetParent();

			pItem->SetVisibleTag(pFolder->GetCheck());
			SetItemExpand(pFolder->GetCheck(),pItem);
			
			return true;
		}
		return true;
	}

	bool CTreeViewUI::OnDBClickItem( void* param )
	{
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if(pMsg->sType == _T("itemdbclick"))
		{
			CTreeNodeUI* pItem		= static_cast<CTreeNodeUI*>(pMsg->pSender);
			CCheckBoxUI* pFolder	= pItem->GetFolderButton();
			pItem->SetVisibleTag(!pFolder->GetCheck());
			SetItemExpand(!pFolder->GetCheck(),pItem);

			if (IsEnabled()) 
			{
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMDBCLICK, GetCurSel());
			}

			return true;
		}
		return false;
	}

	bool CTreeViewUI::SetItemCheckBox( bool _Selected,CTreeNodeUI* _TreeNode /*= NULL*/ )
	{
		if(_TreeNode)
		{
			if(_TreeNode->GetCountChild() > 0)
			{
				int nCount = _TreeNode->GetCountChild();
				for(int nIndex = 0;nIndex < nCount;nIndex++)
				{
					CTreeNodeUI* pItem = _TreeNode->GetChildNode(nIndex);
					if (pItem->IsEnabled())
					{
						pItem->GetCheckBox()->Selected(_Selected);
						if (pItem->GetCountChild())
							SetItemCheckBox(_Selected, pItem);
					}
				}
			}
			return true;
		}
		else
		{
			int nIndex = 0;
			int nCount = GetCount();
			while(nIndex < nCount)
			{
				CTreeNodeUI* pItem = (CTreeNodeUI*)GetItemAt(nIndex);
				if (pItem->IsEnabled())
				{
					pItem->GetCheckBox()->Selected(_Selected);
					if (pItem->GetCountChild())
						SetItemCheckBox(_Selected, pItem);
				}

				nIndex++;
			}
			return true;
		}
		return false;
	}

	void CTreeViewUI::SetItemExpand( bool _Expanded,CTreeNodeUI* _TreeNode /*= NULL*/, bool bExpandChildren /*= false*/ )
	{
		if(_TreeNode)
		{
			if ((_TreeNode->IsExpanded() && !_Expanded) ||
				(!_TreeNode->IsExpanded() && _Expanded))
			{
				_TreeNode->Expand(_Expanded, bExpandChildren);
			}
		}
		else
		{
			int nIndex = 0;
			int nCount = GetCount();
			while(nIndex < nCount)
			{
				CTreeNodeUI* pItem = (CTreeNodeUI*)GetItemAt(nIndex);

				pItem->SetVisible(_Expanded);
				if ((pItem->IsExpanded() && !_Expanded) ||
					(!pItem->IsExpanded() && _Expanded))
				{
					pItem->Expand(_Expanded, bExpandChildren);
				}

				nIndex++;
			}
		}
	}

	void CTreeViewUI::SetVisibleFolderBtn( bool _IsVisibled )
	{
		m_bVisibleFolderBtn = _IsVisibled;
		int nCount = this->GetCount();
		for(int nIndex = 0;nIndex < nCount;nIndex++)
		{
			CTreeNodeUI* pItem = static_cast<CTreeNodeUI*>(this->GetItemAt(nIndex));
			pItem->GetFolderButton()->SetVisible(m_bVisibleFolderBtn);
		}
	}

	bool CTreeViewUI::GetVisibleFolderBtn()
	{
		return m_bVisibleFolderBtn;
	}

	void CTreeViewUI::SetVisibleCheckBtn( bool _IsVisibled )
	{
		m_bVisibleCheckBtn = _IsVisibled;
		int nCount = this->GetCount();
		for(int nIndex = 0;nIndex < nCount;nIndex++)
		{
			CTreeNodeUI* pItem = static_cast<CTreeNodeUI*>(this->GetItemAt(nIndex));
			pItem->GetCheckBox()->SetVisible(m_bVisibleCheckBtn);
		}
	}

	bool CTreeViewUI::GetVisibleCheckBtn()
	{
		return m_bVisibleCheckBtn;
	}

	void CTreeViewUI::SetItemMinWidth( UINT _ItemMinWidth )
	{
		m_uItemMinWidth = _ItemMinWidth;

		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			CTreeNodeUI* pTreeNode = static_cast<CTreeNodeUI*>(GetItemAt(nIndex));
			if(pTreeNode)
				pTreeNode->SetMinWidth(GetItemMinWidth());
		}
		Invalidate();
	}

	UINT CTreeViewUI::GetItemMinWidth()
	{
		return m_uItemMinWidth;
	}

	void CTreeViewUI::SetItemTextColor( DWORD _dwItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			CTreeNodeUI* pTreeNode = static_cast<CTreeNodeUI*>(GetItemAt(nIndex));
			if(pTreeNode)
				pTreeNode->SetItemTextColor(_dwItemTextColor);
		}
	}

	void CTreeViewUI::SetItemHotTextColor( DWORD _dwItemHotTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			CTreeNodeUI* pTreeNode = static_cast<CTreeNodeUI*>(GetItemAt(nIndex));
			if(pTreeNode)
				pTreeNode->SetItemHotTextColor(_dwItemHotTextColor);
		}
	}

	void CTreeViewUI::SetSelItemTextColor( DWORD _dwSelItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			CTreeNodeUI* pTreeNode = static_cast<CTreeNodeUI*>(GetItemAt(nIndex));
			if(pTreeNode)
				pTreeNode->SetSelItemTextColor(_dwSelItemTextColor);
		}
	}

	void CTreeViewUI::SetSelItemHotTextColor( DWORD _dwSelHotItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			CTreeNodeUI* pTreeNode = static_cast<CTreeNodeUI*>(GetItemAt(nIndex));
			if(pTreeNode)
				pTreeNode->SetSelItemHotTextColor(_dwSelHotItemTextColor);
		}
	}

	void CTreeViewUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if(_tcsicmp(pstrName,_T("visiblefolderbtn")) == 0)
			SetVisibleFolderBtn(_tcsicmp(pstrValue,_T("true")) == 0);
		else if(_tcsicmp(pstrName,_T("visiblecheckbtn")) == 0)
			SetVisibleCheckBtn(_tcsicmp(pstrValue,_T("true")) == 0);
		else if(_tcsicmp(pstrName,_T("itemminwidth")) == 0)
			SetItemMinWidth(_ttoi(pstrValue));
		else if(_tcsicmp(pstrName, _T("itemtextcolor")) == 0 ){
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemTextColor(clrColor);
		}
		else if(_tcsicmp(pstrName, _T("itemhottextcolor")) == 0 ){
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemHotTextColor(clrColor);
		}
		else if(_tcsicmp(pstrName, _T("selitemtextcolor")) == 0 ){
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelItemTextColor(clrColor);
		}
		else if(_tcsicmp(pstrName, _T("selitemhottextcolor")) == 0 ){
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelItemHotTextColor(clrColor);
		}
		else CListUI::SetAttribute(pstrName,pstrValue);
	}

}