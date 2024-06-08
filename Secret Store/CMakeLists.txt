# CMakeList.txt : CMake project for Secret Store, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (CMakeTarget "main.cpp"  "Console.h" "Console.cpp" "SecretFile.h" "SecretFile.cpp"  "FileWrapper.h" "KeyDerivationParams.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

include(FetchContent)

# Update the commit to point to whatever libsodium-cmake-commit you want to target.
FetchContent_Declare(Sodium
    GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
    GIT_TAG 99f14233eab1d4f7f49c2af4ec836f2e701c445e # HEAD as of 2022-05-28
)
set(SODIUM_DISABLE_TESTS ON)
FetchContent_MakeAvailable(Sodium)

target_link_libraries(CMakeTarget
    PRIVATE
        sodium
)
