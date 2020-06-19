cmake_minimum_required(VERSION 3.16)
project(carp C)

set(CMAKE_C_STANDARD 99)

add_executable(carp main.c functions.h functions.c heuristic.c heutistic.h arrayoperations.c simulator.c readmap.c dheuristic.c dheuristic.h TSA/TSA.c TSA/TSA.h TSA/tsals.c LMA/LMA.c LMA/LMA.h MAENS/MAENS.c MAENS/MAENS.h MAENS/operators.c LMA/LMAop.c)