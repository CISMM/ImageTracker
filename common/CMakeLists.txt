PROJECT (common)

#######################################
# Common architecture library
#######################################
SET (common_SRCS
                    CommonTypes.h
    file_list.cpp   file_list.h
                    FilePattern.h
    FileSet.cxx     FileSet.h
    FileUtils.cxx   FileUtils.h
    Logger.cxx      Logger.h
                    MathUtils.h
    Mutex.cxx       Mutex.h
    MutexLocker.cxx MutexLocker.h
    StopWatch.cxx   StopWatch.h
)

ADD_LIBRARY (ITCommon ${common_SRCS})
