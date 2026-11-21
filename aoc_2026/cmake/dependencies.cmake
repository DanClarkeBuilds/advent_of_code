include(FetchContent)

set(LEXY_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(lexy
    GIT_REPOSITORY https://github.com/foonathan/lexy.git
    GIT_TAG 982f36fe0f663153463657a166e4f173099dea6f # v2025.05.0
    SYSTEM
)
FetchContent_MakeAvailable(lexy)

if(BUILD_TESTING)
    FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG 5010924c4d8e912e64fd64a11819eecacf572c06 # v3.16.1
        SYSTEM
    )
    FetchContent_MakeAvailable(Catch2)
endif()
