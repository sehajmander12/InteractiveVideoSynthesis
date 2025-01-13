#include "featurepoint.h"

#include "readutility.h"
 
void FeaturePoint::updatePosition(float time, int nFeatures, const vector<float>& frequencies,
                                  const vector<complex<float>>& modalCoords, FeaturePoint*& featurePoints,
                                  FeaturePoint*& fixedPoints, float additionalScale, int selectedFreq, bool composite) {
    assert (frequencies.size() == modalCoords.size());

    if (isFixed) {
        dx = 0.f;
        dy = 0.f;
        int unfixedNeighbors = 0;
        for (int n : neighbors) {
            FeaturePoint neighbor = getPoint(n, nFeatures, featurePoints, fixedPoints);
            if (!neighbor.getFixed()) {
                dx += neighbor.dx;
                dy += neighbor.dy;
                ++unfixedNeighbors;
            }
        }
        if (unfixedNeighbors == 0) {
            dx = 0.f;
            dy = 0.f;
        } else {
            dx /= unfixedNeighbors;
            dy /= unfixedNeighbors;
        }
    } else {
        dx = 0.f;
        dy = 0.f;
        if (composite) {
            for (unsigned int i = 0; i < frequencies.size(); ++i) {
                float timedPhase = time * frequencies[i] * 2 * M_PI;
                dx += abs(modalCoords[i]) * getXMagnitude(i) * sin(timedPhase + getXPhase(i) + arg(modalCoords[i])) * additionalScale;
                dy += abs(modalCoords[i]) * getYMagnitude(i) * sin(timedPhase + getYPhase(i) + arg(modalCoords[i])) * additionalScale;
            }
        } else if (selectedFreq >= 0 && selectedFreq < int(frequencies.size())) {
            float timedPhase = time * frequencies[selectedFreq] * 2 * M_PI;
            dx += abs(modalCoords[selectedFreq]) * getXMagnitude(selectedFreq) * sin(timedPhase + getXPhase(selectedFreq) + arg(modalCoords[selectedFreq])) * additionalScale;
            dy += abs(modalCoords[selectedFreq]) * getYMagnitude(selectedFreq) * sin(timedPhase + getYPhase(selectedFreq) + arg(modalCoords[selectedFreq])) * additionalScale;
        }
    }
    x = x0 + dx;
    y = y0 + dy;
}

void FeaturePoint::fixPosition(int nFeatures, const vector<float>& frequencies, const vector<complex<float>>& modalCoords,
                               FeaturePoint*& featurePoints, FeaturePoint*& fixedPoints, float additionalScale, int selectedFreq, bool composite) {
    updatePosition(0, nFeatures, frequencies, modalCoords, featurePoints, fixedPoints, additionalScale, selectedFreq, composite);
}

void FeaturePoint::computeColors(vector<float> xMaxs, vector<float> yMaxs) {
    assert(xModes.size() == yModes.size());
    xColors.clear();
    yColors.clear();
    if (isFixed) {
        for (int i = 0; i < nModes; ++i) {
            vector<float> black(3, 0.f);
            xColors.push_back(black);
            yColors.push_back(black);
        }
        return;
    }
    for (unsigned int i = 0; i < xModes.size(); ++i) {
        xColors.push_back(complexToColor(getXMode(i), xMaxs[i]));
        yColors.push_back(complexToColor(getYMode(i), yMaxs[i]));
    }
}

std::ostream & operator<<(std::ostream & out, FeaturePoint & v) {
    out << "x: " << v.x << ", y: " << v.y << std::endl;
    out << "u: " << v.getU() << ", v: " << v.getV() << std::endl;
    out << "x0: " << v.getX0() << ", y0: " << v.getY0();
    return out;
}
