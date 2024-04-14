
# указываем минимальную версию CMake
cmake_minimum_required(VERSION 3.10) 

# имя вашего проекта
project(Gravity)                    

# исполняющие файлы и исходные коды
add_executable(${PROJECT_NAME} test.cpp)       

# указываем CMake где искать модули SFML
set(SFML_DIR "./SFML-2.6.1/lib/cmake/SFML")       

find_package(SFML 2.6 COMPONENTS graphics audio REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio)