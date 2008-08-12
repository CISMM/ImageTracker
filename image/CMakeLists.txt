PROJECT (image)

##############################
# Contains image wrangling classes--image IO, 
# utilities, smart image series handlers, etc.
##############################

SET (image_SRCS
    DataSource.cxx                  DataSource.h
    FileSetImageReader.cxx          FileSetImageReader.h
                                    ImageFileSet.h
    ImageFileSetReader.cxx          ImageFileSetReader.h
                                    ImageFileSetTypes.h
                                    ImageSetReader.h
    ImageUtils.cxx                  ImageUtils.h
    TransformGroup.cxx              TransformGroup.h
                                    VectorFileSet.h
    VectorFileSetReader.cxx         VectorFileSetReader.h
)

ADD_LIBRARY (ITImage ${image_SRCS})
TARGET_LINK_LIBRARIES(ITImage ITCommon)