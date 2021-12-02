#pragma once


// CPartEdit

class CPartEdit : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CPartEdit)

public:
	CPartEdit();
	virtual ~CPartEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
};


