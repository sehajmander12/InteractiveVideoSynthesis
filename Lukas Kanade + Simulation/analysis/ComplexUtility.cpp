#include "ComplexUtility.h"
 
#include <assert.h>

using namespace std;

float phase(complex<float> c) {
    float angle = arg(c);
    if (c.imag() < 0.f && angle > 0.f) {
        angle += M_PI / 2.f;
    } else if (c.imag() > 0.f && angle < 0.f) {
        angle += M_PI / 2.f;
    }
    while (angle < 0.f) {
        angle += 2.f * M_PI;
    }
    return angle;
}

float complexToHue(complex<float> c) {
    return phase(c) / 2.f / M_PI * 360.f;
}

#include <iostream>

vector<float> complexToColor(complex<float> c, float max) {
    vector<float> color;
    float h = complexToHue(c);
    int hPrime = h / 60;
    float s = 1.f;
    float v = abs(c) / max;
    assert(v >= 0.f && v <= 1.f);

    // HSV to RGB
    float chroma = v * s;
    float X = chroma * (1 - abs((hPrime % 2)- 1));
    float m = v - chroma;

    float r = m, g = m, b = m;
    if (0 <= hPrime && hPrime < 1) {
        r += chroma;
        g += X;
    } else if (1 <= hPrime && hPrime < 2) {
        r += X;
        g += chroma;
    } else if (2 <= hPrime && hPrime < 3) {
        g += chroma;
        b += X;
    } else if (3 <= hPrime && hPrime < 4) {
        g += X;
        b += chroma;
    } else if (4 <= hPrime && hPrime < 5) {
        r += X;
        b += chroma;
    } else if (5 <= hPrime && hPrime < 6) {
        r += chroma;
        b += X;
    }

    color.push_back(r);
    color.push_back(g);
    color.push_back(b);
    return color;
}
