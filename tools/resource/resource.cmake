find_package(Python COMPONENTS Interpreter)

set(RES_MODES CSTR HEX)

set(RES_CSTR_EXT cstr)
set(RES_CSTR_PY "${CMAKE_CURRENT_LIST_DIR}/cstrdump.py")

set(RES_HEX_EXT hex)
set(RES_HEX_PY "${CMAKE_CURRENT_LIST_DIR}/hexdump.py")

function (add_resource TARGET)
    cmake_parse_arguments(ARG "" "" "${RES_MODES}" ${ARGN})
    add_library("${TARGET}" INTERFACE)
    target_include_directories("${TARGET}" INTERFACE "${PROJECT_BINARY_DIR}/${TARGET}")
    foreach (MODE ${RES_MODES})
        foreach (SOURCE ${ARG_${MODE}})
            set(OUTPUT "${PROJECT_BINARY_DIR}/${TARGET}/${SOURCE}.${RES_${MODE}_EXT}")
            if (NOT IS_ABSOLUTE "${SOURCE}")
                set(SOURCE "${PROJECT_SOURCE_DIR}/${SOURCE}")
            endif()
            add_custom_command(
                OUTPUT "${OUTPUT}"
                DEPENDS "${SOURCE}"
                COMMAND "${Python_EXECUTABLE}" "${RES_${MODE}_PY}" "${SOURCE}" "${OUTPUT}"
            )
            target_sources("${TARGET}" PRIVATE "${OUTPUT}")
        endforeach()
    endforeach()
endfunction()
