######################################################################
# Automatically generated by qmake (2.00a) Wed Jun 22 16:57:14 2005
######################################################################

TEMPLATE = lib
TARGET = z

include( ../../../libs/libsworkbench.pri )

# Change build destdir
DESTDIR = $${SOURCES_CONTRIBS_PATH}/quazip
# redefine the target because included .pri modifies the target
TARGET = z

CONFIG *= staticlib

# CONFIG += qt warn_on staticlib
# QT -= gui
DEPENDPATH *= .
INCLUDEPATH *= .

# Input
HEADERS += crc32.h \
           deflate.h \
           inffast.h \
           inffixed.h \
           inflate.h \
           inftrees.h \
           trees.h \
           zconf.h \
           zconf.in.h \
           zlib.h \
           zutil.h

SOURCES += adler32.c  \
           crc32.c \
           example.c \
           infback.c \
           inflate.c \
           minigzip.c \
           uncompr.c \
           compress.c \
           deflate.c \
           gzio.c\
           inffast.c \
           inftrees.c \
           trees.c \
           zutil.c

