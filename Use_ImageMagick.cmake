#################################################
# Use_ImageMagick.cmake
# Setup project to use ImageMagick libraries and include files.
#
# MAGICK_ROOT_DIR:  The install directory of ImageMagick
# MAGICK_INC_DIR:   ImageMagick include directory
# MAGICK_LIB_DIR:   ImageMagick static library directory
#
#################################################

IF (WIN32)

	SET (MAGICK_FROM_NSRG OFF)
	
	IF (MAGICK_FROM_NSRG)
		# Find the root directory
		SET (MAGICK_POSSIBLE_ROOT_PATHS
			"C:\\NSRG\\external\\pc_win32\\ImageMagick-6.2.3_staticDLL" )
			
		FIND_PATH (MAGICK_ROOT_DIR include\\magick\\magick.h
			${MAGICK_POSSIBLE_ROOT_PATHS} )
			
		# Find the include directory
		FIND_PATH (MAGICK_INC_DIR magick\\magick.h
			${MAGICK_ROOT_DIR}\\include )

		# Find ImageMagick static library directory
		FIND_PATH (MAGICK_LIB_DIR CORE_RL_magick_.lib
			${MAGICK_ROOT_DIR}\\lib )
		
	ELSE (MAGICK_FROM_NSRG) # assume from source
		# Find the program
		SET (MAGICK_POSSIBLE_ROOT_PATHS
			"C:\\ImageMagick-6.2.3" )

		# Find root directory
		FIND_PATH (MAGICK_ROOT_DIR magick\\magick.h
			${MAGICK_POSSIBLE_ROOT_PATHS} )

		# Find include files
		FIND_PATH (MAGICK_INC_DIR magick\\magick.h
			${MAGICK_ROOT_DIR} )
			
		# Find Magick++ include files
		FIND_PATH (MAGICK++_INC_DIR	Magick++.h
			${MAGICK_ROOT_DIR}\\Magick++\\lib )

		# Find static libraries
		FIND_PATH (MAGICK_LIB_DIR CORE_RL_magick_.lib
			${MAGICK_ROOT_DIR}\\VisualMagick\\lib )

	ENDIF (MAGICK_FROM_NSRG)

    INCLUDE_DIRECTORIES( ${MAGICK_INC_DIR} ${MAGICK++_INC_DIR} )
    LINK_DIRECTORIES( ${MAGICK_LIB_DIR} )

ENDIF (WIN32)

MARK_AS_ADVANCED (
    MAGICK_ROOT_DIR
    MAGICK_INC_DIR
    MAGICK_LIB_DIR )
	