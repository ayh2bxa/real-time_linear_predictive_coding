#include "lpc.h"

LPC::LPC() {
    inPtr = 0;
    smpCnt = 0;
    outWtPtr = HOPSIZE;
    outRdPtr = 0;
    exPtr = 0;
    histPtr = 0;
    max_amp = 1;
    phi.resize(ORDER+1);
    a.resize(ORDER+1);
    inBuf.resize(BUFLEN);
    orderedInBuf.resize(FRAMELEN);
    outBuf.resize(BUFLEN);
    window.resize(FRAMELEN);
    out_hist.resize(ORDER);
    noise.resize(EXLEN);
    for (int i = 0; i < FRAMELEN; i++) {
        window[i] = 0.5f*(1.0f-cosf(2.0*M_PI*i/(float)(FRAMELEN-1)));
    }
    for (int i = 0; i < FRAMELEN; i++) {
        orderedInBuf[i] = 0;
    }
    for (int i = 0; i < BUFLEN; i++) {
        inBuf[i] = 0;
        outBuf[i] = 0;
    }
    for (int i = 0; i < ORDER; i++) {
        out_hist[i] = 0;
        phi[i] = 0;
    }
    phi[ORDER] = 0;
    reset_a();
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0, 1.0);
    for (int i = 0; i < EXLEN; i++) {
        noise[i] = distribution(generator);
    }
    for (int i = 0; i < EXLEN; i++) {
        if (abs(noise[i]) > max_amp) {
            max_amp = abs(noise[i]);
        }
    }
    for (int i = 0; i < EXLEN; i++) {
        noise[i] /= (1.1*max_amp);
    }
}

void LPC::levinson_durbin() {
    reset_a();
    a[0] = 1;
    double E = phi[0];
    for (int k = 0; k < ORDER; k++) {
        double lbda = 0;
        for (int j = 0; j < k+1; j++) {
            lbda -= a[j]*phi[k+1-j];
        }
        lbda /= E;
        for (int n = 0; n < 1+(k+1)/2; n++) {
            double tmp = a[k+1-n]+lbda*a[n];
            a[n] += lbda*a[k+1-n];
            a[k+1-n] = tmp;
        }
        E *= (1-lbda*lbda);
    }
}

double LPC::autocorrelate(const vector<double>& x, int lag) {
    double res = 0;
    for (int n = 0; n < x.size()-lag; n++) {
        res += x[n]*x[n+lag];
    }
    return res;
}

void LPC::applyLPC(float *inout, int numSamples, float lpcMix) {
    for (int s = 0; s < numSamples; s++) {
        inBuf[inPtr] = inout[s];
        inPtr++;
        if (inPtr >= BUFLEN) {
            inPtr = 0;
        }
        float out = outBuf[outRdPtr];
        out = out > 1 ? 1 : (out < -1 ? -1 : out);
        inout[s] = lpcMix*out+(1-lpcMix)*inout[s];
        outBuf[outRdPtr] = 0;
        outRdPtr++;
        if (outRdPtr >= BUFLEN) {
            outRdPtr = 0;
        }
        smpCnt++;
        if (smpCnt >= HOPSIZE) {
            max_amp = 1;
            smpCnt = 0;
            for (int i = 0; i < FRAMELEN; i++) {
                int inBufIdx = (inPtr+i-FRAMELEN+BUFLEN)%BUFLEN;
                orderedInBuf[i] = window[i]*inBuf[inBufIdx];
            }
            for (int lag = 0; lag < ORDER+1; lag++) {
                phi[lag] = autocorrelate(orderedInBuf, lag);
            }
            if (phi[0] != 0) {
                levinson_durbin();
                double err = 0;
                for (int n = 0; n < FRAMELEN; n++) {
                    double residual = orderedInBuf[n];
                    double x_hat_n = 0;
                    for (int k = 1; k <= ORDER; k++) {
                        if (n-k >= 0) {
                            x_hat_n += a[k]*orderedInBuf[n-k];
                        }
                    }
                    residual -= x_hat_n;
                    residual *= residual;
                    err += residual;
                }
                double G = sqrt(err)/FRAMELEN;
                for (int n = 0; n < FRAMELEN; n++) {
                    double ex = noise[exPtr];
                    exPtr++;
                    if (exPtr >= EXLEN) {
                        exPtr = 0;
                    }
                    double out_n = G*ex;
                    for (int k = 0; k < ORDER; k++) {
                        int idx = histPtr-k-1;
                        if (idx < 0) {
                            idx += ORDER;
                        }
                        out_n -= a[k+1]*out_hist[idx];
                    }
                    out_hist[histPtr] = out_n;
                    histPtr++;
                    if (histPtr >= ORDER) {
                        histPtr = 0;
                    }
                    int wtIdx = outWtPtr+n;
                    if (wtIdx >= BUFLEN) {
                        wtIdx -= BUFLEN;
                    }
                    outBuf[wtIdx] += out_n;
                }
                outWtPtr += HOPSIZE;
                if (outWtPtr >= BUFLEN) {
                    outWtPtr -= BUFLEN;
                }
            }
        }
    }
    
}

void LPC::reset_a() {
    for (int i = 0; i < ORDER+1; i++) {
        a[i] = 0;
    }
}
