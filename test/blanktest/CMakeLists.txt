CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

PROJECT(blanktest)

SET(BOOST_COMPONENT "filesystem")
LIST(APPEND BOOST_COMPONENT "program_options")
LIST(APPEND BOOST_COMPONENT "thread")
FIND_PACKAGE(Boost REQUIRED COMPONENTS ${BOOST_COMPONENT})


ADD_EXECUTABLE(verifyClient "verifyClient-mock.cc")
target_include_directories(verifyClient PRIVATE "mocks/strongstore")
TARGET_LINK_LIBRARIES(verifyClient tbb ${Boost_LIBRARIES})