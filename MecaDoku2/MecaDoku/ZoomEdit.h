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
    /** @brief �������i�[�����` */
    CRect m_rectDiff;

    /** @brief �L�k�^�C�v */
    UINT  m_baseType;

    /** @brief �ێ����ꂽ�� */
    LONG  m_width;

    /** @brief �ێ����ꂽ���� */
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

    /** @brief �L�k�^�C�v��ݒ肵�܂� 
        @param ui IN �L�k�^�C�v
        @par ���
            ���� ui �ɂ͎��̒l��A�����Ďw�肵�܂�<BR>
        <BR>
            normal = 0x00   �W���̐L�k���T�|�[�g���܂�<BR>
            top    = 0x01   �����_�Ƃ��ď㉺�ɂ͈ړ��݂̂��s���܂�<BR>
            left   = 0x02,  ������_�Ƃ��č��E�ɂ͈ړ��݂̂��s���܂�<BR>
            bottom = 0x04,  ������_�Ƃ��ď㉺�ɂ͈ړ��݂̂��s���܂�<BR>
            right  = 0x08,  �E����_�Ƃ��č��E�ɂ͈ړ��݂̂��s���܂�<BR>
        <BR>
            top, bottom �̉��ꂩ���w�肳�ꂽ�ꍇ�A�E�B���h�E�͍ŏ��̍�����ێ����܂��B<BR>
            left, right �̉��ꂩ���w�肳�ꂽ�ꍇ�A�E�B���h�E�͍ŏ��̕���ێ����܂��B<BR>
    */
    void setBase( UINT ui ) {
        m_baseType = ui;
    }

    /** @brief �L�k�^�C�v��_�������ɐݒ肵�܂�
    */
    void setBase_BottomLeft() {
        m_baseType = bottom | left;
    }

    /** @brief �L�k�^�C�v��_�����ɐݒ肵�܂� */
    void setBase_Bottom() {
        m_baseType = bottom;
    }

    /** @brief �L�k�^�C�v��_���E���ɐݒ肵�܂�
    */
    void setBase_BottomRight() {
        m_baseType = bottom | right;
    }

    /** @brief �L�k�^�C�v��_���E�ɐݒ肵�܂� */
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


