# -*- python -*- =======================================================
# FILE:         SConstruct
# CONTENTS:     scons build script for scum
# AUTHOR:       steve AT k-hornz DOT de
# ======================================================================

import glob
import os
import re
import struct
import tarfile
import time

# ======================================================================
# setup
# ======================================================================

EnsureSConsVersion(0, 96)
EnsurePythonVersion(2, 3)
SConsignFile()

# ======================================================================
# constants
# ======================================================================

PACKAGE = 'scum'
VERSION = '0.0.3'

PLATFORM = os.uname()[0].lower()
CPU = os.uname()[4].lower()
if struct.unpack("=l", struct.pack(">l", 1))[0] == 1:
    ENDIANNESS = 'big'
else:
    ENDIANNESS = 'little'

ANY_FILE_RE = re.compile('.*')
HELP_FILE_RE = re.compile('.*\.(rtf(d)?|sc)$')
SC_FILE_RE = re.compile('.*\.sc$')
SRC_FILE_RE = re.compile('.*\.(c(pp)|h)$')

# ======================================================================
# util
# ======================================================================

def CheckFLTK(context, version):
    context.Message('Checking for fltk ... ')
    ret, ver = context.TryAction('fltk-config --api-version')
    if ret and ver: ret = float(ver) >= float(version)
    context.Result(ret)
    return ret

def flatten_dir(dir):
    res = []
    for root, dirs, files in os.walk(dir):
        if 'CVS' in dirs: dirs.remove('CVS')
        for f in files:
            res.append(os.path.join(root, f))
    return res

def install_dir(env, src_dir, dst_dir, filter_re, strip_levels=0):
    nodes = []
    for root, dirs, files in os.walk(src_dir):
        src_paths = []
        dst_paths = []
        if '.svn' in dirs: dirs.remove('.svn')
        for d in dirs[:]:
            if filter_re.match(d):
                src_paths += flatten_dir(os.path.join(root, d))
                dirs.remove(d)
        for f in files:
            if filter_re.match(f):
                src_paths.append(os.path.join(root, f))
        dst_paths += map(
            lambda f:
            os.path.join(
            dst_dir,
            *f.split(os.path.sep)[strip_levels:]),
            src_paths)
        nodes += env.InstallAs(dst_paths, src_paths)
    return nodes

# ======================================================================
# command line options
# ======================================================================

opts = Options('scache.conf', ARGUMENTS)
opts.AddOptions(
    ('CUSTOMCCFLAGS', 'Custom c compiler flags', ''),
    ('CUSTOMCXXFLAGS', 'Custom c++ compiler flags', ''),
    BoolOption('DEBUG',
               'Build with debugging information', 0),
    PathOption('DESTDIR',
               'Intermediate installation prefix for packaging', '/'),
    PathOption('PREFIX',
               'Installation prefix', '/usr/local'),
    )

# ======================================================================
# basic environment
# ======================================================================

env = Environment(options = opts,
                  PACKAGE = PACKAGE,
                  VERSION = VERSION,
                  TARBALL = PACKAGE + '-' + VERSION + '.tbz2')
env.Append(
    CCFLAGS = env['CUSTOMCCFLAGS'],
    CXXFLAGS = env['CUSTOMCXXFLAGS'],
    CPPPATH = ['#oscpp'])

# ======================================================================
# configure
# ======================================================================

conf = Configure(env, custom_tests = { 'CheckFLTK' : CheckFLTK })
if not conf.CheckFLTK('1.1'):
    Exit(1)
env.ParseConfig('fltk-config --use-gl --cxxflags --ldflags')
env = conf.Finish()

# defines and compiler flags
env.Append(CPPDEFINES = ['_REENTRANT'])
if ENDIANNESS == 'big':
    env.Append(CPPDEFINES = ['OSC_HOST_BIG_ENDIAN'])
else:
    env.Append(CPPDEFINES = ['OSC_HOST_LITTLE_ENDIAN'])

# debugging flags
if env['DEBUG']:
    env.Append(CCFLAGS = '-g')
else:
    env.Append(CPPDEFINES = ['NDEBUG'])

# platform specific
if PLATFORM == 'darwin':
    env.Append(LIBS = 'pthread')
elif PLATFORM == 'linux':
    # needed for posix shm
    env.Append(LIBS = 'rt')
    if ENDIANNESS == 'little':
        env.Append(CCFLAGS = '-fno-strict-aliasing')

opts.Save('scache.conf', env)
Help(opts.GenerateHelpText(env))

# ======================================================================
# scum
# ======================================================================

scum_source_files = Split('''
src/SCUM_App.cpp
src/SCUM_Button.cpp
src/SCUM_Class.cpp
src/SCUM_Client.cpp
src/SCUM_Container.cpp
src/SCUM_FLUtil.cpp
src/SCUM_Font.cpp
src/SCUM_GC.cpp
src/SCUM_GL.cpp
src/SCUM_Graph.cpp
src/SCUM_Handle.cpp
src/SCUM_Image.cpp
src/SCUM_Menu.cpp
src/SCUM_Object.cpp
src/SCUM_Rendezvous.cpp
src/SCUM_ScrollView.cpp
src/SCUM_Shm.cpp
src/SCUM_Slider.cpp
src/SCUM_System.cpp
src/SCUM_Text.cpp
src/SCUM_View.cpp
src/SCUM_Window.cpp
src/st.c
''')

scum_header_files = Split('''
src/SCUM_App.hh
src/SCUM_Button.hh
src/SCUM_Canvas.hh
src/SCUM_Class.hh
src/SCUM_Client.hh
src/SCUM_Color.hh
src/SCUM_Container.hh
src/SCUM_Desktop.hh
src/SCUM_FLUtil.hh
src/SCUM_Font.hh
src/SCUM_GC.hh
src/SCUM_Geometry.hh
src/SCUM_GL.hh
src/SCUM_Graph.hh
src/SCUM_Handle.hh
src/SCUM_Image.hh
src/SCUM_Menu.hh
src/SCUM_NSKey.h
src/SCUM_Object.hh
src/SCUM_Rendezvous.hh
src/SCUM_ScrollView.hh
src/SCUM_Shm.hh
src/SCUM_Slider.hh
src/SCUM_Socket.hh
src/SCUM_System.hh
src/SCUM_Text.hh
src/SCUM_Timer.hh
src/SCUM_Util.hh
src/SCUM_View.hh
src/SCUM_Window.hh
src/st.h
''')

scum = env.Program('scum', scum_source_files)
env.Default(scum)

# ======================================================================
# installation directories
# ======================================================================

INSTALL_PREFIX = os.path.join('$DESTDIR', '$PREFIX')
DATA_DIR = os.path.join(INSTALL_PREFIX, 'share', 'SuperCollider')

ANY_FILE_RE = re.compile('.*')
SC_FILE_RE = re.compile('.*\.sc$')
HELP_FILE_RE = re.compile('.*\.(rtf|rtfd|sc)$')
SRC_FILE_RE = re.compile('.*\.(h|hh|c|cpp)$')

# ======================================================================
# installation
# ======================================================================

env.Alias('install-bin',
          env.Install(os.path.join(INSTALL_PREFIX, 'bin'), scum))

env.Alias('install-data', install_dir(
    env, 'help',
    os.path.join(DATA_DIR, 'Help', env['PACKAGE']), HELP_FILE_RE, 1))

env.Alias('install-data', install_dir(
    env, 'lib',
    os.path.join(DATA_DIR, 'Extensions', env['PACKAGE']), SC_FILE_RE, 1))

env.Alias('install', ['install-bin', 'install-data'])

# ======================================================================
# distribution
# ======================================================================

DIST_FILES = Split('''
AUTHORS
ChangeLog
COPYING
NEWS
README
SConstruct
TODO
''') + scum_source_files + scum_header_files + glob.glob('oscpp/*.hh')

DIST_DIRS = Split('''
help
lib
tools
''')

def dist_paths():
    paths = DIST_FILES[:]
    for base in DIST_DIRS:
        for root, dirs, files in os.walk(base):
            if '.svn' in dirs: dirs.remove('.svn')
            for path in files:
                paths.append(os.path.join(root, path))
    paths.sort()
    return paths

def build_tar(env, target, source):
    paths = dist_paths()
    tarfile_name = str(target[0])
    tar_name = os.path.splitext(os.path.basename(tarfile_name))[0]
    tar = tarfile.open(tarfile_name, "w:bz2")
    for path in paths:
        tar.add(path, os.path.join(tar_name, path))
    tar.close()

snapshot_tarball = PACKAGE + '-' + time.strftime("%Y%m%d", time.localtime()) + '.tbz2'
env.Alias('snapshot-dist', snapshot_tarball)
env.AlwaysBuild(env.Command(snapshot_tarball, None, build_tar))

release_tarball = PACKAGE + '-' + VERSION + '.tbz2'
env.Alias('release-dist', release_tarball)
env.AlwaysBuild(env.Command(release_tarball, None, build_tar))

env.Alias('dist', ['snapshot-dist'])

# ======================================================================
# cleanup
# ======================================================================

env.Clean('scrub',
          Split('config.log scache.conf .sconf_temp .sconsign.dblite'))

# ======================================================================
# EOF
