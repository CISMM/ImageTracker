#pragma once

#include <string>
#include <cassert>

/**
 * Represents a naming pattern for a group of files. A pattern consists
 * of a directory, printf-style format, and start and end numbers.
 */
class FilePattern
{
public:
    /**
     * Legacy constructor. Builds a FilePattern from a directory, prefix, extension, and
     * numeric bounds on the file names.
     */
    FilePattern(const std::string& dir, const std::string& pre, const std::string& ext, 
        unsigned int start, unsigned int end, unsigned int places) :
            directory(dir),
            start(start),
            end(end)
    {
        // Construct a format string from the supplied prefix, extention, and number of places.
        char cformat[40];
        sprintf(cformat, "%s%0%dd%s", pre.c_str(), places, ext.c_str());
        this->format = cformat;
    }

    /**
     * Complete constructor.
     */
    FilePattern(const std::string& dir, const std::string& format, int start, int end) :
            directory(dir),
            format(format),
            start(start),
            end(end)
    {}

    /**
     * Default constructor.
     */
    FilePattern() :
            directory(""),
            format("%01d"),
            start(0),
            end(0)
    {}

    /**
     * Destructor.
     */
    ~FilePattern()
    {}

    /**
     * Determines if all required members have been set to create a pattern.
     */
    bool Validate(void) const
    {
        assert(this->start <= this->end);
        return true;
    }

    std::string directory;
    std::string format;
    unsigned int start;
    unsigned int end;
};
