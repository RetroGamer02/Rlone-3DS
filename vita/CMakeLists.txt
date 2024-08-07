cmake_minimum_required(VERSION 3.2)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(Rlone)
include("${VITASDK}/share/vita.cmake" REQUIRED)

find_package(SDL2 REQUIRED)

set(VITA_APP_NAME "Rlone")
set(VITA_TITLEID  "VSDK00017")
set(VITA_VERSION  "01.00")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c99 -pedantic -DVSN=\\\"1.01\\\" -DVITABUILD -DNODEBUG")

include_directories(
	../include
	../iniparser/src
)

file(GLOB RLONE_SRC	../src/*.c)
file(GLOB MAP_SRC	../src/map/*.c)
file(GLOB OBJ_SRC	../src/object/*.c)
file(GLOB REND_SRC	../src/renderer/*.c)
file(GLOB INI_SRC	../iniparser/src/*.c)

# Manually add the place SDL2_image and SDL2_mixer resides. Nice cmakefiles
# guys :)
#find_package(SDL2_image REQUIRED)
#find_package(SDL2_mixer REQUIRED)
link_directories(
	${VITASDK}/arm-vita-eabi/lib
)

add_executable(${PROJECT_NAME}
	${INI_SRC}
	${MAP_SRC}
	${OBJ_SRC}
	${REND_SRC}
	${RLONE_SRC}
)

target_link_libraries(${PROJECT_NAME}
	# PS Vita
	-lSceLibKernel_stub

	# SDL2
	-lSDL2_image
	-lSDL2_mixer
	SDL2::SDL2

	# SDL2_image
	-lpng
	-ljpeg
	-lwebp
	-lpthread
	-lz

	# SDL2_mixer
	-lmpg123
	-lFLAC
	-lvorbisfile
	-lvorbisenc
	-lvorbis
	-logg
	-lmodplug
	-lstdc++
	-lmikmod
)

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
	VERSION ${VITA_VERSION}
	NAME ${VITA_APP_NAME}
	FILE sce_sys/icon0.png sce_sys/icon0.png
	FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
	FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
	FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
	FILE ../res/aud/bgm.wav aud/bgm.wav
	FILE ../res/aud/menu.wav aud/menu.wav
	FILE ../res/aud/death.wav aud/death.wav
	FILE ../res/tex/font.tga tex/font.tga
	FILE ../res/tex/icon.tga tex/icon.tga
	FILE ../res/tex/title_bg.tga tex/title_bg.tga
	FILE autorun.ini autorun.ini
)
