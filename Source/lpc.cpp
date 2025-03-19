#include "lpc.h"

LPC::LPC(int numChannels) {
    inPtrs.resize(numChannels);
    smpCnts.resize(numChannels);
    outWtPtrs.resize(numChannels);
    outRdPtrs.resize(numChannels);
    exPtrs.resize(numChannels);
    histPtrs.resize(numChannels);
    exCntPtrs.resize(numChannels);
    for (int ch = 0; ch < numChannels; ch++) {
        inPtrs[ch] = 0;
        smpCnts[ch] = 0;
        outWtPtrs[ch] = HOPSIZE;
        outRdPtrs[ch] = 0;
        exPtrs[ch] = 0;
        histPtrs[ch] = 0;
        exCntPtrs[ch] = 0;
    }
    max_amp = 1;
    phi.resize(ORDER+1);
    a.resize(ORDER+1);
    orderedInBuf.resize(FRAMELEN);
    window.resize(FRAMELEN);
    inBuf.resize(numChannels);
    outBuf.resize(numChannels);
    out_hist.resize(numChannels);
    for (int ch = 0; ch < numChannels; ch++) {
        inBuf[ch].resize(BUFLEN);
        outBuf[ch].resize(BUFLEN);
        out_hist[ch].resize(ORDER);
        for (int i = 0; i < BUFLEN; i++) {
            inBuf[ch][i] = 0;
            outBuf[ch][i] = 0;
        }
        for (int i = 0; i < ORDER; i++) {
            out_hist[ch][i] = 0;
        }
    }
    for (int i = 0; i < FRAMELEN; i++) {
        window[i] = 0.5f*(1.0f-cosf(2.0*M_PI*i/(float)(FRAMELEN-1)));
    }
    for (int i = 0; i < FRAMELEN; i++) {
        orderedInBuf[i] = 0;
    }
    for (int i = 0; i < ORDER; i++) {
        phi[i] = 0;
    }
    phi[ORDER] = 0;
    reset_a();
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

void LPC::applyLPC(float *inout, int numSamples, float lpcMix, float exPercentage, int ch, float exStartPos) {
    inPtr = inPtrs[ch];
    smpCnt = smpCnts[ch];
    outWtPtr = outWtPtrs[ch];
    outRdPtr = outRdPtrs[ch];
    if (exTypeChanged) {
        exPtrs[ch] = (int)(EXLEN*exStartPos);
        exCntPtrs[ch] = 0;
        histPtrs[ch] = 0;
    }
    if (orderChanged) {
        for (int i = 0; i < out_hist[ch].size(); i++) {
            out_hist[ch][i] = 0;
        }
//        histPtrs[ch] = 0;
    }
    exPtr = exPtrs[ch];
    exCntPtr = exCntPtrs[ch];
    histPtr = histPtrs[ch];
    int exStart = (int)(exStartPos*EXLEN);
    for (int s = 0; s < numSamples; s++) {
        inBuf[ch][inPtr] = inout[s];
        inPtr++;
        if (inPtr >= BUFLEN) {
            inPtr = 0;
        }
        float out = outBuf[ch][outRdPtr];
        inout[s] = lpcMix*out+(1-lpcMix)*inout[s];
        outBuf[ch][outRdPtr] = 0;
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
                orderedInBuf[i] = window[i]*inBuf[ch][inBufIdx];
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
                double G = sqrt(err/ORDER)/FRAMELEN;
                for (int n = 0; n < FRAMELEN; n++) {
                    double ex = (*noise)[exPtr];
                    exPtr++;
                    exCntPtr++;
                    if (exCntPtr >= (int)(exPercentage*EXLEN)) {
                        exCntPtr = 0;
                        exPtr = exStart;
                    }
                    if (exPtr >= EXLEN) {
                        exPtr = 0;
                    }
                    double out_n = G*ex;
                    for (int k = 0; k < ORDER; k++) {
                        int idx = histPtr-k-1;
                        if (idx < 0) {
                            idx += ORDER;
                        }
                        out_n -= a[k+1]*out_hist[ch][idx];
                    }
                    out_hist[ch][histPtr] = out_n;
                    histPtr++;
                    if (histPtr >= ORDER) {
                        histPtr = 0;
                    }
                    unsigned long wtIdx = outWtPtr+n;
                    if (wtIdx >= BUFLEN) {
                        wtIdx -= BUFLEN;
                    }
                    outBuf[ch][wtIdx] += out_n;
                }
                outWtPtr += HOPSIZE;
                if (outWtPtr >= BUFLEN) {
                    outWtPtr -= BUFLEN;
                }
            }
        }
    }
    inPtrs[ch] = inPtr;
    smpCnts[ch] = smpCnt;
    outWtPtrs[ch] = outWtPtr;
    outRdPtrs[ch] = outRdPtr;
    exPtrs[ch] = exPtr;
    histPtrs[ch] = histPtr;
}

void LPC::reset_a() {
    for (int i = 0; i < ORDER+1; i++) {
        a[i] = 0;
    }
}
