PROTOC_PATH = $$PWD/protoc

INCLUDEPATH += \
    $$PROTOC_PATH

HEADERS += \
    $$files($$PROTOC_PATH/*.h)

SOURCES += \
    $$files($$PROTOC_PATH/*.cc)

CONFIG += link_pkgconfig
PKGCONFIG += protobuf