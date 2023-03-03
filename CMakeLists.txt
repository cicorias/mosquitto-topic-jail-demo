cmake_minimum_required(VERSION 3.25)
project(mosquitto_plugin VERSION 0.1.0)

set (PLUGIN_NAME topic_jail)

add_library(${PLUGIN_NAME} MODULE
	${PLUGIN_NAME}.c
)

if(APPLE)
	set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -undefined dynamic_lookup")
endif()

find_package(unofficial-cunit CONFIG REQUIRED)
target_link_libraries(${PLUGIN_NAME} PRIVATE unofficial::cunit::cunit)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(addMosquitto)
target_link_libraries(${PLUGIN_NAME} PRIVATE mosquitto)



# find_package(mosquitto REQUIRED)
# target_include_directories(${PLUGIN_NAME} PRIVATE ${MOSQUITTO_INCLUDE_DIRS})
# target_link_libraries(${PLUGIN_NAME} PRIVATE ${MOSQUITTO_LIBRARIES})


# target_include_directories(${PLUGIN_NAME} PRIVATE
# 	"${OPENSSL_INCLUDE_DIR}"
# 	"${STDBOOL_H_PATH}"
# 	"${STDINT_H_PATH}"
# 	"${mosquitto_SOURCE_DIR}"
# 	"${mosquitto_SOURCE_DIR}/include"
# )

# link_directories(${mosquitto_SOURCE_DIR})

# set_target_properties(${PLUGIN_NAME} PROPERTIES
# 	PREFIX ""
# 	POSITION_INDEPENDENT_CODE 1
# )

# cmake_minimum_required(VERSION 3.0.0)
# project(mosquitto_plugin VERSION 0.1.0)

# include(CTest)
# enable_testing()

# add_executable(topicjail topicjail.c)

# find_package(mosquitto REQUIRED)
# target_include_directories(topicjail PRIVATE ${MOSQUITTO_INCLUDE_DIRS})
# target_link_libraries(topicjail PRIVATE ${MOSQUITTO_LIBRARIES})

# # set(THREADS_PREFER_PTHREAD_FLAG ON)
# # find_package(Threads REQUIRED)
# # target_link_libraries(topicjail PRIVATE Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
