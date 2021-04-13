FIND_PACKAGE(HDF5 COMPONENTS C CXX HL REQUIRED)

IF(HDF5_FOUND)
    MESSAGE(STATUS "[HDF5]")
    MESSAGE(STATUS "\tVersion: ${HDF5_VERSION}")
    MESSAGE(STATUS "\tCXX Libraries: ${HDF5_CXX_LIBRARIES}")
    MESSAGE(STATUS "\tC Libraries: ${HDF5_C_LIBRARIES}")
    LINK_DIRECTORIES( ${HDF5_INCLUDE_DIRS} )
    SET( HDF5 ${HDF5_C_LIBRARIES} ${HDF5_CXX_LIBRARIES} ${HDF5_CXX_HL_LIBRARIES} )
ELSE()
    INCLUDE(ExternalProject)

    SET( HDF5_VERSION "1.10.5" )

    ExternalProject_Add(hdf5_src
        DOWNLOAD_COMMAND curl http://www.hdfgroup.org/ftp/HDF5/current/src/hdf5-${HDF5_VERSION}.tar.gz | tar xvz -C ${PROJECT_BINARY_DIR}/hdf5_local/src
        CMAKE_ARGS ${ExternalProjectCMakeArgs}
        SOURCE_DIR ${PROJECT_BINARY_DIR}/hdf5_local/src/hdf5-${HDF5_VERSION}
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property( hdf5_src binary_dir )
    ExternalProject_Get_Property( hdf5_src source_dir )

    SET(HDF5_INCLUDE_DIRS
        ${source_dir}/src
        ${source_dir}/c++/src
        ${binary_dir}
    )

    SET( HDF5 ${binary_dir}/libhdf5_cpp${CMAKE_SHARED_LIBRARY_SUFFIX} ${binary_dir}/libhdf5.${CMAKE_SHARED_LIBRARY_SUFFIX} )
ENDIF()