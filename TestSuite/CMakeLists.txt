###################################
# Bruce Eckel's TestSuite Framework
###################################
SET (TestSuite_SRCS
	Test.cpp
	Suite.cpp
)
INCLUDE_DIRECTORIES( ${TestSuite_SOURCE_DIR} )
ADD_LIBRARY(TestSuite ${TestSuite_SRCS} )