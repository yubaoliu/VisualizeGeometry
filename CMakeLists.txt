cmake_minimum_required(VERSION 3.10)

# set the project name
project(useEigen)

find_package(OpenCV 3)
find_package(Eigen3 REQUIRED)

find_package(Pangolin)
include_directories(${Pangolin_INCLUDE_DIRS})
# header files include_directories(include ${EIGEN3_INCLUDE_DIR})

# add the executable
add_executable(main main.cpp)
target_link_libraries(main 
    Eigen3::Eigen
    ${Pangolin_LIBRARIES}
    )

add_executable(rotation_from_euler rotation_from_euler.cpp)
target_link_libraries( rotation_from_euler
    Eigen3::Eigen
    )
