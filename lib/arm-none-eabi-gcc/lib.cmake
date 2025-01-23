
message(STATUS "Starting to copy header files from ${SOURCE_DIR} to ${DEST_DIR}")
# Ensure destination directory exists
file(MAKE_DIRECTORY ${DEST_DIR})

# Find all header files, including those in subdirectories
file(GLOB_RECURSE HEADERS "${SOURCE_DIR}/*.h" "${SOURCE_DIR}/*.hpp")



# Copy all header files to the destination directory without preserving directory structure
set(TOTAL_FILES ${HEADERS})
list(LENGTH TOTAL_FILES TOTAL_COUNT)
set(COUNTER 0)

foreach(HEADER ${HEADERS})
    # Get the filename
    get_filename_component(FILENAME ${HEADER} NAME)
    
    # Copy the file to the destination directory
    configure_file(${HEADER} ${DEST_DIR}/${FILENAME} COPYONLY)
    
    # Increment counter and log progress
    math(EXPR COUNTER "${COUNTER}+1")
    math(EXPR PERCENTAGE "(${COUNTER}*100)/${TOTAL_COUNT}")
    message(STATUS "Copied (${PERCENTAGE}%) ")
endforeach()

# Log the completion of the header file copy process
message(STATUS "Finished copying ${TOTAL_COUNT} header files.")