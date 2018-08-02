add_subdirectory(epoxy)

set(CtC_Included_Files "main.cpp")

if(WIN32)
    set(CtC_Included_Files ${CtC_Included_Files} "wgl.cpp" "wgl.hpp")
else()
    set(CtC_Included_Files ${CtC_Included_Files} "glx.cpp" "glx.hpp")
endif()


add_executable("CoffeeToCPP" ${CtC_Included_Files})
target_link_libraries("CoffeeToCPP" "epoxy" ${CMAKE_DL_LIBS})

