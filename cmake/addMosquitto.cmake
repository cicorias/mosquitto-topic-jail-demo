include(FetchContent)
FetchContent_Declare(mosquittosrc
    GIT_REPOSITORY      https://github.com/eclipse/mosquitto.git
    GIT_TAG             d1b19b22aa5f0576d267e8c83c0634af388c7c5f)
FetchContent_GetProperties(mosquittosrc)
if(NOT mosquittosrc_POPULATED)
    FetchContent_Populate(mosquittosrc)
    add_subdirectory(${mosquitto_SOURCE_DIR} ${mosquitto_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()