PROJECT (common)

#######################################
# Common architecture library
#######################################
SET (common_SRCS
                    CommonTypes.h
                	FilePattern.h
    FileSet.cxx		FileSet.h
    Logger.cxx      Logger.h
					MathUtils.h
    StopWatch.cxx   StopWatch.h
)

ADD_LIBRARY (ITCommon ${common_SRCS})