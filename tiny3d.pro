TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    tiny_system.cpp \
    tiny_math.cpp \
    tiny_draw.cpp \
    tiny_image.cpp \
    tiny_texture.cpp \
    tiny_structs.cpp \
    tests/test_math.cpp \
    tests/test_triangle.cpp \
    tests/test_model.cpp \
    tests/test_aux.cpp

LIBS += \
    -lSDL \
    -lSDLmain

HEADERS += \
    tiny_texture.h \
    tiny_image.h \
    tiny_math.h \
    tiny_draw.h \
    tiny_system.h \
    tiny3d.h \
    tiny_structs.h \
    tests/test_math.h \
    tests/test_triangle.h \
    tests/test_model.h \
    tests/test_aux.h

debug {
  DEFINES += TINY3D_DEBUG
}
release {
  DEFINES += TINY3D_RELEASE
}
