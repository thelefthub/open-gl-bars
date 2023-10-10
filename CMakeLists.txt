cmake_minimum_required(VERSION 3.0.0)
project(open_gl_bars)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(open_gl_bars 
  src/main.cpp
  src/bar.h
)

target_link_libraries(open_gl_bars 
  OpenGL::GL
  GLUT::GLUT
  OpenGL::GLU
)
