MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS	= $(PACKAGE_CFLAGS) -I../oscio/
SCUM_APP = scum
bin_PROGRAMS = $(SCUM_APP)

# if HAVE_CAIRO
# cairo_sources	= SCUM_FLCanvas.cpp
# else
# cairo_sources	=
# endif # HAVE_CAIRO

noinst_HEADERS = \
st.h \
SCUM_App.hh \
SCUM_Button.hh \
SCUM_Class.hh \
SCUM_Client.hh \
SCUM_Color.hh \
SCUM_Container.hh \
SCUM_FLUtil.hh \
SCUM_Font.hh \
SCUM_GC.hh \
SCUM_GL.hh \
SCUM_Geometry.hh \
SCUM_Graph.hh \
SCUM_Handle.hh \
SCUM_Invocation.hh \
SCUM_Menu.hh \
SCUM_NSKey.h \
SCUM_Object.hh \
SCUM_ScrollView.hh \
SCUM_Shm.hh \
SCUM_Slider.hh \
SCUM_System.hh \
SCUM_Text.hh \
SCUM_Util.hh \
SCUM_View.hh \
SCUM_Window.hh

scum_SOURCES	= \
st.c \
SCUM_App.cpp \
SCUM_Button.cpp \
SCUM_Class.cpp \
SCUM_Client.cpp \
SCUM_Container.cpp \
SCUM_FLUtil.cpp \
SCUM_Font.cpp \
SCUM_GC.cpp \
SCUM_GL.cpp \
SCUM_Graph.cpp \
SCUM_Handle.cpp \
SCUM_Invocation.cpp \
SCUM_Menu.cpp \
SCUM_Object.cpp \
SCUM_ScrollView.cpp \
SCUM_Shm.cpp \
SCUM_Slider.cpp \
SCUM_System.cpp \
SCUM_Text.cpp \
SCUM_View.cpp \
SCUM_Window.cpp

scum_LDADD	= $(PACKAGE_LIBS)

install-exec-local:
	$(FLTK_CONFIG) --post $(bindir)/$(SCUM_APP)

# EOF
