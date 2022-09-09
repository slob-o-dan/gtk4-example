set(target_name ${PROJECT_NAME})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GTK REQUIRED gtk4)

add_executable(${target_name})
target_include_directories(${target_name} PUBLIC
        ${GLIB_INCLUDE_DIRS}
        ${GTK_INCLUDE_DIRS})
target_link_libraries(${target_name} PUBLIC
        ${GLIB_LIBRARIES}
        ${GTK_LIBRARIES})
target_sources(${target_name} PRIVATE gtk4-example.c)
