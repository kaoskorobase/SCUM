MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS	= $(PACKAGE_CFLAGS)
bin_PROGRAMS	= sclangx

if HAVE_CAIRO
cairo_sources	= SCUM_FLCanvas.cpp
else
cairo_sources	=
endif # HAVE_CAIRO

noinst_HEADERS = \
SCUM_Button.hh \
SCUM_Canvas.hh \
SCUM_Color.hh \
SCUM_Container.hh \
SCUM_Desktop.hh \
SCUM_Editor.hh \
SCUM_FLApp.hh \
SCUM_FLUtil.hh \
SCUM_Font.hh \
SCUM_GC.hh \
SCUM_GL.hh \
SCUM_Geometry.hh \
SCUM_Graph.hh \
SCUM_Handle.hh \
SCUM_Image.hh \
SCUM_Invocation.hh \
SCUM_Lang.hh \
SCUM_Menu.hh \
SCUM_NSKey.h \
SCUM_Prim.hh \
SCUM_ScrollView.hh \
SCUM_Slider.hh \
SCUM_Symbol.hh \
SCUM_System.hh \
SCUM_Text.hh \
SCUM_Util.hh \
SCUM_View.hh \
SCUM_ViewFactory.hh \
SCUM_Window.hh

sclangx_SOURCES	= \
SCUM_Button.cpp \
SCUM_Canvas.cpp \
SCUM_Container.cpp \
SCUM_Desktop.cpp \
SCUM_DocumentPrim.cpp \
SCUM_Editor.cpp \
SCUM_FLApp.cpp \
SCUM_FLUtil.cpp \
SCUM_Font.cpp \
SCUM_GC.cpp \
SCUM_GL.cpp \
SCUM_Graph.cpp \
SCUM_Handle.cpp \
SCUM_Image.cpp \
SCUM_Invocation.cpp \
SCUM_Lang.cpp \
SCUM_Menu.cpp \
SCUM_Prim.cpp \
SCUM_ScrollView.cpp \
SCUM_Slider.cpp \
SCUM_Symbol.cpp \
SCUM_System.cpp \
SCUM_Text.cpp \
SCUM_View.cpp \
SCUM_ViewFactory.cpp \
SCUM_Window.cpp \
$(cairo_sources)

sclangx_LDADD	= $(PACKAGE_LIBS)

# EOF
