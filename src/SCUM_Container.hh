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

	$Id: SCUM_Container.hh,v 1.2 2004/08/04 11:48:25 steve Exp $
*/


#ifndef SCUM_CONTAINER_HH_INCLUDED
#define SCUM_CONTAINER_HH_INCLUDED

#include "SCUM_View.hh"

#include <stdint.h>
#include <algorithm>
#include <vector>

class SCUM_Container : public SCUM_View
{
	friend class SCUM_View;

public:
	typedef std::vector<SCUM_View*> ChildList;
	typedef ChildList::iterator ChildIter;
	typedef ChildList::reverse_iterator ChildRIter;

public:
	SCUM_Container(SCUM_Container* parent, PyrObject* obj);
	virtual ~SCUM_Container();

	const ChildList& children() const { return m_children; }
	size_t numChildren() const { return m_children.size(); }
	bool isEmpty() const { return m_children.empty(); }
	SCUM_View* childAtPoint(const SCUM_Point& where);
	virtual SCUM_View* viewAtPoint(const SCUM_Point& where);
	void dumpChildren();

	void raise(SCUM_View* view);
	void lower(SCUM_View* view);

	virtual bool mouseDown(int state, const SCUM_Point& where);
	virtual void scrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta);
	virtual void contextMenu(int state, const SCUM_Point& where);

	virtual void draw(const SCUM_Rect& damage);
	virtual void drawChildren(const SCUM_Rect& damage);

	virtual bool canFocus() const;

	// properties
	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	SCUM_Point padding() const { return m_padding; }

protected:
	ChildList& children() { return m_children; }
	ChildIter begin() { return children().begin(); }
	ChildIter end() { return children().end(); }
	ChildRIter rbegin() { return children().rbegin(); }
	ChildRIter rend() { return children().rend(); }
	SCUM_View* front() { return children().front(); }
	SCUM_View* back() { return children().back(); }

	SCUM_View* nextFocus(bool canFocus, bool& foundFocus);
	SCUM_View* prevFocus(bool canFocus, bool& foundFocus);

	virtual void childAdded(SCUM_View* view);
	virtual void childRemoved(SCUM_View* view);

private:
	void addChild(SCUM_View* view);
	void removeChild(SCUM_View* view);

private:
	ChildList	m_children;
	SCUM_Point	m_padding;
};

// container with single child
// can actually contain several children, but only the topmost visible
// view is drawn
class SCUM_Bin : public SCUM_Container
{
public:
	SCUM_Bin(SCUM_Container* parent, PyrObject* obj);

	SCUM_View* firstVisibleChild();

protected:
	virtual SCUM_Size preferredSize();
	virtual void boundsChanged(const SCUM_Rect& bounds);
};

// container with dynamic layout of children
class SCUM_Box : public SCUM_Container
{
public:
	SCUM_Box(SCUM_Container* parent, PyrObject* obj);

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	bool isHomogenous() const { return flags().cHomogenous; }
	float spacing() const { return m_spacing; }

protected:
// 	virtual SCUM_Size preferredSize();
// 	virtual void boundsChanged(const SCUM_Rect& bounds);

// 	virtual SCUM_Size preferredChildrenSize();
// 	virtual void layoutChildren(const SCUM_Size& lastPreferredSize, const SCUM_Rect& bounds);
	
protected:
	float		m_spacing;
	uint16_t	m_numExpand;
	float		m_totalExpand;
};

// horizontal layout
class SCUM_HBox : public SCUM_Box
{
public:
	SCUM_HBox(SCUM_Container* parent, PyrObject* obj);

protected:
	void boundsChanged(const SCUM_Rect& bounds);
	SCUM_Size preferredSize();
};

// vertical layout
class SCUM_VBox : public SCUM_Box
{
public:
	SCUM_VBox(SCUM_Container* parent, PyrObject* obj);

protected:
	void boundsChanged(const SCUM_Rect& bounds);
	SCUM_Size preferredSize();
};

// tabular layout
class SCUM_Grid : public SCUM_Container
{
	struct Info
	{
		Info();
		Info(const Info& info);

		float			preferredSize;
		float			size;
		float			expand;
		bool			visible;
	};

public:
	SCUM_Grid(SCUM_Container* parent, PyrObject* obj);
	virtual ~SCUM_Grid();

	size_t cols() const { return m_cols; }
	size_t rows() const { return m_rows; }
	size_t wrap() const { return m_wrap; }

	bool isHomogenous() const { return flags().cHomogenous; }

	virtual void setProperty(const PyrSymbol* key, PyrSlot* slot);
	virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

	virtual void getDimensions(size_t& rows, size_t& cols) = 0;
	virtual SCUM_View* childAt(size_t row, size_t col) = 0;

protected:
	virtual SCUM_Size preferredSize();
	virtual void boundsChanged(const SCUM_Rect& bounds);

	virtual void childAdded(SCUM_View*);
	virtual void childRemoved(SCUM_View*);

private:
	void updateDimensions();

private:
	SCUM_Point			m_spacing;
	uint16_t			m_wrap;
	uint16_t			m_cols;
	uint16_t			m_rows;
	Info*				m_rowInfo;
	Info*				m_colInfo;
	SCUM_Point			m_totalExpand;
	uint16_t			m_rowsExpand;
	uint16_t			m_colsExpand;
};

// columns first
class SCUM_HGrid : public SCUM_Grid
{
public:
	SCUM_HGrid(SCUM_Container* parent, PyrObject* obj);

	virtual void getDimensions(size_t& rows, size_t& cols);
	virtual SCUM_View* childAt(size_t row, size_t col);
};

// rows first
class SCUM_VGrid : public SCUM_Grid
{
public:
	SCUM_VGrid(SCUM_Container* parent, PyrObject* obj);

	virtual void getDimensions(size_t& rows, size_t& cols);
	virtual SCUM_View* childAt(size_t row, size_t col);
};

// more containers to come:
//  Place -- absolute child positions
//  Scroll -- scroll area with child views (inefficient)
//  Pane -- resizable panes
//  Frame -- frame with optional label (may be in Bin, actually)

// TODO: move border to Bin, add label to Bin (window uses label as title)

#endif // SCUM_CONTAINER_HH_INCLUDED
