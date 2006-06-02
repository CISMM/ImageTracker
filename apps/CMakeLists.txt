PROJECT (apps)

##############################
# Command line and gui executables for the ImageTracker project
##############################

IF (WIN32)
  SET (GUI_EXE WIN32)
ENDIF (WIN32)

ADD_EXECUTABLE(translate TranslationGenerator.cxx)
TARGET_LINK_LIBRARIES(translate ITFilters)

ADD_EXECUTABLE(attenuate TestAttenuation.cxx)
TARGET_LINK_LIBRARIES(attenuate ITFilters)

ADD_EXECUTABLE(variance TestVariance.cxx)
TARGET_LINK_LIBRARIES(variance ITFilters)

ADD_EXECUTABLE(meanrepair TestMeanAttenuationRepair.cxx)
TARGET_LINK_LIBRARIES(meanrepair ITFilters)

ADD_EXECUTABLE(localrepair TestNeighborhoodAttenuationRepair.cxx)
TARGET_LINK_LIBRARIES(localrepair ITFilters)

INCLUDE (../LocalUsewxWidgets.cmake)

###################################
# ImageTracker GUI executables.
#
# Note: It is *absolutely* necessary to include "WIN32" on the
# line below. (e.g. ADD_EXECUTABLE(ImageTracker WIN32 ImageTrackerApp.cxx))
# Failing to do that does not set up a wx application using the 
# IMPLEMENT_APP() macro.
###################################
IF (WIN32)
    SET (GUI_EXE WIN32)
ENDIF (WIN32)

# This seems to be necessary for debug builds...for some reason, though, we
# can't just use CMAKE_DEBUG_EXE_LINKER_FLAGS
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt")

ADD_EXECUTABLE(viewer ${GUI_EXE} VtkViewerApp.cxx)
TARGET_LINK_LIBRARIES(viewer ITView)

ADD_EXECUTABLE(ImageTracker ${GUI_EXE} ImageTrackerApp.cxx)
TARGET_LINK_LIBRARIES(ImageTracker ITCommon ITImage ITAlgorithms ITView ITGui)