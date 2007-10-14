#pragma once

#include <string>

/**
 * Finds the file component of a full directory and file name string.
 * This is the part of the input string after the last path delimiter.
 */
std::string FilePart(const std::string& filename);

/**
 * Finds the directory component of a full directory and file name
 * string. This is the part of the input string up to and including
 * the last path delimiter.
 */
std::string DirectoryPart(const std::string& filename);

/**
 * Finds the file extension for a given file name. This is the part of
 * the input string after the last dot (.).
 */
std::string ExtensionPart(const std::string& filename);

/**
 * Finds the prefix part of a string that specifies a file name pattern.
 * This is the file name part up to a percent (%) sign.
 */
std::string PrefixPart(const std::string& filename);

/**
 * Finds the prefix part of a string that specifies a file name pattern.
 * Within the string there should be a section like '%04d'.
 */
std::string FormatPart(const std::string& filename);

/**
 * Returns the last numeric value in a filename string.
 */
unsigned long NumberPart(const std::string& filename);

/**
 * Ensures that the supplied directory string ends with a path
 * delimiter. This modifies the input string.
 */
void CapDirectory(std::string& directory);
