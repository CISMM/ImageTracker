PROJECT (image)

##############################
# Contains image wrangling classes--image IO, 
# utilities, smart image series handlers, etc.
##############################

SET (image_SRCS
	FileSetImageReader.cxx	FileSetImageReader.h
							ImageUtils.h
	TransformGroup.cxx		TransformGroup.h
)

ADD_LIBRARY (ITImage ${image_SRCS})