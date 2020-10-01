project (jgen)
cmake_minimum_required (VERSION 2.6)

#generator sources
set (SRCS
  src/TDecayPolarized.cxx
  src/TGenUPCJpsiFlat.cxx
  src/TGenPsi2S.cxx
)

#binary generator executables
set (BIN boxgen fdgen)

#generator library
set (LIB libgen)

#C++ flags
set (CMAKE_CXX_COMPILER /usr/bin/g++)
set (CMAKE_CXX_FLAGS "-Wall -g")
include_directories (include)

#ROOT flags
execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_FLAGS_CMD)
string(REPLACE "\n" "" ROOT_FLAGS "${ROOT_FLAGS_CMD}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_FLAGS}")

#ROOT include
execute_process(COMMAND root-config --libdir OUTPUT_VARIABLE ROOT_LIBDIR_CMD)
string(REPLACE "\n" "" ROOT_LIBDIR "${ROOT_LIBDIR_CMD}")
link_directories (${ROOT_LIBDIR})

#ROOT libraries for binary executable
set(ROOT_DEPS Core EG Hist Physics RIO Tree MathCore EGPythia8)

#compile and link the generator library
add_library (${LIB} SHARED ${SRCS})
target_link_libraries(${LIB})

#build the executables
foreach(IBIN ${BIN})
  add_executable(${IBIN} src/${IBIN}.cxx)
  target_link_libraries(${IBIN} ${LIB} ${ROOT_DEPS})
endforeach(IBIN ${BIN})

#add_executable(${BIN} src/${BIN}.cxx)
#target_link_libraries(${BIN} ${LIB} ${ROOT_DEPS})











