add_subdirectory(epoxy)

add_executable("CoffeeToCPP" "main.cpp")
target_link_libraries("CoffeeToCPP" "epoxy")
