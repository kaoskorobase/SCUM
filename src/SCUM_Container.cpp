/*  -*- mode: c++; indent-tabs-mode: t; c-basic-offset: 4 -*-
    vi: noet sta sw=4:

    SCUM. copyright (c) 2004, 2005 stefan kersten.

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


#include "SCUM_Container.hh"
#include "SCUM_GC.hh"
#include "SCUM_Window.hh"
#include "SCUM_Util.hh"

using namespace SCUM;

// =====================================================================
// SCUM_Container

SCUM_Container::SCUM_Container(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_View(klass, client, oid, args),
      m_firstChild(0), m_lastChild(0), m_numChildren(0),
      m_padding(0.)
{
}

SCUM_Container::~SCUM_Container()
{
    m_numChildren = 0;
    SCUM_View* child = firstChild();
    m_firstChild = m_lastChild = 0;
    while (child) {
	SCUM_View* next = child->nextView();
	delete child;
	child = next;
    }
}

SCUM_View* SCUM_Container::childAtPoint(const SCUM_Point& where)
{
    SCUM_View* child = lastChild();
    while (child) {
	SCUM_View* hit = child->viewAtPoint(where);
	if (hit) return hit;
	child = child->prevView();
    }
    return 0;
}

SCUM_View* SCUM_Container::viewAtPoint(const SCUM_Point& where)
{
    return childAtPoint(where);
}

void SCUM_Container::dumpChildren()
{
    printf("SCUM_Container %p:\n", this);
    SCUM_View* child = firstChild();
    while (child) {
	printf("    %p\n", child);
	child = child->nextView();
    }
}

void SCUM_Container::raise(SCUM_View* view, int n)
{
    SCUM_ASSERT(view->parent() == this);

    if (numChildren() > 1) {
	while ((n != 0) && (view != m_firstChild)) {
	    SCUM_View* prev = view->prevView();
	    SCUM_ASSERT(prev != 0);
	    view->setPrevView(prev->prevView());
	    if (view->prevView()) view->prevView()->setNextView(view);
	    prev->setNextView(view->nextView());
	    if (prev->nextView()) prev->nextView()->setPrevView(prev);
	    view->setNextView(prev);
	    prev->setPrevView(view);
	    if (prev == m_firstChild) m_firstChild = view;
	    if (view == m_lastChild) m_lastChild = prev;
	    n--;
	}
	updateLayout();
    }
}

void SCUM_Container::lower(SCUM_View* view, int n)
{
    SCUM_ASSERT(view->parent() == this);

    if (numChildren() > 1) {
	while ((n != 0) && (view != m_lastChild)) {
	    SCUM_View* next = view->nextView();
	    SCUM_ASSERT(next != 0);
	    next->setPrevView(view->prevView());
	    if (next->prevView()) next->prevView()->setNextView(next);
	    view->setPrevView(next);
	    view->setNextView(next->nextView());
	    if (view->nextView()) view->nextView()->setPrevView(view);
	    next->setNextView(view);
	    if (view == m_firstChild) m_firstChild = next;
	    if (next == m_lastChild) m_lastChild = view;
	    n--;
	}
	updateLayout();
    }
}

bool SCUM_Container::mouseDown(int state, const SCUM_Point& where)
{
    SCUM_View* view = viewAtPoint(where);
    if (view) {
	if (view->canFocus()) {
	    if (!view->hasFocus()) view->makeFocus(true, true);
	} else {
	    window()->unsetFocus(true);
	}
	if (view->isEnabled() && view->mouseDown(state, where)) {
	    window()->setMouseView(view);
	}
    } else {
	window()->unsetFocus(true);
    }
    return false;
}

void SCUM_Container::scrollWheel(int state, const SCUM_Point& where, const SCUM_Point& delta)
{
    SCUM_View* view = childAtPoint(where);
    if (view && view->isEnabled()) view->scrollWheel(state, where, delta);
}

void SCUM_Container::contextMenu(int state, const SCUM_Point& where)
{
    SCUM_View* view = childAtPoint(where);
    if (view && view->isEnabled()) view->contextMenu(state, where);
}

void SCUM_Container::draw(const SCUM_Rect& damage)
{
    if (isVisible() && damage.intersects(bounds())) {
	drawView(damage);
	drawChildren(damage);
	drawDisabled(damage);
    }
}

void SCUM_Container::drawChildren(const SCUM_Rect& damage)
{
    SCUM_View* child = firstChild();
    while (child) {
	child->draw(damage);
	child = child->nextView();
    }
}

void SCUM_Container::setProperty(const char* key, SCUM_ArgStream& args)
{
    if (equal(key, "xPadding")) {
	m_padding.x = max(0.f, args.get_f());
	updateLayout();
    } else if (equal(key, "yPadding")) {
	m_padding.y = max(0.f, args.get_f());
	updateLayout();
    } else {
	SCUM_View::setProperty(key, args);
    }
}

#if 0
void SCUM_Container::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (equal(key, "padding")) {
	setPointValue(slot, m_padding);
    } else {
	SCUM_View::getProperty(key, slot);
    }
}
#endif

bool SCUM_Container::canFocus() const
{
    return isVisible() && isEnabled() && (!parent() || parent()->canFocus());
}

SCUM_View* SCUM_Container::nextFocus(bool canFocus, bool& foundFocus)
{
    canFocus = canFocus && isEnabled() && isVisible();
    SCUM_View* child = firstChild();
    while (child) {
	SCUM_View* focus = child->nextFocus(canFocus, foundFocus);
	if (focus) return focus;
	child = child->nextView();
    }
    return 0;
}

SCUM_View* SCUM_Container::prevFocus(bool canFocus, bool& foundFocus)
{
    canFocus = canFocus && isEnabled() && isVisible();
    SCUM_View* child = lastChild();
    while (child) {
	SCUM_View* focus = child->prevFocus(canFocus, foundFocus);
	if (focus) return focus;
	child = child->prevView();
    }
    return 0;
}

void SCUM_Container::childAdded(SCUM_View* view)
{
}

void SCUM_Container::childRemoved(SCUM_View* view)
{
}

void SCUM_Container::addChild(SCUM_View* view)
{
    SCUM_ASSERT(
		(view != 0) && (view->parent() == 0) &&
		(view->nextView() == 0) && (view->prevView() == 0)
		);
    view->m_parent = this;
    view->m_window = window();

    if (m_lastChild) m_lastChild->setNextView(view);
    view->setPrevView(m_lastChild);
    view->setNextView(0);
    m_lastChild = view;
    if (!m_firstChild) m_firstChild = view;
    m_numChildren++;

#ifdef SCUM_DEBUG
    SCUM_ASSERT(m_numChildren > 0);
    if (m_numChildren == 1) {
	SCUM_ASSERT(m_firstChild == m_lastChild);
	SCUM_ASSERT(m_firstChild == view);
    } else {
	SCUM_ASSERT(m_firstChild != m_lastChild);
    }
    SCUM_ASSERT((m_firstChild->prevView() == 0) && (m_lastChild->nextView() == 0));
#endif

    childAdded(view);
    updateLayout();
}

void SCUM_Container::removeChild(SCUM_View* view)
{
    SCUM_ASSERT(
		(view != 0) && (view->parent() == this)
		);

#ifdef SCUM_DEBUG__
    fprintf(stderr, "SCUM_Container::removeChild: %p %p %p\n",
	    this, view, view->parent());
#endif

    if (isEmpty()) return;

    SCUM_View* prev = view->prevView();
    SCUM_View* next = view->nextView();

    if (prev) {
	prev->setNextView(next);
    } else {
	SCUM_ASSERT(view == m_firstChild);
	m_firstChild = next;
    }

    if (next) {
	next->setPrevView(prev);
    } else {
	SCUM_ASSERT(view == m_lastChild);
	m_lastChild = prev;
    }

    m_numChildren--;

    view->m_window = 0;
    view->m_parent = 0;
    view->setPrevView(0);
    view->setNextView(0);

#ifdef SCUM_DEBUG
    if (m_numChildren == 0) {
	SCUM_ASSERT((m_firstChild == 0) && (m_lastChild == 0));
    } else if (m_numChildren == 1) {
	SCUM_ASSERT(m_firstChild == m_lastChild);
    } else {
	SCUM_ASSERT(m_firstChild != m_lastChild);
    }
    if (m_numChildren > 0) {
	SCUM_ASSERT((m_firstChild->prevView() == 0) && (m_lastChild->nextView() == 0));
    }
#endif

#ifdef SCUM_DEBUG
    dumpChildren();
#endif

    childRemoved(view);
    updateLayout();
}

// =====================================================================
// SCUM_Bin

SCUM_Bin::SCUM_Bin(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_Container(klass, client, oid, args),
      m_visibleChild(0),
      m_border(kBorderNone)
{
}

SCUM_View* SCUM_Bin::firstVisibleChild()
{
    SCUM_View* child = lastChild();
    while (child) {
	if (child->isVisible()) return child;
	child = child->prevView();
    }
    return 0;
}

void SCUM_Bin::drawView(const SCUM_Rect& damage)
{
    SCUM_Container::drawView(damage);
    if (m_border != kBorderNone) {
	GCDrawBeveledRect(bounds(), 1, m_border == kBorderIn);
    }
}

void SCUM_Bin::drawChildren(const SCUM_Rect& damage)
{
    if (visibleChild()) {
	visibleChild()->draw(damage);
    }
}

SCUM_Size SCUM_Bin::getMinSize()
{
    SCUM_Size size;
    if (visibleChild()) {
	size = visibleChild()->minSize();
    } else {
	size = SCUM_Container::getMinSize();
    }
    return size.padded(padding() + (m_border == kBorderNone ? 0.f : 1.f));
}

void SCUM_Bin::boundsChanged(const SCUM_Rect& bounds)
{
    if (visibleChild()) {
	visibleChild()->setBounds(bounds.inset(padding() + (m_border == kBorderNone ? 0.f : 1.f)));
    }
}

void SCUM_Bin::childAdded(SCUM_View* view)
{
    m_visibleChild = firstVisibleChild();
}

void SCUM_Bin::childRemoved(SCUM_View* view)
{
    m_visibleChild = firstVisibleChild();
}

// =====================================================================
// SCUM_Box

SCUM_Box::SCUM_Box(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_Container(klass, client, oid, args),
      m_spacing(0.0f),
      m_numExpand(0),
      m_totalExpand(0.f)
{
    flags().cHomogenous = false;
    flags().cHasVisible = false;
}

void SCUM_Box::setProperty(const char* key, SCUM_ArgStream& args)
{
    if (equal(key, "homogenous")) {
	flags().cHomogenous = args.get_i();
	updateLayout();
    } else if (equal(key, "spacing")) {
	m_spacing = args.get_f();
	updateLayout();
    } else {
	SCUM_Container::setProperty(key, args);
    }
}

#if 0
void SCUM_Box::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (equal("homogenous", key)) {
	setBoolValue(slot, isHomogenous());
    } else if (equal("spacing", key)) {
	setFloatValue(slot, m_spacing);
    } else {
	SCUM_Container::getProperty(key, slot);
    }
}
#endif

// =====================================================================
// SCUM_HBox

SCUM_HBox::SCUM_HBox(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_Box(klass, client, oid, args)
{
}

SCUM_Size SCUM_HBox::getMinSize()
{
    const bool homogenous = isHomogenous();

    SCUM_Size size;
    int numVisible = 0;
    int numExpand = 0;
    float totalExpand = 0.f;

    SCUM_View* child = firstChild();
    while (child) {
	if (child->isVisible()) {
	    numVisible++;

	    SCUM_Size childSize(child->minSize());

	    if (homogenous) {
		size.w = max(size.w, childSize.w);
		numExpand++;
		totalExpand += child->layout().expand.x;
	    } else {
		size.w += childSize.w;
		if (child->layout().expand.x > 0.f) {
		    numExpand++;
		    totalExpand += child->layout().expand.x;
		}
	    }

	    size.h = max(size.h, childSize.h);
	}

	child = child->nextView();
    }

    flags().cHasVisible = numVisible > 0;
    m_numExpand = numExpand;
    m_totalExpand = totalExpand;

    if (flags().cHasVisible) {
	if (homogenous) size.w *= numVisible;
	size.w += (numVisible - 1) * spacing();
    }

    return size.padded(padding());
}

void SCUM_HBox::boundsChanged(const SCUM_Rect& newBounds)
{
    const SCUM_Rect bounds(newBounds.inset(padding()));
    const bool homogenous = isHomogenous();

    bool hasVisible = flags().cHasVisible;
    int numExpand = m_numExpand;
    float totalExpand = m_totalExpand;

    if (hasVisible) {
	double curExtra, uniExtra, totalExtra, expandScale;

	if (homogenous) {
	    curExtra = bounds.w - (numExpand - 1) * spacing();
	    uniExtra = floor(curExtra / numExpand);
	} else if (numExpand > 0) {
	    curExtra = totalExtra = bounds.w - minSize().w + 2. * padding().x;
	    expandScale = 1. / totalExpand;
	}
		
	SCUM_Point pen(bounds.x, bounds.y);
	SCUM_Size availSize(0.0f, bounds.h);

	SCUM_View* child = firstChild();
	while (child) {
	    if (child->isVisible()) {
		SCUM_Layout childLayout(child->layout());
		SCUM_Size wantSize = child->minSize();

		if (homogenous) {
		    if (numExpand == 1) {
			availSize.w = curExtra;
		    } else {
			availSize.w = uniExtra;
		    }
		    numExpand--;
		    curExtra -= uniExtra;
		} else {
		    availSize.w = wantSize.w;

		    if (childLayout.expand.x > 0.0f) {
			double extra = floor(totalExtra * childLayout.expand.x * expandScale);
			if (numExpand == 1) {
			    availSize.w += curExtra;
			} else {
			    availSize.w += extra;
			}
			numExpand--;
			curExtra -= extra;
		    }
		}
				
		// fill available space
		if (childLayout.fill.x > 0.0f) {
		    // fill but constrain to min width
		    wantSize.w = SCUM::max(wantSize.w, childLayout.fill.x * availSize.w);
		}
		if (childLayout.fill.y > 0.0f) {
		    // fill but constrain to min height
		    wantSize.h = SCUM::max(wantSize.h, childLayout.fill.y * availSize.h);
		}

		SCUM_Point origin;
				
		if ((availSize.w > wantSize.w) || (availSize.h > wantSize.h)) {
		    // need to align
		    origin = pen + availSize.layout(wantSize, makeAlign(childLayout.alignment));
		} else {
		    origin = pen;
		}

		child->setBounds(SCUM_Rect(origin, wantSize));

		pen.x += availSize.w + spacing();
	    }

	    child = child->nextView();
	}
    }
}

// =====================================================================
// SCUM_VBox

SCUM_VBox::SCUM_VBox(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_Box(klass, client, oid, args)
{
}

SCUM_Size SCUM_VBox::getMinSize()
{
    const bool homogenous = isHomogenous();

    SCUM_Size size;
    int numVisible = 0;
    int numExpand = 0;
    float totalExpand = 0.f;

    SCUM_View* child = firstChild();
    while (child) {
	if (child->isVisible()) {
	    numVisible++;

	    SCUM_Size childSize(child->minSize());

	    if (homogenous) {
		size.h = max(size.h, childSize.h);
		numExpand++;
		totalExpand += child->layout().expand.y;
	    } else {
		size.h += childSize.h;
		if (child->layout().expand.y > 0.f) {
		    numExpand++;
		    totalExpand += child->layout().expand.y;
		}
	    }

	    size.w = max(size.w, childSize.w);
	}
		
	child = child->nextView();
    }

    flags().cHasVisible = numVisible > 0;
    m_numExpand = numExpand;
    m_totalExpand = totalExpand;

    if (flags().cHasVisible) {
	if (homogenous) size.h *= numVisible;
	size.h += (numVisible - 1) * spacing();
    }

    return size.padded(padding());
}

void SCUM_VBox::boundsChanged(const SCUM_Rect& newBounds)
{
    const SCUM_Rect bounds(newBounds.inset(padding()));
    const bool homogenous = isHomogenous();

    bool hasVisible = flags().cHasVisible;
    int numExpand = m_numExpand;
    float totalExpand = m_totalExpand;

    if (hasVisible) {
	double curExtra, uniExtra, totalExtra, expandScale;

	if (homogenous) {
	    curExtra = bounds.h - (numExpand - 1) * spacing();
	    uniExtra = floor(curExtra / numExpand);
	} else if (numExpand > 0) {
	    curExtra = totalExtra = bounds.h - minSize().h + 2. * padding().y;
	    expandScale = 1. / totalExpand;
	}

	SCUM_Point pen(bounds.x, bounds.y);
	SCUM_Size availSize(bounds.w, 0.0f);

	SCUM_View* child = firstChild();
	while (child) {
	    if (child->isVisible()) {
		SCUM_Layout childLayout(child->layout());
		SCUM_Size wantSize = child->minSize();

		if (homogenous) {
		    if (numExpand == 1) {
			availSize.h = curExtra;
		    } else {
			availSize.h = uniExtra;
		    }
		    curExtra -= uniExtra;
		    numExpand--;
		} else {
		    availSize.h = wantSize.h;

		    if (childLayout.expand.y > 0.0f) {
			double extra = floor(totalExtra * childLayout.expand.y * expandScale);
			if (numExpand == 1) {
			    availSize.h += curExtra;
			} else {
			    availSize.h += extra;
			    curExtra -= extra;
			}
			numExpand--;
		    }
		}

		// fill available space
		if (childLayout.fill.x > 0.0f) {
		    // fill but constrain to min width
		    wantSize.w = SCUM::max(wantSize.w, childLayout.fill.x * availSize.w);
		}
		if (childLayout.fill.y > 0.0f) {
		    // fill but constrain to min height
		    wantSize.h = SCUM::max(wantSize.h, childLayout.fill.y * availSize.h);
		}

		SCUM_Point origin;

		if ((availSize.w > wantSize.w) || (availSize.h > wantSize.h)) {
		    // need to align
		    origin = pen + availSize.layout(wantSize, makeAlign(childLayout.alignment));
		} else {
		    origin = pen;
		}

		child->setBounds(SCUM_Rect(origin, wantSize));

		pen.y += availSize.h + spacing();
	    }

	    child = child->nextView();
	}
    }
}

// =====================================================================
// SCUM_Grid

SCUM_Grid::SCUM_Grid(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_Container(klass, client, oid, args),
      //	  m_wrap(1),
      m_numRows(0), m_numCols(0),
      m_grid(0), m_rowInfo(0), m_colInfo(0),
      m_colsExpand(0), m_rowsExpand(0)
{
    flags().cHomogenous = false;
    flags().cHasVisible = false;
}

SCUM_Grid::~SCUM_Grid()
{
    free(m_grid);
    free(m_rowInfo);
    free(m_colInfo);
}

void SCUM_Grid::setProperty(const char* key, SCUM_ArgStream& args)
{
    if (equal(key, "homogenous")) {
	flags().cHomogenous = args.get_i();
	updateLayout();
    } else if (equal(key, "xSpacing")) {
	m_spacing.x = max(0.f, args.get_f());
	updateLayout();
    } else if (equal(key, "ySpacing")) {
	m_spacing.y = max(0.f, args.get_f());
	updateLayout();
    } else if (equal(key, "dimensions")) {
	SCUM_Size size;
	size.w = args.get_f();
	size.h = args.get_f();
	m_numRows = max(0, (int)size.h);
	m_numCols = max(0, (int)size.w);
	dimensionsChanged();
	updateLayout();
	//	} else if (equal(key, "wrap")) {
	//		m_wrap = (uint16_t)max(1, intValue(slot));
	//		updateGrid(true);
    } else {
	SCUM_Container::setProperty(key, args);
    }
}

#if 0
void SCUM_Grid::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
    if (equal(key, "homogenous")) {
	setBoolValue(slot, isHomogenous());
    } else if (equal(key, "spacing")) {
	setPointValue(slot, m_spacing);
    } else if (equal(key, "dimensions")) {
	setSizeValue(slot, SCUM_Size(numCols(), numRows()));
	//	} else if (equal(key, "wrap")) {
	//		setIntValue(slot, m_wrap);
    } else {
	SCUM_Container::getProperty(key, slot);
    }
}
#endif

SCUM_Size SCUM_Grid::getMinSize()
{
    const bool homogenous = isHomogenous();
    const size_t nr = numRows();
    const size_t nc = numCols();
    size_t r, c;

    size_t rowsVisible = 0;
    size_t colsVisible = 0;

    m_totalExpand = SCUM_Point();
    m_rowsExpand = 0;
    m_colsExpand = 0;

    for (r=0; r < nr; r++) {
	Info* info = m_rowInfo + r;

	info->preferredSize = 0.f;
	info->expand = 0.f;
	info->visible = false;

	for (c=0; c < nc; c++) {
	    SCUM_View* child = childAt(r, c);
	    if (child == 0) break;

	    if (child->isVisible()) {
		info->visible = true;
		info->preferredSize
		    = max(info->preferredSize, child->minSize().h);
		info->expand = homogenous ? 1.f : max(info->expand, child->layout().expand.y);
	    }
	}

	if (info->visible) {
	    rowsVisible++;
	}
	if (info->expand > 0.f) {
	    m_rowsExpand++;
	    m_totalExpand.y += info->expand;
	}
    }

    for (c=0; c < nc; c++) {
	Info* info = m_colInfo + c;

	info->preferredSize = 0.f;
	info->expand = 0.f;
	info->visible = false;

	for (r=0; r < nr; r++) {
	    SCUM_View* child = childAt(r, c);
	    if (child == 0) break;

	    if (child->isVisible()) {
		info->visible = true;
		info->preferredSize
		    = max(info->preferredSize, child->minSize().w);
		info->expand = homogenous ? 1.f : max(info->expand, child->layout().expand.x);
	    }
	}

	if (info->visible) {
	    colsVisible++;
	}
	if (info->expand > 0.f) {
	    m_colsExpand++;
	    m_totalExpand.x += info->expand;
	}
    }

    flags().cHasVisible = (rowsVisible > 0) && (colsVisible > 0);
    SCUM_Size size;

    if (flags().cHasVisible) {
	if (homogenous) {
	    for (c=0; c < nc; c++) size.w = max(size.w, m_colInfo[c].preferredSize);
	    for (r=0; r < nr; r++) size.h = max(size.h, m_rowInfo[r].preferredSize);
	    size.w *= colsVisible;
	    size.h *= rowsVisible;
	} else {
	    for (c=0; c < nc; c++) size.w += m_colInfo[c].preferredSize;
	    for (r=0; r < nr; r++) size.h += m_rowInfo[r].preferredSize;
	}

	size.w += (colsVisible - 1) * m_spacing.x;
	size.h += (rowsVisible - 1) * m_spacing.y;
    }

    return size.padded(padding());
}

void SCUM_Grid::boundsChanged(const SCUM_Rect& newBounds)
{
    if (flags().cHasVisible) {
	const SCUM_Rect bounds(newBounds.inset(padding()));
	const bool homogenous = isHomogenous();
	const size_t nr = numRows();
	const size_t nc = numCols();

	size_t numExpand, c, r;
	double curExtra, uniExtra, totalExtra, expandScale;

	// pass 1: allocate column widths
	numExpand = m_colsExpand;

	if (homogenous) {
	    curExtra = bounds.w - (numExpand - 1) * m_spacing.x;
	    uniExtra = floor(curExtra / numExpand);
	} else {
	    curExtra = totalExtra = bounds.w - minSize().w + 2. * padding().x;
	    expandScale = 1. / m_totalExpand.x;
	}

	for (c=0; c < nc; c++) {
	    Info* info = m_colInfo + c;

	    if (info->visible) {
		if (homogenous) {
		    if (numExpand == 1) {
			info->size = curExtra;
		    } else {
			info->size = uniExtra;
		    }
		    numExpand--;
		    curExtra -= uniExtra;
		} else {
		    info->size = info->preferredSize;

		    if (info->expand > 0.f) {
			double extra = floor(totalExtra * info->expand * expandScale);
			if (numExpand == 1) {
			    info->size += curExtra;
			} else {
			    info->size += extra;
			}
			numExpand--;
			curExtra -= extra;
		    }
		}
	    }
	}

	// pass 2: allocate row heights
	numExpand = m_rowsExpand;

	if (homogenous) {
	    curExtra = bounds.h - (numExpand - 1) * m_spacing.y;
	    uniExtra = floor(curExtra / numExpand);
	} else {
	    curExtra = totalExtra = bounds.h - minSize().h + 2. * padding().y;
	    expandScale = 1. / m_totalExpand.y;
	}

	for (r=0; r < nr; r++) {
	    Info* info = m_rowInfo + r;
	    info->size = 0.f;

	    if (info->visible) {
		if (homogenous) {
		    if (numExpand == 1) {
			info->size = curExtra;
		    } else {
			info->size = uniExtra;
		    }
		    numExpand--;
		    curExtra -= uniExtra;
		} else {
		    info->size = info->preferredSize;
		    if (info->expand > 0.f) {
			double extra = floor(totalExtra * info->expand * expandScale);
			if (numExpand == 1) {
			    info->size += curExtra;
			} else {
			    info->size += extra;
			}
			numExpand--;
			curExtra -= extra;
		    }
		}
	    }
	}

	// pass 3: layout the suckers
	SCUM_Point pen(bounds.x, bounds.y);

	for (c=0; c < nc; c++) {
	    Info* colInfo = m_colInfo + c;

	    for (r=0; r < nr; r++) {
		SCUM_View* child = childAt(r, c);
		if (child == 0) break;

		SCUM_Layout childLayout(child->layout());
		Info* rowInfo = m_rowInfo + r;

		SCUM_Size availSize(colInfo->size, rowInfo->size);
		SCUM_Size wantSize = child->minSize();

		// fill packing space
		if (childLayout.fill.x > 0.f) {
		    // fill but constrain to min width
		    wantSize.w = max(wantSize.w, childLayout.fill.x * availSize.w);
		}
		if (childLayout.fill.y > 0.f) {
		    // fill but constrain to min height
		    wantSize.h = max(wantSize.h, childLayout.fill.y * availSize.h);
		}

		SCUM_Point origin;
				
		if ((availSize.w > wantSize.w) || (availSize.h > wantSize.h)) {
		    // need to align
		    origin = pen + availSize.layout(wantSize, makeAlign(childLayout.alignment));
		} else {
		    origin = pen;
		}

		child->setBounds(SCUM_Rect(origin, wantSize));

		pen.y += rowInfo->size + m_spacing.y;
	    }

	    pen.x += colInfo->size + m_spacing.x;
	    pen.y = bounds.y;
	}
    }
}

void SCUM_Grid::childAdded(SCUM_View*)
{
    updateGrid();
}

void SCUM_Grid::childRemoved(SCUM_View*)
{
    updateGrid();
}

void SCUM_Grid::dimensionsChanged()
{
    const size_t rows = numRows();
    const size_t cols = numCols();
    const size_t gridSize = rows * cols;
    size_t bytes;

    bytes = sizeof(SCUM_View*) * gridSize;
    m_grid = (SCUM_View**)realloc(m_grid, bytes);
    updateGrid();

    bytes = sizeof(Info) * rows;
    m_rowInfo = (Info*)realloc(m_rowInfo, bytes);
    memset(m_rowInfo, 0, bytes);
	
    bytes = sizeof(Info) * cols;
    m_colInfo = (Info*)realloc(m_colInfo, bytes);
    memset(m_colInfo, 0, bytes);
}

void SCUM_Grid::updateGrid()
{
    const size_t rows = numRows();
    const size_t cols = numCols();

    memset(m_grid, 0, sizeof(SCUM_View*) * rows * cols);

    SCUM_View* child = firstChild();
    for (int i = 0; i < numChildren(); ++i) {
	m_grid[gridIndex(i)] = child;
	child = child->nextView();
    }
//     for (size_t r=0; r < rows; r++) {
//	   for (size_t c=0; c < cols; c++) {
//	       if (child == 0) return;
//	       childPut(r, c, child);
//	       child = child->nextView();
//	   }
//     }
}

size_t SCUM_Grid::gridIndex(size_t index)
{
    return index;
}

// =====================================================================
// SCUM_HGrid

SCUM_HGrid::SCUM_HGrid(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_Grid(klass, client, oid, args)
{ }

// =====================================================================
// SCUM_VGrid

SCUM_VGrid::SCUM_VGrid(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args)
    : SCUM_Grid(klass, client, oid, args)
{ }

// size_t SCUM_VGrid::childIndex(size_t row, size_t col)
// {
// //	  return col * numRows() + row;
//     size_t i = row * numCols() + col;
//     row = i % numRows();
//     col = i / numRows();
//     size_t j = row * numCols() + col;
// //	  printf("%d <-> %d\n", i, j);
//     return j;
// }

size_t SCUM_VGrid::gridIndex(size_t index)
{
//     return col * numRows() + row;
    size_t row = index % numRows();
    size_t col = index / numRows();
    return row * numCols() + col;
}

// =====================================================================
// SCUM_Place

#if 0

SCUM_Place::SCUM_Place(SCUM_Container* parent, PyrObject* obj)
    : SCUM_Container(parent, obj),
      m_adapt(false)
{ }

SCUM_Size SCUM_Place::preferredSize()
{
    if (m_adapt) {
	SCUM_Rect bounds;

		
    } else {
	return SCUM_Container::preferredSize();
    }
}

void SCUM_Place::boundsChanged(const SCUM_Rect& bounds)
{
}

// =====================================================================
// SCUM_Scroll

SCUM_Scroll::SCUM_Scroll(SCUM_Container* parent, PyrObject* obj)
    : SCUM_Bin(parent, obj)
{ }

SCUM_Size SCUM_Scroll::preferredSize()
{
    if (m_current) m_current->updatePreferredSize();
    return layout().minSize + padding() + m_thumbSize;
}

void SCUM_Scroll::boundsChanged(const SCUM_Rect& bounds)
{
    m_viewportBounds = bounds().inset(padding() + m_thumbSize);
    if (m_current) {
	m_current->setBounds(
			     SCUM_Rect(m_viewportBounds.origin(), m_current->layout().lastPreferredSize)
			     );
	m_current->setScrollOffset(m_scroll);
    }
}

void SCUM_Scroll::setScrollOffset(const SCUM_Point& scroll)
{
    if (scroll != m_scroll) {
	m_scroll = scroll;
	if (m_current) m_current->setScrollOffset(m_scroll);
	refresh();
    }
}

void SCUM_Scroll::drawView(const SCUM_Rect& damage)
{
    // draw scrollbars
    GCSave();
    GCSetColor(bgColor());
    GCFillRect(bounds());
    GCDrawBeveledRect(m_hThumbBounds, 1, true);
    GCDrawBeveledRect(m_vThumbBounds, 1, true);
    GCRestore();
}

void SCUM_Scroll::drawChildren(const SCUM_Rect& damage)
{
    if (m_current) {
	GCSave();
	GCClip(m_viewPortBounds);
	m_current->draw(damage);
	GCRestore();
    }
}

SCUM_View* SCUM_Scroll::viewAtPoint(const SCUM_Point& where)
{
    return m_current ? m_current->viewAtPoint(where) : 0;
}

bool SCUM_Scroll::mouseDown(int state, const SCUM_Point& where)
{
    if (m_viewportBounds.contains(where)) {
	SCUM_Bin::mouseDown(state, where);
    } else if (m_hThumbBounds.contains(where)) {
    } else if (m_vThumbBounds.contains(where)) {
    } else {
    }
}

void SCUM_Scroll::updateLayout()
{
    if (m_current) {
	SCUM_Size csize(m_current->updatePreferredSize());
	// recompute scroll ratio
	size().w / csize.w;
	size().h / csize.h;
    }
}
#endif // 0

#include "SCUM_Class.hh"

void SCUM_Container_Init(SCUM_ClassRegistry* reg)
{
    new SCUM_ClassT<SCUM_Container>(reg, "Container", "View", true);
    new SCUM_ClassT<SCUM_Bin>(reg, "Bin", "Container");
    new SCUM_ClassT<SCUM_Box>(reg, "Box", "Container", true);
    new SCUM_ClassT<SCUM_HBox>(reg, "HBox", "Box");
    new SCUM_ClassT<SCUM_VBox>(reg, "VBox", "Box");
    new SCUM_ClassT<SCUM_Grid>(reg, "Grid", "Container", true);
    new SCUM_ClassT<SCUM_HGrid>(reg, "HGrid", "Grid");
    new SCUM_ClassT<SCUM_VGrid>(reg, "VGrid", "Grid");
}

// EOF
