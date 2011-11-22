find_package(Qt4 REQUIRED)

set(wte_SOURCES
  main.cpp
  mainwindow.cpp
  rendertriangulation.cpp
  triangulatedmap.cpp)

set(wte_HEADERS
  mainwindow.h
  rendertriangulation.h
  triangulatedmap.h)

qt4_wrap_cpp(wte_HEADERS_MOC ${wte_HEADERS})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(wte ${wte_SOURCES}
    ${wte_HEADERS_MOC})
target_link_libraries(wte ${QT_LIBRARIES})
