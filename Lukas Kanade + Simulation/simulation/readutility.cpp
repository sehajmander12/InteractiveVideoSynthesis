#include "readutility.h"

using namespace std;
 
/****************************************************************************************************************/
/************************************************ Read Functions ************************************************/
FeaturePoint& getPoint(int i, int nFeatures, FeaturePoint* featurePoints, FeaturePoint* fixedPoints) {
    if (i >= nFeatures) {
        return fixedPoints[i - nFeatures];
    }
    return featurePoints[i];
}

void throwFeaturePosFileError(string filename) {
    cout << "Cannot read feature position file " << filename << endl;
    exit(1);
}

void readPoints(ifstream& fs, const string filename, int nPoints, TriangulationData& tri, bool rotateRef, bool fixed = false, int start = 0) {
    string line;

    for (int i = 0; i < nPoints; ++i) {
        if (!getline(fs, line)) throwFeaturePosFileError(filename);

        string n;
        istringstream ss(line);
        int it = 0;
        float u = 0.f, v = 0.f;
        while (ss) {
            if (!getline(ss, n, ',')) break;
            float coord = stof(n);
            if (it == 0) {
                u = coord / (1.f * tri.tex_width);
            } else {
                v = coord / (1.f * tri.tex_height);
            }
            ++it;
        }
        if (start == 0) {
            tri.featurePoints[i] = FeaturePoint(u, v, i + start, rotateRef);
            tri.featurePoints[i].setFixed(fixed);
        } else {
            tri.fixedPoints[i] = FeaturePoint(u, v, i + start, rotateRef);
            tri.fixedPoints[i].setFixed(fixed);
        }
        assert (it == 2);
    }
}

void readTriangles(ifstream& fs, string filename, TriangulationData& tri) {
    string line;
    tri.indexArray = new unsigned int[tri.nTriangles * 3];
    tri.triangles.clear();
    for (int i = 0; i < tri.nTriangles; ++i) {
        if (!getline(fs, line)) throwFeaturePosFileError(filename);

        istringstream ss(line);
        string n;
        int it = 0;
        vector<FeaturePoint*> triangle;
        while (ss) {
            if (!getline(ss, n, ',')) break;
            tri.indexArray[3 * i + it] = stoi(n);
            triangle.push_back(&getPoint(stoi(n), tri.nFeatures, tri.featurePoints, tri.fixedPoints));
            ++it;
        }
        assert(it == 3);
        Triangle t(triangle[0], triangle[1], triangle[2]);
       tri.triangles.push_back(t);
    }

    for (int i = 0; i < tri.nTriangles; ++i) {
        for (int j = 0; j < 3; ++j)  {
            FeaturePoint& vertex = getPoint(tri.indexArray[3 * i + j], tri.nFeatures, tri.featurePoints, tri.fixedPoints);
            vertex.addNeighbors(tri.indexArray[3 * i + ((j + 1) % 3)]);
            vertex.addNeighbors(tri.indexArray[3 * i + ((j + 2) % 3)]);
        }
    }
}

TriangulationData readFeaturePositionFile(string filename, bool rotateRef) {
    TriangulationData tri;

    ifstream fs (filename.c_str());

    if (!fs.is_open()) throwFeaturePosFileError(filename);

    // read width and height
    string line;
    if (!getline(fs, line)) throwFeaturePosFileError(filename);
    tri.tex_width = stoi(line);

    if (!getline(fs, line)) throwFeaturePosFileError(filename);
    tri.tex_height = stoi(line);

    // read feature points and calculate uvs
    if (!getline(fs, line)) throwFeaturePosFileError(filename);
    tri.nFeatures = stoi(line);

    tri.featurePoints = new FeaturePoint[tri.nFeatures];
    readPoints(fs, filename, tri.nFeatures, tri, rotateRef);

    // read fixed points and calculate uvs
    if (!getline(fs, line)) throwFeaturePosFileError(filename);
    tri.nFixed = stoi(line);

    tri.fixedPoints = new FeaturePoint[tri.nFixed];
    readPoints(fs, filename, tri.nFixed, tri, rotateRef, true, tri.nFeatures);

    // read triangles and set index array
    if (!getline(fs, line)) throwFeaturePosFileError(filename);
    tri.nTriangles = stoi(line);
    readTriangles(fs, filename, tri);

    return tri;
}



void throwFFTFileError(string filename) {
    cout << "Cannot read fft file " << filename << endl;
    exit(1);
}

void readFFTLine(ifstream& fs, string filename, float* fft, int offset, int nFeatures) {
    string line;
    if (!getline(fs, line)) throwFFTFileError(filename);

    string n;
    istringstream ssReal(line);

    int it = 0;
    while (ssReal) {
        if (!getline(ssReal, n, ',')) break;
        fft[it + offset] = stof(n);
        ++it;
    }
    assert(it == nFeatures);
}

SimulationData* readFFT(const string &filename, TriangulationData* tri, SimulationParams *params) {
    SimulationData* sim = (SimulationData*) malloc(sizeof(SimulationData));
    if (!sim) return nullptr;
    sim->tri = tri;

    ifstream fs(filename.c_str());
    if (!fs.is_open()) throwFFTFileError(filename);

    string line;
    if (!getline(fs, line)) throwFFTFileError(filename);
    sim->nModes = stoi(line);

    if (!getline(fs, line)) throwFFTFileError(filename);
    assert(stoi(line) == sim->tri->nFeatures);

    sim->frequencies.clear();
    vector<float> xMags;
    vector<float> yMags;
    float sumXMag = 0.f;
    float sumYMag = 0.f;
    for (int i = 0; i < sim->nModes; ++i) {
        if (!getline(fs, line)) throwFFTFileError(filename);

        string n;
        istringstream ss(line);

        if (!getline(ss, n, ',')) break;
        sim->frequencies.push_back(stof(n));

        if (!getline(ss, n, ',')) break;
        xMags.push_back(stof(n));
        sumXMag += stof(n);

        if (!getline(ss, n, ',')) break;
        yMags.push_back(stof(n));
        sumYMag += stof(n);
    }
    sumXMag /= sim->nModes;
    sumYMag /= sim->nModes;

    float xFFT[sim->tri->nFeatures * 2];
    for (int i = 0; i < sim->nModes; ++i) {
        readFFTLine(fs, filename, xFFT, 0, sim->tri->nFeatures);
        readFFTLine(fs, filename, xFFT, sim->tri->nFeatures, sim->tri->nFeatures);

        for (int n = 0; n < sim->tri->nFeatures; ++n) {
            FeaturePoint& fp = getPoint(n, sim->tri->nFeatures, sim->tri->featurePoints, sim->tri->fixedPoints);
            complex<float> mode(- xFFT[n], - xFFT[n + sim->tri->nFeatures]);
            if (params->normalized) {
                mode /= (xMags[i] + yMags[i]);
            }
            if (params->rotateRef) {
                fp.addYMode(mode);
            } else {
                mode *= params->aspect_ratio;
                fp.addXMode(-mode);
            }
        }
    }

    float yFFT[sim->tri->nFeatures * 2];
    for (int i = 0; i < sim->nModes; ++i) {
        readFFTLine(fs, filename, yFFT, 0, sim->tri->nFeatures);
        readFFTLine(fs, filename, yFFT, sim->tri->nFeatures, sim->tri->nFeatures);

        for (int n = 0; n < sim->tri->nFeatures; ++n) {
            FeaturePoint& fp = getPoint(n, sim->tri->nFeatures, sim->tri->featurePoints, sim->tri->fixedPoints);
            complex<float> mode(- yFFT[n], - yFFT[n + sim->tri->nFeatures]);
            if (params->normalized) {
                mode /= (xMags[i] + yMags[i]);
            }
            if (params->rotateRef) {
                mode *= params->aspect_ratio;
                fp.addXMode(mode);
            } else {
                fp.addYMode(mode);
            }
        }
    }

    vector<float> xMaxs;
    vector<float> yMaxs;

    for (int i = 0; i < sim->nModes; ++i) {
        float xMax = -1.f;
        float yMax = -1.f;
        for (int j = 0; j < sim->tri->nFeatures; ++j) {
            FeaturePoint& fp = getPoint(j, sim->tri->nFeatures, sim->tri->featurePoints, sim->tri->fixedPoints);
            if (xMax < fp.getXMagnitude(i)) xMax = fp.getXMagnitude(i);
            if (yMax < fp.getYMagnitude(i)) yMax = fp.getYMagnitude(i);
        }
        xMaxs.push_back(xMax);
        yMaxs.push_back(yMax);
    }

    if (!params->normalized) {
        params->force_scale /= (sumYMag + sumXMag) * (sumYMag + sumXMag);
    }
    cout << params->force_scale << endl;

    for (int i = 0; i < sim->tri->nFeatures + sim->tri->nFixed; ++i) {
        FeaturePoint& fp = getPoint(i, sim->tri->nFeatures, sim->tri->featurePoints, sim->tri->fixedPoints);
        fp.setNModes(sim->nModes);
        fp.computeColors(xMaxs, yMaxs);
    }
    sim->modalCoords.clear();
    for (int i = 0; i < sim->nModes; ++i) {
        if (params->normalized) {
            sim->modalCoords.push_back(1.f / (sumYMag + sumXMag) * (sumYMag + sumXMag));
        } else {
            sim->modalCoords.push_back(1.f);
        }
    }

    return sim;
}

/************************************************ Read Functions ************************************************/
/****************************************************************************************************************/
