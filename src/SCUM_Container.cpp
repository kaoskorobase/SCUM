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

	$Id: SCUM_Container.cpp,v 1.2 2004/08/04 11:48:25 steve Exp $
*/


#include "SCUM_Container.hh"
#include "SCUM_Window.hh"
#include "SCUM_System.hh"

#include <assert.h>
#include <vector>
#include <iostream>

#include <PyrSlot.h>
#include <PyrKernel.h>

using namespace SCUM;

// =====================================================================
// SCUM_Container

SCUM_Container::SCUM_Container(SCUM_Container* parent, PyrObject* obj)
	: SCUM_View(parent, obj),
	  m_padding(0.)
{
}

SCUM_Container::~SCUM_Container()
{
	ChildList l(m_children);
	m_children.clear();
	for (ChildIter it=l.begin(); it != l.end(); it++) {
		delete *it;
	}
}

SCUM_View* SCUM_Container::childAtPoint(const SCUM_Point& where)
{
	ChildRIter it = rbegin();
	while (it != rend()) {
		SCUM_View* hit = (*it++)->viewAtPoint(where);
		if (hit) return hit;
	}
	return 0;
}

SCUM_View* SCUM_Container::viewAtPoint(const SCUM_Point& where)
{
	return childAtPoint(where);
}

void SCUM_Container::dumpChildren()
{
	ChildIter it = begin();
	ChildIter itEnd = end();
	printf("SCUM_Container %p:\n", this);
	while (it != itEnd) {
		SCUM_View* view = *it++;
		printf("    %p\n", view);
	}
}

void SCUM_Container::raise(SCUM_View* view)
{
	ChildRIter it1 = std::find(rbegin(), rend(), view);
	assert(it1 != rend());
	ChildRIter it2 = it1 + 1;
	if (it2 != rend()) {
		std::iter_swap(it1, it2);
		updateLayout();
	}
}

void SCUM_Container::lower(SCUM_View* view)
{
	ChildIter it1 = std::find(begin(), end(), view);
	assert(it1 != end());
	ChildIter it2 = it1 + 1;
	if (it2 != end()) {
		std::iter_swap(it1, it2);
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
	if (isVisible() && !GCIsClipped(bounds())) {
		SCUM_Rect myDamage(bounds() & damage);
		drawView(myDamage);
		drawChildren(myDamage);
		drawDisabled(myDamage);
		drawFocus(myDamage);
	}
}

void SCUM_Container::drawChildren(const SCUM_Rect& damage)
{
	ChildIter it = begin();
	ChildIter itEnd = end();
	while (it != itEnd) (*it++)->draw(damage);
}

void SCUM_Container::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "xPadding")) {
		m_padding.x = max(0., floatValue(slot));
		updateLayout();
	} else if (equal(key, "yPadding")) {
		m_padding.y = max(0., floatValue(slot));
		updateLayout();
	} else {
		SCUM_View::setProperty(key, slot);
	}
}

void SCUM_Container::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "padding")) {
		setPointValue(slot, m_padding);
	} else {
		SCUM_View::getProperty(key, slot);
	}
}

bool SCUM_Container::canFocus() const
{
	return isVisible() && isEnabled() && (!parent() || parent()->canFocus());
}

SCUM_View* SCUM_Container::nextFocus(bool canFocus, bool& foundFocus)
{
	canFocus = canFocus && isEnabled() && isVisible();
	ChildIter it = begin();
	while (it != end()) {
		SCUM_View* child = *it++;
		SCUM_View* focus = child->nextFocus(canFocus, foundFocus);
		if (focus) return focus;
	}
	return 0;
}

SCUM_View* SCUM_Container::prevFocus(bool canFocus, bool& foundFocus)
{
	canFocus = canFocus && isEnabled() && isVisible();
	ChildRIter it = rbegin();
	while (it != rend()) {
		SCUM_View* child = *it++;
		SCUM_View* focus = child->prevFocus(canFocus, foundFocus);
		if (focus) return focus;
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
	view->m_parent = this;
	view->m_window = window();
	children().push_back(view);
	childAdded(view);
	updateLayout();
}

void SCUM_Container::removeChild(SCUM_View* view)
{
	if (!isEmpty()) {
		ChildIter it = std::find(begin(), end(), view);
		if (it != end()) {
			view->makeFocus(false, false);
			view->m_parent = 0;
			view->m_window = 0;
			children().erase(it);
			childRemoved(view);
			updateLayout();
		}
	}
}

// =====================================================================
// SCUM_Bin

SCUM_Bin::SCUM_Bin(SCUM_Container* parent, PyrObject* obj)
	: SCUM_Container(parent, obj)
{
}

SCUM_Size SCUM_Bin::preferredSize()
{
	SCUM_Size size;
	SCUM_View* child = firstVisibleChild();
	if (child) {
		size = child->updatePreferredSize().padded(padding());
	}
	return size;
}

void SCUM_Bin::boundsChanged(const SCUM_Rect& bounds)
{
	SCUM_View* child = firstVisibleChild();
	if (child) child->setBounds(bounds.inset(padding()));
}

SCUM_View* SCUM_Bin::firstVisibleChild()
{
	ChildRIter it = rbegin();
	ChildRIter itEnd = rend();

	while (it != itEnd) {
		if ((*it)->isVisible())
			return *it;
		it++;
	}

	return 0;
}

// =====================================================================
// SCUM_Box

SCUM_Box::SCUM_Box(SCUM_Container* parent, PyrObject* obj)
	: SCUM_Container(parent, obj),
	  m_spacing(0.0f),
	  m_numExpand(0),
	  m_totalExpand(0.f)
{
	flags().cHomogenous = false;
	flags().cHasVisible = false;
}

void SCUM_Box::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "homogenous")) {
		flags().cHomogenous = boolValue(slot);
		updateLayout();
	} else if (equal(key, "spacing")) {
		m_spacing = floatValue(slot);
		updateLayout();
	} else {
		SCUM_Container::setProperty(key, slot);
	}
}

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

// =====================================================================
// SCUM_HBox

SCUM_HBox::SCUM_HBox(SCUM_Container* parent, PyrObject* obj)
	: SCUM_Box(parent, obj)
{
}

SCUM_Size SCUM_HBox::preferredSize()
{
	const bool homogenous = isHomogenous();
	const ChildIter itEnd = end();

	SCUM_Size size;
	ChildIter it;
	int numVisible = 0;
	int numExpand = 0;
	float totalExpand = 0.f;

	for (it=begin(); it != itEnd; it++) {
		SCUM_View* child = *it;

		if (child->isVisible()) {
			numVisible++;

			SCUM_Size childSize(child->updatePreferredSize());

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
	const ChildIter itEnd = end();

	bool hasVisible = flags().cHasVisible;
	int numExpand = m_numExpand;
	float totalExpand = m_totalExpand;

	ChildIter it;

	if (hasVisible) {
		double curExtra, uniExtra, totalExtra, expandScale;

		if (homogenous) {
			curExtra = bounds.w - (numExpand - 1) * spacing();
			uniExtra = floor(curExtra / numExpand);
		} else if (numExpand > 0) {
			curExtra = totalExtra = bounds.w - layout().lastPreferredSize.w + 2. * padding().x;
			expandScale = 1. / totalExpand;
		}
		
		SCUM_Point pen(bounds.x, bounds.y);
		SCUM_Size availSize(0.0f, bounds.h);

		for (it=begin(); it != itEnd; it++) {
			SCUM_View* child = *it;
			
			if (child->isVisible()) {
				SCUM_Layout childLayout(child->layout());
				SCUM_Size wantSize = childLayout.lastPreferredSize;

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
					origin = pen + availSize.layout(wantSize, childLayout.alignment);
				} else {
					origin = pen;
				}

				child->setBounds(SCUM_Rect(origin, wantSize));

				pen.x += availSize.w + spacing();
			}
		}
	}
}

// =====================================================================
// SCUM_VBox

SCUM_VBox::SCUM_VBox(SCUM_Container* parent, PyrObject* obj)
	: SCUM_Box(parent, obj)
{
}

SCUM_Size SCUM_VBox::preferredSize()
{
	const bool homogenous = isHomogenous();
	const ChildIter itEnd = end();

	SCUM_Size size;
	ChildIter it;
	int numVisible = 0;
	int numExpand = 0;
	float totalExpand = 0.f;

	for (ChildIter it=begin(); it != itEnd; it++) {
		SCUM_View* child = *it;

		if (child->isVisible()) {
			numVisible++;

			SCUM_Size childSize(child->updatePreferredSize());

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
	const ChildIter itEnd = end();

	bool hasVisible = flags().cHasVisible;
	int numExpand = m_numExpand;
	float totalExpand = m_totalExpand;

	ChildIter it;

	if (hasVisible) {
		double curExtra, uniExtra, totalExtra, expandScale;

		if (homogenous) {
			curExtra = bounds.h - (numExpand - 1) * spacing();
			uniExtra = floor(curExtra / numExpand);
		} else if (numExpand > 0) {
			curExtra = totalExtra = bounds.h - layout().lastPreferredSize.h + 2. * padding().y;
			expandScale = 1. / totalExpand;
		}

		SCUM_Point pen(bounds.x, bounds.y);
		SCUM_Size availSize(bounds.w, 0.0f);

		for (it=begin(); it != itEnd; it++) {
			SCUM_View* child = *it;
			
			if (child->isVisible()) {
				SCUM_Layout childLayout(child->layout());
				SCUM_Size wantSize = childLayout.lastPreferredSize;

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
					origin = pen + availSize.layout(wantSize, childLayout.alignment);
				} else {
					origin = pen;
				}

				child->setBounds(SCUM_Rect(origin, wantSize));

				pen.y += availSize.h + spacing();
			}
		}
	}
}

// =====================================================================
// SCUM_Grid

SCUM_Grid::Info::Info()
	: preferredSize(0.f),
	  size(0.f),
	  expand(0.f),
	  visible(false)
{ }

SCUM_Grid::Info::Info(const SCUM_Grid::Info& info)
	: preferredSize(info.preferredSize),
	  size(info.size),
	  expand(info.expand),
	  visible(info.visible)
{ }

SCUM_Grid::SCUM_Grid(SCUM_Container* parent, PyrObject* obj)
	: SCUM_Container(parent, obj),
	  m_wrap(2),
	  m_cols(0), m_rows(0),
	  m_rowInfo(0), m_colInfo(0),
	  m_colsExpand(0), m_rowsExpand(0)
{
	flags().cHomogenous = false;
	flags().cHasVisible = false;
}

SCUM_Grid::~SCUM_Grid()
{
	delete [] m_rowInfo;
	delete [] m_colInfo;
}

void SCUM_Grid::setProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "homogenous")) {
		flags().cHomogenous = boolValue(slot);
		updateLayout();
	} else if (equal(key, "xSpacing")) {
		m_spacing.x = max(0., floatValue(slot));
		updateLayout();
	} else if (equal(key, "ySpacing")) {
		m_spacing.y = max(0., floatValue(slot));
		updateLayout();
	} else if (equal(key, "wrap")) {
		m_wrap = (size_t)max(2, intValue(slot));
		updateLayout();
	} else {
		SCUM_Container::setProperty(key, slot);
	}
}

void SCUM_Grid::getProperty(const PyrSymbol* key, PyrSlot* slot)
{
	if (equal(key, "homogenous")) {
		setBoolValue(slot, isHomogenous());
	} else if (equal(key, "spacing")) {
		setPointValue(slot, m_spacing);
	} else if (equal(key, "wrap")) {
		setIntValue(slot, m_wrap);
	} else {
		SCUM_Container::getProperty(key, slot);
	}
}

SCUM_Size SCUM_Grid::preferredSize()
{
	const bool homogenous = isHomogenous();
	const size_t nr = rows();
	const size_t nc = cols();
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
			if (child->isVisible()) {
				info->visible = true;
				info->preferredSize
					= max(info->preferredSize, child->updatePreferredSize().h);
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
			if (child->isVisible()) {
				info->visible = true;
				info->preferredSize
					= max(info->preferredSize, child->layout().lastPreferredSize.w);
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
		const size_t nc = cols();
		const size_t nr = rows();

		size_t numExpand, c, r;
		double curExtra, uniExtra, totalExtra, expandScale;

		// pass 1: allocate column widths
		numExpand = m_colsExpand;

		if (homogenous) {
			curExtra = bounds.w - (numExpand - 1) * m_spacing.x;
			uniExtra = floor(curExtra / numExpand);
		} else {
			curExtra = totalExtra = bounds.w - layout().lastPreferredSize.w + 2. * padding().x;
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
			curExtra = totalExtra = bounds.h - layout().lastPreferredSize.h + 2. * padding().y;
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
				SCUM_Layout childLayout(child->layout());
				Info* rowInfo = m_rowInfo + r;

				SCUM_Size availSize(colInfo->size, rowInfo->size);
				SCUM_Size wantSize = childLayout.lastPreferredSize;

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
					origin = pen + availSize.layout(wantSize, childLayout.alignment);
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
	updateDimensions();
}

void SCUM_Grid::childRemoved(SCUM_View*)
{
	updateDimensions();
}

void SCUM_Grid::updateDimensions()
{
	size_t rows = 0;
	size_t cols = 0;

	getDimensions(rows, cols);

	if (rows != m_rows) {
		Info* info = new Info[rows];
		delete [] m_rowInfo;
		m_rowInfo = info;
		m_rows = rows;
	}

	if (cols != m_cols) {
		Info* info = new Info[cols];
		delete [] m_colInfo;
		m_colInfo = info;
		m_cols = cols;
	}
}

// =====================================================================
// SCUM_HGrid

SCUM_HGrid::SCUM_HGrid(SCUM_Container* parent, PyrObject* obj)
	: SCUM_Grid(parent, obj)
{ }

void SCUM_HGrid::getDimensions(size_t& rows, size_t& cols)
{
	const size_t n = children().size();
	const size_t w = wrap();

	if (n < w) {
		rows = cols = 0;
	} else {
		cols = w;
		rows = n / cols;
	}
}

SCUM_View* SCUM_HGrid::childAt(size_t row, size_t col)
{
	return children()[row*cols()+col];
}

// =====================================================================
// SCUM_VGrid

SCUM_VGrid::SCUM_VGrid(SCUM_Container* parent, PyrObject* obj)
	: SCUM_Grid(parent, obj)
{ }

void SCUM_VGrid::getDimensions(size_t& rows, size_t& cols)
{
	const size_t n = children().size();
	const size_t w = wrap();

	if (n < w) {
		rows = cols = 0;
	} else {
		rows = w;
		cols = n / rows;
	}
}

SCUM_View* SCUM_VGrid::childAt(size_t row, size_t col)
{
	return children()[col*rows()+row];
}

// EOF
