#include <iostream>
#include <array>
#include <random>
#include <cmath>

using namespace std;

class LPC {
private:
    int ORDER = 25;
    int FRAMELEN = 882;
    int HOPSIZE = FRAMELEN/2;
    int BUFLEN = 2*FRAMELEN;
    int SAMPLERATE = 44100;
    int EXLEN = 2*SAMPLERATE;
    vector<double> phi;
    vector<double> a;
    int inPtr;
    int outWtPtr;
    int outRdPtr;
    int smpCnt;
    int exPtr;
    int histPtr;
    double max_amp;
    vector<double> inBuf;
    vector<double> orderedInBuf;
    vector<double> outBuf;
    vector<double> window;
    void levinson_durbin();
    double autocorrelate(const vector<double>& x, int lag);
    void reset_a();
    vector<double> out_hist;
    vector<double> noise;
public:
    LPC();
    void applyLPC(float *inout, int numSamples, float lpcMix);
};
