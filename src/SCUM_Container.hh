/*	-*- mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
	vi: ts=4 noet sw=4:

	SCUM. copyright (c) 2004 stefan kersten.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of the
	License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307 USA

	$Id$
*/


#ifndef SCUM_CONTAINER_HH_INCLUDED
#define SCUM_CONTAINER_HH_INCLUDED

#include "SCUM_View.hh"

#include <stdint.h>

// =====================================================================
// SCUM_Container

class SCUM_Container : public SCUM_View
{
	friend class SCUM_View;

public:
	SCUM_Container(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);
	virtual ~SCUM_Container();

	// hierarchy access
	inline const SCUM_View* firstChild() const;
	inline SCUM_View* firstChild();
	inline const SCUM_View* lastChild() const;
	inline SCUM_View* lastChild();

	inline size_t numChildren() const;
	inline bool isEmpty() const;

	SCUM_View* childAtPoint(const SCUM_Point& where);
	virtual SCUM_View* viewAtPoint(const SCUM_Point& where);

	void dumpChildren();

	// events
	virtual bool mouseDown(int state, const SCUM_Point& where);
	virtual void scrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta);
	virtual void contextMenu(int state, const SCUM_Point& where);

	virtual void draw(const SCUM_Rect& damage);
	virtual void drawChildren(const SCUM_Rect& damage);

	virtual bool canFocus() const;

	// properties
	virtual void setProperty(const char* key, SCUM_ArgStream& args);
	//virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	inline bool isHomogenous() const { return flags().cHomogenous; }
	inline SCUM_Point padding() const { return m_padding; }

protected:
	void raise(SCUM_View* view, int n=1);
	void lower(SCUM_View* view, int n=1);

	SCUM_View* nextFocus(bool canFocus, bool& foundFocus);
	SCUM_View* prevFocus(bool canFocus, bool& foundFocus);

	virtual void childAdded(SCUM_View* view);
	virtual void childRemoved(SCUM_View* view);

private:
	void addChild(SCUM_View* view);
	void removeChild(SCUM_View* view);

private:
	SCUM_View*	m_firstChild;
	SCUM_View*	m_lastChild;
	size_t		m_numChildren;
	SCUM_Point	m_padding;
};

// =====================================================================
// SCUM_Bin
//
//     container with single visible child (topmost view is drawn)

class SCUM_Bin : public SCUM_Container
{
public:
	SCUM_Bin(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);

	inline SCUM_View* visibleChild();
	SCUM_View* firstVisibleChild();
	virtual void drawView(const SCUM_Rect& damage);
	virtual void drawChildren(const SCUM_Rect& damage);
	virtual SCUM_Size getMinSize();

protected:
	virtual void boundsChanged(const SCUM_Rect& bounds);
	virtual void childAdded(SCUM_View* view);
	virtual void childRemoved(SCUM_View* view);

private:
	SCUM_View*	m_visibleChild;
	uint8_t		m_border;
};

// =====================================================================
// SCUM_Box
//
//     container with dynamic layout of children

class SCUM_Box : public SCUM_Container
{
public:
	SCUM_Box(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);

	virtual void setProperty(const char* key, SCUM_ArgStream& args);
	//virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	float spacing() const { return m_spacing; }

protected:
	float		m_spacing;
	uint16_t	m_numExpand;
	float		m_totalExpand;
};

// =====================================================================
// SCUM_HBox
//
//     horizontal layout

class SCUM_HBox : public SCUM_Box
{
public:
	SCUM_HBox(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);

	virtual SCUM_Size getMinSize();

protected:
	void boundsChanged(const SCUM_Rect& bounds);
};

// =====================================================================
// SCUM_VBox
//
//     vertical layout

class SCUM_VBox : public SCUM_Box
{
public:
	SCUM_VBox(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);

	virtual SCUM_Size getMinSize();

protected:
	virtual void boundsChanged(const SCUM_Rect& bounds);
};

// =====================================================================
// SCUM_Grid
//
//     tabular layout

class SCUM_Grid : public SCUM_Container
{
	struct Info
	{
		float			preferredSize;
		float			size;
		float			expand;
		bool			visible;
	};

public:
	SCUM_Grid(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);
	virtual ~SCUM_Grid();

	size_t numRows() const { return m_numRows; }
	size_t numCols() const { return m_numCols; }

	virtual void setProperty(const char* key, SCUM_ArgStream& args);
	//virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	inline SCUM_View* childAt(size_t row, size_t col);
	inline void childPut(size_t row, size_t col, SCUM_View* view);

	virtual size_t childIndex(size_t row, size_t col);

protected:
	virtual SCUM_Size getMinSize();
	virtual void boundsChanged(const SCUM_Rect& bounds);

	virtual void childAdded(SCUM_View*);
	virtual void childRemoved(SCUM_View*);

private:
	void updateGrid();
	void dimensionsChanged();

private:
	SCUM_Point			m_spacing;
// 	uint16_t			m_wrap;
	uint16_t			m_numRows;
	uint16_t			m_numCols;
	SCUM_View**			m_grid;
	Info*				m_rowInfo;
	Info*				m_colInfo;
	SCUM_Point			m_totalExpand;
	uint16_t			m_rowsExpand;
	uint16_t			m_colsExpand;
};

// =====================================================================
// SCUM_HGrid
//
//     columns first

class SCUM_HGrid : public SCUM_Grid
{
public:
	SCUM_HGrid(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);
	virtual size_t childIndex(size_t row, size_t col);
};

// =====================================================================
// SCUM_VGrid
//
//     rows first

class SCUM_VGrid : public SCUM_Grid
{
public:
	SCUM_VGrid(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);
	virtual size_t childIndex(size_t row, size_t col);
};

// more containers to come:
//  Place -- absolute child positions
//  Scroll -- scroll area with child views (inefficient)
//  Pane -- resizable panes
//  Frame -- frame with optional label (may be in Bin, actually)

// TODO: move border to Bin, add label to Bin (window uses label as title)

// =====================================================================
// SCUM_Scroll
//
//     scrollable bin
#if 0
class SCUM_Scroll : public SCUM_Bin
{
public:
	SCUM_Scroll(SCUM_Container* parent, PyrObject* obj);

protected:
	virtual SCUM_Size getMinSize();
	virtual void boundsChanged(const SCUM_Rect& bounds);

private:
	int			m_thumbSize;
	SCUM_Rect	m_viewPortBounds;
	SCUM_Rect	m_contentBounds;
	SCUM_Rect	m_vThumbBounds;
	SCUM_Rect	m_hThumbBounds;
	SCUM_Point	m_scrollRatio;
	float		m_mouseMoveStart;
	uint8_t		m_mouseMoveOrient;
	uint8_t		m_hThumb;
	uint8_t		m_vThumb;
	SCUM_View*	m_current;
};

// =====================================================================
// SCUM_Place
//
//     absolute placement of child views

class SCUM_Place : public SCUM_Container
{
public:
	SCUM_Place(SCUM_Container* parent, PyrObject* obj);

protected:
	virtual SCUM_Size getMinSize();
	virtual void boundsChanged(const SCUM_Rect& bounds);

private:
	uint8_t		m_adapt;
};
#endif // 0

// =====================================================================
// SCUM_Container (inline functions)

inline const SCUM_View* SCUM_Container::firstChild() const
{
	return m_firstChild;
}

inline SCUM_View* SCUM_Container::firstChild()
{
	return m_firstChild;
}

inline const SCUM_View* SCUM_Container::lastChild() const
{
	return m_lastChild;
}

inline SCUM_View* SCUM_Container::lastChild()
{
	return m_lastChild;
}

inline size_t SCUM_Container::numChildren() const
{
	return m_numChildren;
}

inline bool SCUM_Container::isEmpty() const
{
	return numChildren() == 0;
}

// =====================================================================
// SCUM_Bin (inline functions)

SCUM_View* SCUM_Bin::visibleChild()
{
	return m_visibleChild;
}

// =====================================================================
// SCUM_Grid (inline functions)

inline SCUM_View* SCUM_Grid::childAt(size_t row, size_t col)
{
	return m_grid[childIndex(row, col)];
}

inline void SCUM_Grid::childPut(size_t row, size_t col, SCUM_View* view)
{
	m_grid[childIndex(row, col)] = view;
}

#endif // SCUM_CONTAINER_HH_INCLUDED
