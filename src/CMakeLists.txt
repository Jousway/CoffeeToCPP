add_subdirectory(epoxy)

file( GLOB APP_CPP *.cpp )
file( GLOB APP_HPP *.hpp )

add_executable("CoffeeToCPP" ${APP_CPP} ${APP_HPP})
target_link_libraries("CoffeeToCPP" "epoxy")