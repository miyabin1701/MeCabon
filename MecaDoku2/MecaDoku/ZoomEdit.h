#pragma once


// CZoomEdit

template<class T>
class CZoomEdit : public T
//class CZoomEdit : public CEdit
{
//	DECLARE_DYNAMIC(CZoomEdit)

public:
	CZoomEdit() : T(), m_baseType( 0 ), m_width( 0 ), m_height( 0 ) {};
	virtual ~CZoomEdit() {};

protected:
    /** @brief 差分を格納する矩形 */
    CRect m_rectDiff;

    /** @brief 伸縮タイプ */
    UINT  m_baseType;

    /** @brief 保持された幅 */
    LONG  m_width;

    /** @brief 保持された高さ */
    LONG  m_height;

public:
    enum {
        normal = 0x00,
        top    = 0x01,
        left   = 0x02,
        bottom = 0x04,
        right  = 0x08,
    };

    void Resize() {
        if( !m_hWnd ) {
            return;
        }
        CRect rect, me;

        GetParent()->GetClientRect( &rect );

        me.top    = rect.top    + m_rectDiff.top;
        me.left   = rect.left   + m_rectDiff.left;
        me.bottom = rect.bottom + m_rectDiff.bottom;
        me.right  = rect.right  + m_rectDiff.right;

        if( m_baseType & top    ) { me.bottom = me.top    + m_height; }
        if( m_baseType & left   ) { me.right  = me.left   + m_width;  }
        if( m_baseType & bottom ) { me.top    = me.bottom - m_height; }
        if( m_baseType & right  ) { me.left   = me.right  - m_width;  }

        MoveWindow( &me );

        InvalidateRect( NULL );
    }

    /** @brief 伸縮タイプを設定します 
        @param ui IN 伸縮タイプ
        @par 解説
            引数 ui には次の値を連結して指定します<BR>
        <BR>
            normal = 0x00   標準の伸縮をサポートします<BR>
            top    = 0x01   上を基点として上下には移動のみを行います<BR>
            left   = 0x02,  左を基点として左右には移動のみを行います<BR>
            bottom = 0x04,  下を基点として上下には移動のみを行います<BR>
            right  = 0x08,  右を基点として左右には移動のみを行います<BR>
        <BR>
            top, bottom の何れかが指定された場合、ウィンドウは最初の高さを保持します。<BR>
            left, right の何れかが指定された場合、ウィンドウは最初の幅を保持します。<BR>
    */
    void setBase( UINT ui ) {
        m_baseType = ui;
    }

    /** @brief 伸縮タイプ基点を左下に設定します
    */
    void setBase_BottomLeft() {
        m_baseType = bottom | left;
    }

    /** @brief 伸縮タイプ基点を下に設定します */
    void setBase_Bottom() {
        m_baseType = bottom;
    }

    /** @brief 伸縮タイプ基点を右下に設定します
    */
    void setBase_BottomRight() {
        m_baseType = bottom | right;
    }

    /** @brief 伸縮タイプ基点を右に設定します */
    void setBase_Right() {
        m_baseType = right;
    }

protected:
    virtual void PreSubclassWindow() {
        CRect rect, me;
        GetParent()->GetClientRect( &rect );

        GetWindowRect( &me );

        m_width  = me.Width();
        m_height = me.Height();

        GetParent()->ScreenToClient( &me );

        m_rectDiff.top    = me.top    - rect.top;
        m_rectDiff.left   = me.left   - rect.left;
        m_rectDiff.bottom = me.bottom - rect.bottom;
        m_rectDiff.right  = me.right  - rect.right;
    }

//	DECLARE_MESSAGE_MAP()
};


