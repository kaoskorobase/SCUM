/*  -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4 -*-
    vi: et sta sw=4:

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


#ifndef SCUM_GRAPH_HH_INCLUDED
#define SCUM_GRAPH_HH_INCLUDED

#include "SCUM_GL.hh"
#include "SCUM_Shm.hh"

#include <stdint.h>
#include <vector>

// =====================================================================
// SCUM_Scope

struct SCUM_SndBuf
{
    double	samplerate;
    uint32_t	channels;
    uint32_t	samples;
    float	data[1];
};

class SCUM_Scope : public SCUM_GLView
{
public:
    enum
        {
            kStyleSeparate,
            kStyleOverlay,
            kStyleXY
        };

    SCUM_Scope(SCUM_Class* klass, SCUM_Client* client, int oid, SCUM_ArgStream& args);
    ~SCUM_Scope();

    // 	virtual void scrollWheel(int, const SCUM_Point&, const SCUM_Point&);

    // 	virtual void refresh();
    // 	virtual void initGL();
    // 	virtual void resizeGL(int w, int h);
    // 	virtual void drawGL();
    // 	virtual void draw(const SCUM_Rect& damage);
    virtual void initGL();
    virtual void drawGL();

    virtual void setProperty(const char* key, SCUM_ArgStream& args);
    //virtual void getProperty(const PyrSymbol* key, PyrSlot* slot);

    virtual void animate();

protected:
    // 	virtual void boundsChanged(const SCUM_Rect& bounds);
    // 	void updateContentSize();

    void drawSeparate();
    void drawOverlay();
    void drawXY();
    void drawInvalid();

private:
    SCUM_SharedMemory		m_shm;
    SCUM_SndBuf*		m_buf;
    SCUM_Point			m_zoom;
    std::vector<SCUM_Color>	m_waveColors;
    uint8_t			m_style;
};

#endif // SCUM_GRAPH_HH_INCLUDED
