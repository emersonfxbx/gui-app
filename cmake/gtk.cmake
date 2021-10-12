function(gtk4_win_deploy)
    # only MSYS2/MINGW supported
    if(NOT MINGW)
        return()
    endif()

    set(TMP_FLAGS)
    set(TMP_SINGLE_VALUES TARGET DEPLOY_DIR)
    set(TMP_MULTIPLE_VALUES)
    cmake_parse_arguments(
        TMP
        "${TMP_FLAGS}"
        "${TMP_SINGLE_VALUES}"
        "${TMP_MULTIPLE_VALUES}"
        ${ARGN})

    if(NOT TMP_DEPLOY_DIR)
        set(TMP_DEPLOY_DIR "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    endif()

    add_custom_command(
        TARGET ${TMP_TARGET}
        POST_BUILD
        VERBATIM
        COMMENT "Creating ${TMP_DEPLOY_DIR} directory for target ${TMP_TARGET}"
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TMP_TARGET}>"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${TMP_DEPLOY_DIR}/bin"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${TMP_TARGET}>" "${TMP_DEPLOY_DIR}/bin"
        COMMAND sh -c "for l in `ntldd -R \"$<TARGET_FILE:${TMP_TARGET}>\"|grep -iv \"winsxs\\|system32\\|not found\"|awk '{print \$3}'`;do cp -u \"$l\" \"${TMP_DEPLOY_DIR}/bin\";done"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "$ENV{MINGW_PREFIX}/lib/gdk-pixbuf-2.0" "${TMP_DEPLOY_DIR}/lib/gdk-pixbuf-2.0"
        COMMAND sh -c "find \"${TMP_DEPLOY_DIR}/lib/gdk-pixbuf-2.0/2.10.0/loaders\" -name *.a -exec rm -f \"{}\" \\;"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "$ENV{MINGW_PREFIX}/share/icons/Adwaita" "${TMP_DEPLOY_DIR}/share/icons/Adwaita"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "$ENV{MINGW_PREFIX}/share/icons/hicolor" "${TMP_DEPLOY_DIR}/share/icons/hicolor")
endfunction()

function(gtk_add_gresources)
    set(TMP_FLAGS)
    set(TMP_SINGLE_VALUES TARGET RES_FILE)
    set(TMP_MULTIPLE_VALUES DEPENDS)
    cmake_parse_arguments(
        TMP
        "${TMP_FLAGS}"
        "${TMP_SINGLE_VALUES}"
        "${TMP_MULTIPLE_VALUES}"
        ${ARGN})
    get_filename_component(TMP_ABS_RES_FILE ${TMP_RES_FILE} ABSOLUTE)
    get_filename_component(TMP_RES_FILE_DIR ${TMP_ABS_RES_FILE} DIRECTORY)
    get_filename_component(TMP_RES_BASENAME ${TMP_RES_FILE} NAME_WLE)

    get_target_property(TMP_BINARY_DIR ${TMP_TARGET} BINARY_DIR)
    set(TMP_TARGET_SOURCE "${TMP_BINARY_DIR}/${TMP_RES_BASENAME}.c")

    find_program(TMP_XMLLINT xmllint)
    if(EXISTS "${TMP_XMLLINT}")
        # read dependency list from the resource script
        execute_process(
            COMMAND sh -c "${TMP_XMLLINT} --xpath '//gresources/gresource/file/text()' \"${TMP_ABS_RES_FILE}\""
            OUTPUT_VARIABLE TMP_RES_FILES
            RESULT_VARIABLE TMP_XMLLINT_RESULT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        if("${TMP_XMLLINT_RESULT}" EQUAL "0")
            string(REGEX REPLACE "\n" ";" TMP_RES_FILES "${TMP_RES_FILES}")
            set(TMP_DEPENDS)
            foreach(TMP_FILE ${TMP_RES_FILES})
                list(APPEND TMP_DEPENDS ${TMP_FILE})
            endforeach()
        endif()
    endif()

    foreach(TMP_DEP ${TMP_DEPENDS})
        get_filename_component(TMP_ABS_DEP ${TMP_DEP} ABSOLUTE BASE_DIR "${TMP_RES_FILE_DIR}") 
        list(APPEND TMP_ABS_DEPENDS ${TMP_ABS_DEP})
    endforeach()

    add_custom_command(
        OUTPUT "${TMP_TARGET_SOURCE}"
        MAIN_DEPENDENCY "${TMP_ABS_RES_FILE}"
        DEPENDS ${TMP_ABS_DEPENDS}
        WORKING_DIRECTORY "${TMP_RES_FILE_DIR}"
        COMMAND glib-compile-resources --generate-source --target="${TMP_TARGET_SOURCE}" "${TMP_ABS_RES_FILE}")

    target_sources(${TMP_TARGET} PRIVATE ${TMP_TARGET_SOURCE})
endfunction()
