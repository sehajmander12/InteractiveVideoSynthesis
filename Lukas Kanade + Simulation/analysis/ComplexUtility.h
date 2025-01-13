#ifndef COMPLEXUTILITY_H
#define COMPLEXUTILITY_H

#include <complex>
#include <vector>

// converts the complex number into phase in the range of [0, 2pi]
float phase(std::complex<float> c);

// converts the complex number into hue in the range of [0, 360]
float copmlexToHue(std::complex<float> c);

// converts complex number into color using HSV
std::vector<float> complexToColor(std::complex<float> c, float max);

#endif // COMPLEXUTILITY_H
 
