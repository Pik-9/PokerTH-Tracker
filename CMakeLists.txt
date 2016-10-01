project(PokerTH_tracker)
cmake_minimum_required(VERSION 2.8.11)

option(PKG_GEN "The package system to use (DEB, RPM, etc). Empty line: Don't create a pkg at all." "")

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

if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
  set(CMAKE_INSTALL_PREFIX "/usr")
endif("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${PTHT_ICON} DESTINATION share/PokerTH_tracker)
install(PROGRAMS ${PTHT_DSKT} DESTINATION /usr/share/applications)

if(NOT ("${PKG_GEN}" STREQUAL ""))
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
  set(CPACK_PACKAGE_VERSION "0.2.3.1")
  set(CPACK_PACKAGE_CONTACT "Daniel Steinhauer <d.steinhauer@mailbox.org>")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple poker tracker for PokerTH.")
  set(CPACK_PACKAGE_DESCRIPTION "This tracker analyses the players' behavior at the table and gives you hints about their weaknesses.")
  set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libgcc1, libgomp1, libstdc++6, libqt5core5a, libqt5gui5, libqt5xml5, libqt5sql5, libqt5sql5-sqlite, libqt5widgets5")
  set(CPACK_DEBIAN_PACKAGE_SUGGESTS "pokerth")
  
  set(CPACK_RPM_PACKAGE_LICENSE "GNU GENERAL PUBLIC LICENSE Version 3")
  set(CPACK_RPM_PACKAGE_REQUIRES "libc6, libgcc1, libgomp1, libstdc++6, libqt5core5a, libqt5gui5, libqt5xml5, libqt5sql5, libqt5sql5-sqlite, libqt5widgets5")
  
  set(CPACK_GENERATOR "${PKG_GEN}")
  include(CPack)
endif(NOT ("${PKG_GEN}" STREQUAL ""))