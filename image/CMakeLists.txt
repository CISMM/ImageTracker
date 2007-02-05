PROJECT (image)

##############################
# Contains image wrangling classes--image IO, 
# utilities, smart image series handlers, etc.
##############################

SET (image_SRCS
    DataSource.cxx                  DataSource.h
    FileSetImageReader.cxx          FileSetImageReader.h
                                    ImageSetReader.h
                                    ImageUtils.h
    TransformGroup.cxx              TransformGroup.h
)

ADD_LIBRARY (ITImage ${image_SRCS})
TARGET_LINK_LIBRARIES(ITImage ITCommon)