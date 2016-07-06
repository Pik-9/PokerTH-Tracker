project(PokerTH_tracker)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PTHT_SRC
  src/main.cpp
  src/mainwindow.cpp
  src/PlayerStat.cpp
  src/multiview.cpp
  src/analysis_widget.cpp
  src/notes_stars.cpp
  src/global.cpp
  src/worker.cpp
)
set(PTHT_LANG
  Lang/PokerTH_tracker_de.ts
)
set(PTHT_ICON
  Icons/PokerTH_Tracker.png
)
set(PTHT_DSKT PokerTH_tracker.desktop)

find_package(OpenMP)
if(${OPENMP_FOUND})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif(${OPENMP_FOUND})

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5LinguistTools)

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
configure_file(
  ${CMAKE_SOURCE_DIR}/misc/version.hpp.in
  ${CMAKE_BINARY_DIR}/version.hpp
)

if(${Qt5LinguistTools_FOUND})
  configure_file(Trans_cmake.qrc Trans_cmake.qrc COPYONLY)
  qt5_add_translation(QM ${PTHT_LANG})
  qt5_add_resources(QM_RC ${CMAKE_CURRENT_BINARY_DIR}/Trans_cmake.qrc)
else(${Qt5LinguistTools_FOUND})
  set(QM_RC "")
endif(${Qt5LinguistTools_FOUND})

add_executable(${PROJECT_NAME} ${PTHT_SRC} ${QM_RC} Icons.qrc)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Sql Qt5::Xml)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${PTHT_ICON} DESTINATION share/PokerTH_tracker)
install(PROGRAMS ${PTHT_DSKT} DESTINATION /usr/share/applications)