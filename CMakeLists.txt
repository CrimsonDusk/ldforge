######################################################################
# CMake file for LDForge.
# The original QMake file was created on Sat Sep 22 17:29:49 2012
######################################################################

project (ldforge)
add_subdirectory (updaterevision)
cmake_minimum_required (VERSION 2.6)
find_package (Qt4 REQUIRED)
find_package (OpenGL REQUIRED)

option (TRANSPARENT_DIRECT_COLORS "Enables non-standard transparent direct colors" OFF)

get_target_property (UPDATEREVISION_EXE updaterevision LOCATION)

add_custom_target (revision_check ALL
    COMMAND ${UPDATEREVISION_EXE} gitinfo.h
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS updaterevision)

include_directories (${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set (LDFORGE_SOURCES
	src/actions.cc
	src/actionsEdit.cc
	src/addObjectDialog.cc
	src/basics.cc
	src/colors.cc
	src/colorSelector.cc
	src/configuration.cc
	src/configDialog.cc
	src/crashCatcher.cc
	src/dialogs.cc
	src/documentation.cc
	src/editHistory.cc
	src/extPrograms.cc
	src/glRenderer.cc
	src/glCompiler.cc
	src/ldConfig.cc
	src/ldDocument.cc
	src/ldObject.cc
	src/main.cc
	src/mainWindow.cc
	src/messageLog.cc
	src/miscallenous.cc
	src/partDownloader.cc
	src/primitives.cc
	src/radioGroup.cc
	src/version.cc
)

set (LDFORGE_HEADERS
	src/macros.h
	src/crashCatcher.h
	src/colors.h
	src/misc/ringFinder.h
	src/ldDocument.h
	src/addObjectDialog.h
	src/ldConfig.h
	src/partDownloader.h
	src/ldObject.h
	src/primitives.h
	src/miscallenous.h
	src/messageLog.h
	src/dialogs.h
	src/radioGroup.h
	src/documentation.h
	src/main.h
	src/basics.h
	src/colorSelector.h
	src/configDialog.h
	src/glRenderer.h
	src/glCompiler.h
	src/configuration.h
	src/mainWindow.h
	src/editHistory.h
	src/format.h
)

set (LDFORGE_FORMS
	ui/about.ui
	ui/addhistoryline.ui
	ui/bombbox.ui
	ui/colorsel.ui
	ui/config.ui
	ui/coverer.ui
	ui/downloadfrom.ui
	ui/edger2.ui
	ui/editraw.ui
	ui/extprogpath.ui
	ui/flip.ui
	ui/intersector.ui
	ui/isecalc.ui
	ui/ldforge.ui
	ui/ldrawpath.ui
	ui/makeprim.ui
	ui/newpart.ui
	ui/openprogress.ui
	ui/overlay.ui
	ui/rectifier.ui
	ui/replcoords.ui
	ui/rotpoint.ui
	ui/ytruder.ui
)

set (LDFORGE_RESOURCES ldforge.qrc)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -W -Wall")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lGLU")

if (TRANSPARENT_DIRECT_COLORS)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTRANSPARENT_DIRECT_COLORS")
endif()

# With clang, we need to set -Wno-deprecated since Qt headers seem to use the
# register keyword which clang doesn't seem to like. It also appears that Qt
# doesn't set Q_COMPILER_INITIALIZER_LISTS when compiling with clang? What's
# up with that?
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -DQ_COMPILER_INITIALIZER_LISTS")
endif()

include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_BINARY_DIR}/src")
include_directories ("${PROJECT_BINARY_DIR}/src/misc")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif()

qt4_wrap_cpp (LDFORGE_MOC ${LDFORGE_HEADERS})
qt4_wrap_ui (LDFORGE_FORMS_HEADERS ${LDFORGE_FORMS})
qt4_add_resources (LDFORGE_RCC ${LDFORGE_RESOURCES})

add_executable (ldforge
	${LDFORGE_SOURCES}
	${LDFORGE_RCC}
	${LDFORGE_FORMS_HEADERS}
	${LDFORGE_MOC}
)

target_link_libraries (ldforge
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTOPENGL_LIBRARY}
	${OPENGL_LIBRARIES}
)

add_dependencies (ldforge revision_check)
install (TARGETS ldforge RUNTIME DESTINATION bin)
