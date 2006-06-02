#pragma once

#include <string>

/*
 * Represents a naming pattern for a group of files.  
 * A pattern consists of a directory, file prefix, extension, and
 * start and end numbers.  The numbering scheme should have a fixed
 * number of places.
 */
class FilePattern
{
public:
    /*
     * Complete constructor.
     */
    FilePattern(std::string dir, std::string pre, std::string ext, unsigned int start, unsigned int end, unsigned int places);
    
    /*
     * Default constructor.
     */
    FilePattern(void);

    /*
     * Destructor.
     */
    ~FilePattern(void);

    /*
     * Determines if all required members have been set to create a pattern.
     */
    bool Validate(void) const;

    std::string directory;
    std::string prefix;
    std::string extension;
    unsigned int start;
    unsigned int end;
    unsigned int places;
};
