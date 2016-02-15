project(PokerTH_tracker)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PTHT_SRC
  main.cpp
  mainwindow.cpp
  PlayerStat.cpp
  multiview.cpp
  analysis_widget.cpp
  global.cpp
)
set(PTHT_LANG Lang/PokerTH_tracker_de.ts)
set(PTHT_ICON Icons/PokerTH_Tracker.png)
set(PTHT_DSKT PokerTH_tracker.desktop)

find_package(Qt5Widgets)
find_package(Qt5Sql)
if(${Qt5Widgets_FOUND})
  find_package(Qt5LinguistTools)
  if(${Qt5LinguistTools_FOUND})
    qt5_add_translation(QM ${PTHT_LANG})
  else(${Qt5LinguistTools_FOUND})
    set(QM "")
  endif(${Qt5LinguistTools_FOUND})
  add_executable(${PROJECT_NAME} ${PTHT_SRC} ${QM})
  target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Sql)
else(${Qt5Widgets_FOUND})
  find_package(Qt4 REQUIRED)
  qt4_add_translation(QM ${PTHT_LANG})
  add_executable(${PROJECT_NAME} ${PTHT_SRC} ${QM})
  include(${QT_USE_FILE})
  target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})
endif(${Qt5Widgets_FOUND})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${PTHT_ICON} DESTINATION share/PokerTH_tracker)
install(PROGRAMS ${PTHT_DSKT} DESTINATION /usr/share/applications)
install(FILES ${QM} DESTINATION share/PokerTH_tracker)