#include "FilePattern.h"

FilePattern::FilePattern(void)
{
}

FilePattern::FilePattern(std::string dir, std::string pre, std::string ext, unsigned int start, unsigned int end, unsigned int places)
{
    this->directory = dir;
    this->prefix = pre;
    this->extension = ext;
    this->start = start;
    this->end = end;
    this->places = places;
}

FilePattern::~FilePattern(void)
{
}

bool FilePattern::Validate()
{
    // Validate the pattern
    assert(this->start <= this->end);
    //assert(this->directory != NULL);
    //assert(this->prefix != NULL);
    //assert(this->extension != null);

    // if we passed all assertions, we're ok
    return true;
}
