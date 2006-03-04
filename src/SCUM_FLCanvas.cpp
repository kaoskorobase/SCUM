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


#include "SCUM_Canvas.hh"
#include "SCUM_Util.hh"

#include <FL/Fl_Window.H>
#include <cairo.h>

#define SCUM_CANVAS_BACKEND_NONE		-1
#define SCUM_CANVAS_BACKEND_XLIB		0
#define SCUM_CANVAS_BACKEND_GLITZ		1

#define SCUM_GLITZ_BACKEND_NONE			-1
#define SCUM_GLITZ_BACKEND_GLX			0
#define SCUM_GLITZ_BACKEND_AGL			1
#define SCUM_GLITZ_BACKEND_WGL			2

#if HAVE_CONFIG_H
# include "config.h"
#endif

#if HAVE_CAIRO
# if HAVE_GLITZ
#  define SCUM_CANVAS_BACKEND SCUM_CANVAS_BACKEND_GLITZ
#  define SCUM_GLITZ_BACKEND  SCUM_GLITZ_BACKEND_GLX
# else
#  define SCUM_CANVAS_BACKEND SCUM_CANVAS_BACKEND_XLIB
# endif
#else
# define SCUM_CANVAS_BACKEND SCUM_CANVAS_BACKEND_NONE
#endif

#if SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_XLIB
# ifdef CAIRO_HAS_XLIB_SURFACE
#  include <FL/fl_draw.H>
#  include <FL/x.H>
# else
#  undef SCUM_CANVAS_BACKEND
#  define SCUM_CANVAS_BACKEND SCUM_CANVAS_BACKEND_NONE
# endif
#endif

#if SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_GLITZ
# ifdef CAIRO_HAS_GL_SURFACE
#  include <glitz.h>
#  if SCUM_GLITZ_BACKEND == SCUM_GLITZ_BACKEND_GLX
#   include <FL/x.H>
#   include <glitz-glx.h>
#  endif
#  if SCUM_GLITZ_BACKEND == SCUM_GLITZ_BACKEND_AGL
#   include <FL/mac.H>
#   include <glitz-agl.h>
#  endif
# else
#  undef  SCUM_CANVAS_BACKEND
#  define SCUM_CANVAS_BACKEND SCUM_CANVAS_BACKEND_NONE
# endif
#endif

#if SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_NONE
SCUM::CanvasHandle* SCUM::CanvasHandle::create(SCUM::WindowHandle* window, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer)
{
  return 0;
}
#else
namespace SCUM
{
  class FLCanvasHandle : protected Fl_Window, public CanvasHandle
  {
  public:
    FLCanvasHandle(Fl_Group* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer);
    virtual ~FLCanvasHandle();

    virtual SCUM_Rect bounds();
    virtual void setBounds(const SCUM_Rect& bounds);
    virtual void refresh();
    virtual void show();
    virtual void hide();

  protected:
    virtual void resize(int, int, int, int);
    virtual void draw();
    virtual int handle(int);

    virtual void initCanvas();
    virtual void paint();

  protected:
    SCUM_CanvasView*		m_view;
    cairo_t*				m_cr;
    bool					m_valid;
    bool					m_doubleBuffered;
  };

#if SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_XLIB
#   define SCUM_CANVAS_CLASS FLCanvasHandle_XLib
  class FLCanvasHandle_XLib : public FLCanvasHandle
  {
  public:
    FLCanvasHandle_XLib(Fl_Group* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer);

    virtual void show();

  protected:
    virtual void paint();
  };
#endif

#if SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_GLITZ
  class FLCanvasHandle_Glitz : public FLCanvasHandle
  {
  public:
    FLCanvasHandle_Glitz(Fl_Group* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer);
    virtual ~FLCanvasHandle_Glitz();

    virtual void show();
    virtual void hide();

  protected:
    virtual glitz_surface_t* createSurface(unsigned long option_mask,
					   glitz_format_name_t format_name) = 0;
    virtual void initCanvas();
    virtual void paint();

  protected:
    glitz_surface_t* m_glitz;
  };

# if SCUM_GLITZ_BACKEND == SCUM_GLITZ_BACKEND_GLX
#   define SCUM_CANVAS_CLASS FLCanvasHandle_GLX
  class FLCanvasHandle_GLX : public FLCanvasHandle_Glitz
  {
  public:
    FLCanvasHandle_GLX(Fl_Group* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer);
    virtual ~FLCanvasHandle_GLX();

    virtual void hide();

  protected:
    virtual glitz_surface_t* createSurface(unsigned long option_mask,
					   glitz_format_name_t format_name);

  private:
    Colormap m_colormap;
  };
#endif // SCUM_GLITZ_BACKEND == SCUM_GLITZ_BACKEND_GLX

# if SCUM_GLITZ_BACKEND == SCUM_GLITZ_BACKEND_AGL
#   define SCUM_CANVAS_CLASS FLCanvasHandle_AGL
  class FLCanvasHandle_AGL : public FLCanvasHandle_Glitz
  {
  public:
    FLCanvasHandle_AGL(Fl_Group* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer);

  protected:
    virtual glitz_surface_t* createSurface(unsigned long option_mask,
					   glitz_format_name_t format_name);
  };
# endif // SCUM_GLITZ_BACKEND == SCUM_GLITZ_BACKEND_AGL
#endif // SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_GLITZ
};

// =====================================================================
// SCUM::FLCanvasHandle

SCUM::FLCanvasHandle::FLCanvasHandle(Fl_Group* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer)
  : Fl_Window(0, 0, 10, 10),
    m_view(view),
    m_cr(cr),
    m_valid(false),
    m_doubleBuffered(doubleBuffer)
{
  SCUM_ASSERT_PTR(parent);
  SCUM_ASSERT_PTR(m_view);
  SCUM_ASSERT_PTR(m_cr);
  end();
  parent->add(this);
}

SCUM::FLCanvasHandle::~FLCanvasHandle()
{
  parent()->remove(this);
}

SCUM_Rect SCUM::FLCanvasHandle::bounds()
{
  return SCUM_Rect(x(), y(), w(), h());
}

void SCUM::FLCanvasHandle::setBounds(const SCUM_Rect& bounds)
{
  resize((int)bounds.x, (int)bounds.y, (int)bounds.w, (int)bounds.h);
}

void SCUM::FLCanvasHandle::initCanvas()
{
  m_view->initCanvas();
}

void SCUM::FLCanvasHandle::paint()
{
  m_view->paint();
}

void SCUM::FLCanvasHandle::resize(int X, int Y, int W, int H)
{
  m_valid = m_valid && (W == w()) && (H == h());
  Fl_Window::resize(X, Y, W, H);
}

void SCUM::FLCanvasHandle::show()
{
  Fl_Window::show();
}

void SCUM::FLCanvasHandle::hide()
{
  Fl_Window::hide();
  m_valid = false;
}

void SCUM::FLCanvasHandle::draw()
{
  if (!m_valid) {
    initCanvas();
    m_valid = true;
  }

  paint();
}

int SCUM::FLCanvasHandle::handle(int evt)
{
  switch (evt)
    {
    case FL_SHOW:
    case FL_HIDE:
      return Fl_Window::handle(evt);
    }
  return 0;
}

void SCUM::FLCanvasHandle::refresh()
{
  this->damage(FL_DAMAGE_ALL);
}

// =====================================================================
// SCUM::FLCanvasHandle_XLib

#if SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_XLIB
SCUM::FLCanvasHandle_XLib::FLCanvasHandle_XLib(Fl_Group* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer)
  : FLCanvasHandle(parent, view, cr, doubleBuffer)
{
}

void SCUM::FLCanvasHandle_XLib::show()
{
  bool shown = this->shown();
  FLCanvasHandle::show();
  if (!shown) {
    cairo_set_target_drawable(m_cr, fl_display, fl_xid(this));
  }
}

void SCUM::FLCanvasHandle_XLib::paint()
{
  // TODO: double buffering
  FLCanvasHandle::paint();
}
#endif // SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_XLIB

// =====================================================================
// SCUM::FLCanvasHandle_Glitz

#if SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_GLITZ
SCUM::FLCanvasHandle_Glitz::FLCanvasHandle_Glitz(Fl_Group* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer)
  : FLCanvasHandle(parent, view, cr, doubleBuffer),
    m_glitz(0)
{
}

SCUM::FLCanvasHandle_Glitz::~FLCanvasHandle_Glitz()
{
  if (m_glitz) {
    glitz_surface_destroy(m_glitz);
  }
}

void SCUM::FLCanvasHandle_Glitz::show()
{
  if (!shown()) {
    SCUM_ASSERT(m_glitz == 0);
    m_glitz = createSurface(
			    GLITZ_FORMAT_DRAW_ONSCREEN_MASK |
			    (m_doubleBuffered ? GLITZ_FORMAT_DOUBLEBUFFER_MASK : 0),
			    GLITZ_STANDARD_ARGB32
			    );
    CheckAlloc(m_glitz);
    cairo_set_target_gl(m_cr, m_glitz);
    m_valid = false;
  } else {
    FLCanvasHandle::show();
  }
}

void SCUM::FLCanvasHandle_Glitz::hide()
{
  if (m_glitz) {
    glitz_surface_destroy(m_glitz);
    m_glitz = 0;
  }
  FLCanvasHandle::hide();
}

void SCUM::FLCanvasHandle_Glitz::initCanvas()
{
  if (m_glitz) {
    glitz_surface_update_size(m_glitz);
  }
  FLCanvasHandle::initCanvas();
}

void SCUM::FLCanvasHandle_Glitz::paint()
{
  FLCanvasHandle::paint();
  SCUM_ASSERT_PTR(m_glitz);
  glitz_surface_swap_buffers(m_glitz);
  XSync(fl_display, 0);
}

// =====================================================================
// SCUM::FLCanvasHandle_GLX

# if SCUM_GLITZ_BACKEND == SCUM_GLITZ_BACKEND_GLX
SCUM::FLCanvasHandle_GLX::FLCanvasHandle_GLX(Fl_Group* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer)
  : FLCanvasHandle_Glitz(parent, view, cr, doubleBuffer),
    m_colormap(0)
{
}

SCUM::FLCanvasHandle_GLX::~FLCanvasHandle_GLX()
{
  if (m_colormap) XFreeColormap(fl_display, m_colormap);
}

void SCUM::FLCanvasHandle_GLX::hide()
{
  FLCanvasHandle_Glitz::hide();
  if (m_colormap) {
    XFreeColormap(fl_display, m_colormap);
    m_colormap = 0;
  }
}

glitz_surface_t* SCUM::FLCanvasHandle_GLX::createSurface(unsigned long option_mask,
							 glitz_format_name_t format_name)
{
  glitz_format_t* format;

  fl_open_display();

  format = glitz_glx_find_standard_format(
					  fl_display, fl_screen,
					  /* option_mask, */ format_name
					  );
  if (!format) return 0;

  XVisualInfo* vis = glitz_glx_get_visual_info_from_format(
							   fl_display, fl_screen, format
							   );
  m_colormap = XCreateColormap(
			       fl_display, RootWindow(fl_display, fl_screen), vis->visual, AllocNone
			       );
  Fl_X::make_xid(this, vis, m_colormap);

  return glitz_glx_surface_create_for_window(
					     fl_display, fl_screen,
					     format,
					     fl_xid(this)
					     );
}
# endif // SCUM_GLITZ_BACKEND == SCUM_GLITZ_BACKEND_GLX

// =====================================================================
// SCUM::FLCanvasHandle_AGL

# if SCUM_GLITZ_BACKEND == SCUM_GLITZ_BACKEND_AGL
SCUM::FLCanvasHandle_AGL::FLCanvasHandle_AGL(Fl_Group* parent, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer)
  : FLCanvasHandle_Glitz(parent, view, cr, doubleBuffer)
{
}

glitz_surface_t* SCUM::FLCanvasHandle_AGL::createSurface(unsigned long option_mask,
							 glitz_format_name_t format_name)
{
  glitz_format_t* format;

  Fl_Window::show();

  format = glitz_glx_find_standard_format(
					  option_mask, format_name
					  );
  if (!format) return 0;

  return glitz_agl_surface_create_for_window(
					     format,
					     fl_xid(this)
					     );
}
# endif // SCUM_GLITZ_BACKEND == SCUM_GLITZ_BACKEND_AGL
#endif // SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_GLITZ

// =====================================================================
// SCUM::CanvasHandle::create

SCUM::CanvasHandle* SCUM::CanvasHandle::create(SCUM::WindowHandle* window, SCUM_CanvasView* view, cairo_t* cr, bool doubleBuffer)
{
  Fl_Group* parent = dynamic_cast<Fl_Group*>(window);
  if (parent) return new SCUM_CANVAS_CLASS(parent, view, cr, doubleBuffer);
  return 0;
}
#endif // SCUM_CANVAS_BACKEND == SCUM_CANVAS_BACKEND_NONE

// EOF
