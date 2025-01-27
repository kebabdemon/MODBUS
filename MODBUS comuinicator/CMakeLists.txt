cmake_minimum_required(VERSION 3.10)
project(my_gtk_app)

# Set the C standard
set(CMAKE_C_STANDARD 11)

# Define the path to pkg-config executable
set(PKG_CONFIG_EXECUTABLE "C:/msys64/mingw64/bin/pkg-config.exe")

# Find PkgConfig
find_package(PkgConfig REQUIRED)

# Locate GTK4 using pkg-config
pkg_check_modules(GTK4 REQUIRED gtk4)

# Add include directories for GTK4
include_directories(${GTK4_INCLUDE_DIRS})

# Add linker directories for GTK4
link_directories(${GTK4_LIBRARY_DIRS})

# Define compilation flags for GTK4
add_definitions(${GTK4_CFLAGS_OTHER})

# Add the executable and source files
add_executable(my_gtk_app
        main.c
        src/com.c
        lib/com.h
        src/port.c
        lib/port.h
        src/listen_write.c
        lib/listen_write.h
)

# Include GTK4 directories for this target
target_include_directories(my_gtk_app PRIVATE ${GTK4_INCLUDE_DIRS})

# Link the GTK4 libraries to the executable
target_link_libraries(my_gtk_app ${GTK4_LIBRARIES})
