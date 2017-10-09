link_directories( ${GTKMM_LIBRARY_DIRS}  )

include_directories( ${GTKMM_INCLUDE_DIRS}  )

add_executable(com.github.georgekap1an.chores main.cpp choredialog.cpp chorelist.cpp mainwindow.cpp)

target_link_libraries(com.github.georgekap1an.chores ${GTKMM_LIBRARIES}  )

MESSAGE(${GTKMM_LIBRARIES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wextra")

install(TARGETS com.github.georgekap1an.chores DESTINATION  bin)




