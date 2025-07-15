# CMakeList.txt : CMake project for EEC_BSRV, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (EEC_BSRV "EEC_BSRV.cpp" "EEC_BSRV.h" "ServerSocket.cpp" "ServerSocket.h" "ClientSocket.cpp" "ClientSocket.h" "Logger.h" "Logger.cpp" "Misc.cpp" "Misc.h" "Sample.h" "Sample.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET EEC_BSRV PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
