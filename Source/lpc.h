#include <iostream>
#include <array>
#include <random>
#include <cmath>
//#include "constants.h"
//#include "signalsmith-fft.h"

using namespace std;

class LPC {
private:
    int ORDER = 25;
    int FRAMELEN = 882;
    int HOPSIZE = FRAMELEN/2;
    int BUFLEN = 2*FRAMELEN;
    int SAMPLERATE = 44100;
    int MAX_EXLEN = 2*SAMPLERATE;
    int EXLEN = MAX_EXLEN;
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
    bool start = false;
    void applyLPC(float *inout, int numSamples, float lpcMix);
    void set_exlen(int val) {EXLEN = val;};
    int get_exlen() {return EXLEN;};
    int get_max_exlen() {return MAX_EXLEN;};
};
