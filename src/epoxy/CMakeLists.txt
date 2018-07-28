cmake_minimum_required(VERSION 3.0)

find_package(OpenGL)
find_package(X11)

set (GLU_NAMES ${GLU_NAMES} GLU MesaGLU)
find_library (GLU_LIBRARY NAMES ${GLU_NAMES} PATHS /usr/include/GL /usr/lib/x86_64-linux-gnu)

set (EGL_NAMES ${EGL_NAMES} egl EGL libEGL)
find_library (EGL_LIBRARY NAMES ${EGL_NAMES} PATHS /usr/include/GL /usr/lib/x86_64-linux-gnu)

set (EPOXY_APIS gl egl wgl glx)
set (EPOXY_GENERATED_FILES "")
find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
    foreach (EPOXY_API ${EPOXY_APIS})
        execute_process (
          COMMAND "${PYTHON_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/gen_dispatch.py" --srcdir "${CMAKE_CURRENT_SOURCE_DIR}/src/" --includedir "${CMAKE_CURRENT_SOURCE_DIR}/include/epoxy/"
            "${CMAKE_CURRENT_SOURCE_DIR}/registry/${EPOXY_API}.xml")
    endforeach ()
else()
    message(FATAL_ERROR "System Python not found! install the library, then try again.")
endif()

if(EGL_LIBRARY)
	set(ENABLE_EGL "1")
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.in.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h" @ONLY)
message("-- Generated Epoxy Files.")

set(EPOXY_PLATFORM_SPECIFIC
"${CMAKE_CURRENT_SOURCE_DIR}/src/dispatch_common.c" 
"${CMAKE_CURRENT_SOURCE_DIR}/src/dispatch_common.h"
"${CMAKE_CURRENT_SOURCE_DIR}/src/gl_generated_dispatch.c" )
set(EPOXY_INCLUDED_LIBS ${OPENGL_gl_LIBARY})

if(OPENGL_FOUND)
	if(WIN32)	
		message("-- Windows found, Using WGL")
		set(EPOXY_PLATFORM_SPECIFIC ${EPOXY_PLATFORM_SPECIFIC}
		"${CMAKE_CURRENT_SOURCE_DIR}/src/wgl_generated_dispatch.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/dispatch_wgl.c")
	endif()
	
	if(GLU_LIBRARY)
		if(UNIX AND NOT APPLE)
			message("-- GLU and Linux found, Using GLU")
			set(EPOXY_INCLUDED_LIBS ${EPOXY_INCLUDED_LIBS} ${GLU_LIBRARY})
		endif()
	endif()
	
	if(EGL_LIBRARY)
		message("-- epoxy Found EGL")
		set(EPOXY_PLATFORM_SPECIFIC ${EPOXY_PLATFORM_SPECIFIC}
		"${CMAKE_CURRENT_SOURCE_DIR}/src/egl_generated_dispatch.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/dispatch_egl.c")
		set(EPOXY_INCLUDED_LIBS ${EPOXY_INCLUDED_LIBS} ${EGL_LIBRARY})
	endif()

	if(X11_FOUND)
		message("-- epoxy Found X11")
		set(EPOXY_PLATFORM_SPECIFIC ${EPOXY_PLATFORM_SPECIFIC}
		"${CMAKE_CURRENT_SOURCE_DIR}/src/glx_generated_dispatch.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/dispatch_glx.c")
		set(EPOXY_INCLUDED_LIBS ${EPOXY_INCLUDED_LIBS} ${X11_LIBRARIES})
	endif()
else()
	message(FATAL_ERROR "-- No OpenGL Found.")
endif()

source_group("" FILES ${EPOXY_PLATFORM_SPECIFIC})

add_library("epoxy" STATIC ${EPOXY_PLATFORM_SPECIFIC})

TARGET_LINK_LIBRARIES("epoxy" ${EPOXY_INCLUDED_LIBS})

STRING(REPLACE "/O2" "/Od" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})

if(UNIX AND NOT APPLE)
	target_compile_options(
	"epoxy" PRIVATE 
		"-Wl,-Bsymbolic" "-Wl,-z,relro" "-Wl,-z,now" "-std=gnu11"
    )
endif()

set_target_properties("epoxy" PROPERTIES LINKER_LANGUAGE C)

target_include_directories("epoxy" PUBLIC "include")
