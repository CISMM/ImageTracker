#include "FileUtils.h"

#include <cstdlib>

#ifdef _WIN32
extern const std::string PATH_DELIMITER("\\");
#else
extern const std::string PATH_DELIMITER("/");
#endif

std::string ExtensionPart(const std::string& filename)
{
    // find the last "." in the filename
    std::string::size_type idx = filename.rfind(".");
    // extract the extension, . included
    return(idx == std::string::npos ? std::string("") : filename.substr(idx));
}

std::string PrefixPart(const std::string& filename)
{
    // find the last directory delimiter
    std::string::size_type directoryIdx = filename.rfind(PATH_DELIMITER);
    // increment the index, if we found a directory delimiter
    directoryIdx = directoryIdx == std::string::npos ? 0 : directoryIdx+1;

    // find the last pattern indicator in the filename
    std::string::size_type patternIdx = filename.rfind("%");
    // find the length of the prefix substring
    std::string::size_type prefixLength = patternIdx == std::string::npos ? filename.size() : patternIdx - directoryIdx;
    
    return filename.substr(directoryIdx, prefixLength);
}

std::string FormatPart(const std::string& filename)
{
    // find the pattern index
    std::string::size_type patternStart = filename.rfind("%");
    // find the number indicator d, u, f
    std::string::size_type patternEnd = filename.find_first_of("duf", patternStart);
    
    return filename.substr(patternStart, patternEnd-patternStart+1);
}

void CapDirectory(std::string& directory)
{
    std::string::size_type lastDir = directory.rfind(PATH_DELIMITER);
    if (lastDir != directory.size()-1)
        directory.append(PATH_DELIMITER);
}

std::string DirectoryPart(const std::string& filename)
{
    std::string dirOnly("");
    std::string::size_type idx = filename.rfind(PATH_DELIMITER);
    if (idx != std::string::npos)
    {
        // the directory part from the start 
        // up to and including the last "/"
        dirOnly = filename.substr(0, idx+1);
    }
    
    return dirOnly;
}

std::string FilePart(const std::string& filename)
{
    std::string fileOnly("");
    std::string::size_type idx = filename.rfind(PATH_DELIMITER);
    if (idx != std::string::npos)
    {
        // the file part is everything after the last "/"
        fileOnly = filename.substr(idx+1);
    }
    
    return fileOnly;
}
 
unsigned long NumberPart(const std::string& filename)
{
    std::string::size_type firstNumber = filename.size();
    std::string::size_type lastNumber = filename.find_last_of("0123456789", firstNumber-1);
    firstNumber=filename.find_last_not_of("0123456789", lastNumber);
    firstNumber = firstNumber == std::string::npos ? 0 : firstNumber+1;
    return (unsigned long) atoi(&filename.c_str()[firstNumber]);
}