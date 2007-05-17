

#include <cmath>



#ifndef _MATHUTILS_H

#define _MATHUTILS_H



// Defines some helpful math functions and constants left out of

// the Visual Studio math libraries.  I guess GNU's gcc adds a

// little special sauce for math folks.

#ifdef _WIN32



// A definition of pi.

#define M_PI 2*acos(0.0)



// logs and exponents

inline double log2(const double& num) { return (log10(num) / log10(2.0)); }

inline double exp2(const double& num) { return pow(2.0, num); }

inline double exp10(const double& num) { return pow(10.0, num); }



#endif // ifdef _WIN32



#endif // ifndef _MATHUTILS_H

