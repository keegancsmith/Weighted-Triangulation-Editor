find_package(Qt4 REQUIRED)

set(gui_SOURCES main.cpp mainwindow.cpp rendertriangulation.cpp tmap.cpp)
set(gui_HEADERS mainwindow.h rendertriangulation.h tmap.h)
qt4_wrap_cpp(gui_HEADERS_MOC ${gui_HEADERS})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(gui ${gui_SOURCES}
    ${gui_HEADERS_MOC})
target_link_libraries(gui ${QT_LIBRARIES})
