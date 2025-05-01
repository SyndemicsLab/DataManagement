# build position independent code
option(DATAMANAGEMENT_BUILD_PIC "Build position independent code (-fPIC)" OFF)

# testing options
option(DATAMANAGEMENT_BUILD_TESTS "Build tests" OFF)

option(DATAMANAGEMENT_CALCULATE_COVERAGE "Build Coverage Files" OFF)

# install options
option(DATAMANAGEMENT_INSTALL "Generate the install target" ${DATAMANAGEMENT_MASTER_PROJECT})