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

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5LinguistTools)
if(${Qt5LinguistTools_FOUND})
  configure_file(Trans_cmake.qrc Trans_cmake.qrc COPYONLY)
  qt5_add_translation(QM ${PTHT_LANG})
  qt5_add_resources(QM_RC ${CMAKE_CURRENT_BINARY_DIR}/Trans_cmake.qrc)
else(${Qt5LinguistTools_FOUND})
  set(QM_RC "")
endif(${Qt5LinguistTools_FOUND})

add_executable(${PROJECT_NAME} ${PTHT_SRC} ${QM_RC} Icons.qrc)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Sql)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${PTHT_ICON} DESTINATION share/PokerTH_tracker)
install(PROGRAMS ${PTHT_DSKT} DESTINATION /usr/share/applications)