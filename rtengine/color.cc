/*
*  This file is part of RawTherapee.
*
*  Copyright (c) 2004-2010 Gabor Horvath <hgabor@rawtherapee.com>
*
*  RawTherapee is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  RawTherapee is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with RawTherapee.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "rtengine.h"
#include "color.h"
#include "iccmatrices.h"
#include "mytime.h"
#include "sleef.c"
#include "opthelper.h"
#include "iccstore.h"

using namespace std;

namespace rtengine
{

extern const Settings* settings;

cmsToneCurve* Color::linearGammaTRC;
LUTf Color::cachef;
LUTf Color::cachefy;
LUTf Color::gamma2curve;

LUTf Color::gammatab;
LUTuc Color::gammatabThumb;
LUTf Color::igammatab_srgb;
LUTf Color::igammatab_srgb1;
LUTf Color::gammatab_srgb;
LUTf Color::gammatab_srgb1;

LUTf Color::denoiseGammaTab;
LUTf Color::denoiseIGammaTab;

LUTf Color::igammatab_24_17;
LUTf Color::gammatab_24_17a;
LUTf Color::gammatab_13_2;
LUTf Color::igammatab_13_2;
LUTf Color::gammatab_115_2;
LUTf Color::igammatab_115_2;
LUTf Color::gammatab_145_3;
LUTf Color::igammatab_145_3;

/*
 * Munsell Lch correction
 * Copyright (c) 2011  Jacques Desmis <jdesmis@gmail.com>
*/
// Munsell Lch LUTf : 195 LUT
// about 70% data are corrected with significative corrections
// almost all data are taken for BG, YR, G excepted a few extreme values with a slight correction
// No LUTf for BG and Y : low corrections
// Only between 5B and 5PB for L > 40 : under very low corrections for L < 40

//give hue in function of L and C : Munsell  correction
LUTf Color::_4P10, Color::_4P20, Color::_4P30, Color::_4P40, Color::_4P50, Color::_4P60;
LUTf Color::_1P10, Color::_1P20, Color::_1P30, Color::_1P40, Color::_1P50, Color::_1P60;
LUTf Color::_10PB10, Color::_10PB20, Color::_10PB30, Color::_10PB40, Color::_10PB50, Color::_10PB60;
LUTf Color::_9PB10, Color::_9PB20, Color::_9PB30, Color::_9PB40, Color::_9PB50, Color::_9PB60, Color::_9PB70, Color::_9PB80;
LUTf Color::_75PB10, Color::_75PB20, Color::_75PB30, Color::_75PB40, Color::_75PB50, Color::_75PB60, Color::_75PB70, Color::_75PB80;
LUTf Color::_6PB10, Color::_6PB20, Color::_6PB30, Color::_6PB40, Color::_6PB50, Color::_6PB60, Color::_6PB70, Color::_6PB80;
LUTf Color::_45PB10, Color::_45PB20, Color::_45PB30, Color::_45PB40, Color::_45PB50, Color::_45PB60, Color::_45PB70, Color::_45PB80;
LUTf Color::_3PB10, Color::_3PB20, Color::_3PB30, Color::_3PB40, Color::_3PB50, Color::_3PB60, Color::_3PB70, Color::_3PB80;
LUTf Color::_15PB10, Color::_15PB20, Color::_15PB30, Color::_15PB40, Color::_15PB50, Color::_15PB60, Color::_15PB70, Color::_15PB80;
LUTf Color::_05PB40, Color::_05PB50, Color::_05PB60, Color::_05PB70, Color::_05PB80;
LUTf Color::_10B40, Color::_10B50, Color::_10B60, Color::_10B70, Color::_10B80;
LUTf Color::_9B40, Color::_9B50, Color::_9B60, Color::_9B70, Color::_9B80;
LUTf Color::_7B40, Color::_7B50, Color::_7B60, Color::_7B70, Color::_7B80;
LUTf Color::_5B40, Color::_5B50, Color::_5B60, Color::_5B70, Color::_5B80;
LUTf Color::_10YR20, Color::_10YR30, Color::_10YR40, Color::_10YR50, Color::_10YR60, Color::_10YR70, Color::_10YR80, Color::_10YR90;
LUTf Color::_85YR20, Color::_85YR30, Color::_85YR40, Color::_85YR50, Color::_85YR60, Color::_85YR70, Color::_85YR80, Color::_85YR90;
LUTf Color::_7YR30, Color::_7YR40, Color::_7YR50, Color::_7YR60, Color::_7YR70, Color::_7YR80;
LUTf Color::_55YR30, Color::_55YR40, Color::_55YR50, Color::_55YR60, Color::_55YR70, Color::_55YR80, Color::_55YR90;
LUTf Color::_4YR30, Color::_4YR40, Color::_4YR50, Color::_4YR60, Color::_4YR70, Color::_4YR80;
LUTf Color::_25YR30, Color::_25YR40, Color::_25YR50, Color::_25YR60, Color::_25YR70;
LUTf Color::_10R30, Color::_10R40, Color::_10R50, Color::_10R60, Color::_10R70;
LUTf Color::_9R30, Color::_9R40, Color::_9R50, Color::_9R60, Color::_9R70;
LUTf Color::_7R30, Color::_7R40, Color::_7R50, Color::_7R60, Color::_7R70;
LUTf Color::_5R10, Color::_5R20, Color::_5R30;
LUTf Color::_25R10, Color::_25R20, Color::_25R30;
LUTf Color::_10RP10, Color::_10RP20, Color::_10RP30;
LUTf Color::_7G30, Color::_7G40, Color::_7G50, Color::_7G60, Color::_7G70, Color::_7G80;
LUTf Color::_5G30, Color::_5G40, Color::_5G50, Color::_5G60, Color::_5G70, Color::_5G80;
LUTf Color::_25G30, Color::_25G40, Color::_25G50, Color::_25G60, Color::_25G70, Color::_25G80;
LUTf Color::_1G30, Color::_1G40, Color::_1G50, Color::_1G60, Color::_1G70, Color::_1G80;
LUTf Color::_10GY30, Color::_10GY40, Color::_10GY50, Color::_10GY60, Color::_10GY70, Color::_10GY80;
LUTf Color::_75GY30, Color::_75GY40, Color::_75GY50, Color::_75GY60, Color::_75GY70, Color::_75GY80;
LUTf Color::_5GY30, Color::_5GY40, Color::_5GY50, Color::_5GY60, Color::_5GY70, Color::_5GY80;

#ifdef _DEBUG
MunsellDebugInfo::MunsellDebugInfo()
{
    reinitValues();
}
void MunsellDebugInfo::reinitValues()
{
    maxdhue[0] = maxdhue[1] = maxdhue[2] = maxdhue[3] = 0.0f;
    maxdhuelum[0] = maxdhuelum[1] = maxdhuelum[2] = maxdhuelum[3] = 0.0f;
    depass = depassLum = 0;
}
#endif


void Color::init ()
{

    /*******************************************/

    constexpr auto maxindex = 65536;

    cachef(maxindex, LUT_CLIP_BELOW);
    cachefy(maxindex, LUT_CLIP_BELOW);
    gammatab(maxindex, 0);
    gammatabThumb(maxindex, 0);

    igammatab_srgb(maxindex, 0);
    igammatab_srgb1(maxindex, 0);
    gammatab_srgb(maxindex, 0);
    gammatab_srgb1(maxindex, 0);

    denoiseGammaTab(maxindex, 0);
    denoiseIGammaTab(maxindex, 0);

    igammatab_24_17(maxindex, 0);
    gammatab_24_17a(maxindex, LUT_CLIP_ABOVE | LUT_CLIP_BELOW);
    gammatab_13_2(maxindex, 0);
    igammatab_13_2(maxindex, 0);
    gammatab_115_2(maxindex, 0);
    igammatab_115_2(maxindex, 0);
    gammatab_145_3(maxindex, 0);
    igammatab_145_3(maxindex, 0);

#ifdef _OPENMP
    #pragma omp parallel sections
#endif
    {
#ifdef _OPENMP
        #pragma omp section
#endif
        {
            int i = 0;
            int epsmaxint = eps_max;

            for (; i <= epsmaxint; i++)
            {
                cachef[i] = 327.68 * ((kappa * i / MAXVALF + 16.0) / 116.0);
            }

            for(; i < maxindex; i++)
            {
                cachef[i] = 327.68 * std::cbrt((double)i / MAXVALF);
            }
        }
#ifdef _OPENMP
        #pragma omp section
#endif
        {
            int i = 0;
            int epsmaxint = eps_max;

            for (; i <= epsmaxint; i++)
            {
                cachefy[i] = 327.68 * (kappa * i / MAXVALF);
            }

            for(; i < maxindex; i++)
            {
                cachefy[i] = 327.68 * (116.0 * std::cbrt((double)i / MAXVALF) - 16.0);
            }
        }
#ifdef _OPENMP
        #pragma omp section
#endif
        {
            for (int i = 0; i < maxindex; i++)
            {
                gammatab_srgb[i] = gammatab_srgb1[i] = gamma2(i / 65535.0);
            }
            gammatab_srgb *= 65535.f;
            gamma2curve.share(gammatab_srgb, LUT_CLIP_BELOW | LUT_CLIP_ABOVE); // shares the buffer with gammatab_srgb but has different clip flags
        }
#ifdef _OPENMP
        #pragma omp section
#endif
        {
            for (int i = 0; i < maxindex; i++)
            {
                igammatab_srgb[i] = igammatab_srgb1[i] = igamma2 (i / 65535.0);
            }

            igammatab_srgb *= 65535.f;
        }
#ifdef _OPENMP
        #pragma omp section
#endif
        {
            double rsRGBGamma = 1.0 / sRGBGamma;

            for (int i = 0; i < maxindex; i++)
            {
                double val = pow (i / 65535.0, rsRGBGamma);
                gammatab[i] = 65535.0 * val;
                gammatabThumb[i] = (unsigned char)(255.0 * val);
            }
        }

#ifdef _OPENMP
        #pragma omp section
#endif
        // modify arbitrary data for Lab..I have test : nothing, gamma 2.6 11 - gamma 4 5 - gamma 5.5 10
        // we can put other as gamma g=2.6 slope=11, etc.
        // but noting to do with real gamma !!!: it's only for data Lab # data RGB
        // finally I opted for gamma55 and with options we can change

        switch(settings->denoiselabgamma) {
            case 0:
                for (int i = 0; i < maxindex; i++) {
                    denoiseGammaTab[i] = 65535.0 * gamma26_11 (i / 65535.0);
                }

                break;

            case 1:
                for (int i = 0; i < maxindex; i++) {
                    denoiseGammaTab[i] = 65535.0 * gamma4 (i / 65535.0);
                }

                break;

            default:
                for (int i = 0; i < maxindex; i++) {
                    denoiseGammaTab[i] = 65535.0 * gamma55 (i / 65535.0);
                }

                break;
        }

#ifdef _OPENMP
        #pragma omp section
#endif
        // modify arbitrary data for Lab..I have test : nothing, gamma 2.6 11 - gamma 4 5 - gamma 5.5 10
        // we can put other as gamma g=2.6 slope=11, etc.
        // but noting to do with real gamma !!!: it's only for data Lab # data RGB
        // finally I opted for gamma55 and with options we can change

        switch(settings->denoiselabgamma) {
            case 0:
                for (int i = 0; i < maxindex; i++) {
                    denoiseIGammaTab[i] = 65535.0 * igamma26_11 (i / 65535.0);
                }

                break;

            case 1:
                for (int i = 0; i < maxindex; i++) {
                    denoiseIGammaTab[i] = 65535.0 * igamma4 (i / 65535.0);
                }

                break;

            default:
                for (int i = 0; i < maxindex; i++) {
                    denoiseIGammaTab[i] = 65535.0 * igamma55 (i / 65535.0);
                }

                break;
        }

#ifdef _OPENMP
        #pragma omp section
#endif

        for (int i = 0; i < maxindex; i++) {
            gammatab_13_2[i] = 65535.0 * gamma13_2 (i / 65535.0);
        }

#ifdef _OPENMP
        #pragma omp section
#endif

        for (int i = 0; i < maxindex; i++) {
            igammatab_13_2[i] = 65535.0 * igamma13_2 (i / 65535.0);
        }

#ifdef _OPENMP
        #pragma omp section
#endif

        for (int i = 0; i < maxindex; i++) {
            gammatab_115_2[i] = 65535.0 * gamma115_2 (i / 65535.0);
        }

#ifdef _OPENMP
        #pragma omp section
#endif

        for (int i = 0; i < maxindex; i++) {
            igammatab_115_2[i] = 65535.0 * igamma115_2 (i / 65535.0);
        }

#ifdef _OPENMP
        #pragma omp section
#endif

        for (int i = 0; i < maxindex; i++) {
            gammatab_145_3[i] = 65535.0 * gamma145_3 (i / 65535.0);
        }

#ifdef _OPENMP
        #pragma omp section
#endif

        for (int i = 0; i < maxindex; i++) {
            igammatab_145_3[i] = 65535.0 * igamma145_3 (i / 65535.0);
        }

#ifdef _OPENMP
        #pragma omp section
#endif

        for (int i = 0; i < maxindex; i++) {
            gammatab_24_17a[i] = gamma24_17(i / 65535.0);
        }

#ifdef _OPENMP
        #pragma omp section
#endif

        for (int i = 0; i < maxindex; i++) {
            igammatab_24_17[i] = 65535.0 * igamma24_17 (i / 65535.0);
        }

#ifdef _OPENMP
        #pragma omp section
#endif
        initMunsell();

#ifdef _OPENMP
        #pragma omp section
#endif
        linearGammaTRC = cmsBuildGamma(nullptr, 1.0);
    }
}

void Color::cleanup ()
{
    if (linearGammaTRC) {
        cmsFreeToneCurve(linearGammaTRC);
    }
}

void Color::rgb2lab01 (const Glib::ustring &profile, const Glib::ustring &profileW, float r, float g, float b, float &LAB_l, float &LAB_a, float &LAB_b, bool workingSpace)
{ // do not use this function in a loop. It really eats processing time caused by Glib::ustring comparisons

    Glib::ustring profileCalc = "sRGB"; //default

    if (workingSpace) {//display working profile
        profileCalc = profileW;
        if (profileW == "sRGB") { //apply sRGB inverse gamma

            if (r > 0.04045f) {
                r = xpowf(((r + 0.055f) / 1.055f), rtengine::Color::sRGBGammaCurve);
            } else {
                r /= 12.92f;
            }

            if (g > 0.04045f) {
                g = xpowf(((g + 0.055f) / 1.055f), rtengine::Color::sRGBGammaCurve);
            } else {
                g /= 12.92f;
            }

            if (b > 0.04045f) {
                b = xpowf(((b + 0.055f) / 1.055f), rtengine::Color::sRGBGammaCurve);
            } else {
                b /= 12.92f;
            }
        } else if (profileW == "ProPhoto") { // apply inverse gamma 1.8
            r = xpowf(r, 1.8f);
            g = xpowf(g, 1.8f);
            b = xpowf(b, 1.8f);
        } else if (profileW == "Rec2020") {
            if (r > 0.0795f) {
                r = xpowf(((r + 0.0954f) / 1.0954f), 2.2f);
            } else {
                r /= 4.5f;
            }

            if (g > 0.0795f) {
                g = xpowf(((g + 0.0954f) / 1.0954f), 2.2f);
            } else {
                g /= 4.5f;
            }

            if (b > 0.0795f) {
                b = xpowf(((b + 0.0954f) / 1.0954f), 2.2f);
            } else {
                b /= 4.5f;
            }
        } else { // apply inverse gamma 2.2
            r = xpowf(r, 2.2f);
            g = xpowf(g, 2.2f);
            b = xpowf(b, 2.2f);
        }
    } else { //display output profile
        if (profile == settings->srgb) {
            // use default "sRGB"
        } else if (profile == "ProPhoto" || profile == settings->prophoto) {
            profileCalc = "ProPhoto";
        } else if (profile == "AdobeRGB1998" || profile == settings->adobe) {
            profileCalc = "Adobe RGB";
        } else if (profile == settings->widegamut) {
            profileCalc = "WideGamut";
        }

        if (profile == settings->srgb || profile == settings->adobe) { //apply sRGB inverse gamma
            if (r > 0.04045f) {
                r = xpowf(((r + 0.055f) / 1.055f), rtengine::Color::sRGBGammaCurve);
            } else {
                r /= 12.92f;
            }

            if (g > 0.04045f) {
                g = xpowf(((g + 0.055f) / 1.055f), rtengine::Color::sRGBGammaCurve);
            } else {
                g /= 12.92f;
            }

            if (b > 0.04045f) {
                b = xpowf(((b + 0.055f) / 1.055f), rtengine::Color::sRGBGammaCurve);
            } else {
                b /= 12.92f;
            }
        } else if (profile == settings->prophoto || profile == settings->rec2020) {
            if (r > 0.0795f) {
                r = xpowf(((r + 0.0954f) / 1.0954f), 2.2f);
            } else {
                r /= 4.5f;
            }

            if (g > 0.0795f) {
                g = xpowf(((g + 0.0954f) / 1.0954f), 2.2f);
            } else {
                g /= 4.5f;
            }

            if (b > 0.0795f) {
                b = xpowf(((b + 0.0954f) / 1.0954f), 2.2f);
            } else {
                b /= 4.5f;
            }

        } else if (profile == "ProPhoto") { // apply inverse gamma 1.8

            r = xpowf(r, 1.8f);
            g = xpowf(g, 1.8f);
            b = xpowf(b, 1.8f);
        } else {// apply inverse gamma 2.2

            r = xpowf(r, 2.2f);
            g = xpowf(g, 2.2f);
            b = xpowf(b, 2.2f);
        }
    }

    const TMatrix wprof = rtengine::ICCStore::getInstance()->workingSpaceMatrix(profileCalc);

    const float xyz_rgb[3][3] = { {static_cast<float>(wprof[0][0]), static_cast<float>(wprof[0][1]), static_cast<float>(wprof[0][2])},
                                  {static_cast<float>(wprof[1][0]), static_cast<float>(wprof[1][1]), static_cast<float>(wprof[1][2])},
                                  {static_cast<float>(wprof[2][0]), static_cast<float>(wprof[2][1]), static_cast<float>(wprof[2][2])}
                                };

    const float var_X = (xyz_rgb[0][0] * r + xyz_rgb[0][1] * g + xyz_rgb[0][2] * b) / Color::D50x;
    const float var_Y = (xyz_rgb[1][0] * r + xyz_rgb[1][1] * g + xyz_rgb[1][2] * b);
    const float var_Z = (xyz_rgb[2][0] * r + xyz_rgb[2][1] * g + xyz_rgb[2][2] * b) / Color::D50z;

    const float varxx = var_X > epsf ? xcbrtf(var_X) : (kappaf * var_X  + 16.f) / 116.f ;
    const float varyy = var_Y > epsf ? xcbrtf(var_Y) : (kappaf * var_Y  + 16.f) / 116.f ;
    const float varzz = var_Z > epsf ? xcbrtf(var_Z) : (kappaf * var_Z  + 16.f) / 116.f ;

    LAB_l = var_Y > epsf ? (xcbrtf(var_Y) * 116.f) - 16.f : kappaf * var_Y;
    LAB_a = 500.f * (varxx - varyy);
    LAB_b = 200.f * (varyy - varzz);

}

void Color::rgb2hsl(float r, float g, float b, float &h, float &s, float &l)
{

    double var_R = double(r) / 65535.0;
    double var_G = double(g) / 65535.0;
    double var_B = double(b) / 65535.0;

    double m = min(var_R, var_G, var_B);
    double M = max(var_R, var_G, var_B);
    double C = M - m;

    double l_ = (M + m) / 2.;
    l = float(l_);

    if (C < 0.00001 && C > -0.00001) { // no fabs, slow!
        h = 0.f;
        s = 0.f;
    } else {
        double h_;

        if (l_ <= 0.5) {
            s = float( (M - m) / (M + m) );
        } else {
            s = float( (M - m) / (2.0 - M - m) );
        }

        if      ( var_R == M ) {
            h_ =      (var_G - var_B) / C;
        } else if ( var_G == M ) {
            h_ = 2. + (var_B - var_R) / C;
        } else {
            h_ = 4. + (var_R - var_G) / C;
        }

        h = float(h_ / 6.0);

        if ( h < 0.f ) {
            h += 1.f;
        }

        if ( h > 1.f ) {
            h -= 1.f;
        }
    }
}

#ifdef __SSE2__
void Color::rgb2hsl(vfloat r, vfloat g, vfloat b, vfloat &h, vfloat &s, vfloat &l)
{
    vfloat maxv = vmaxf(r, vmaxf(g, b));
    vfloat minv = vminf(r, vminf(g, b));
    vfloat C = maxv - minv;
    vfloat tempv = maxv + minv;
    l = (tempv) * f2v(7.6295109e-6f);
    s = (maxv - minv);
    s /= vself(vmaskf_gt(l, f2v(0.5f)), f2v(131070.f) - tempv, tempv);

    h = f2v(4.f) * C + r - g;
    h = vself(vmaskf_eq(g, maxv), f2v(2.f) * C + b - r, h);
    h = vself(vmaskf_eq(r, maxv), g - b, h);

    h /= (f2v(6.f) * C);
    vfloat onev = f2v(1.f);
    h = vself(vmaskf_lt(h, zerov), h + onev, h);

    vmask zeromask = vmaskf_lt(C, f2v(0.65535f));
    h = vself(zeromask, zerov, h);
    s = vself(zeromask, zerov, s);
}
#endif

double Color::hue2rgb(double p, double q, double t)
{
    if (t < 0.) {
        t += 6.;
    } else if( t > 6.) {
        t -= 6.;
    }

    if      (t < 1.) {
        return p + (q - p) * t;
    } else if (t < 3.) {
        return q;
    } else if (t < 4.) {
        return p + (q - p) * (4. - t);
    } else {
        return p;
    }
}

float Color::hue2rgbfloat(float p, float q, float t)
{
    if (t < 0.f) {
        t += 6.f;
    } else if( t > 6.f) {
        t -= 6.f;
    }

    if      (t < 1.f) {
        return p + (q - p) * t;
    } else if (t < 3.f) {
        return q;
    } else if (t < 4.f) {
        return p + (q - p) * (4.f - t);
    } else {
        return p;
    }
}

#ifdef __SSE2__
vfloat Color::hue2rgb(vfloat p, vfloat q, vfloat t)
{
    vfloat fourv = f2v(4.f);
    vfloat threev = f2v(3.f);
    vfloat sixv = threev + threev;
    t = vself(vmaskf_lt(t, zerov), t + sixv, t);
    t = vself(vmaskf_gt(t, sixv), t - sixv, t);

    vfloat temp1 = p + (q - p) * t;
    vfloat temp2 = p + (q - p) * (fourv - t);
    vfloat result = vself(vmaskf_lt(t, fourv), temp2, p);
    result = vself(vmaskf_lt(t, threev), q, result);
    return vself(vmaskf_lt(t, fourv - threev), temp1, result);
}
#endif

void Color::hsl2rgb (float h, float s, float l, float &r, float &g, float &b)
{

    if (s == 0) {
        r = g = b = 65535.0f * l;    //  achromatic
    } else {
        double m2;
        double h_ = double(h);
        double s_ = double(s);
        double l_ = double(l);

        if (l <= 0.5f) {
            m2 = l_ * (1.0 + s_);
        } else {
            m2 = l_ + s_ - l_ * s_;
        }

        double m1 = 2.0 * l_ - m2;

        r = float(65535.0 * hue2rgb (m1, m2, h_ * 6.0 + 2.0));
        g = float(65535.0 * hue2rgb (m1, m2, h_ * 6.0));
        b = float(65535.0 * hue2rgb (m1, m2, h_ * 6.0 - 2.0));
    }
}

#ifdef __SSE2__
void Color::hsl2rgb (vfloat h, vfloat s, vfloat l, vfloat &r, vfloat &g, vfloat &b)
{

    vfloat m2 = s * l;
    m2 = vself(vmaskf_gt(l, f2v(0.5f)), s - m2, m2);
    m2 += l;

    vfloat twov = f2v(2.f);
    vfloat c65535v = f2v(65535.f);
    vfloat m1 = l + l - m2;

    h *= f2v(6.f);
    r = c65535v * hue2rgb (m1, m2, h + twov);
    g = c65535v * hue2rgb (m1, m2, h);
    b = c65535v * hue2rgb (m1, m2, h - twov);

    vmask selectsMask = vmaskf_eq(zerov, s);
    vfloat lc65535v = c65535v * l;
    r = vself(selectsMask, lc65535v, r);
    g = vself(selectsMask, lc65535v, g);
    b = vself(selectsMask, lc65535v, b);
}
#endif

void Color::hsl2rgb01 (float h, float s, float l, float &r, float &g, float &b)
{

    if (s == 0) {
        r = g = b = l;    //  achromatic
    } else {
        double m2;
        double h_ = double(h);
        double s_ = double(s);
        double l_ = double(l);

        if (l <= 0.5f) {
            m2 = l_ * (1.0 + s_);
        } else {
            m2 = l_ + s_ - l_ * s_;
        }

        double m1 = 2.0 * l_ - m2;

        r = float(hue2rgb (m1, m2, h_ * 6.0 + 2.0));
        g = float(hue2rgb (m1, m2, h_ * 6.0));
        b = float(hue2rgb (m1, m2, h_ * 6.0 - 2.0));
    }
}

void Color::rgb2hsv(float r, float g, float b, float &h, float &s, float &v)
{
    const double var_R = r / 65535.0;
    const double var_G = g / 65535.0;
    const double var_B = b / 65535.0;

    const double var_Min = min(var_R, var_G, var_B);
    const double var_Max = max(var_R, var_G, var_B);
    const double del_Max = var_Max - var_Min;

    h = 0.f;
    v = var_Max;

    if (del_Max < 0.00001 && del_Max > -0.00001) { // no fabs, slow!
        s = 0.f;
    } else {
        s = del_Max / (var_Max == 0.0 ? 1.0 : var_Max);

        if (var_R == var_Max) {
            h = (var_G - var_B) / del_Max;
        } else if (var_G == var_Max) {
            h = 2.0 + (var_B - var_R) / del_Max;
        } else if (var_B == var_Max) {
            h = 4.0 + (var_R - var_G) / del_Max;
        }

        h /= 6.f;

        if (h < 0.f) {
            h += 1.f;
        }

        if (h > 1.f) {
            h -= 1.f;
        }
    }
}

void Color::rgb2hsv01(float r, float g, float b, float &h, float &s, float &v)
{

    const float minVal = min(r, g, b);
    v = max(r, g, b);
    const float delta = v - minVal;

    h = 0.f;

    if (delta < 0.00001f) {
        s = 0.f;
    } else {
        s = delta / (v == 0.f ? 1.f : v);

        if (r == v) {
            h = (g - b) / delta;
        } else if (g == v) {
            h = 2.f + (b - r) / delta;
        } else if (b == v) {
            h = 4.f + (r - g) / delta;
        }

        h /= 6.f;

        if (h < 0.f) {
            h += 1.f;
        }
    }
}

void Color::hsv2rgb (float h, float s, float v, float &r, float &g, float &b)
{

    float h1 = h * 6.f; // sector 0 to 5
    int i = (int)h1;  // floor() is very slow, and h1 is always >0
    float f = h1 - i; // fractional part of h

    float p = v * ( 1.f - s );
    float q = v * ( 1.f - s * f );
    float t = v * ( 1.f - s * ( 1.f - f ) );

    float r1, g1, b1;

    if      (i == 1)    {
        r1 = q;
        g1 = v;
        b1 = p;
    } else if (i == 2)    {
        r1 = p;
        g1 = v;
        b1 = t;
    } else if (i == 3)    {
        r1 = p;
        g1 = q;
        b1 = v;
    } else if (i == 4)    {
        r1 = t;
        g1 = p;
        b1 = v;
    } else if (i == 5)    {
        r1 = v;
        g1 = p;
        b1 = q;
    } else { /*i==(0|6)*/
        r1 = v;
        g1 = t;
        b1 = p;
    }

    r = ((r1) * 65535.0f);
    g = ((g1) * 65535.0f);
    b = ((b1) * 65535.0f);
}

// Function copied for speed concerns
// Not exactly the same as above ; this one return a result in the [0.0 ; 1.0] range
void Color::hsv2rgb01 (float h, float s, float v, float &r, float &g, float &b)
{
    float h1 = h * 6; // sector 0 to 5
    int i = int(h1);
    float f = h1 - i; // fractional part of h

    float p = v * ( 1 - s );
    float q = v * ( 1 - s * f );
    float t = v * ( 1 - s * ( 1 - f ) );

    if      (i == 1)    {
        r = q;
        g = v;
        b = p;
    } else if (i == 2)    {
        r = p;
        g = v;
        b = t;
    } else if (i == 3)    {
        r = p;
        g = q;
        b = v;
    } else if (i == 4)    {
        r = t;
        g = p;
        b = v;
    } else if (i == 5)    {
        r = v;
        g = p;
        b = q;
    } else { /*(i==0|6)*/
        r = v;
        g = t;
        b = p;
    }
}

void Color::hsv2rgb (float h, float s, float v, int &r, int &g, int &b)
{

    float h1 = h * 6; // sector 0 to 5
    int i = floor( h1 );
    float f = h1 - i; // fractional part of h

    float p = v * ( 1 - s );
    float q = v * ( 1 - s * f );
    float t = v * ( 1 - s * ( 1 - f ) );

    float r1, g1, b1;

    if (i == 0) {
        r1 = v;
        g1 = t;
        b1 = p;
    } else if (i == 1) {
        r1 = q;
        g1 = v;
        b1 = p;
    } else if (i == 2) {
        r1 = p;
        g1 = v;
        b1 = t;
    } else if (i == 3) {
        r1 = p;
        g1 = q;
        b1 = v;
    } else if (i == 4) {
        r1 = t;
        g1 = p;
        b1 = v;
    } else /*if (i == 5)*/ {
        r1 = v;
        g1 = p;
        b1 = q;
    }

    r = (int)( r1 * 65535);
    g = (int)( g1 * 65535);
    b = (int)( b1 * 65535);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Color::xyz2srgb (float x, float y, float z, float &r, float &g, float &b)
{

    //Transform to output color.  Standard sRGB is D65, but internal representation is D50
    //Note that it is only at this point that we should have need of clipping color data

    /*float x65 = d65_d50[0][0]*x + d65_d50[0][1]*y + d65_d50[0][2]*z ;
    float y65 = d65_d50[1][0]*x + d65_d50[1][1]*y + d65_d50[1][2]*z ;
    float z65 = d65_d50[2][0]*x + d65_d50[2][1]*y + d65_d50[2][2]*z ;

    r = sRGB_xyz[0][0]*x65 + sRGB_xyz[0][1]*y65 + sRGB_xyz[0][2]*z65;
    g = sRGB_xyz[1][0]*x65 + sRGB_xyz[1][1]*y65 + sRGB_xyz[1][2]*z65;
    b = sRGB_xyz[2][0]*x65 + sRGB_xyz[2][1]*y65 + sRGB_xyz[2][2]*z65;*/

    /*r = sRGBd65_xyz[0][0]*x + sRGBd65_xyz[0][1]*y + sRGBd65_xyz[0][2]*z ;
    g = sRGBd65_xyz[1][0]*x + sRGBd65_xyz[1][1]*y + sRGBd65_xyz[1][2]*z ;
    b = sRGBd65_xyz[2][0]*x + sRGBd65_xyz[2][1]*y + sRGBd65_xyz[2][2]*z ;*/

    r = ((sRGB_xyz[0][0] * x + sRGB_xyz[0][1] * y + sRGB_xyz[0][2] * z)) ;
    g = ((sRGB_xyz[1][0] * x + sRGB_xyz[1][1] * y + sRGB_xyz[1][2] * z)) ;
    b = ((sRGB_xyz[2][0] * x + sRGB_xyz[2][1] * y + sRGB_xyz[2][2] * z)) ;

}
void Color::xyz2Prophoto (float x, float y, float z, float &r, float &g, float &b)
{
    r = ((prophoto_xyz[0][0] * x + prophoto_xyz[0][1] * y + prophoto_xyz[0][2] * z)) ;
    g = ((prophoto_xyz[1][0] * x + prophoto_xyz[1][1] * y + prophoto_xyz[1][2] * z)) ;
    b = ((prophoto_xyz[2][0] * x + prophoto_xyz[2][1] * y + prophoto_xyz[2][2] * z)) ;
}
void Color::Prophotoxyz (float r, float g, float b, float &x, float &y, float &z)
{
    x = ((xyz_prophoto[0][0] * r + xyz_prophoto[0][1] * g + xyz_prophoto[0][2] * b)) ;
    y = ((xyz_prophoto[1][0] * r + xyz_prophoto[1][1] * g + xyz_prophoto[1][2] * b)) ;
    z = ((xyz_prophoto[2][0] * r + xyz_prophoto[2][1] * g + xyz_prophoto[2][2] * b)) ;
}

void Color::rgbxyz (float r, float g, float b, float &x, float &y, float &z, const double xyz_rgb[3][3])
{
    x = ((xyz_rgb[0][0] * r + xyz_rgb[0][1] * g + xyz_rgb[0][2] * b)) ;
    y = ((xyz_rgb[1][0] * r + xyz_rgb[1][1] * g + xyz_rgb[1][2] * b)) ;
    z = ((xyz_rgb[2][0] * r + xyz_rgb[2][1] * g + xyz_rgb[2][2] * b)) ;
}

void Color::rgbxyz (float r, float g, float b, float &x, float &y, float &z, const float xyz_rgb[3][3])
{
    x = ((xyz_rgb[0][0] * r + xyz_rgb[0][1] * g + xyz_rgb[0][2] * b)) ;
    y = ((xyz_rgb[1][0] * r + xyz_rgb[1][1] * g + xyz_rgb[1][2] * b)) ;
    z = ((xyz_rgb[2][0] * r + xyz_rgb[2][1] * g + xyz_rgb[2][2] * b)) ;
}

#ifdef __SSE2__
void Color::rgbxyz (vfloat r, vfloat g, vfloat b, vfloat &x, vfloat &y, vfloat &z, const vfloat xyz_rgb[3][3])
{
    x = ((xyz_rgb[0][0] * r + xyz_rgb[0][1] * g + xyz_rgb[0][2] * b)) ;
    y = ((xyz_rgb[1][0] * r + xyz_rgb[1][1] * g + xyz_rgb[1][2] * b)) ;
    z = ((xyz_rgb[2][0] * r + xyz_rgb[2][1] * g + xyz_rgb[2][2] * b)) ;
}
#endif

void Color::xyz2rgb (float x, float y, float z, float &r, float &g, float &b, const double rgb_xyz[3][3])
{
    //Transform to output color.  Standard sRGB is D65, but internal representation is D50
    //Note that it is only at this point that we should have need of clipping color data

    /*float x65 = d65_d50[0][0]*x + d65_d50[0][1]*y + d65_d50[0][2]*z ;
    float y65 = d65_d50[1][0]*x + d65_d50[1][1]*y + d65_d50[1][2]*z ;
    float z65 = d65_d50[2][0]*x + d65_d50[2][1]*y + d65_d50[2][2]*z ;

    r = sRGB_xyz[0][0]*x65 + sRGB_xyz[0][1]*y65 + sRGB_xyz[0][2]*z65;
    g = sRGB_xyz[1][0]*x65 + sRGB_xyz[1][1]*y65 + sRGB_xyz[1][2]*z65;
    b = sRGB_xyz[2][0]*x65 + sRGB_xyz[2][1]*y65 + sRGB_xyz[2][2]*z65;*/

    /*r = sRGBd65_xyz[0][0]*x + sRGBd65_xyz[0][1]*y + sRGBd65_xyz[0][2]*z ;
    g = sRGBd65_xyz[1][0]*x + sRGBd65_xyz[1][1]*y + sRGBd65_xyz[1][2]*z ;
    b = sRGBd65_xyz[2][0]*x + sRGBd65_xyz[2][1]*y + sRGBd65_xyz[2][2]*z ;*/

    r = ((rgb_xyz[0][0] * x + rgb_xyz[0][1] * y + rgb_xyz[0][2] * z)) ;
    g = ((rgb_xyz[1][0] * x + rgb_xyz[1][1] * y + rgb_xyz[1][2] * z)) ;
    b = ((rgb_xyz[2][0] * x + rgb_xyz[2][1] * y + rgb_xyz[2][2] * z)) ;
}

void Color::xyz2r (float x, float y, float z, float &r, const double rgb_xyz[3][3]) // for black & white we need only r channel
{
    //Transform to output color.  Standard sRGB is D65, but internal representation is D50
    //Note that it is only at this point that we should have need of clipping color data

    r = ((rgb_xyz[0][0] * x + rgb_xyz[0][1] * y + rgb_xyz[0][2] * z)) ;
}

// same for float
void Color::xyz2rgb (float x, float y, float z, float &r, float &g, float &b, const float rgb_xyz[3][3])
{
    r = ((rgb_xyz[0][0] * x + rgb_xyz[0][1] * y + rgb_xyz[0][2] * z)) ;
    g = ((rgb_xyz[1][0] * x + rgb_xyz[1][1] * y + rgb_xyz[1][2] * z)) ;
    b = ((rgb_xyz[2][0] * x + rgb_xyz[2][1] * y + rgb_xyz[2][2] * z)) ;
}

#ifdef __SSE2__
void Color::xyz2rgb (vfloat x, vfloat y, vfloat z, vfloat &r, vfloat &g, vfloat &b, const vfloat rgb_xyz[3][3])
{
    r = ((rgb_xyz[0][0] * x + rgb_xyz[0][1] * y + rgb_xyz[0][2] * z)) ;
    g = ((rgb_xyz[1][0] * x + rgb_xyz[1][1] * y + rgb_xyz[1][2] * z)) ;
    b = ((rgb_xyz[2][0] * x + rgb_xyz[2][1] * y + rgb_xyz[2][2] * z)) ;
}
#endif // __SSE2__

#ifdef __SSE2__
void Color::trcGammaBW (float &r, float &g, float &b, float gammabwr, float gammabwg, float gammabwb)
{
    // correct gamma for black and white image : pseudo TRC curve of ICC profile
    vfloat rgbv = f2v(0.f, r, r, r); // input channel is always r
    vfloat gammabwv = f2v(0.f, gammabwb, gammabwg, gammabwr);
    vfloat c65535v = f2v(65535.f);
    rgbv /= c65535v;
    rgbv = vmaxf(rgbv, zerov);
    rgbv = xpowf(rgbv, gammabwv);
    rgbv *= c65535v;
    float temp[4] ALIGNED16;
    stvf(temp[0], rgbv);
    r = temp[0];
    g = temp[1];
    b = temp[2];
}
void Color::trcGammaBWRow (float *r, float *g, float *b, int width, float gammabwr, float gammabwg, float gammabwb)
{
    // correct gamma for black and white image : pseudo TRC curve of ICC profile
    vfloat c65535v = f2v(65535.f);
    vfloat gammabwrv = f2v(gammabwr);
    vfloat gammabwgv = f2v(gammabwg);
    vfloat gammabwbv = f2v(gammabwb);
    int i = 0;
    for(; i < width - 3; i += 4 ) {
        vfloat inv = lvfu(r[i]); // input channel is always r
        inv /= c65535v;
        inv = vmaxf(inv, zerov);
        vfloat rv = xpowf(inv, gammabwrv);
        vfloat gv = xpowf(inv, gammabwgv);
        vfloat bv = xpowf(inv, gammabwbv);
        rv *= c65535v;
        gv *= c65535v;
        bv *= c65535v;
        stvfu(r[i], rv);
        stvfu(g[i], gv);
        stvfu(b[i], bv);
    }
    for(; i < width; i++) {
        trcGammaBW(r[i], g[i], b[i], gammabwr, gammabwg, gammabwb);
    }
}

#else
void Color::trcGammaBW (float &r, float &g, float &b, float gammabwr, float gammabwg, float gammabwb)
{
    // correct gamma for black and white image : pseudo TRC curve of ICC profile
    float in = r; // input channel is always r
    in /= 65535.0f;
    in = max(in, 0.f);
    b = xpowf (in, gammabwb);
    b *= 65535.0f;
    r = xpowf (in, gammabwr);
    r *= 65535.0f;
    g = xpowf (in, gammabwg);
    g *= 65535.0f;
}
#endif

/** @brief Compute the B&W constants for the B&W processing and its tool's GUI
 *
 * @param setting BlackWhite::setting
 * @param setting BlackWhite::filter
 */
void Color::computeBWMixerConstants (const Glib::ustring &setting, const Glib::ustring &filter,  const Glib::ustring &algo, float &filcor, float &mixerRed, float &mixerGreen,
                                     float &mixerBlue, float mixerOrange, float mixerYellow, float mixerCyan, float mixerPurple, float mixerMagenta,
                                     bool autoc, bool complement, float &kcorec, double &rrm, double &ggm, double &bbm)
{
    float somm;
    float som = mixerRed + mixerGreen + mixerBlue;

    if(som >= 0.f && som < 1.f) {
        som = 1.f;
    }

    if(som < 0.f && som > -1.f) {
        som = -1.f;
    }


    // rM = mixerRed, gM = mixerGreen, bM = mixerBlue !
    //presets
    if     (setting == "RGB-Abs" || setting == "ROYGCBPM-Abs") {
        kcorec = som / 100.f;
    }

    if (!autoc) {
        //if     (setting=="RGB-Abs" || setting=="ROYGCBPM-Abs")  {} //Keep the RGB mixer values as is!
        //else if(setting=="RGB-Rel" || setting=="ROYGCBPM-Rel")  {} //Keep the RGB mixer values as is!
        if     (setting == "NormalContrast")    {
            mixerRed = 43.f ;
            mixerGreen = 33.f;
            mixerBlue = 30.f;
        } else if(setting == "Panchromatic")      {
            mixerRed = 33.3f;
            mixerGreen = 33.3f;
            mixerBlue = 33.3f;
        } else if(setting == "HyperPanchromatic") {
            mixerRed = 41.f ;
            mixerGreen = 25.f;
            mixerBlue = 34.f;
        } else if(setting == "LowSensitivity")    {
            mixerRed = 27.f ;
            mixerGreen = 27.f;
            mixerBlue = 46.f;
        } else if(setting == "HighSensitivity")   {
            mixerRed = 30.f ;
            mixerGreen = 28.f;
            mixerBlue = 42.f;
        } else if(setting == "Orthochromatic")    {
            mixerRed = 0.f  ;
            mixerGreen = 42.f;
            mixerBlue = 58.f;
        } else if(setting == "HighContrast")      {
            mixerRed = 40.f ;
            mixerGreen = 34.f;
            mixerBlue = 60.f;
        } else if(setting == "Luminance")         {
            mixerRed = 30.f ;
            mixerGreen = 59.f;
            mixerBlue = 11.f;
        } else if(setting == "Landscape")         {
            mixerRed = 66.f ;
            mixerGreen = 24.f;
            mixerBlue = 10.f;
        } else if(setting == "Portrait")          {
            mixerRed = 54.f ;
            mixerGreen = 44.f;
            mixerBlue = 12.f;
        } else if(setting == "InfraRed")          {
            mixerRed = -40.f;
            mixerGreen = 200.f;
            mixerBlue = -17.f;
        }
    }

    rrm = mixerRed;
    ggm = mixerGreen;
    bbm = mixerBlue;

    somm = mixerRed + mixerGreen + mixerBlue;

    if(somm >= 0.f && somm < 1.f) {
        somm = 1.f;
    }

    if(somm < 0.f && somm > -1.f) {
        somm = -1.f;
    }

    mixerRed = mixerRed / somm;
    mixerGreen = mixerGreen / somm;
    mixerBlue = mixerBlue / somm;
    float koymcp = 0.f;

    if(setting == "ROYGCBPM-Abs" || setting == "ROYGCBPM-Rel") {
        float obM = 0.f;
        float ogM = 0.f;
        float orM = 0.f;

        float ybM = 0.f;
        float yrM = 0.f;
        float ygM = 0.f;

        float mgM = 0.f;
        float mrM = 0.f;
        float mbM = 0.f;

        float pgM = 0.f;
        float prM = 0.f;
        float pbM = 0.f;

        float crM = 0.f;
        float cgM = 0.f;
        float cbM = 0.f;
        //printf("mixred=%f\n",mixerRed);

        float fcompl = 1.f;

        if(complement && algo == "SP") {
            fcompl = 3.f;    //special
        } else if(complement && algo == "LI") {
            fcompl = 1.5f;    //linear
        }

        // ponderate filters: report to R=G=B=33
        // I ponder RGB channel, not only orange or yellow or cyan, etc...it's my choice !
        if(mixerOrange != 33) {
            if (algo == "SP") { //special
                if (mixerOrange >= 33) {
                    orM = fcompl * (mixerOrange * 0.67f - 22.11f) / 100.f;
                } else {
                    orM = fcompl * (-0.3f * mixerOrange + 9.9f) / 100.f;
                }

                if (mixerOrange >= 33) {
                    ogM = fcompl * (-0.164f * mixerOrange + 5.412f) / 100.f;
                } else {
                    ogM = fcompl * (0.4f * mixerOrange - 13.2f) / 100.f;
                }
            } else if (algo == "LI") { //linear
                orM = fcompl * (mixerOrange - 33.f) / 100.f;
                ogM = fcompl * (0.5f * mixerOrange - 16.5f) / 100.f;
            }

            if(complement) {
                obM = (-0.492f * mixerOrange + 16.236f) / 100.f;
            }

            mixerRed   += orM;
            mixerGreen += ogM;
            mixerBlue  += obM;
            koymcp += (orM + ogM + obM);
            //  printf("mixred+ORange=%f\n",mixerRed);

        }

        if(mixerYellow != 33) {
            if (algo == "SP") {
                yrM = fcompl * (-0.134f * mixerYellow + 4.422f) / 100.f;    //22.4
            } else if (algo == "LI") {
                yrM = fcompl * (0.5f * mixerYellow - 16.5f) / 100.f;    //22.4
            }

            ygM = fcompl * (0.5f  * mixerYellow - 16.5f ) / 100.f;

            if(complement) {
                ybM = (-0.492f * mixerYellow + 16.236f) / 100.f;
            }

            mixerRed   += yrM;
            mixerGreen += ygM;
            mixerBlue  += ybM;
            koymcp += (yrM + ygM + ybM);
        }

        if(mixerMagenta != 33) {
            if (algo == "SP") {
                if(mixerMagenta >= 33) {
                    mrM = fcompl * ( 0.67f * mixerMagenta - 22.11f) / 100.f;
                } else {
                    mrM = fcompl * (-0.3f * mixerMagenta + 9.9f) / 100.f;
                }

                if(mixerMagenta >= 33) {
                    mbM = fcompl * (-0.164f * mixerMagenta + 5.412f) / 100.f;
                } else {
                    mbM = fcompl * ( 0.4f * mixerMagenta - 13.2f) / 100.f;
                }
            } else if (algo == "LI") {
                mrM = fcompl * (mixerMagenta - 33.f) / 100.f;
                mbM = fcompl * (0.5f * mixerMagenta - 16.5f) / 100.f;
            }

            if(complement) {
                mgM = (-0.492f * mixerMagenta + 16.236f) / 100.f;
            }

            mixerRed   += mrM;
            mixerGreen += mgM;
            mixerBlue  += mbM;
            koymcp += (mrM + mgM + mbM);
        }

        if(mixerPurple != 33) {
            if (algo == "SP") {
                prM = fcompl * (-0.134f * mixerPurple + 4.422f) / 100.f;
            } else if (algo == "LI") {
                prM = fcompl * (0.5f * mixerPurple - 16.5f) / 100.f;
            }

            pbM = fcompl * (0.5f * mixerPurple - 16.5f) / 100.f;

            if(complement) {
                pgM = (-0.492f * mixerPurple + 16.236f) / 100.f;
            }

            mixerRed   += prM;
            mixerGreen += pgM;
            mixerBlue  += pbM;
            koymcp += (prM + pgM + pbM);
        }

        if(mixerCyan != 33) {
            if (algo == "SP") {
                cgM = fcompl * (-0.134f * mixerCyan + 4.422f) / 100.f;
            } else if (algo == "LI") {
                cgM = fcompl * (0.5f * mixerCyan - 16.5f) / 100.f;
            }

            cbM = fcompl * (0.5f * mixerCyan - 16.5f) / 100.f;

            if(complement) {
                crM = (-0.492f * mixerCyan + 16.236f) / 100.f;
            }

            mixerRed   += crM;
            mixerGreen += cgM;
            mixerBlue  += cbM;
            koymcp += (crM + cgM + cbM);
        }
    }

    if(setting == "ROYGCBPM-Abs") {
        kcorec = koymcp + som / 100.f;
    }

    //Color filters
    float filred, filgreen, filblue;
    filred = 1.f;
    filgreen = 1.f;
    filblue = 1.f;
    filcor = 1.f;

    if          (filter == "None")        {
        filred = 1.f;
        filgreen = 1.f;
        filblue = 1.f;
        filcor = 1.f;
    } else if     (filter == "Red")         {
        filred = 1.f;
        filgreen = 0.05f;
        filblue = 0.f;
        filcor = 1.08f;
    } else if     (filter == "Orange")      {
        filred = 1.f;
        filgreen = 0.6f;
        filblue = 0.f;
        filcor = 1.35f;
    } else if     (filter == "Yellow")      {
        filred = 1.f;
        filgreen = 1.f;
        filblue = 0.05f;
        filcor = 1.23f;
    } else if     (filter == "YellowGreen") {
        filred = 0.6f;
        filgreen = 1.f;
        filblue = 0.3f;
        filcor = 1.32f;
    } else if     (filter == "Green")       {
        filred = 0.2f;
        filgreen = 1.f;
        filblue = 0.3f;
        filcor = 1.41f;
    } else if     (filter == "Cyan")        {
        filred = 0.05f;
        filgreen = 1.f;
        filblue = 1.f;
        filcor = 1.23f;
    } else if     (filter == "Blue")        {
        filred = 0.f;
        filgreen = 0.05f;
        filblue = 1.f;
        filcor = 1.20f;
    } else if     (filter == "Purple")      {
        filred = 1.f;
        filgreen = 0.05f;
        filblue = 1.f;
        filcor = 1.23f;
    }

    mixerRed   = mixerRed * filred;
    mixerGreen = mixerGreen * filgreen;
    mixerBlue  = mixerBlue  * filblue;

    if(mixerRed + mixerGreen + mixerBlue == 0) {
        mixerRed += 1.f;
    }

    mixerRed   = filcor * mixerRed   / (mixerRed + mixerGreen + mixerBlue);
    mixerGreen = filcor * mixerGreen / (mixerRed + mixerGreen + mixerBlue);
    mixerBlue  = filcor * mixerBlue  / (mixerRed + mixerGreen + mixerBlue);

    if(filter != "None") {
        som = mixerRed + mixerGreen + mixerBlue;

        if(som >= 0.f && som < 1.f) {
            som = 1.f;
        }

        if(som < 0.f && som > -1.f) {
            som = -1.f;
        }

        if(setting == "RGB-Abs" || setting == "ROYGCBPM-Abs") {
            kcorec = kcorec * som;
        }
    }

}

void Color::interpolateRGBColor (const float balance, const float r1, const float g1, const float b1, const float r2, const float g2, const float b2, int toDo, const double xyz_rgb[3][3], const double rgb_xyz[3][3], float &ro, float &go, float &bo)
{
    float X1, Y1, Z1, X2, Y2, Z2, X, Y, Z;
    float L1, L2, a_1, b_1, a_2, b_2, a, b;
    float c1, c2, h1, h2;
    float RR, GG, BB;
    float Lr;

    // converting color 1 to Lch
    Color::rgbxyz(r1, g1, b1, X1, Y1, Z1, xyz_rgb);
    Color::XYZ2Lab(X1, Y1, Z1, L1, a_1, b_1);
    Color::Lab2Lch(a_1, b_1, c1, h1);
    Lr = L1 / 327.68f; //for gamutlch
    //gamut control on r1 g1 b1
#ifndef NDEBUG
    bool neg = false;
    bool more_rgb = false;

    //gamut control : Lab values are in gamut
    Color::gamutLchonly(h1, Lr, c1, RR, GG, BB, xyz_rgb, false, 0.15f, 0.96f, neg, more_rgb);
#else
    Color::gamutLchonly(h1, Lr, c1, RR, GG, BB, xyz_rgb, false, 0.15f, 0.96f);
#endif

    L1 = Lr * 327.68f;

    // converting color 2 to Lch
    Color::rgbxyz(r2, g2, b2, X2, Y2, Z2, xyz_rgb);
    Color::XYZ2Lab(X2, Y2, Z2, L2, a_2, b_2);
    Color::Lab2Lch(a_2, b_2, c2, h2);

    Lr = L2 / 327.68f; //for gamutlch
    //gamut control on r2 g2 b2
#ifndef NDEBUG
    neg = false;
    more_rgb = false;
    //gamut control : Lab values are in gamut
    Color::gamutLchonly(h2, Lr, c2, RR, GG, BB, xyz_rgb, false, 0.15f, 0.96f, neg, more_rgb);
#else
    Color::gamutLchonly(h2, Lr, c2, RR, GG, BB, xyz_rgb, false, 0.15f, 0.96f);
#endif
    L2 = Lr * 327.68f;

    // interpolating Lch values
    if (toDo & CHANNEL_LIGHTNESS)     {
        L1 = L1 + (L2 - L1) * balance;    //do not allow negative L

        if(L1 < 0.f) {
            L1 = 0.f;
        }
    }

    if (toDo & CHANNEL_CHROMATICITY)  {
        c1 = c1 + (c2 - c1) * balance;    //limit C to reasonable value

        if(c1 < 0.f) {
            c1 = 0.f;
        }

        if(c1 > 180.f) {
            c1 = 180.f;
        }
    }

    if (toDo & CHANNEL_HUE) {
        h1 = interpolatePolarHue_PI<float, float>(h1, h2, balance);
    }

    // here I have put gamut control with gamutlchonly  on final process
    Lr = L1 / 327.68f; //for gamutlch
#ifndef NDEBUG
    neg = false;
    more_rgb = false;
    //gamut control : Lab values are in gamut
    Color::gamutLchonly(h1, Lr, c1, RR, GG, BB, xyz_rgb, false, 0.15f, 0.96f, neg, more_rgb);
#else
    //gamut control : Lab values are in gamut
    Color::gamutLchonly(h1, Lr, c1, RR, GG, BB, xyz_rgb, false, 0.15f, 0.96f);
#endif
    //convert CH ==> ab
    L1 = Lr * 327.68f;

    // converting back to rgb
    Color::Lch2Lab(c1, h1, a, b);
    Color::Lab2XYZ(L1, a, b, X, Y, Z);
    Color::xyz2rgb(X, Y, Z, ro, go, bo, rgb_xyz);
}


void Color::interpolateRGBColor (float realL, float iplow, float iphigh, int algm, const float balance, int twoc, int metchrom,
                                 float chromat, float luma, const float r1, const float g1, const float b1,
                                 const float xl, const float yl, const float zl, const float x2, const float y2, const float z2,
                                 const double xyz_rgb[3][3], const double rgb_xyz[3][3], float &ro, float &go, float &bo)
{
    float X1, Y1, Z1, X2, Y2, Z2, X, Y, Z, XL, YL, ZL;
    float L1 = 0.f, L2, LL, a_1 = 0.f, b_1 = 0.f, a_2 = 0.f, b_2 = 0.f, a_L, b_L;

    // converting color 1 to Lab  (image)
    Color::rgbxyz(r1, g1, b1, X1, Y1, Z1, xyz_rgb);

    if(algm == 1) {//use H interpolate
        Color::XYZ2Lab(X1, Y1, Z1, L1, a_1, b_1);
        //Color::Lab2Lch(a_1, b_1, c_1, h_1) ;
    }

    // converting color l lab(low) first color
    if(twoc == 0) { // 2 colours
        //Color::rgbxyz(rl, gl, bl, XL, YL, ZL, xyz_rgb);
        XL = xl;
        YL = yl;
        ZL = zl;

        if(algm <= 1) {//use H interpolate
            Color::XYZ2Lab(XL, YL, ZL, LL, a_L, b_L);
        }
    }

    // converting color 2 to lab (universal or high)
    X2 = x2;
    Y2 = y2;
    Z2 = z2;

    if(algm == 1 ) {
        Color::XYZ2Lab(X2, Y2, Z2, L2, a_2, b_2);
        //Color::Lab2Lch(a_2, b_2, c_2, h_2) ;
    }

    float cal, calH, calm;
    cal = calH = calm = 1.f - chromat;
    float med = 1.f;
    float medH = 0.f;

    float calan;
    calan = chromat;

    float calby;
    calby = luma;

    if(twoc == 0) { // 2 colours
        calan = chromat;

        //calculate new balance chroma
        if      (realL > iplow && realL <= med) {
            cal = realL * calan / (iplow - med) - med * calan / (iplow - med);
        } else if (realL <= iplow) {
            cal = realL * calan / iplow;
        }

        if      (realL > medH && realL <= iphigh) {
            calH = realL * calan / (iphigh - medH) - medH * calan / (iphigh - medH);
        } else if (realL > iphigh) {
            calH = realL * calan;    //*(iphigh-1.f) - calan*(iphigh-1.f);//it is better without transition in highlight
        }
    }

    float aaH, bbH;

    if(algm <= 1) {
        if(twoc == 0  && metchrom == 3) { // 2 colours  only with special "ab"
            if(algm == 1) {
                aaH = a_1 + (a_2 - a_1) * calH;
                bbH = b_1 + (b_2 - b_1) * calH; //pass to line after
                a_1 = aaH + (a_L - aaH) * cal * balance;
                b_1 = bbH + (b_L - bbH) * cal * balance;
            }
        } else if(twoc == 1) {
            if(metchrom == 0) {
                a_1 = a_1 + (a_2 - a_1) * balance;
                b_1 = b_1 + (b_2 - b_1) * balance;
            } else if(metchrom == 1) {
                a_1 = a_1 + (a_2 - a_1) * calan * balance;
                b_1 = b_1 + (b_2 - b_1) * calan * balance;
            } else if(metchrom == 2) {
                a_1 = a_1 + (a_2 - a_1) * calan * balance;
                b_1 = b_1 + (b_2 - b_1) * calby * balance;
            }
        }
    }

    Color::Lab2XYZ(L1, a_1, b_1, X, Y, Z);

    Color::xyz2rgb(X, Y, Z, ro, go, bo, rgb_xyz);// ro go bo in gamut
}

void Color::calcGamma (double pwr, double ts, int mode, GammaValues &gamma)
{
    //from Dcraw (D.Coffin)
    int i;
    double g[6], bnd[2] = {0., 0.};

    g[0] = pwr;
    g[1] = ts;
    g[2] = g[3] = g[4] = 0.;
    bnd[g[1] >= 1.] = 1.;

    if (g[1] && (g[1] - 1.) * (g[0] - 1.) <= 0.) {
        for (i = 0; i < 99; i++) {
            g[2] = (bnd[0] + bnd[1]) / 2.;

            if (g[0]) {
                bnd[(pow(g[2] / g[1], -g[0]) - 1.) / g[0] - 1. / g[2] > -1.] = g[2];
            } else {
                bnd[g[2] / exp(1. - 1. / g[2]) < g[1]] = g[2];
            }
        }

        g[3] = g[2] / g[1];

        if (g[0]) {
            g[4] = g[2] * (1. / g[0] - 1.);
        }
    }

    if (g[0]) {
        g[5] = 1. / (g[1] * SQR(g[3]) / 2. - g[4] * (1. - g[3]) + (1. - pow(g[3], 1. + g[0])) * (1. + g[4]) / (1. + g[0])) - 1.;
    } else {
        g[5] = 1. / (g[1] * SQR(g[3]) / 2. + 1. - g[2] - g[3] - g[2] * g[3] * (log(g[3]) - 1.)) - 1.;
    }

    if (!mode--) {
        gamma[0] = g[0];
        gamma[1] = g[1];
        gamma[2] = g[2];
        gamma[3] = g[3];
        gamma[4] = g[4];
        gamma[5] = g[5];
        gamma[6] = 0.;
        return;
    }
}
void Color::gammaf2lut (LUTf &gammacurve, float gamma, float start, float slope, float divisor, float factor)
{
#ifdef __SSE2__
    // SSE2 version is more than 6 times faster than scalar version
    vfloat iv = f2v(3.f, 2.f, 1.f, 0.f);
    vfloat fourv = f2v(4.f);
    vfloat gammav = f2v(1.f / gamma);
    vfloat slopev = f2v((slope / divisor) * factor);
    vfloat divisorv = f2v(xlogf(divisor));
    vfloat factorv = f2v(factor);
    vfloat comparev = f2v(start * divisor);
    int border = start * divisor;
    int border1 = border - (border & 3);
    int border2 = border1 + 4;
    int i = 0;

    for(; i < border1; i += 4) {
        vfloat resultv = iv * slopev;
        stvfu(gammacurve[i], resultv);
        iv += fourv;
    }

    for(; i < border2; i += 4) {
        vfloat result0v = iv * slopev;
        vfloat result1v = xexpf((xlogf(iv) - divisorv) * gammav) * factorv;
        stvfu(gammacurve[i], vself(vmaskf_le(iv, comparev), result0v, result1v));
        iv += fourv;
    }

    for(; i < 65536; i += 4) {
        vfloat resultv = xexpfNoCheck((xlogfNoCheck(iv) - divisorv) * gammav) * factorv;
        stvfu(gammacurve[i], resultv);
        iv += fourv;
    }

#else

    for (int i = 0; i < 65536; ++i) {
        gammacurve[i] = gammaf(static_cast<float>(i) / divisor, gamma, start, slope) * factor;
    }

#endif
}

void Color::gammanf2lut (LUTf &gammacurve, float gamma, float divisor, float factor)           //standard gamma without slope...
{
#ifdef __SSE2__
    // SSE2 version is more than 6 times faster than scalar version
    vfloat iv = f2v(3.f, 2.f, 1.f, 0.f);
    vfloat fourv = f2v(4.f);
    vfloat gammav = f2v(1.f / gamma);
    vfloat divisorv = f2v(xlogf(divisor));
    vfloat factorv = f2v(factor);

    // first input value is zero => we have to use the xlogf function which checks this
    vfloat resultv = xexpf((xlogf(iv) - divisorv) * gammav) * factorv;
    stvfu(gammacurve[0], resultv);
    iv += fourv;

    // inside the loop we can use xlogfNoCheck and xexpfNoCheck because we know about the input values
    for(int i = 4; i < 65536; i += 4) {
        resultv = xexpfNoCheck((xlogfNoCheck(iv) - divisorv) * gammav) * factorv;
        stvfu(gammacurve[i], resultv);
        iv += fourv;
    }

#else

    for (int i = 0; i < 65536; ++i) {
        gammacurve[i] = Color::gammanf(static_cast<float>(i) / divisor, gamma) * factor;
    }

#endif
}

void Color::Lab2XYZ(float L, float a, float b, float &x, float &y, float &z)
{
    float LL = L / 327.68f;
    float aa = a / 327.68f;
    float bb = b / 327.68f;
    float fy = (c1By116 * LL) + c16By116; // (L+16)/116
    float fx = (0.002f * aa) + fy;
    float fz = fy - (0.005f * bb);
    x = 65535.0f * f2xyz(fx) * D50x;
    z = 65535.0f * f2xyz(fz) * D50z;
    y = (LL > epskap) ? 65535.0f * fy * fy * fy : 65535.0f * LL / kappa;
}

void Color::L2XYZ(float L, float &x, float &y, float &z) // for black & white
{
    float LL = L / 327.68f;
    float fy = (c1By116 * LL) + c16By116; // (L+16)/116
    float fxz = 65535.f * f2xyz(fy);
    x = fxz * D50x;
    z = fxz * D50z;
    y = (LL > epskap) ? 65535.0f * fy * fy * fy : 65535.0f * LL / kappa;
}


#ifdef __SSE2__
void Color::Lab2XYZ(vfloat L, vfloat a, vfloat b, vfloat &x, vfloat &y, vfloat &z)
{
    vfloat c327d68 = f2v(327.68f);
    L /= c327d68;
    a /= c327d68;
    b /= c327d68;
    vfloat fy = f2v(c1By116) * L + f2v(c16By116);
    vfloat fx = f2v(0.002f) * a + fy;
    vfloat fz = fy - (f2v(0.005f) * b);
    vfloat c65535 = f2v(65535.f);
    x = c65535 * f2xyz(fx) * f2v(D50x);
    z = c65535 * f2xyz(fz) * f2v(D50z);
    vfloat res1 = fy * fy * fy;
    vfloat res2 = L / f2v(kappa);
    y = vself(vmaskf_gt(L, f2v(epskap)), res1, res2);
    y *= c65535;
}
#endif // __SSE2__

inline float Color::computeXYZ2Lab(float f)
{
    if (f < 0.f) {
        return 327.68 * ((kappa * f / MAXVALF + 16.0) / 116.0);
    } else if (f > 65535.f) {
        return (327.68f * xcbrtf(f / MAXVALF));
    } else {
        return cachef[f];
    }
}


inline float Color::computeXYZ2LabY(float f)
{
    if (f < 0.f) {
        return 327.68 * (kappa * f / MAXVALF);
    } else if (f > 65535.f) {
        return 327.68f * (116.f * xcbrtf(f / MAXVALF) - 16.f);
    } else {
        return cachefy[f];
    }
}


void Color::RGB2Lab(float *R, float *G, float *B, float *L, float *a, float *b, const float wp[3][3], int width)
{

#ifdef __SSE2__
    vfloat minvalfv = f2v(0.f);
    vfloat maxvalfv = f2v(MAXVALF);
    vfloat c500v = f2v(500.f);
    vfloat c200v = f2v(200.f);
#endif
    int i = 0;
    
#ifdef __SSE2__
    for(;i < width - 3; i+=4) {
        const vfloat rv = lvfu(R[i]);
        const vfloat gv = lvfu(G[i]);
        const vfloat bv = lvfu(B[i]);
        const vfloat xv = f2v(wp[0][0]) * rv + f2v(wp[0][1]) * gv + f2v(wp[0][2]) * bv;
        const vfloat yv = f2v(wp[1][0]) * rv + f2v(wp[1][1]) * gv + f2v(wp[1][2]) * bv;
        const vfloat zv = f2v(wp[2][0]) * rv + f2v(wp[2][1]) * gv + f2v(wp[2][2]) * bv;

        vmask maxMask = vmaskf_gt(vmaxf(xv, vmaxf(yv, zv)), maxvalfv);
        vmask minMask = vmaskf_lt(vminf(xv, vminf(yv, zv)), minvalfv);
        if (_mm_movemask_ps((vfloat)maxMask) || _mm_movemask_ps((vfloat)minMask)) {
            // take slower code path for all 4 pixels if one of the values is > MAXVALF. Still faster than non SSE2 version
            for(int k = 0; k < 4; ++k) {
                float x = xv[k];
                float y = yv[k];
                float z = zv[k];
                float fx = computeXYZ2Lab(x);
                float fy = computeXYZ2Lab(y);
                float fz = computeXYZ2Lab(z);

                L[i + k] = computeXYZ2LabY(y);
                a[i + k] = (500.f * (fx - fy) );
                b[i + k] = (200.f * (fy - fz) );
            }
        } else {
            const vfloat fx = cachef[xv];
            const vfloat fy = cachef[yv];
            const vfloat fz = cachef[zv];

            stvfu(L[i], cachefy[yv]);
            stvfu(a[i], c500v * (fx - fy));
            stvfu(b[i], c200v * (fy - fz));
        }
    }
#endif
    for(;i < width; ++i) {
        const float rv = R[i];
        const float gv = G[i];
        const float bv = B[i];
        float x = wp[0][0] * rv + wp[0][1] * gv + wp[0][2] * bv;
        float y = wp[1][0] * rv + wp[1][1] * gv + wp[1][2] * bv;
        float z = wp[2][0] * rv + wp[2][1] * gv + wp[2][2] * bv;
        float fx, fy, fz;

        fx = computeXYZ2Lab(x);
        fy = computeXYZ2Lab(y);
        fz = computeXYZ2Lab(z);

        L[i] = computeXYZ2LabY(y);
        a[i] = 500.0f * (fx - fy);
        b[i] = 200.0f * (fy - fz);
    }
}

void Color::RGB2L(float *R, float *G, float *B, float *L, const float wp[3][3], int width)
{

#ifdef __SSE2__
    vfloat minvalfv = f2v(0.f);
    vfloat maxvalfv = f2v(MAXVALF);
#endif
    int i = 0;
    
#ifdef __SSE2__
    for(;i < width - 3; i+=4) {
        const vfloat rv = lvfu(R[i]);
        const vfloat gv = lvfu(G[i]);
        const vfloat bv = lvfu(B[i]);
        const vfloat yv = f2v(wp[1][0]) * rv + f2v(wp[1][1]) * gv + f2v(wp[1][2]) * bv;

        vmask maxMask = vmaskf_gt(yv, maxvalfv);
        vmask minMask = vmaskf_lt(yv, minvalfv);
        if (_mm_movemask_ps((vfloat)vorm(maxMask, minMask))) {
            // take slower code path for all 4 pixels if one of the values is > MAXVALF. Still faster than non SSE2 version
            for(int k = 0; k < 4; ++k) {
                float y = yv[k];
                L[i + k] = computeXYZ2LabY(y);
            }
        } else {
            stvfu(L[i], cachefy[yv]);
        }
    }
#endif
    for(;i < width; ++i) {
        const float rv = R[i];
        const float gv = G[i];
        const float bv = B[i];
        float y = wp[1][0] * rv + wp[1][1] * gv + wp[1][2] * bv;

        L[i] = computeXYZ2LabY(y);
    }
}

void Color::XYZ2Lab(float X, float Y, float Z, float &L, float &a, float &b)
{

    float x = X / D50x;
    float z = Z / D50z;
    float y = Y;
    float fx, fy, fz;

    fx = computeXYZ2Lab(x);
    fy = computeXYZ2Lab(y);
    fz = computeXYZ2Lab(z);

    L = computeXYZ2LabY(y);
    a = (500.0f * (fx - fy) );
    b = (200.0f * (fy - fz) );
}

void Color::Lab2Yuv(float L, float a, float b, float &Y, float &u, float &v)
{
    float fy = (c1By116 * L / 327.68) + c16By116; // (L+16)/116
    float fx = (0.002 * a / 327.68) + fy;
    float fz = fy - (0.005 * b / 327.68);
    float LL = L / 327.68;

    float X = 65535.0 * f2xyz(fx) * D50x;
    // Y = 65535.0*f2xyz(fy);
    float Z = 65535.0 * f2xyz(fz) * D50z;
    Y = (LL / 327.68f > epskap) ? 65535.0 * fy * fy * fy : 65535.0 * LL / kappa;

    u = 4.0 * X / (X + 15 * Y + 3 * Z) - u0;
    v = 9.0 * Y / (X + 15 * Y + 3 * Z) - v0;
}

void Color::Yuv2Lab(float Yin, float u, float v, float &L, float &a, float &b, const double wp[3][3])
{
    float u1 = u + u0;
    float v1 = v + v0;

    float Y = Yin;
    float X = (9 * u1 * Y) / (4 * v1 * D50x);
    float Z = (12 - 3 * u1 - 20 * v1) * Y / (4 * v1 * D50z);

    gamutmap(X, Y, Z, wp);

    float fx = computeXYZ2Lab(X);
    float fy = computeXYZ2Lab(Y);
    float fz = computeXYZ2Lab(Z);

    L = computeXYZ2LabY(Y);
    a = (500.0 * (fx - fy) );
    b = (200.0 * (fy - fz) );
}

void Color::Lab2Lch(float a, float b, float &c, float &h)
{
    c = (sqrtf(a * a + b * b)) / 327.68f;
    h = xatan2f(b, a);
}

#ifdef __SSE2__
void Color::Lab2Lch(float *a, float *b, float *c, float *h, int w)
{
    int i = 0;
    vfloat c327d68v = f2v(327.68f);
    for (; i < w - 3; i += 4) {
        vfloat av = lvfu(a[i]);
        vfloat bv = lvfu(b[i]);
        stvfu(c[i], vsqrtf(SQRV(av) + SQRV(bv)) / c327d68v);
        stvfu(h[i], xatan2f(bv, av));
    }
    for (; i < w; ++i) {
        c[i] = sqrtf(SQR(a[i]) + SQR(b[i])) / 327.68f;
        h[i] = xatan2f(b[i], a[i]);
    }
}
#endif

void Color::Lch2Lab(float c, float h, float &a, float &b)
{
    float2 sincosval = xsincosf(h);
    a = 327.68f * c * sincosval.y;
    b = 327.68f * c * sincosval.x;
}

void Color::Luv2Lch(float u, float v, float &c, float &h)
{
    c = sqrtf(u * u + v * v);
    h = xatan2f(v, u);  //WARNING: should we care of division by zero here?

    if (h < 0.f) {
        h += 1.f;
    }
}

void Color::Lch2Luv(float c, float h, float &u, float &v)
{
    float2 sincosval = xsincosf(h);
    u = c * sincosval.x;
    v = c * sincosval.y;
}

// NOT TESTED
void Color::XYZ2Luv (float X, float Y, float Z, float &L, float &u, float &v)
{

    X /= 65535.f;
    Y /= 65535.f;
    Z /= 65535.f;

    if (Y > float(eps)) {
        L = 116.f * std::cbrt(Y) - 16.f;
    } else {
        L = float(kappa) * Y;
    }

    u = 13.f * L * float(u0);
    v = 13.f * L * float(v0);
}

// NOT TESTED
void Color::Luv2XYZ (float L, float u, float v, float &X, float &Y, float &Z)
{
    if (L > float(epskap)) {
        float t = (L + 16.f) / 116.f;
        Y = t * t * t;
    } else {
        Y = L / float(kappa);
    }

    float a = ((52.f * L) / (u + 13.f * L * float(u0)) - 1.f) / 3.f;
    float d = Y * (((39 * L) / (v + 13 * float(v0))) - 5.f);
    float b = -5.f * Y;
    X = (d - b) / (a + 1.f / 3.f);

    Z = X * a + b;

    X *= 65535.f;
    Y *= 65535.f;
    Z *= 65535.f;
}

/*
 * Gamut mapping algorithm
 * Copyright (c) 2010-2011  Emil Martinec <ejmartin@uchicago.edu>
 *
 * Solutions to scaling u and v to XYZ paralleliped boundaries
 * Some equations:
 *
 *    fu(X,Y,Z) = 4 X/(X + 15 Y + 3 Z);
 *    fv(X,Y,Z) = 9 Y/(X + 15 Y + 3 Z);
 *
 * take the plane spanned by X=a*Xr+b*Xg+c*Xb etc with one of a,b,c equal to 0 or 1,
 * and itersect with the line u0+lam*u, or in other words solve
 *
 *    u0+lam*u=fu(X,Y,Z)
 *    v0+lam*v=fv(X,Y,Z)
 *
 * The value of lam is the scale factor that takes the color to the gamut boundary
 * columns of the matrix p=xyz_rgb are RGB tristimulus primaries in XYZ
 * c is the color fixed on the boundary; and m=0 for c=0, m=1 for c=255
 */
void Color::gamutmap(float &X, float &Y, float &Z, const double p[3][3])
{
    float u = 4 * X / (X + 15 * Y + 3 * Z) - u0;
    float v = 9 * Y / (X + 15 * Y + 3 * Z) - v0;

    float lam[3][2];
    float lam_min = 1.0;

    for (int c = 0; c < 3; c++)
        for (int m = 0; m < 2; m++) {

            int c1 = (c + 1) % 3;
            int c2 = (c + 2) % 3;

            lam[c][m] = (-(p[0][c1] * p[1][c] * ((-12 + 3 * u0 + 20 * v0) * Y + 4 * m * 65535 * v0 * p[2][c2])) +
                         p[0][c] * p[1][c1] * ((-12 + 3 * u0 + 20 * v0) * Y + 4 * m * 65535 * v0 * p[2][c2]) -
                         4 * v0 * p[0][c1] * (Y - m * 65535 * p[1][c2]) * p[2][c] + 4 * v0 * p[0][c] * (Y - m * 65535 * p[1][c2]) * p[2][c1] -
                         (4 * m * 65535 * v0 * p[0][c2] - 9 * u0 * Y) * (p[1][c1] * p[2][c] - p[1][c] * p[2][c1]));

            lam[c][m] /= (3 * u * Y * (p[0][c1] * p[1][c] - p[1][c1] * (p[0][c] + 3 * p[2][c]) + 3 * p[1][c] * p[2][c1]) +
                          4 * v * (p[0][c1] * (5 * Y * p[1][c] + m * 65535 * p[1][c] * p[2][c2] + Y * p[2][c] - m * 65535 * p[1][c2] * p[2][c]) -
                                   p[0][c] * (5 * Y * p[1][c1] + m * 65535 * p[1][c1] * p[2][c2] + Y * p[2][c1] - m * 65535 * p[1][c2] * p[2][c1]) +
                                   m * 65535 * p[0][c2] * (p[1][c1] * p[2][c] - p[1][c] * p[2][c1])));

            if (lam[c][m] < lam_min && lam[c][m] > 0) {
                lam_min = lam[c][m];
            }

        }

    u = u * lam_min + u0;
    v = v * lam_min + v0;

    X = (9 * u * Y) / (4 * v);
    Z = (12 - 3 * u - 20 * v) * Y / (4 * v);
}

void Color::skinred ( double J, double h, double sres, double Sp, float dred, float protect_red, int sk, float rstprotection, float ko, double &s)
{
    float factorskin, factorsat, factor, factorskinext, interm;
    float scale = 100.0f / 100.1f; //reduction in normal zone
    float scaleext = 1.0f; //reduction in transition zone
    float deltaHH = 0.3f; //HH value transition : I have choice 0.3 radians
    float HH;
    bool doskin = false;

    //rough correspondence between h (JC) and H (lab) that has relatively little importance because transitions that blur the correspondence is not linear
    if     ((float)h > 8.6f  && (float)h <= 74.f ) {
        HH = (1.15f / 65.4f) * (float)h - 0.0012f;     //H > 0.15   H<1.3
        doskin = true;
    } else if((float)h > 0.f   && (float)h <= 8.6f ) {
        HH = (0.19f / 8.6f ) * (float)h - 0.04f;       //H>-0.04 H < 0.15
        doskin = true;
    } else if((float)h > 355.f && (float)h <= 360.f) {
        HH = (0.11f / 5.0f ) * (float)h - 7.96f;       //H>-0.15 <-0.04
        doskin = true;
    } else if((float)h > 74.f  && (float)h < 95.f  ) {
        HH = (0.30f / 21.0f) * (float)h + 0.24285f;    //H>1.3  H<1.6
        doskin = true;
    }

    if(doskin) {
        float chromapro = sres / Sp;

        if(sk == 1) { //in C mode to adapt dred to J
            if     (J < 16.0) {
                dred = 40.0f;
            } else if(J < 22.0) {
                dred = 2.5f * (float)J;
            } else if(J < 60.0) {
                dred = 55.0f;
            } else if(J < 70.0) {
                dred = -1.5f * (float)J + 145.0f;
            } else {
                dred = 40.0f;
            }
        }

        if(chromapro > 0.0) {
            Color::scalered ( rstprotection, chromapro, 0.0, HH, deltaHH, scale, scaleext);    //Scale factor
        }

        if(chromapro > 1.0) {
            interm = (chromapro - 1.0f) * 100.0f;
            factorskin = 1.0f + (interm * scale) / 100.0f;
            factorskinext = 1.0f + (interm * scaleext) / 100.0f;
        } else {
            factorskin = chromapro ;
            factorskinext = chromapro ;
        }

        factorsat = chromapro;
        factor = factorsat;
        Color::transitred ( HH, s, dred, factorskin, protect_red, factorskinext, deltaHH, factorsat, factor);   //transition
        s *= factor;
    } else {
        s = ko * sres;
    }

}
void Color::skinredfloat ( float J, float h, float sres, float Sp, float dred, float protect_red, int sk, float rstprotection, float ko, float &s)
{
    float HH;
    bool doskin = false;

    //rough correspondence between h (JC) and H (lab) that has relatively little importance because transitions that blur the correspondence is not linear
    if     ((float)h > 8.6f  && (float)h <= 74.f ) {
        HH = (1.15f / 65.4f) * (float)h - 0.0012f;     //H > 0.15   H<1.3
        doskin = true;
    } else if((float)h > 0.f   && (float)h <= 8.6f ) {
        HH = (0.19f / 8.6f ) * (float)h - 0.04f;       //H>-0.04 H < 0.15
        doskin = true;
    } else if((float)h > 355.f && (float)h <= 360.f) {
        HH = (0.11f / 5.0f ) * (float)h - 7.96f;       //H>-0.15 <-0.04
        doskin = true;
    } else if((float)h > 74.f  && (float)h < 95.f  ) {
        HH = (0.30f / 21.0f) * (float)h + 0.24285f;    //H>1.3  H<1.6
        doskin = true;
    }

    if(doskin) {
        float factorskin, factorsat, factor, factorskinext;
        float deltaHH = 0.3f; //HH value transition : I have choice 0.3 radians
        float chromapro = sres / Sp;

        if(sk == 1) { //in C mode to adapt dred to J
            if     (J < 16.f) {
                dred = 40.f;
            } else if(J < 22.f) {
                dred = 2.5f * J;
            } else if(J < 60.f) {
                dred = 55.f;
            } else if(J < 70.f) {
                dred = 145.f - 1.5f * J;
            } else {
                dred = 40.f;
            }
        }

        if(chromapro > 1.0) {
            float scale = 0.999000999f;  // 100.0f/100.1f; reduction in normal zone
            float scaleext = 1.0f; //reduction in transition zone
            Color::scalered ( rstprotection, chromapro, 0.0, HH, deltaHH, scale, scaleext);//Scale factor
            float interm = (chromapro - 1.0f);
            factorskin = 1.0f + (interm * scale);
            factorskinext = 1.0f + (interm * scaleext);
        } else {
            factorskin = chromapro ;
            factorskinext = chromapro ;
        }

        factorsat = chromapro;
        factor = factorsat;
        Color::transitred ( HH, s, dred, factorskin, protect_red, factorskinext, deltaHH, factorsat, factor);   //transition
        s *= factor;
    } else {
        s = ko * sres;
    }

}






void Color::scalered ( const float rstprotection, const float param, const float limit, const float HH, const float deltaHH, float &scale, float &scaleext)
{
    if(rstprotection < 99.9999f) {
        if(param > limit) {
            scale = rstprotection / 100.1f;
        }

        if((HH < (1.3f + deltaHH) && HH >= 1.3f))
            // scaleext=HH*(1.0f-scale)/deltaHH + 1.0f - (1.3f+deltaHH)*(1.0f-scale)/deltaHH;    //transition for Hue (red - yellow)
            // optimized formula
        {
            scaleext = (HH * (1.0f - scale) + deltaHH - (1.3f + deltaHH) * (1.0f - scale)) / deltaHH;    //transition for Hue (red - yellow)
        } else if((HH < 0.15f && HH > (0.15f - deltaHH)))
            // scaleext=HH*(scale-1.0f)/deltaHH + 1.0f - (0.15f-deltaHH)*(scale-1.0f)/deltaHH;   //transition for hue (red purple)
            // optimized formula
        {
            scaleext = (HH * (scale - 1.0f) + deltaHH - (0.15f - deltaHH) * (scale - 1.0f)) / deltaHH;    //transition for hue (red purple)
        }
    }
}

void Color::transitred (const float HH, float const Chprov1, const float dred, const float factorskin, const float protect_red, const float factorskinext, const float deltaHH, const float factorsat, float &factor)
{
    if(HH >= 0.15f && HH < 1.3f) {
        if (Chprov1 < dred) {
            factor = factorskin;
        } else if(Chprov1 < (dred + protect_red)) {
            factor = ((factorsat - factorskin) * Chprov1 + factorsat * protect_red - (dred + protect_red) * (factorsat - factorskin)) / protect_red;
        }
    } else if ( HH > (0.15f - deltaHH) && HH < (1.3f + deltaHH) ) { // test if chroma is in the extended range
        if (Chprov1 < dred) {
            factor = factorskinext;    // C=dred=55 => real max of skin tones
        } else if (Chprov1 < (dred + protect_red)) {// transition
            factor = ((factorsat - factorskinext) * Chprov1 + factorsat * protect_red - (dred + protect_red) * (factorsat - factorskinext)) / protect_red;
        }
    }
}

/*
 * AllMunsellLch correction
 * Copyright (c) 2012  Jacques Desmis <jdesmis@gmail.com>
 *
 * This function corrects the color (hue) for changes in chromaticity and luminance
 * to use in a "for" or "do while" statement
 *
 * Parameters:
 *    bool lumaMuns : true => luminance correction (for delta L > 10) and chroma correction ; false : only chroma
 *    float Lprov1 , Loldd : luminance after and before
 *    float HH: hue before
 *    float Chprov1, CC : chroma after and before
 *    float coorectionHuechroma : correction Hue for chromaticity (saturation)
 *    float correctlum : correction Hue for luminance (brigtness, contrast,...)
 *    MunsellDebugInfo* munsDbgInfo: (Debug target only) object to collect information.
 */
#ifdef _DEBUG
void Color::AllMunsellLch(bool lumaMuns, float Lprov1, float Loldd, float HH, float Chprov1, float CC, float &correctionHuechroma, float &correctlum, MunsellDebugInfo* munsDbgInfo)
#else
void Color::AllMunsellLch(bool lumaMuns, float Lprov1, float Loldd, float HH, float Chprov1, float CC, float &correctionHuechroma, float &correctlum)
#endif
{

    bool contin1, contin2;
    float correctionHue = 0.0, correctionHueLum = 0.0;
    bool correctL;

    if(CC >= 6.0 && CC < 140) {          //if C > 140 we say C=140 (only in Prophoto ...with very large saturation)
        static const float huelimit[8] = { -2.48, -0.55, 0.44, 1.52, 1.87, 3.09, -0.27, 0.44}; //limits hue of blue-purple, red-yellow, green-yellow, red-purple

        if (Chprov1 > 140.f) {
            Chprov1 = 139.f;    //limits of LUTf
        }

        if (Chprov1 < 6.f) {
            Chprov1 = 6.f;
        }

        for(int zo = 1; zo <= 4; zo++) {
            if(HH > huelimit[2 * zo - 2] && HH < huelimit[2 * zo - 1]) {
                //zone=zo;
                contin1 = contin2 = false;
                correctL = false;
                MunsellLch (Lprov1, HH, Chprov1, CC, correctionHue, zo, correctionHueLum, correctL);       //munsell chroma correction
#ifdef _DEBUG
                float absCorrectionHue = fabs(correctionHue);

                if(correctionHue != 0.0) {
                    int idx = zo - 1;
                    #pragma omp critical (maxdhue)
                    {
                        munsDbgInfo->maxdhue[idx] = MAX(munsDbgInfo->maxdhue[idx], absCorrectionHue);
                    }
                }

                if(absCorrectionHue > 0.45)
                    #pragma omp atomic
                    munsDbgInfo->depass++;        //verify if no bug in calculation

#endif
                correctionHuechroma = correctionHue;  //preserve

                if(lumaMuns) {
                    float correctlumprov = 0.f;
                    float correctlumprov2 = 0.f;

                    if(correctL) {
                        //for Munsell luminance correction
                        correctlumprov = correctionHueLum;
                        contin1 = true;
                        correctL = false;
                    }

                    correctionHueLum = 0.0;
                    correctionHue = 0.0;

                    if(fabs(Lprov1 - Loldd) > 6.0) {
                        // correction if delta L significative..Munsell luminance
                        MunsellLch (Loldd, HH, Chprov1, Chprov1, correctionHue, zo, correctionHueLum, correctL);

                        if(correctL) {
                            correctlumprov2 = correctionHueLum;
                            contin2 = true;
                            correctL = false;
                        }

                        correctionHueLum = 0.0;

                        if(contin1 && contin2) {
                            correctlum = correctlumprov2 - correctlumprov;
                        }

#ifdef _DEBUG
                        float absCorrectLum = fabs(correctlum);

                        if(correctlum != 0.0) {
                            int idx = zo - 1;
                            #pragma omp critical (maxdhuelum)
                            {
                                munsDbgInfo->maxdhuelum[idx] = MAX(munsDbgInfo->maxdhuelum[idx], absCorrectLum);
                            }
                        }

                        if(absCorrectLum > 0.35)
                            #pragma omp atomic
                            munsDbgInfo->depassLum++;    //verify if no bug in calculation

#endif
                    }
                }
            }
        }

    }

#ifdef _DEBUG

    if     (correctlum < -0.35f) {
        correctlum = -0.35f;
    } else if(correctlum >  0.35f) {
        correctlum = 0.35f;
    }

    if     (correctionHuechroma < -0.45f) {
        correctionHuechroma = -0.45f;
    } else if(correctionHuechroma > 0.45f) {
        correctionHuechroma = 0.45f;
    }

#endif

}

/*
 * AllMunsellLch correction
 * Copyright (c) 2012  Jacques Desmis <jdesmis@gmail.com>
 *
 * This function corrects the color (hue) for changes in chromaticity and luminance
 * to use in a "for" or "do while" statement
 *
 * Parameters:
 *    float Lprov1: luminance
 *    float HH: hue before
 *    float Chprov1, CC : chroma after and before
 *    float coorectionHuechroma : correction Hue for chromaticity (saturation)
 */
void Color::AllMunsellLch(float Lprov1, float HH, float Chprov1, float CC, float &correctionHuechroma)
{

    float correctionHue = 0.f, correctionHueLum = 0.f;
    bool correctL;

    if(CC >= 6.f && CC < 140.f) {          //if C > 140 we say C=140 (only in Prophoto ...with very large saturation)
        static const float huelimit[8] = { -2.48f, -0.55f, 0.44f, 1.52f, 1.87f, 3.09f, -0.27f, 0.44f}; //limits hue of blue-purple, red-yellow, green-yellow, red-purple

        if (Chprov1 > 140.f) {
            Chprov1 = 139.f;    //limits of LUTf
        }

        Chprov1 = rtengine::max(Chprov1, 6.f);

        for(int zo = 1; zo <= 4; zo++) {
            if(HH > huelimit[2 * zo - 2] && HH < huelimit[2 * zo - 1]) {
                //zone=zo;
                correctL = false;
                MunsellLch (Lprov1, HH, Chprov1, CC, correctionHue, zo, correctionHueLum, correctL);       //munsell chroma correction
                correctionHuechroma = correctionHue;  //preserve
                break;
            }
        }
    }
}

/*
 * GamutLchonly correction
 * Copyright (c)2012  Jacques Desmis <jdesmis@gmail.com> and Jean-Christophe Frisch <natureh@free.fr>
 *
 * This function puts the data (Lab) in the gamut of "working profile":
 * it returns the corrected values of the chromaticity and luminance
 *
 * float HH : hue
 * float Lprov1 : input luminance value, sent back corrected
 * float Chprov1: input chroma value, sent back corrected
 * float R,G,B : red, green and blue value of the corrected color
 * double wip : working profile
 * bool isHLEnabled : if "highlight reconstruction " is enabled
 * float coef : a float number between [0.95 ; 1.0[... the nearest it is from 1.0, the more precise it will be... and the longer too as more iteration will be necessary)
 * bool neg and moreRGB : only in DEBUG mode to calculate iterations for negatives values and > 65535
 */
#ifdef _DEBUG
void Color::gamutLchonly (float HH, float &Lprov1, float &Chprov1, float &R, float &G, float &B, const double wip[3][3], const bool isHLEnabled, const float lowerCoef, const float higherCoef, bool &neg, bool &more_rgb)
#else
void Color::gamutLchonly (float HH, float &Lprov1, float &Chprov1, float &R, float &G, float &B, const double wip[3][3], const bool isHLEnabled, const float lowerCoef, const float higherCoef)
#endif
{
    const float ClipLevel = 65535.0f;
    bool inGamut;
#ifdef _DEBUG
    neg = false, more_rgb = false;
#endif
    float2  sincosval = xsincosf(HH);

    do {
        inGamut = true;

        //Lprov1=LL;
        float aprov1 = Chprov1 * sincosval.y;
        float bprov1 = Chprov1 * sincosval.x;

        //conversion Lab RGB to limit Lab values - this conversion is useful before Munsell correction
        float fy = (c1By116 * Lprov1 ) + c16By116;
        float fx = (0.002f * aprov1) + fy;
        float fz = fy - (0.005f * bprov1);

        float x_ = 65535.0f * f2xyz(fx) * D50x;
        // float y_ = 65535.0f * f2xyz(fy);
        float z_ = 65535.0f * f2xyz(fz) * D50z;
        float y_ = (Lprov1 > epskap) ? 65535.0 * fy * fy * fy : 65535.0 * Lprov1 / kappa;

        xyz2rgb(x_, y_, z_, R, G, B, wip);

        // gamut control before saturation to put Lab values in future gamut, but not RGB
        if (R < 0.0f || G < 0.0f || B < 0.0f) {
#ifdef _DEBUG
            neg = true;
#endif

            if (Lprov1 < 0.1f) {
                Lprov1 = 0.1f;
            }

            //gamut for L with ultra blue : we can improve the algorithm ... thinner, and other color ???
            if(HH < -0.9f && HH > -1.55f ) {//ultra blue
                if(Chprov1 > 160.f) if (Lprov1 < 5.f) {
                        Lprov1 = 5.f;    //very very very very high chroma
                    }

                if(Chprov1 > 140.f) if (Lprov1 < 3.5f) {
                        Lprov1 = 3.5f;
                    }

                if(Chprov1 > 120.f) if (Lprov1 < 2.f) {
                        Lprov1 = 2.f;
                    }

                if(Chprov1 > 105.f) if (Lprov1 < 1.f) {
                        Lprov1 = 1.f;
                    }

                if(Chprov1 > 90.f) if (Lprov1 < 0.7f) {
                        Lprov1 = 0.7f;
                    }

                if(Chprov1 > 50.f) if (Lprov1 < 0.5f) {
                        Lprov1 = 0.5f;
                    }

                if(Chprov1 > 20.f) if (Lprov1 < 0.4f) {
                        Lprov1 = 0.4f;
                    }
            }

            Chprov1 *= higherCoef; // decrease the chromaticity value

            if (Chprov1 <= 3.0f) {
                Lprov1 += lowerCoef;
            }

            inGamut = false;
        } else if (!isHLEnabled && rtengine::max(R, G, B) > ClipLevel && rtengine::min(R, G, B) <= ClipLevel) {

            // if "highlight reconstruction" is enabled or the point is completely white (clipped, no color), don't control Gamut
#ifdef _DEBUG
            more_rgb = true;
#endif

            if (Lprov1 > 99.999f) {
                Lprov1 = 99.98f;
            }

            Chprov1 *= higherCoef;

            if (Chprov1 <= 3.0f) {
                Lprov1 -= lowerCoef;
            }

            inGamut = false;
        }
    } while (!inGamut);

    //end first gamut control
}

/*
 * GamutLchonly correction
 * Copyright (c)2012  Jacques Desmis <jdesmis@gmail.com> and Jean-Christophe Frisch <natureh@free.fr>
 *
 * This function puts the data (Lab) in the gamut of "working profile":
 * it returns the corrected values of the chromaticity and luminance
 *
 * float HH : hue
 * float2 sincosval : sin and cos of HH
 * float Lprov1 : input luminance value, sent back corrected
 * float Chprov1: input chroma value, sent back corrected
 * float R,G,B : red, green and blue value of the corrected color
 * double wip : working profile
 * bool isHLEnabled : if "highlight reconstruction " is enabled
 * float coef : a float number between [0.95 ; 1.0[... the nearest it is from 1.0, the more precise it will be... and the longer too as more iteration will be necessary)
 * bool neg and moreRGB : only in DEBUG mode to calculate iterations for negatives values and > 65535
 */
#ifdef _DEBUG
void Color::gamutLchonly (float HH, float2 sincosval, float &Lprov1, float &Chprov1, float &R, float &G, float &B, const double wip[3][3], const bool isHLEnabled, const float lowerCoef, const float higherCoef, bool &neg, bool &more_rgb)
#else
void Color::gamutLchonly (float HH, float2 sincosval, float &Lprov1, float &Chprov1, float &R, float &G, float &B, const double wip[3][3], const bool isHLEnabled, const float lowerCoef, const float higherCoef)
#endif
{
    constexpr float ClipLevel = 65535.0f;
    bool inGamut;
#ifdef _DEBUG
    neg = false, more_rgb = false;
#endif
    float ChprovSave = Chprov1;

    do {
        inGamut = true;

        float aprov1 = Chprov1 * sincosval.y;
        float bprov1 = Chprov1 * sincosval.x;

        //conversion Lab RGB to limit Lab values - this conversion is useful before Munsell correction
        float fy = (c1By116 * Lprov1 ) + c16By116;
        float fx = (0.002f * aprov1) + fy;
        float fz = fy - (0.005f * bprov1);

        float x_ = 65535.0f * f2xyz(fx) * D50x;
        float z_ = 65535.0f * f2xyz(fz) * D50z;
        float y_ = (Lprov1 > epskap) ? 65535.0f * fy * fy * fy : 65535.0f * Lprov1 / kappa;

        xyz2rgb(x_, y_, z_, R, G, B, wip);

        // gamut control before saturation to put Lab values in future gamut, but not RGB
        if (R < 0.0f || G < 0.0f || B < 0.0f) {
#ifdef _DEBUG
            neg = true;
#endif

            if (isnan(HH)) {
                float atemp = ChprovSave * sincosval.y * 327.68;
                float btemp = ChprovSave * sincosval.x * 327.68;
                HH = xatan2f(btemp, atemp);
            }

            if (Lprov1 < 0.1f) {
                Lprov1 = 0.1f;
            }

            //gamut for L with ultra blue : we can improve the algorithm ... thinner, and other color ???
            if(HH < -0.9f && HH > -1.55f ) {//ultra blue
                if(Chprov1 > 160.f) if (Lprov1 < 5.f) {
                        Lprov1 = 5.f;    //very very very very high chroma
                    }

                if(Chprov1 > 140.f) if (Lprov1 < 3.5f) {
                        Lprov1 = 3.5f;
                    }

                if(Chprov1 > 120.f) if (Lprov1 < 2.f) {
                        Lprov1 = 2.f;
                    }

                if(Chprov1 > 105.f) if (Lprov1 < 1.f) {
                        Lprov1 = 1.f;
                    }

                if(Chprov1 > 90.f) if (Lprov1 < 0.7f) {
                        Lprov1 = 0.7f;
                    }

                if(Chprov1 > 50.f) if (Lprov1 < 0.5f) {
                        Lprov1 = 0.5f;
                    }

                if(Chprov1 > 20.f) if (Lprov1 < 0.4f) {
                        Lprov1 = 0.4f;
                    }
            }

            Chprov1 *= higherCoef; // decrease the chromaticity value

            if (Chprov1 <= 3.0f) {
                Lprov1 += lowerCoef;
            }

            inGamut = false;
        } else if (!isHLEnabled && rtengine::max(R, G, B) > ClipLevel && rtengine::min(R, G, B) <= ClipLevel) {

            // if "highlight reconstruction" is enabled or the point is completely white (clipped, no color), don't control Gamut
#ifdef _DEBUG
            more_rgb = true;
#endif

            if (Lprov1 > 99.999f) {
                Lprov1 = 99.98f;
            }

            Chprov1 *= higherCoef;

            if (Chprov1 <= 3.0f) {
                Lprov1 -= lowerCoef;
            }

            inGamut = false;
        }
    } while (!inGamut);

    //end first gamut control
}

/*
 * GamutLchonly correction
 * Copyright (c)2012  Jacques Desmis <jdesmis@gmail.com> and Jean-Christophe Frisch <natureh@free.fr>
 *
 * This function puts the data (Lab) in the gamut of "working profile":
 * it returns the corrected values of the chromaticity and luminance
 *
 * float HH : hue
 * float2 sincosval : sin and cos of HH
 * float Lprov1 : input luminance value, sent back corrected
 * float Chprov1: input chroma value, sent back corrected
 * float wip : working profile
 * bool isHLEnabled : if "highlight reconstruction " is enabled
 * float coef : a float number between [0.95 ; 1.0[... the nearest it is from 1.0, the more precise it will be... and the longer too as more iteration will be necessary)
 */
void Color::gamutLchonly (float HH, float2 sincosval, float &Lprov1, float &Chprov1, float &saturation, const float wip[3][3], const bool isHLEnabled, const float lowerCoef, const float higherCoef)
{
    constexpr float ClipLevel = 1.f;
    bool inGamut;
    float R, G, B;

    do {
        inGamut = true;

        float aprov1 = Chprov1 * sincosval.y;
        float bprov1 = Chprov1 * sincosval.x;

        //conversion Lab RGB to limit Lab values - this conversion is useful before Munsell correction
        float fy = c1By116 * Lprov1 + c16By116;
        float fx = 0.002f * aprov1 + fy;
        float fz = fy - 0.005f * bprov1;

        float x_ = f2xyz(fx) * D50x;
        float z_ = f2xyz(fz) * D50z;
        float y_ = (Lprov1 > epskap) ? fy * fy * fy : Lprov1 / kappaf;

        xyz2rgb(x_, y_, z_, R, G, B, wip);

        // gamut control before saturation to put Lab values in future gamut, but not RGB
        if (rtengine::min(R, G, B) < 0.f) {

            Lprov1 = rtengine::max(Lprov1, 0.1f);

            //gamut for L with ultra blue : we can improve the algorithm ... thinner, and other color ???
            if(HH < -0.9f && HH > -1.55f ) {//ultra blue
                if(Chprov1 > 160.f) if (Lprov1 < 5.f) {
                        Lprov1 = 5.f;    //very very very very high chroma
                    }

                if(Chprov1 > 140.f) if (Lprov1 < 3.5f) {
                        Lprov1 = 3.5f;
                    }

                if(Chprov1 > 120.f) if (Lprov1 < 2.f) {
                        Lprov1 = 2.f;
                    }

                if(Chprov1 > 105.f) if (Lprov1 < 1.f) {
                        Lprov1 = 1.f;
                    }

                if(Chprov1 > 90.f) if (Lprov1 < 0.7f) {
                        Lprov1 = 0.7f;
                    }

                if(Chprov1 > 50.f) if (Lprov1 < 0.5f) {
                        Lprov1 = 0.5f;
                    }

                if(Chprov1 > 20.f) if (Lprov1 < 0.4f) {
                        Lprov1 = 0.4f;
                    }
            }

            Chprov1 *= higherCoef; // decrease the chromaticity value

            if (Chprov1 <= 3.f) {
                Lprov1 += lowerCoef;
            }

            inGamut = false;
        } else if (!isHLEnabled && rtengine::max(R, G, B) > ClipLevel && rtengine::min(R, G, B) <= ClipLevel) {

            // if "highlight reconstruction" is enabled or the point is completely white (clipped, no color), don't control Gamut

            if (Lprov1 > 99.999f) {
                Lprov1 = 99.98f;
            }

            Chprov1 *= higherCoef;

            if (Chprov1 <= 3.f) {
                Lprov1 -= lowerCoef;
            }

            inGamut = false;
        }
    } while (!inGamut);

    saturation = 1.f - (rtengine::min(R, G, B) / rtengine::max(R, G, B));
    //end first gamut control
}


#ifdef _DEBUG
void Color::gamutLchonly (float2 sincosval, float &Lprov1, float &Chprov1, const float wip[3][3], const bool isHLEnabled, const float lowerCoef, const float higherCoef, bool &neg, bool &more_rgb)
#else
void Color::gamutLchonly (float2 sincosval, float &Lprov1, float &Chprov1, const float wip[3][3], const bool isHLEnabled, const float lowerCoef, const float higherCoef)
#endif
{
    const float ClipLevel = 65535.0f;
    bool inGamut;
#ifdef _DEBUG
    neg = false, more_rgb = false;
#endif

    do {
        inGamut = true;

        //Lprov1=LL;
        float aprov1 = Chprov1 * sincosval.y;
        float bprov1 = Chprov1 * sincosval.x;

        //conversion Lab RGB to limit Lab values - this conversion is useful before Munsell correction
        float fy = (c1By116 * Lprov1 ) + c16By116;
        float fx = (0.002f * aprov1) + fy;
        float fz = fy - (0.005f * bprov1);

        float x_ = 65535.0f * f2xyz(fx) * D50x;
        // float y_ = 65535.0f * f2xyz(fy);
        float z_ = 65535.0f * f2xyz(fz) * D50z;
        float y_ = (Lprov1 > epskap) ? 65535.0 * fy * fy * fy : 65535.0 * Lprov1 / kappa;

        float R, G, B;
        xyz2rgb(x_, y_, z_, R, G, B, wip);

        // gamut control before saturation to put Lab values in future gamut, but not RGB
        if (R < 0.0f || G < 0.0f || B < 0.0f) {
#ifdef _DEBUG
            neg = true;
#endif

            if (Lprov1 < 0.01f) {
                Lprov1 = 0.01f;
            }

            Chprov1 *= higherCoef; // decrease the chromaticity value

            if (Chprov1 <= 3.0f) {
                Lprov1 += lowerCoef;
            }

            inGamut = false;
        } else if (!isHLEnabled && rtengine::max(R, G, B) > ClipLevel && rtengine::min(R, G, B) <= ClipLevel) {

            // if "highlight reconstruction" is enabled or the point is completely white (clipped, no color), don't control Gamut
#ifdef _DEBUG
            more_rgb = true;
#endif

            if (Lprov1 > 99.999f) {
                Lprov1 = 99.98f;
            }

            Chprov1 *= higherCoef;

            if (Chprov1 <= 3.0f) {
                Lprov1 -= lowerCoef;
            }

            inGamut = false;
        }
    } while (!inGamut);

    //end first gamut control
}
/*
 * LabGamutMunsell
 * Copyright (c) 2012  Jacques Desmis <jdesmis@gmail.com>
 *
 * This function is the overall Munsell's corrections, but only on global statement: I think it's better to use local statement with AllMunsellLch
 * not for use in a "for" or "do while" loop
 * they are named accordingly :  gamutLchonly and AllMunsellLch
 * it can be used before and after treatment (saturation, gamma, luminance, ...)
 *
 * Parameters:
 *    float *labL     :       RT Lab L channel data
 *    float *laba     :       RT Lab a channel data
 *    float *labb     :       RT Lab b channel data
 *    bool corMunsell :       performs Munsell correction
 *    bool lumaMuns   :       (used only if corMuns=true)
 *                            true:  apply luma + chroma Munsell correction if delta L > 10;
 *                            false: only chroma correction only
 *    bool gamut            : performs gamutLch
 *    const double wip[3][3]: matrix for working profile
 *    bool multiThread      : parallelize the loop
 */
void Color::LabGamutMunsell(float *labL, float *laba, float *labb, const int N, bool corMunsell, bool lumaMuns, bool isHLEnabled, bool gamut, const double wip[3][3])
{
#ifdef _DEBUG
    MyTime t1e, t2e;
    t1e.set();
    int negat = 0, moreRGB = 0;
    MunsellDebugInfo* MunsDebugInfo = nullptr;

    if (corMunsell) {
        MunsDebugInfo = new MunsellDebugInfo();
    }

#endif
    float correctlum = 0.f;
    float correctionHuechroma = 0.f;
#ifdef __SSE2__
    // precalculate H and C using SSE
    float HHBuffer[N];
    float CCBuffer[N];
    vfloat c327d68v = f2v(327.68f);
    vfloat av, bv;
    int k;

    for (k = 0; k < N - 3; k += 4) {
        av = lvfu(laba[k]);
        bv = lvfu(labb[k]);
        stvfu(HHBuffer[k], xatan2f(bv, av));
        stvfu(CCBuffer[k], vsqrtf(SQRV(av) + SQRV(bv)) / c327d68v);
    }

    for(; k < N; k++) {
        HHBuffer[k] = xatan2f(labb[k], laba[k]);
        CCBuffer[k] = sqrt(SQR(laba[k]) + SQR(labb[k])) / 327.68f;
    }

#endif // __SSE2__

    for (int j = 0; j < N; j++) {
#ifdef __SSE2__
        float HH  = HHBuffer[j];
        float Chprov1 = CCBuffer[j];
#else
        float HH = xatan2f(labb[j], laba[j]);
        float Chprov1 = sqrtf(SQR(laba[j]) + SQR(labb[j])) / 327.68f;
#endif
        float Lprov1 = labL[j] / 327.68f;
        float Loldd = Lprov1;
        float Coldd = Chprov1;
        float2 sincosval;

        if(gamut) {
#ifdef _DEBUG
            bool neg, more_rgb;
#endif
            // According to mathematical laws we can get the sin and cos of HH by simple operations
            float R, G, B;

            if(Chprov1 == 0.f) {
                sincosval.y = 1.f;
                sincosval.x = 0.f;
            } else {
                sincosval.y = laba[j] / (Chprov1 * 327.68f);
                sincosval.x = labb[j] / (Chprov1 * 327.68f);
            }

            //gamut control : Lab values are in gamut
#ifdef _DEBUG
            gamutLchonly(HH, sincosval, Lprov1, Chprov1, R, G, B, wip, isHLEnabled, 0.15f, 0.96f, neg, more_rgb);
#else
            gamutLchonly(HH, sincosval, Lprov1, Chprov1, R, G, B, wip, isHLEnabled, 0.15f, 0.96f);
#endif

#ifdef _DEBUG

            if(neg) {
                negat++;
            }

            if(more_rgb) {
                moreRGB++;
            }

#endif
        }

        labL[j] = Lprov1 * 327.68f;
        correctionHuechroma = 0.f;
        correctlum = 0.f;

        if(corMunsell)
#ifdef _DEBUG
            AllMunsellLch(lumaMuns, Lprov1, Loldd, HH, Chprov1, Coldd, correctionHuechroma, correctlum, MunsDebugInfo);

#else
            AllMunsellLch(lumaMuns, Lprov1, Loldd, HH, Chprov1, Coldd, correctionHuechroma, correctlum);
#endif

        if(correctlum == 0.f && correctionHuechroma == 0.f) {
            if(!gamut) {
                if(Coldd == 0.f) {
                    sincosval.y = 1.f;
                    sincosval.x = 0.f;
                } else {
                    sincosval.y = laba[j] / (Coldd * 327.68f);
                    sincosval.x = labb[j] / (Coldd * 327.68f);
                }
            }

        } else {
            HH += correctlum;      //hue Munsell luminance correction
            sincosval = xsincosf(HH + correctionHuechroma);
        }

        laba[j] = Chprov1 * sincosval.y * 327.68f;
        labb[j] = Chprov1 * sincosval.x * 327.68f;
    }

#ifdef _DEBUG
    t2e.set();

    if (settings->verbose) {
        printf("Color::LabGamutMunsell (correction performed in %d usec):\n", t2e.etime(t1e));
        printf("   Gamut              : G1negat=%iiter G165535=%iiter \n", negat, moreRGB);

        if (MunsDebugInfo) {
            printf("   Munsell chrominance: MaxBP=%1.2frad  MaxRY=%1.2frad  MaxGY=%1.2frad  MaxRP=%1.2frad  depass=%u\n", MunsDebugInfo->maxdhue[0],    MunsDebugInfo->maxdhue[1],    MunsDebugInfo->maxdhue[2],    MunsDebugInfo->maxdhue[3],    MunsDebugInfo->depass);
            printf("   Munsell luminance  : MaxBP=%1.2frad  MaxRY=%1.2frad  MaxGY=%1.2frad  MaxRP=%1.2frad  depass=%u\n", MunsDebugInfo->maxdhuelum[0] , MunsDebugInfo->maxdhuelum[1], MunsDebugInfo->maxdhuelum[2], MunsDebugInfo->maxdhuelum[3], MunsDebugInfo->depassLum);
        } else {
            printf("   Munsell correction wasn't requested\n");
        }
    }

    if (MunsDebugInfo) {
        delete MunsDebugInfo;
    }

#endif

}

/*
 * MunsellLch correction
 * Copyright (c) 2012  Jacques Desmis <jdesmis@gmail.com>
 *
 * Find the right LUT and calculate the correction
 */
void Color::MunsellLch (float lum, float hue, float chrom, float memChprov, float &correction, int zone, float &lbe, bool &correctL)
{

    int x = int(memChprov);
    int y = int(chrom);

    //begin PB correction + sky
    if(zone == 1) {
        if(lum > 5.0) {
            if(lum < 15.0) {
                if( (hue >= (_15PB10[x] - 0.035)) && (hue < (_15PB10[x] + 0.052) && x <= 45)) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _15PB10[y] - _15PB10[x] ;
                    lbe = _15PB10[y];
                    correctL = true;
                } else if (( hue >= ( _3PB10[x] - 0.052))  && (hue < (_45PB10[x] + _3PB10[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _3PB10[y] - _3PB10[x];
                    lbe = _3PB10[y];
                    correctL = true;
                } else if (( hue >= (_45PB10[x] + _3PB10[x]) / 2.0)  && (hue < (_45PB10[x] + 0.052)) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _45PB10[y] - _45PB10[x] ;
                    lbe = _45PB10[y];
                    correctL = true;
                } else if (( hue >= (_6PB10[x] - 0.052)  && (hue < (_6PB10[x] + _75PB10[x]) / 2.0))) {
                    correction =  _6PB10[y] - _6PB10[x] ;
                    lbe = _6PB10[y];
                    correctL = true;
                } else if (( hue >= (_6PB10[x] + _75PB10[x]) / 2.0)  && (hue < (_9PB10[x] + _75PB10[x]) / 2.0)) {
                    correction =  _75PB10[y] - _75PB10[x] ;
                    lbe = _75PB10[y];
                    correctL = true;
                } else if (( hue >= (_9PB10[x] + _75PB10[x]) / 2.0)  && (hue < (_9PB10[x] + _10PB10[x]) / 2.0)) {
                    correction =  _9PB10[y] - _9PB10[x] ;
                    lbe = _9PB10[y];
                    correctL = true;
                } else if (( hue >= (_10PB10[x] + _9PB10[x]) / 2.0)  && (hue < (_1P10[x] + _10PB10[x]) / 2.0)) {
                    correction =  _10PB10[y] - _10PB10[x] ;
                    lbe = _10PB10[y];
                    correctL = true;
                } else if (( hue >= (_10PB10[x] + _1P10[x]) / 2.0)  && (hue < (_1P10[x] + _4P10[x]) / 2.0)) {
                    correction =  _1P10[y] - _1P10[x];
                    lbe = _1P10[y];
                    correctL = true;
                } else if (( hue >= (_1P10[x] + _4P10[x]) / 2.0)  && (hue < (0.035 + _4P10[x]) / 2.0)) {
                    correction =  _4P10[y] - _4P10[x] ;
                    lbe = _4P10[y];
                    correctL = true;
                }
            } else if (lum < 25.0) {
                if( (hue >= (_15PB20[x] - 0.035)) && (hue < (_15PB20[x] + _3PB20[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _15PB20[y] - _15PB20[x] ;
                    lbe = _15PB20[y];
                    correctL = true;
                } else if (( hue >= (_15PB20[x] + _3PB20[x]) / 2.0)  && (hue < (_45PB20[x] + _3PB20[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _3PB20[y] - _3PB20[x] ;
                    lbe = _3PB20[y];
                    correctL = true;
                } else if (( hue >= (_45PB20[x] + _3PB20[x]) / 2.0)  && (hue < ( _45PB20[x] + 0.052)) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _45PB20[y] - _45PB20[x] ;
                    lbe = _45PB20[y];
                    correctL = true;
                } else if (( hue >= (_45PB20[x] + 0.052))  && (hue < (_6PB20[x] + _75PB20[x]) / 2.0)) {
                    correction =  _6PB20[y] - _6PB20[x];
                    lbe = _6PB20[y];
                    correctL = true;
                } else if (( hue >= (_6PB20[x] + _75PB20[x]) / 2.0)  && (hue < (_9PB20[x] + _75PB20[x]) / 2.0)) {
                    correction =  _75PB20[y] - _75PB20[x] ;
                    lbe = _75PB20[y];
                    correctL = true;
                } else if (( hue >= (_9PB20[x] + _75PB20[x]) / 2.0)  && (hue < (_9PB20[x] + _10PB20[x]) / 2.0)) {
                    correction =  _9PB20[y] - _9PB20[x] ;
                    lbe = _9PB20[y];
                    correctL = true;
                } else if (( hue >= (_10PB20[x] + _9PB20[x]) / 2.0)  && (hue < (_1P20[x] + _10PB20[x]) / 2.0)) {
                    correction =  _10PB20[y] - _10PB20[x] ;
                    lbe = _10PB20[y];
                    correctL = true;
                } else if (( hue >= (_10PB20[x] + _1P20[x]) / 2.0)  && (hue < (_1P20[x] + _4P20[x]) / 2.0)) {
                    correction =  _1P20[y] - _1P20[x] ;
                    lbe = _1P20[y];
                    correctL = true;
                } else if (( hue >= (_1P20[x] + _4P20[x]) / 2.0)  && (hue < (0.035 + _4P20[x]) / 2.0)) {
                    correction =  _4P20[y] - _4P20[x] ;
                    lbe = _4P20[y];
                    correctL = true;
                }
            } else if (lum < 35.0) {
                if( (hue >= (_15PB30[x] - 0.035)) && (hue < (_15PB30[x] + _3PB30[x]) / 2.0) && x <= 85 ) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _15PB30[y] - _15PB30[x] ;
                    lbe = _15PB30[y];
                    correctL = true;
                } else if (( hue >= (_15PB30[x] + _3PB30[x]) / 2.0)  && (hue < (_45PB30[x] + _3PB30[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _3PB30[y] - _3PB30[x] ;
                    lbe = _3PB30[y];
                    correctL = true;
                } else if (( hue >= (_45PB30[x] + _3PB30[x]) / 2.0)  && (hue < (_45PB30[x] + 0.052)) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _45PB30[y] - _45PB30[x] ;
                    lbe = _45PB30[y];
                    correctL = true;
                } else if (( hue >= ( _45PB30[x] + 0.052))  && (hue < (_6PB30[x] + _75PB30[x]) / 2.0)) {
                    correction =  _6PB30[y] - _6PB30[x] ;
                    lbe = _6PB30[y];
                    correctL = true;
                } else if (( hue >= (_6PB30[x] + _75PB30[x]) / 2.0)  && (hue < (_9PB30[x] + _75PB30[x]) / 2.0)) {
                    correction =  _75PB30[y] - _75PB30[x] ;
                    lbe = _75PB30[y] ;
                    correctL = true;
                } else if (( hue >= (_9PB30[x] + _75PB30[x]) / 2.0)  && (hue < (_9PB30[x] + _10PB30[x]) / 2.0)) {
                    correction =  _9PB30[y] - _9PB30[x] ;
                    lbe = _9PB30[y];
                    correctL = true;
                } else if (( hue >= (_10PB30[x] + _9PB30[x]) / 2.0)  && (hue < (_1P30[x] + _10PB30[x]) / 2.0)) {
                    correction =  _10PB30[y] - _10PB30[x] ;
                    lbe = _10PB30[y];
                    correctL = true;
                } else if (( hue >= (_10PB30[x] + _1P30[x]) / 2.0)  && (hue < (_1P30[x] + _4P30[x]) / 2.0)) {
                    correction =  _1P30[y] - _1P30[x] ;
                    lbe = _1P30[y];
                    correctL = true;
                } else if (( hue >= (_1P30[x] + _4P30[x]) / 2.0)  && (hue < (0.035 + _4P30[x]) / 2.0)) {
                    correction =  _4P30[y] - _4P30[x] ;
                    lbe = _4P30[y];
                    correctL = true;
                }
            } else if (lum < 45.0) {
                if( (hue <= (_05PB40[x] + _15PB40[x]) / 2.0) && (hue > (_05PB40[x] + _10B40[x]) / 2.0) && x < 75 ) {
                    if(y > 75) {
                        y = 75;
                    }

                    correction =  _05PB40[y] - _05PB40[x] ;
                    lbe = _05PB40[y];
                    correctL = true;
                } else if( (hue <= (_05PB40[x] + _10B40[x]) / 2.0) && (hue > (_10B40[x] + _9B40[x]) / 2.0) && x < 70 ) {
                    if(y > 70) {
                        y = 70;
                    }

                    correction =  _10B40[y] - _10B40[x] ;
                    lbe = _10B40[y];
                    correctL = true;
                } else if( (hue <= (_10B40[x] + _9B40[x]) / 2.0) && (hue > (_9B40[x] + _7B40[x]) / 2.0) && x < 70 ) {
                    if(y > 70) {
                        y = 70;
                    }

                    correction =  _9B40[y] - _9B40[x] ;
                    lbe = _9B40[y];
                    correctL = true;
                } else if( (hue <= (_9B40[x] + _7B40[x]) / 2.0) && (hue > (_5B40[x] + _7B40[x]) / 2.0) && x < 70 ) {
                    if(y > 70) {
                        y = 70;
                    }

                    correction =  _7B40[y] - _7B40[x] ;
                    lbe = _7B40[y];
                    correctL = true;
                } else if (( hue <= (_5B40[x] + _7B40[x]) / 2.0)  && (hue > (_5B40[x] - 0.035)) && x < 70) {
                    if(y > 70) {
                        y = 70;    //
                    }

                    correction =  _5B40[y] - _5B40[x] ;
                    lbe =  _5B40[y];
                    correctL = true;
                }

                else if( (hue >= (_15PB40[x] - 0.035)) && (hue < (_15PB40[x] + _3PB40[x]) / 2.0) && x <= 85 ) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _15PB40[y] - _15PB40[x] ;
                    lbe = _15PB40[y];
                    correctL = true;
                } else if (( hue >= (_15PB40[x] + _3PB40[x]) / 2.0)  && (hue < (_45PB40[x] + _3PB40[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _3PB40[y] - _3PB40[x] ;
                    lbe = _3PB40[y];
                    correctL = true;
                } else if (( hue >= (_45PB40[x] + _3PB40[x]) / 2.0)  && (hue < (_45PB40[x] + 0.052)) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _45PB40[y] - _45PB40[x] ;
                    lbe = _45PB40[y] ;
                    correctL = true;
                } else if (( hue >= (_45PB40[x] + 0.052))  && (hue < (_6PB40[x] + _75PB40[x]) / 2.0)) {
                    correction =  _6PB40[y] - _6PB40[x] ;
                    lbe = _6PB40[y];
                    correctL = true;
                } else if (( hue >= (_6PB40[x] + _75PB40[x]) / 2.0)  && (hue < (_9PB40[x] + _75PB40[x]) / 2.0)) {
                    correction =  _75PB40[y] - _75PB40[x] ;
                    lbe = _75PB40[y];
                    correctL = true;
                } else if (( hue >= (_9PB40[x] + _75PB40[x]) / 2.0)  && (hue < (_9PB40[x] + _10PB40[x]) / 2.0)) {
                    correction =  _9PB40[y] - _9PB40[x] ;
                    lbe = _9PB40[y];
                    correctL = true;
                } else if (( hue >= (_10PB40[x] + _9PB40[x]) / 2.0)  && (hue < (_1P40[x] + _10PB40[x]) / 2.0)) {
                    correction =  _10PB40[y] - _10PB40[x] ;
                    lbe = _10PB40[y];
                    correctL = true;
                } else if (( hue >= (_10PB40[x] + _1P40[x]) / 2.0)  && (hue < (_1P40[x] + _4P40[x]) / 2.0)) {
                    correction =  _1P40[y] - _1P40[x] ;
                    lbe = _1P40[y];
                    correctL = true;
                } else if (( hue >= (_1P40[x] + _4P40[x]) / 2.0)  && (hue < (0.035 + _4P40[x]) / 2.0)) {
                    correction =  _4P40[y] - _4P40[x] ;
                    lbe = _4P40[y];
                    correctL = true;
                }
            } else if (lum < 55.0) {
                if( (hue <= (_05PB50[x] + _15PB50[x]) / 2.0) && (hue > (_05PB50[x] + _10B50[x]) / 2.0) && x < 79 ) {
                    if(y > 79) {
                        y = 79;
                    }

                    correction =  _05PB50[y] - _05PB50[x] ;
                    lbe = _05PB50[y];
                    correctL = true;
                } else if( (hue <= (_05PB50[x] + _10B50[x]) / 2.0) && (hue > (_10B50[x] + _9B50[x]) / 2.0) && x < 79 ) {
                    if(y > 79) {
                        y = 79;
                    }

                    correction =  _10B50[y] - _10B50[x] ;
                    lbe = _10B50[y];
                    correctL = true;
                } else if( (hue <= (_10B50[x] + _9B50[x]) / 2.0) && (hue > (_9B50[x] + _7B50[x]) / 2.0) && x < 79 ) {
                    if(y > 79) {
                        y = 79;
                    }

                    correction =  _9B50[y] - _9B50[x] ;
                    lbe = _9B50[y];
                    correctL = true;
                } else if( (hue <= (_9B50[x] + _7B50[x]) / 2.0) && (hue > (_5B50[x] + _7B50[x]) / 2.0) && x < 79 ) {
                    if(y > 79) {
                        y = 79;
                    }

                    correction =  _7B50[y] - _7B50[x] ;
                    lbe = _7B50[y];
                    correctL = true;
                } else if (( hue <= (_5B50[x] + _7B50[x]) / 2.0)  && (hue > (_5B50[x] - 0.035)) && x < 79) {
                    if(y > 79) {
                        y = 79;    //
                    }

                    correction =  _5B50[y] - _5B50[x] ;
                    lbe = _5B50[y];
                    correctL = true;
                }

                else if( (hue >= (_15PB50[x] - 0.035)) && (hue < (_15PB50[x] + _3PB50[x]) / 2.0) && x <= 85 ) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _15PB50[y] - _15PB50[x] ;
                    lbe = _15PB50[y];
                    correctL = true;
                } else if (( hue >= (_15PB50[x] + _3PB50[x]) / 2.0)  && (hue < (_45PB50[x] + _3PB50[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _3PB50[y] - _3PB50[x] ;
                    lbe = _3PB50[y];
                    correctL = true;
                } else if (( hue >= (_45PB50[x] + _3PB50[x]) / 2.0)  && (hue < (_6PB50[x] + _45PB50[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _45PB50[y] - _45PB50[x] ;
                    lbe = _45PB50[y];
                    correctL = true;
                } else if (( hue >= (_6PB50[x] + _45PB50[x]) / 2.0)  && (hue < (_6PB50[x] + _75PB50[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _6PB50[y] - _6PB50[x] ;
                    lbe = _6PB50[y];
                    correctL = true;
                } else if (( hue >= (_6PB50[x] + _75PB50[x]) / 2.0)  && (hue < (_9PB50[x] + _75PB50[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _75PB50[y] - _75PB50[x] ;
                    lbe = _75PB50[y];
                    correctL = true;
                } else if (( hue >= (_9PB50[x] + _75PB50[x]) / 2.0)  && (hue < (_9PB50[x] + _10PB50[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _9PB50[y] - _9PB50[x] ;
                    lbe = _9PB50[y];
                    correctL = true;
                } else if (( hue >= (_10PB50[x] + _9PB50[x]) / 2.0)  && (hue < (_1P50[x] + _10PB50[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10PB50[y] - _10PB50[x] ;
                    lbe = _10PB50[y];
                    correctL = true;
                } else if (( hue >= (_10PB50[x] + _1P50[x]) / 2.0)  && (hue < (_1P50[x] + _4P50[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _1P50[y] - _1P50[x] ;
                    lbe = _1P50[y];
                    correctL = true;
                } else if (( hue >= (_1P50[x] + _4P50[x]) / 2.0)  && (hue < (0.035 + _4P50[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _4P50[y] - _4P50[x] ;
                    lbe = _4P50[y];
                    correctL = true;
                }
            } else if (lum < 65.0) {
                if( (hue <= (_05PB60[x] + _15PB60[x]) / 2.0) && (hue > (_05PB60[x] + _10B60[x]) / 2.0) && x < 79 ) {
                    if(y > 79) {
                        y = 79;
                    }

                    correction =  _05PB60[y] - _05PB60[x] ;
                    lbe = _05PB60[y];
                    correctL = true;
                } else if( (hue <= (_05PB60[x] + _10B60[x]) / 2.0) && (hue > (_10B60[x] + _9B60[x]) / 2.0) && x < 79 ) {
                    if(y > 79) {
                        y = 79;
                    }

                    correction =  _10B60[y] - _10B60[x] ;
                    lbe = _10B60[y];
                    correctL = true;
                } else if( (hue <= (_10B60[x] + _9B60[x]) / 2.0) && (hue > (_9B60[x] + _7B60[x]) / 2.0) && x < 79 ) {
                    if(y > 79) {
                        y = 79;
                    }

                    correction =  _9B60[y] - _9B60[x] ;
                    lbe = _9B60[y];
                    correctL = true;
                } else if( (hue <= (_9B60[x] + _7B60[x]) / 2.0) && (hue > (_5B60[x] + _7B60[x]) / 2.0) && x < 79 ) {
                    if(y > 79) {
                        y = 79;
                    }

                    correction =  _7B60[y] - _7B60[x] ;
                    lbe = _7B60[y];
                    correctL = true;
                } else if (( hue <= (_5B60[x] + _7B60[x]) / 2.0)  && (hue > (_5B60[x] - 0.035)) && x < 79) {
                    if(y > 79) {
                        y = 79;    //
                    }

                    correction =  _5B60[y] - _5B60[x] ;
                    lbe = _5B60[y];
                    correctL = true;
                }

                else if( (hue >= (_15PB60[x] - 0.035)) && (hue < (_15PB60[x] + _3PB60[x]) / 2.0) && x <= 85 ) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _15PB60[y] - _15PB60[x] ;
                    lbe = _15PB60[y];
                    correctL = true;
                } else if (( hue >= (_15PB60[x] + _3PB60[x]) / 2.0)  && (hue < (_45PB60[x] + _3PB60[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _3PB60[y] - _3PB60[x] ;
                    lbe = _3PB60[y];
                    correctL = true;
                } else if (( hue >= (_45PB60[x] + _3PB60[x]) / 2.0)  && (hue < (_6PB60[x] + _45PB60[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _45PB60[y] - _45PB60[x] ;
                    lbe = _45PB60[y];
                    correctL = true;
                } else if (( hue >= (_6PB60[x] + _45PB60[x]) / 2.0)  && (hue < (_6PB60[x] + _75PB60[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _6PB60[y] - _6PB60[x] ;
                    lbe = _6PB60[y];
                    correctL = true;
                } else if (( hue >= (_6PB60[x] + _75PB60[x]) / 2.0)  && (hue < (_9PB60[x] + _75PB60[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _75PB60[y] - _75PB60[x] ;
                    lbe = _75PB60[y];
                    correctL = true;
                } else if (( hue >= (_9PB60[x] + _75PB60[x]) / 2.0)  && (hue < (_9PB60[x] + _10PB60[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _9PB60[y] - _9PB60[x] ;
                    lbe = _9PB60[y];
                    correctL = true;
                } else if (( hue >= (_10PB60[x] + _9PB60[x]) / 2.0)  && (hue < (_1P60[x] + _10PB60[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10PB60[y] - _10PB60[x] ;
                    lbe = _10PB60[y];
                    correctL = true;
                } else if (( hue >= (_10PB60[x] + _1P60[x]) / 2.0)  && (hue < (_1P60[x] + _4P60[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _1P60[y] - _1P60[x] ;
                    lbe = _1P60[y];
                    correctL = true;
                } else if (( hue >= (_1P60[x] + _4P60[x]) / 2.0)  && (hue < (0.035 + _4P60[x]) / 2.0) && x <= 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _4P60[y] - _4P60[x] ;
                    lbe = _4P60[y];
                    correctL = true;
                }
            } else if (lum < 75.0) {
                if( (hue <= (_05PB70[x] + _15PB70[x]) / 2.0) && (hue > (_05PB70[x] + _10B70[x]) / 2.0) && x < 50 ) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _05PB70[y] - _05PB70[x] ;
                    lbe = _05PB70[y];
                    correctL = true;
                } else if( (hue <= (_05PB70[x] + _10B70[x]) / 2.0) && (hue > (_10B70[x] + _9B70[x]) / 2.0) && x < 50 ) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _10B70[y] - _10B70[x] ;
                    lbe = _10B70[y];
                    correctL = true;
                } else if( (hue <= (_10B70[x] + _9B70[x]) / 2.0) && (hue > (_9B70[x] + _7B70[x]) / 2.0) && x < 50 ) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _9B70[y] - _9B70[x] ;
                    lbe = _9B70[y];
                    correctL = true;
                } else if( (hue <= (_9B70[x] + _7B70[x]) / 2.0) && (hue > (_5B70[x] + _7B70[x]) / 2.0) && x < 50 ) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _7B70[y] - _7B70[x] ;
                    lbe = _7B70[y];
                    correctL = true;
                } else if (( hue <= (_5B70[x] + _7B70[x]) / 2.0)  && (hue > (_5B70[x] - 0.035)) && x < 50) {
                    if(y > 49) {
                        y = 49;    //
                    }

                    correction =  _5B70[y] - _5B70[x] ;
                    lbe =  _5B70[y];
                    correctL = true;
                }

                else if( (hue >= (_15PB70[x] - 0.035)) && (hue < (_15PB70[x] + _3PB70[x]) / 2.0) && x < 50 ) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _15PB70[y] - _15PB70[x] ;
                    lbe = _15PB70[y];
                    correctL = true;
                } else if (( hue >= (_45PB70[x] + _3PB70[x]) / 2.0)  && (hue < (_6PB70[x] + _45PB70[x]) / 2.0) && x < 50) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _45PB70[y] - _45PB70[x] ;
                    lbe = _45PB70[y];
                    correctL = true;
                } else if (( hue >= (_6PB70[x] + _45PB70[x]) / 2.0)  && (hue < (_6PB70[x] + _75PB70[x]) / 2.0) && x < 50) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _6PB70[y] - _6PB70[x] ;
                    lbe = _6PB70[y];
                    correctL = true;
                } else if (( hue >= (_6PB70[x] + _75PB70[x]) / 2.0)  && (hue < (_9PB70[x] + _75PB70[x]) / 2.0) && x < 50) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _75PB70[y] - _75PB70[x] ;
                    lbe = _75PB70[y];
                    correctL = true;
                } else if (( hue >= (_9PB70[x] + _75PB70[x]) / 2.0)  && (hue < (_9PB70[x] + 0.035)) && x < 50) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _9PB70[y] - _9PB70[x] ;
                    lbe = _9PB70[y];
                    correctL = true;
                }
            } else if (lum < 85.0) {
                if( (hue <= (_05PB80[x] + _15PB80[x]) / 2.0) && (hue > (_05PB80[x] + _10B80[x]) / 2.0) && x < 40 ) {
                    if(y > 39) {
                        y = 39;
                    }

                    correction =  _05PB80[y] - _05PB80[x] ;
                    lbe = _05PB80[y] ;
                    correctL = true;
                } else if( (hue <= (_05PB80[x] + _10B80[x]) / 2.0) && (hue > (_10B80[x] + _9B80[x]) / 2.0) && x < 40 ) {
                    if(y > 39) {
                        y = 39;
                    }

                    correction =  _10B80[y] - _10B80[x] ;
                    lbe = _10B80[y];
                    correctL = true;
                } else if( (hue <= (_10B80[x] + _9B80[x]) / 2.0) && (hue > (_9B80[x] + _7B80[x]) / 2.0) && x < 40 ) {
                    if(y > 39) {
                        y = 39;
                    }

                    correction =  _9B80[y] - _9B80[x] ;
                    lbe = _9B80[y];
                    correctL = true;
                } else if( (hue <= (_9B80[x] + _7B80[x]) / 2.0) && (hue > (_5B80[x] + _7B80[x]) / 2.0) && x < 50 ) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _7B80[y] - _7B80[x] ;
                    lbe = _7B80[y];
                    correctL = true;
                } else if (( hue <= (_5B80[x] + _7B80[x]) / 2.0)  && (hue > (_5B80[x] - 0.035)) && x < 50) {
                    if(y > 49) {
                        y = 49;    //
                    }

                    correction =  _5B80[y] - _5B80[x] ;
                    lbe = _5B80[y];
                    correctL = true;
                }

                else if( (hue >= (_15PB80[x] - 0.035)) && (hue < (_15PB80[x] + _3PB80[x]) / 2.0) && x < 50 ) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _15PB80[y] - _15PB80[x] ;
                    lbe = _15PB80[y];
                    correctL = true;
                } else if (( hue >= (_45PB80[x] + _3PB80[x]) / 2.0)  && (hue < (_6PB80[x] + _45PB80[x]) / 2.0) && x < 50) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _45PB80[y] - _45PB80[x] ;
                    lbe = _45PB80[y];
                    correctL = true;
                } else if (( hue >= (_6PB80[x] + _45PB80[x]) / 2.0)  && (hue < (_6PB80[x] + _75PB80[x]) / 2.0) && x < 50) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _6PB80[y] - _6PB80[x] ;
                    lbe = _6PB80[y];
                    correctL = true;
                } else if (( hue >= (_6PB80[x] + _75PB80[x]) / 2.0)  && (hue < (_9PB80[x] + _75PB80[x]) / 2.0) && x < 50) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _75PB80[y] - _75PB80[x] ;
                    lbe = _75PB80[y];
                    correctL = true;
                } else if (( hue >= (_9PB80[x] + _75PB80[x]) / 2.0)  && (hue < (_9PB80[x] + 0.035)) && x < 50) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _9PB80[y] - _9PB80[x] ;
                    lbe = _9PB80[y];
                    correctL = true;
                }
            }
        }
    }
    // end PB correction

    //red yellow correction
    else if(zone == 2) {
        if(lum > 15.0) {
            if(lum < 25.0) {
                if( (hue <= (_10YR20[x] + 0.035)) && (hue > (_10YR20[x] + _85YR20[x]) / 2.0) && x <= 45) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _10YR20[y] - _10YR20[x] ;
                    lbe = _10YR20[y];
                    correctL = true;
                } else if (( hue <= (_85YR20[x] + _10YR20[x]) / 2.0)  && (hue > (_85YR20[x] + 0.035) && x <= 45)) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _85YR20[y] - _85YR20[x] ;
                    lbe = _85YR20[y];
                    correctL = true;
                }
            } else if (lum < 35.0) {
                if( (hue <= (_10YR30[x] + 0.035)) && (hue > (_10YR30[x] + _85YR30[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10YR30[y] - _10YR30[x] ;
                    lbe = _10YR30[y];
                    correctL = true;
                } else if( (hue <= (_10YR30[x] + _85YR30[x]) / 2.0) && (hue > (_85YR30[x] + _7YR30[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _85YR30[y] - _85YR30[x] ;
                    lbe = _85YR30[y];
                    correctL = true;
                } else if (( hue <= (_85YR30[x] + _7YR30[x]) / 2.0)  && (hue > (_7YR30[x] + _55YR30[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _7YR30[y] - _7YR30[x] ;
                    lbe = _7YR30[y];
                    correctL = true;
                } else if (( hue <= (_7YR30[x] + _55YR30[x]) / 2.0)  && (hue > (_55YR30[x] + _4YR30[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _55YR30[y] - _55YR30[x] ;
                    lbe = _55YR30[y];
                    correctL = true;
                } else if (( hue <= (_55YR30[x] + _4YR30[x]) / 2.0)  && (hue > (_4YR30[x] + _25YR30[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _4YR30[y] - _4YR30[x] ;
                    lbe = _4YR30[y];
                    correctL = true;
                } else if (( hue <= (_4YR30[x] + _25YR30[x]) / 2.0)  && (hue > (_25YR30[x] + _10R30[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _25YR30[y] - _25YR30[x] ;
                    lbe = _25YR30[y];
                    correctL = true;
                } else if (( hue <= (_25YR30[x] + _10R30[x]) / 2.0)  && (hue > (_10R30[x] + _9R30[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10R30[y] - _10R30[x] ;
                    lbe = _10R30[y];
                    correctL = true;
                } else if (( hue <= (_10R30[x] + _9R30[x]) / 2.0)  && (hue > (_9R30[x] + _7R30[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _9R30[y] - _9R30[x] ;
                    lbe = _9R30[y];
                    correctL = true;
                } else if (( hue <= (_9R30[x] + _7R30[x]) / 2.0)  && (hue > (_7R30[x] - 0.035)) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _7R30[y] - _7R30[x] ;
                    lbe = _7R30[y] ;
                    correctL = true;
                }
            } else if (lum < 45.0) {
                if( (hue <= (_10YR40[x] + 0.035)) && (hue > (_10YR40[x] + _85YR40[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10YR40[y] - _10YR40[x] ;
                    lbe = _10YR40[y];
                    correctL = true;
                } else if( (hue <= (_10YR40[x] + _85YR40[x]) / 2.0) && (hue > (_85YR40[x] + _7YR40[x]) / 2.0) && x < 85 ) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _85YR40[y] - _85YR40[x] ;
                    lbe = _85YR40[y];
                    correctL = true;
                } else if (( hue <= (_85YR40[x] + _7YR40[x]) / 2.0)  && (hue > (_7YR40[x] + _55YR40[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _7YR40[y] - _7YR40[x] ;
                    lbe = _7YR40[y];
                    correctL = true;
                } else if (( hue <= (_7YR40[x] + _55YR40[x]) / 2.0)  && (hue > (_55YR40[x] + _4YR40[x]) / 2.0) && x < 85 ) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _55YR40[y] - _55YR40[x] ;
                    lbe = _55YR40[y];
                    correctL = true;
                } else if (( hue <= (_55YR40[x] + _4YR40[x]) / 2.0)  && (hue > (_4YR40[x] + _25YR40[x]) / 2.0) && x < 85 ) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _4YR40[y] - _4YR40[x] ;
                    lbe = _4YR40[y];
                    correctL = true;
                } else if (( hue <= (_4YR40[x] + _25YR40[x]) / 2.0)  && (hue > (_25YR40[x] + _10R40[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _25YR40[y] - _25YR40[x] ;
                    lbe = _25YR40[y] ;
                    correctL = true;
                } else if (( hue <= (_25YR40[x] + _10R40[x]) / 2.0)  && (hue > (_10R40[x] + _9R40[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10R40[y] - _10R40[x] ;
                    lbe = _10R40[y];
                    correctL = true;
                } else if (( hue <= (_10R40[x] + _9R40[x]) / 2.0)  && (hue > (_9R40[x] + _7R40[x]) / 2.0) && x < 85 ) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _9R40[y] - _9R40[x] ;
                    lbe = _9R40[y];
                    correctL = true;
                } else if (( hue <= (_9R40[x] + _7R40[x]) / 2.0)  && (hue > (_7R40[x] - 0.035)) && x < 85 ) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _7R40[y] - _7R40[x] ;
                    lbe = _7R40[y];
                    correctL = true;
                }
            } else if (lum < 55.0) {
                if( (hue <= (_10YR50[x] + 0.035)) && (hue > (_10YR50[x] + _85YR50[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10YR50[y] - _10YR50[x] ;
                    lbe = _10YR50[y];
                    correctL = true;
                } else if( (hue <= (_10YR50[x] + _85YR50[x]) / 2.0) && (hue > (_85YR50[x] + _7YR50[x]) / 2.0) && x < 85 ) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _85YR50[y] - _85YR50[x] ;
                    lbe = _85YR50[y];
                    correctL = true;
                } else if (( hue <= (_85YR50[x] + _7YR50[x]) / 2.0)  && (hue > (_7YR50[x] + _55YR50[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _7YR50[y] - _7YR50[x] ;
                    lbe = _7YR50[y];
                    correctL = true;
                } else if (( hue <= (_7YR50[x] + _55YR50[x]) / 2.0)  && (hue > (_55YR50[x] + _4YR50[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _55YR50[y] - _55YR50[x] ;
                    lbe = _55YR50[y];
                    correctL = true;
                } else if (( hue <= (_55YR50[x] + _4YR50[x]) / 2.0)  && (hue > (_4YR50[x] + _25YR50[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _4YR50[y] - _4YR50[x] ;
                    lbe = _4YR50[y];
                    correctL = true;
                } else if (( hue <= (_4YR50[x] + _25YR50[x]) / 2.0)  && (hue > (_25YR50[x] + _10R50[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _25YR50[y] - _25YR50[x] ;
                    lbe = _25YR50[y];
                    correctL = true;
                } else if (( hue <= (_25YR50[x] + _10R50[x]) / 2.0)  && (hue > (_10R50[x] + _9R50[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10R50[y] - _10R50[x] ;
                    lbe = _10R50[y];
                    correctL = true;
                } else if (( hue <= (_10R50[x] + _9R50[x]) / 2.0)  && (hue > (_9R50[x] + _7R50[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _9R50[y] - _9R50[x] ;
                    lbe = _9R50[y];
                    correctL = true;
                } else if (( hue <= (_9R50[x] + _7R50[x]) / 2.0)  && (hue > (_7R50[x] - 0.035)) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _7R50[y] - _7R50[x] ;
                    lbe = _7R50[y];
                    correctL = true;
                }
            } else if (lum < 65.0) {
                if( (hue <= (_10YR60[x] + 0.035)) && (hue > (_10YR60[x] + _85YR60[x]) / 2.0)) {
                    ;
                    correction =  _10YR60[y] - _10YR60[x] ;
                    lbe = _10YR60[y];
                    correctL = true;
                } else if( (hue <= (_10YR60[x] + _85YR60[x]) / 2.0) && (hue > (_85YR60[x] + _7YR60[x]) / 2.0) ) {
                    ;
                    correction =  _85YR60[y] - _85YR60[x] ;
                    lbe = _85YR60[y];
                    correctL = true;
                } else if (( hue <= (_85YR60[x] + _7YR60[x]) / 2.0)  && (hue > (_7YR60[x] + _55YR60[x]) / 2.0)) {
                    correction =  _7YR60[y] - _7YR60[x] ;
                    lbe = _7YR60[y];
                    correctL = true;
                } else if (( hue <= (_7YR60[x] + _55YR60[x]) / 2.0)  && (hue > (_55YR60[x] + _4YR60[x]) / 2.0)) {
                    correction =  _55YR60[y] - _55YR60[x] ;
                    lbe = _55YR60[y];
                    correctL = true;
                } else if (( hue <= (_55YR60[x] + _4YR60[x]) / 2.0)  && (hue > (_4YR60[x] + _25YR60[x]) / 2.0)) {
                    correction =  _4YR60[y] - _4YR60[x] ;
                    lbe = _4YR60[y];
                    correctL = true;
                } else if (( hue <= (_4YR60[x] + _25YR60[x]) / 2.0)  && (hue > (_25YR60[x] + _10R60[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _25YR60[y] - _25YR60[x] ;
                    lbe = _25YR60[y];
                    correctL = true;
                } else if (( hue <= (_25YR60[x] + _10R60[x]) / 2.0)  && (hue > (_10R60[x] + _9R60[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10R60[y] - _10R60[x] ;
                    lbe = _10R60[y];
                    correctL = true;
                } else if (( hue <= (_10R60[x] + _9R60[x]) / 2.0)  && (hue > (_9R60[x] + _7R60[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _9R60[y] - _9R60[x] ;
                    lbe = _9R60[y];
                    correctL = true;
                } else if (( hue <= (_9R60[x] + _7R60[x]) / 2.0)  && (hue > (_7R60[x] - 0.035)) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _7R60[y] - _7R60[x] ;
                    lbe = _7R60[y];
                    correctL = true;
                }
            } else if (lum < 75.0) {
                if( (hue <= (_10YR70[x] + 0.035)) && (hue > (_10YR70[x] + _85YR70[x]) / 2.0)) {
                    correction =  _10YR70[y] - _10YR70[x] ;
                    lbe = _10YR70[y];
                    correctL = true;
                } else if( (hue <= (_10YR70[x] + _85YR70[x]) / 2.0) && (hue > (_85YR70[x] + _7YR70[x]) / 2.0)) {
                    correction =  _85YR70[y] - _85YR70[x] ;
                    lbe = _85YR70[y];
                    correctL = true;
                }

                if (( hue <= (_85YR70[x] + _7YR70[x]) / 2.0)  && (hue > (_7YR70[x] + _55YR70[x]) / 2.0)) {
                    correction =  _7YR70[y] - _7YR70[x] ;
                    lbe = _7YR70[y];
                    correctL = true;
                } else if (( hue <= (_7YR70[x] + _55YR70[x]) / 2.0)  && (hue > (_55YR70[x] + _4YR70[x]) / 2.0)) {
                    correction =  _55YR70[y] - _55YR70[x] ;
                    lbe = _55YR70[y];
                    correctL = true;
                } else if (( hue <= (_55YR70[x] + _4YR70[x]) / 2.0)  && (hue > (_4YR70[x] + _25YR70[x]) / 2.0)) {
                    correction =  _4YR70[y] - _4YR70[x] ;
                    lbe = _4YR70[y];
                    correctL = true;
                } else if (( hue <= (_4YR70[x] + _25YR70[x]) / 2.0)  && (hue > (_25YR70[x] + _10R70[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _25YR70[y] - _25YR70[x] ;
                    lbe = _25YR70[y];
                    correctL = true;
                } else if (( hue <= (_25YR70[x] + _10R70[x]) / 2.0)  && (hue > (_10R70[x] + _9R70[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10R70[y] - _10R70[x] ;
                    lbe = _10R70[y];
                    correctL = true;
                } else if (( hue <= (_10R70[x] + _9R70[x]) / 2.0)  && (hue > (_9R70[x] + _7R70[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _9R70[y] - _9R70[x] ;
                    lbe = _9R70[y] ;
                    correctL = true;
                } else if (( hue <= (_9R70[x] + _7R70[x]) / 2.0)  && (hue > (_7R70[x] - 0.035)) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _7R70[y] - _7R70[x] ;
                    lbe = _7R70[y];
                    correctL = true;
                }
            } else if (lum < 85.0) {
                if( (hue <= (_10YR80[x] + 0.035)) && (hue > (_10YR80[x] + _85YR80[x]) / 2.0)) {
                    correction =  _10YR80[y] - _10YR80[x] ;
                    lbe = _10YR80[y];
                    correctL = true;
                } else if( (hue <= (_10YR80[x] + _85YR80[x]) / 2.0) && (hue > (_85YR80[x] + _7YR80[x]) / 2.0)) {
                    correction =  _85YR80[y] - _85YR80[x] ;
                    lbe = _85YR80[y];
                } else if (( hue <= (_85YR80[x] + _7YR80[x]) / 2.0)  && (hue > (_7YR80[x] + _55YR80[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _7YR80[y] - _7YR80[x] ;
                    lbe = _7YR80[y];
                    correctL = true;
                } else if (( hue <= (_7YR80[x] + _55YR80[x]) / 2.0)  && (hue > (_55YR80[x] + _4YR80[x]) / 2.0) && x < 45) {
                    correction =  _55YR80[y] - _55YR80[x] ;
                    lbe = _55YR80[y];
                    correctL = true;
                } else if (( hue <= (_55YR80[x] + _4YR80[x]) / 2.0)  && (hue > (_4YR80[x] - 0.035) && x < 45)) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _4YR80[y] - _4YR80[x] ;
                    lbe = _4YR80[y] ;
                    correctL = true;
                }
            } else if (lum < 95.0) {
                if( (hue <= (_10YR90[x] + 0.035)) && (hue > (_10YR90[x] - 0.035) && x < 85)) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10YR90[y] - _10YR90[x] ;
                    lbe = _10YR90[y];
                    correctL = true;
                } else if ( hue <= (_85YR90[x] + 0.035)  && hue > (_85YR90[x] - 0.035) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _85YR90[y] - _85YR90[x] ;
                    lbe = _85YR90[y];
                    correctL = true;
                } else if (( hue <= (_55YR90[x] + 0.035)  && (hue > (_55YR90[x] - 0.035) && x < 45))) {
                    if(y > 49) {
                        y = 49;
                    }

                    correction =  _55YR90[y] - _55YR90[x] ;
                    lbe = _55YR90[y];
                    correctL = true;
                }
            }
        }
    }
    //end red yellow

    //Green yellow correction
    else if(zone == 3) {
        if (lum >= 25.0) {
            if (lum < 35.0) {
                if( (hue <= (_7G30[x] + 0.035)) && (hue > (_7G30[x] + _5G30[x]) / 2.0) ) {
                    correction =  _7G30[y] - _7G30[x] ;
                    lbe = _7G30[y];
                    correctL = true;
                } else if( (hue <= (_7G30[x] + _5G30[x]) / 2.0) && (hue > (_5G30[x] + _25G30[x]) / 2.0)) {
                    correction =  _5G30[y] - _5G30[x] ;
                    lbe = _5G30[y];
                    correctL = true;
                } else if (( hue <= (_25G30[x] + _5G30[x]) / 2.0)  && (hue > (_25G30[x] + _1G30[x]) / 2.0)) {
                    correction =  _25G30[y] - _25G30[x] ;
                    lbe = _25G30[y];
                    correctL = true;
                } else if (( hue <= (_1G30[x] + _25G30[x]) / 2.0)  && (hue > (_1G30[x] + _10GY30[x]) / 2.0)) {
                    correction =  _1G30[y] - _1G30[x] ;
                    lbe = _1G30[y];
                    correctL = true;
                } else if (( hue <= (_1G30[x] + _10GY30[x]) / 2.0)  && (hue > (_10GY30[x] + _75GY30[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10GY30[y] - _10GY30[x] ;
                    lbe =  _10GY30[y];
                    correctL = true;
                } else if (( hue <= (_10GY30[x] + _75GY30[x]) / 2.0)  && (hue > (_75GY30[x] + _5GY30[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _75GY30[y] - _75GY30[x] ;
                    lbe = _75GY30[y];
                    correctL = true;
                } else if (( hue <= (_5GY30[x] + _75GY30[x]) / 2.0)  && (hue > (_5GY30[x] - 0.035)) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _5GY30[y] - _5GY30[x] ;
                    lbe = _5GY30[y] ;
                    correctL = true;
                }
            } else if (lum < 45.0) {
                if( (hue <= (_7G40[x] + 0.035)) && (hue > (_7G40[x] + _5G40[x]) / 2.0) ) {
                    correction =  _7G40[y] - _7G40[x] ;
                    lbe = _7G40[y];
                    correctL = true;
                } else if( (hue <= (_7G40[x] + _5G40[x]) / 2.0) && (hue > (_5G40[x] + _25G40[x]) / 2.0)) {
                    correction =  _5G40[y] - _5G40[x] ;
                    lbe = _5G40[y];
                    correctL = true;
                } else if (( hue <= (_25G40[x] + _5G40[x]) / 2.0)  && (hue > (_25G40[x] + _1G40[x]) / 2.0)) {
                    correction =  _25G40[y] - _25G40[x] ;
                    lbe = _25G40[y];
                    correctL = true;
                } else if (( hue <= (_1G40[x] + _25G40[x]) / 2.0)  && (hue > (_1G40[x] + _10GY40[x]) / 2.0)) {
                    correction =  _1G40[y] - _1G40[x] ;
                    lbe = _1G40[y];
                    correctL = true;
                } else if (( hue <= (_1G40[x] + _10GY40[x]) / 2.0)  && (hue > (_10GY40[x] + _75GY40[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _10GY40[y] - _10GY40[x] ;
                    lbe = _10GY40[y];
                    correctL = true;
                } else if (( hue <= (_10GY40[x] + _75GY40[x]) / 2.0)  && (hue > (_75GY40[x] + _5GY40[x]) / 2.0) && x < 85) {
                    if(y > 89) {
                        y = 89;
                    }

                    correction =  _75GY40[y] - _75GY40[x] ;
                    lbe = _75GY40[y];
                    correctL = true;
                } else if (( hue <= (_5GY40[x] + _75GY40[x]) / 2.0)  && (hue > (_5GY40[x] - 0.035)) && x < 85) {
                    if(y > 89) {
                        y = 89;    //
                    }

                    correction =  _5GY40[y] - _5GY40[x] ;
                    lbe = _5GY40[y];
                    correctL = true;
                }
            } else if (lum < 55.0) {
                if( (hue <= (_7G50[x] + 0.035)) && (hue > (_7G50[x] + _5G50[x]) / 2.0) ) {
                    correction =  _7G50[y] - _7G50[x] ;
                    lbe = _7G50[y];
                    correctL = true;
                } else if( (hue <= (_7G50[x] + _5G50[x]) / 2.0) && (hue > (_5G50[x] + _25G50[x]) / 2.0)) {
                    correction =  _5G50[y] - _5G50[x] ;
                    lbe = _5G50[y];
                    correctL = true;
                } else if (( hue <= (_25G50[x] + _5G50[x]) / 2.0)  && (hue > (_25G50[x] + _1G50[x]) / 2.0)) {
                    correction =  _25G50[y] - _25G50[x] ;
                    lbe = _25G50[y];
                    correctL = true;
                } else if (( hue <= (_1G50[x] + _25G50[x]) / 2.0)  && (hue > (_1G50[x] + _10GY50[x]) / 2.0)) {
                    correction =  _1G50[y] - _1G50[x] ;
                    lbe = _1G50[y];
                    correctL = true;
                } else if (( hue <= (_1G50[x] + _10GY50[x]) / 2.0)  && (hue > (_10GY50[x] + _75GY50[x]) / 2.0)) {
                    correction =  _10GY50[y] - _10GY50[x] ;
                    lbe = _10GY50[y];
                    correctL = true;
                } else if (( hue <= (_10GY50[x] + _75GY50[x]) / 2.0)  && (hue > (_75GY50[x] + _5GY50[x]) / 2.0)) {
                    correction =  _75GY50[y] - _75GY50[x] ;
                    lbe = _75GY50[y];
                    correctL = true;
                } else if (( hue <= (_5GY50[x] + _75GY50[x]) / 2.0)  && (hue > (_5GY50[x] - 0.035))) {
                    correction =  _5GY50[y] - _5GY50[x] ;
                    lbe = _5GY50[y];
                    correctL = true;
                }
            } else if (lum < 65.0) {
                if( (hue <= (_7G60[x] + 0.035)) && (hue > (_7G60[x] + _5G60[x]) / 2.0) ) {
                    correction =  _7G60[y] - _7G60[x] ;
                    lbe = _7G60[y];
                    correctL = true;
                } else if( (hue <= (_7G60[x] + _5G60[x]) / 2.0) && (hue > (_5G60[x] + _25G60[x]) / 2.0)) {
                    correction =  _5G60[y] - _5G60[x] ;
                    lbe = _5G60[y];
                    correctL = true;
                } else if (( hue <= (_25G60[x] + _5G60[x]) / 2.0)  && (hue > (_25G60[x] + _1G60[x]) / 2.0)) {
                    correction =  _25G60[y] - _25G60[x] ;
                    lbe = _25G60[y];
                    correctL = true;
                } else if (( hue <= (_1G60[x] + _25G60[x]) / 2.0)  && (hue > (_1G60[x] + _10GY60[x]) / 2.0)) {
                    correction =  _1G60[y] - _1G60[x] ;
                    lbe = _1G60[y];
                    correctL = true;
                } else if (( hue <= (_1G60[x] + _10GY60[x]) / 2.0)  && (hue > (_10GY60[x] + _75GY60[x]) / 2.0)) {
                    correction =  _10GY60[y] - _10GY60[x] ;
                    lbe = _10GY60[y];
                    correctL = true;
                } else if (( hue <= (_10GY60[x] + _75GY60[x]) / 2.0)  && (hue > (_75GY60[x] + _5GY60[x]) / 2.0)) {
                    correction =  _75GY60[y] - _75GY60[x] ;
                    lbe = _75GY60[y] ;
                    correctL = true;
                } else if (( hue <= (_5GY60[x] + _75GY60[x]) / 2.0)  && (hue > (_5GY60[x] - 0.035))) {
                    correction =  _5GY60[y] - _5GY60[x] ;
                    lbe = _5GY60[y];
                    correctL = true;
                }
            } else if (lum < 75.0) {
                if( (hue <= (_7G70[x] + 0.035)) && (hue > (_7G70[x] + _5G70[x]) / 2.0) ) {
                    correction =  _7G70[y] - _7G70[x] ;
                    lbe = _7G70[y];
                    correctL = true;
                } else if( (hue <= (_7G70[x] + _5G70[x]) / 2.0) && (hue > (_5G70[x] + _25G70[x]) / 2.0)) {
                    correction =  _5G70[y] - _5G70[x] ;
                    lbe = _5G70[y];
                    correctL = true;
                } else if (( hue <= (_25G70[x] + _5G70[x]) / 2.0)  && (hue > (_25G70[x] + _1G70[x]) / 2.0)) {
                    correction =  _25G70[y] - _25G70[x] ;
                    lbe = _25G70[y];
                    correctL = true;
                } else if (( hue <= (_1G70[x] + _25G70[x]) / 2.0)  && (hue > (_1G70[x] + _10GY70[x]) / 2.0)) {
                    correction =  _1G70[y] - _1G70[x] ;
                    lbe = _1G70[y] ;
                    correctL = true;
                } else if (( hue <= (_1G70[x] + _10GY70[x]) / 2.0)  && (hue > (_10GY70[x] + _75GY70[x]) / 2.0)) {
                    correction =  _10GY70[y] - _10GY70[x] ;
                    lbe = _10GY70[y];
                    correctL = true;
                } else if (( hue <= (_10GY70[x] + _75GY70[x]) / 2.0)  && (hue > (_75GY70[x] + _5GY70[x]) / 2.0)) {
                    correction =  _75GY70[y] - _75GY70[x] ;
                    lbe = _75GY70[y];
                    correctL = true;
                } else if (( hue <= (_5GY70[x] + _75GY70[x]) / 2.0)  && (hue > (_5GY70[x] - 0.035))) {
                    correction =  _5GY70[y] - _5GY70[x] ;
                    lbe =  _5GY70[y];
                    correctL = true;
                }
            } else if (lum < 85.0) {
                if( (hue <= (_7G80[x] + 0.035)) && (hue > (_7G80[x] + _5G80[x]) / 2.0) ) {
                    correction =  _7G80[y] - _7G80[x] ;
                    lbe = _7G80[y];
                    correctL = true;
                } else if( (hue <= (_7G80[x] + _5G80[x]) / 2.0) && (hue > (_5G80[x] + _25G80[x]) / 2.0)) {
                    correction =  _5G80[y] - _5G80[x] ;
                    lbe = _5G80[y];
                    correctL = true;
                } else if (( hue <= (_25G80[x] + _5G80[x]) / 2.0)  && (hue > (_25G80[x] + _1G80[x]) / 2.0)) {
                    correction =  _25G80[y] - _25G80[x] ;
                    lbe = _25G80[y];
                    correctL = true;
                } else if (( hue <= (_1G80[x] + _25G80[x]) / 2.0)  && (hue > (_1G80[x] + _10GY80[x]) / 2.0)) {
                    correction =  _1G80[y] - _1G80[x] ;
                    lbe = _1G80[y];
                    correctL = true;
                } else if (( hue <= (_1G80[x] + _10GY80[x]) / 2.0)  && (hue > (_10GY80[x] + _75GY80[x]) / 2.0)) {
                    correction =  _10GY80[y] - _10GY80[x] ;
                    lbe = _10GY80[y];
                    correctL = true;
                } else if (( hue <= (_10GY80[x] + _75GY80[x]) / 2.0)  && (hue > (_75GY80[x] + _5GY80[x]) / 2.0)) {
                    correction =  _75GY80[y] - _75GY80[x] ;
                    lbe = _75GY80[y];
                    correctL = true;
                } else if (( hue <= (_5GY80[x] + _75GY80[x]) / 2.0)  && (hue > (_5GY80[x] - 0.035))) {
                    correction =  _5GY80[y] - _5GY80[x] ;
                    lbe = _5GY80[y];
                    correctL = true;
                }
            }
        }
    }
    //end green yellow

    //Red purple correction : only for L < 30
    else if(zone == 4) {
        if (lum > 5.0) {
            if (lum < 15.0) {
                if( (hue <= (_5R10[x] + 0.035)) && (hue > (_5R10[x] - 0.043)) && x < 45) {
                    if(y > 44) {
                        y = 44;
                    }

                    correction =  _5R10[y] - _5R10[x] ;
                    lbe = _5R10[y];
                    correctL = true;
                } else if( (hue <= (_25R10[x] + 0.043)) && (hue > (_25R10[x] + _10RP10[x]) / 2.0) && x < 45 ) {
                    if(y > 44) {
                        y = 44;
                    }

                    correction =  _25R10[y] - _25R10[x] ;
                    lbe = _25R10[y];
                    correctL = true;
                } else if ( (hue <= (_25R10[x] + _10RP10[x]) / 2.0) && (hue > (_10RP10[x] - 0.035) ) && x < 45) {
                    if(y > 44) {
                        y = 44;
                    }

                    correction =  _10RP10[y] - _10RP10[x] ;
                    lbe = _10RP10[y];
                    correctL = true;
                }
            } else if (lum < 25.0) {
                if( (hue <= (_5R20[x] + 0.035)) && (hue > (_5R20[x] + _25R20[x]) / 2.0) && x < 70 ) {
                    if(y > 70) {
                        y = 70;
                    }

                    correction =  _5R20[y] - _5R20[x] ;
                    lbe = _5R20[y];
                    correctL = true;
                } else if( (hue <= (_5R20[x] + _25R20[x]) / 2.0) && (hue > (_10RP20[x] + _25R20[x]) / 2.0) && x < 70) {
                    if(y > 70) {
                        y = 70;
                    }

                    correction =  _25R20[y] - _25R20[x] ;
                    lbe = _25R20[y];
                    correctL = true;
                } else if (( hue <= (_10RP20[x] + _25R20[x]) / 2.0)  && (hue > (_10RP20[x] - 0.035)) && x < 70) {
                    if(y > 70) {
                        y = 70;
                    }

                    correction =  _10RP20[y] - _10RP20[x] ;
                    lbe = _10RP20[y];
                    correctL = true;
                }
            } else if (lum < 35.0) {
                if( (hue <= (_5R30[x] + 0.035)) && (hue > (_5R30[x] + _25R30[x]) / 2.0) && x < 85 ) {
                    if(y > 85) {
                        y = 85;
                    }

                    correction =  _5R30[y] - _5R30[x] ;
                    lbe = _5R30[y];
                    correctL = true;
                } else if( (hue <= (_5R30[x] + _25R30[x]) / 2.0) && (hue > (_10RP30[x] + _25R30[x]) / 2.0) && x < 85) {
                    if(y > 85) {
                        y = 85;
                    }

                    correction =  _25R30[y] - _25R30[x] ;
                    lbe = _25R30[y];
                    correctL = true;
                } else if (( hue <= (_10RP30[x] + _25R30[x]) / 2.0)  && (hue > (_10RP30[x] - 0.035)) && x < 85) {
                    if(y > 85) {
                        y = 85;
                    }

                    correction =  _10RP30[y] - _10RP30[x] ;
                    lbe = _10RP30[y];
                    correctL = true;
                }
            }
        }
    }

    //end red purple
}


/*
 * SkinSat
 * Copyright (c)2011  Jacques Desmis <jdesmis@gmail.com>
 *
 * skin color: mixed from NX2 skin color palette, Von Luschan, and photos of people white,
 * black, yellow....there are some little exceptions...cover 99% case
 * pay attention to white balance, and do not change hue and saturation, upstream of the modification
 *
 */
void Color::SkinSat (float lum, float hue, float chrom, float &satreduc)
{

    // to be adapted...by tests
    constexpr float reduction = 0.3f;         // use "reduction" for  "real" skin color : take into account a slightly usage of contrast and saturation in RT if option "skin" = 1
    constexpr float extendedreduction = 0.4f; // use "extendedreduction" for wide area skin color, useful if not accurate colorimetry or if the user has changed hue and saturation
    constexpr float extendedreduction2 = 0.6f; // use "extendedreduction2" for wide area for transition

    constexpr float C9 = 8.f, C8 = 15.f, C7 = 12.f, C4 = 7.f, C3 = 5.f, C2 = 5.f, C1 = 5.f;
    constexpr float H9 = 0.05f, H8 = 0.25f, H7 = 0.1f, H4 = 0.02f, H3 = 0.02f, H2 = 0.1f, H1 = 0.1f, H10 = -0.2f, H11 = -0.2f; //H10 and H11 are curious...H11=-0.8 ??

    if (lum >= 85.f) {
        if((hue > (0.78f - H9) && hue < (1.18f + H9)) && (chrom > 8.f && chrom < (14.f + C9))) {
            satreduc = reduction;
        } else if (lum >= 92.f) {
            if((hue > 0.8f && hue < 1.65f) && (chrom > 7.f && chrom < (15.f))) {
                satreduc = extendedreduction;
            } else if ((hue > -0.1f && hue < 1.65f) && (chrom > 7.f && chrom < (18.f))) {
                satreduc = extendedreduction2;
            }
        } else if ((hue > 0.7f && hue < 1.4f) && (chrom > 7.f && chrom < (26.f + C9))) {
            satreduc = extendedreduction;
        } else if (lum < 92.f && (hue > 0.f && hue < 1.65f) && (chrom > 7.f && chrom < (35.f + C9))) {
            satreduc = extendedreduction2;
        }
    } else if (lum >= 70.f) {
        if((hue > 0.4f && hue < (1.04f + H8)) && (chrom > 8.f && chrom < (35.f + C8))) {
            satreduc = reduction;
        } else if ((hue > (0.02f + H11) && hue < 1.5f) && (chrom > 7.0f && chrom < (48.f + C9) )) {
            satreduc = extendedreduction;
        } else if ((hue > (0.02f + H11) && hue < 1.65f) && (chrom > 7.f && chrom < (55.f + C9) )) {
            satreduc = extendedreduction2;
        }
    } else if (lum >= 52.f) {
        if((hue > 0.3f && hue < (1.27f + H7)) && (chrom > 11.f && chrom < (35.f + C7))) {
            satreduc = reduction;
        } else if ((hue > (0.02f + H11) && hue < 1.5f) && (chrom > 7.0f && chrom < (48.f + C9) )) {
            satreduc = extendedreduction;
        } else if ((hue > (0.02f + H11) && hue < 1.65f) && (chrom > 7.f && chrom < (55.f + C9) )) {
            satreduc = extendedreduction2;
        }
    } else if (lum >= 35.f) {
        if((hue > 0.3f && hue < (1.25f + H4)) && (chrom > 13.f && chrom < (37.f + C4))) {
            satreduc = reduction;
        } else if ((hue > (0.02f + H11) && hue < 1.5f) && (chrom > 7.0f && chrom < (48.f + C9) )) {
            satreduc = extendedreduction;
        } else if ((hue > (0.02f + H11) && hue < 1.65f) && (chrom > 7.f && chrom < (55.f + C9) )) {
            satreduc = extendedreduction2;
        }
    } else if (lum >= 20.f) {
        if((hue > 0.3f && hue < (1.2f + H3)) && (chrom > 7.f && chrom < (35.f + C3) )) {
            satreduc = reduction;
        } else if ((hue > (0.02f + H11) && hue < 1.5f) && (chrom > 7.0f && chrom < (48.f + C9) )) {
            satreduc = extendedreduction;
        } else if ((hue > (0.02f + H11) && hue < 1.65f) && (chrom > 7.f && chrom < (55.f + C9) )) {
            satreduc = extendedreduction2;
        }
    } else if (lum > 10.f) {
        if((hue > (0.f + H10) && hue < (0.95f + H2)) && (chrom > 8.f && chrom < (23.f + C2))) {
            satreduc = reduction;
        } else if ((hue > (0.02f + H11) && hue < 1.f) && (chrom > 7.f && chrom < (35.f + C1) )) {
            satreduc = extendedreduction;
        } else if ((hue > (0.02f + H11) && hue < 1.6f) && (chrom > 7.f && chrom < (45.f + C1) )) {
            satreduc = extendedreduction2;
        }
    } else {
        if((hue > (0.02f + H10) && hue < (0.9f + H1)) && (chrom > 8.f && chrom < (23.f + C1))) {
            satreduc = reduction;    // no data : extrapolate
        } else if ((hue > (0.02f + H11) && hue < 1.f) && (chrom > 7.f && chrom < (35.f + C1) )) {
            satreduc = extendedreduction;
        } else if ((hue > (0.02f + H11) && hue < 1.6f) && (chrom > 7.f && chrom < (45.f + C1) )) {
            satreduc = extendedreduction2;
        }

    }

}

/*
 * Munsell Lch correction
 * Copyright (c) 2011  Jacques Desmis <jdesmis@gmail.com>
 *
 * data (Munsell ==> Lab) obtained with WallKillcolor and http://www.cis.rit.edu/research/mcsl2/online/munsell.php
 * each LUT give Hue in function of C, for each color Munsell and Luminance
 * eg: _6PB20 : color Munsell 6PB for L=20 c=5 c=45 c=85 c=125..139 when possible: interpolation betwwen values
 * no value for C<5  (gray)
 * low memory footprint -- maximum: 195 LUTf * 140 values
 * errors due to small number of samples in LUT and linearization are very low (1 to 2%)
 * errors due to a different illuminant "Daylight" than "C" are low, about 10%. For example, a theoretical correction of 0.1 radian will be made with a real correction of 0.09 or 0.11 depending on the color illuminant D50
 * errors due to the use of a very different illuminant "C", for example illuminant "A" (tungsten) are higher, about 20%. Theoretical correction of 0.52 radians will be made with a real correction of 0.42
 */
void Color::initMunsell ()
{
#ifdef _DEBUG
    MyTime t1e, t2e;
    t1e.set();
#endif

    const int maxInd  = 140;
    const int maxInd2 = 90;
    const int maxInd3 = 50;

    //blue for sky
    _5B40(maxInd2);
    _5B40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _5B40[i] = -2.3 + 0.0025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _5B40[i] = -2.2 + 0.00 * (i - 45);
        }
    }

    //printf("5B %1.2f  %1.2f\n",_5B40[44],_5B40[89]);
    _5B50(maxInd2);
    _5B50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _5B50[i] = -2.34 + 0.0025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _5B50[i] = -2.24 + 0.0003 * (i - 45);
        }
    }

    //printf("5B %1.2f  %1.2f\n",_5B50[44],_5B50[89]);
    _5B60(maxInd2);
    _5B60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _5B60[i] = -2.4 + 0.003 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _5B60[i] = -2.28 + 0.0005 * (i - 45);
        }
    }

    //printf("5B %1.2f  %1.2f\n",_5B60[44],_5B60[89]);
    _5B70(maxInd2);
    _5B70.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _5B70[i] = -2.41 + 0.00275 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _5B70[i] = -2.30 + 0.00025 * (i - 45);
        }
    }

    //printf("5B %1.2f  %1.2f\n",_5B70[44],_5B70[89]);
    _5B80(maxInd3);
    _5B80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _5B80[i] = -2.45 + 0.003 * (i - 5);
        }
    }

    //printf("5B %1.2f\n",_5B80[49]);

    _7B40(maxInd2);
    _7B40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _7B40[i] = -2.15 + 0.0027 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _7B40[i] = -2.04 + 0.00 * (i - 45);
        }
    }

    //printf("7B %1.2f  %1.2f\n",_7B40[44],_7B40[89]);
    _7B50(maxInd2);
    _7B50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _7B50[i] = -2.20 + 0.003 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _7B50[i] = -2.08 + 0.001 * (i - 45);
        }
    }

    //printf("7B %1.2f  %1.2f\n",_7B50[44],_7B50[79]);
    _7B60(maxInd2);
    _7B60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _7B60[i] = -2.26 + 0.0035 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _7B60[i] = -2.12 + 0.001 * (i - 45);
        }
    }

    //printf("7B %1.2f  %1.2f\n",_7B60[44],_7B60[79]);
    _7B70(maxInd2);
    _7B70.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _7B70[i] = -2.28 + 0.003 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _7B70[i] = -2.16 + 0.0015 * (i - 45);
        }
    }

    //printf("7B %1.2f  %1.2f\n",_7B70[44],_7B70[64]);
    _7B80(maxInd3);
    _7B80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _7B80[i] = -2.30 + 0.0028 * (i - 5);
        }
    }

    //printf("5B %1.2f\n",_7B80[49]);

    _9B40(maxInd2);
    _9B40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _9B40[i] = -1.99 + 0.0022 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _9B40[i] = -1.90 + 0.0008 * (i - 45);
        }
    }

    //printf("9B %1.2f  %1.2f\n",_9B40[44],_9B40[69]);
    _9B50(maxInd2);
    _9B50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _9B50[i] = -2.04 + 0.0025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _9B50[i] = -1.94 + 0.0013 * (i - 45);
        }
    }

    //printf("9B %1.2f  %1.2f\n",_9B50[44],_9B50[77]);
    _9B60(maxInd2);
    _9B60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _9B60[i] = -2.10 + 0.0033 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _9B60[i] = -1.97 + 0.001 * (i - 45);
        }
    }

    //printf("9B %1.2f  %1.2f\n",_9B60[44],_9B60[79]);
    _9B70(maxInd2);
    _9B70.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _9B70[i] = -2.12 + 0.003 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _9B70[i] = -2.00 + 0.001 * (i - 45);
        }
    }

    //printf("9B %1.2f  %1.2f\n",_9B70[44],_9B70[54]);
    _9B80(maxInd3);
    _9B80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _9B80[i] = -2.16 + 0.0025 * (i - 5);
        }
    }

    //printf("9B %1.2f\n",_9B80[49]);

    _10B40(maxInd2);
    _10B40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10B40[i] = -1.92 + 0.0022 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10B40[i] = -1.83 + 0.0012 * (i - 45);
        }
    }

    //printf("10B %1.2f  %1.2f\n",_10B40[44],_10B40[76]);
    _10B50(maxInd2);
    _10B50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10B50[i] = -1.95 + 0.0022 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10B50[i] = -1.86 + 0.0008 * (i - 45);
        }
    }

    //printf("10B %1.2f  %1.2f\n",_10B50[44],_10B50[85]);
    _10B60(maxInd2);
    _10B60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10B60[i] = -2.01 + 0.0027 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10B60[i] = -1.90 + 0.0012 * (i - 45);
        }
    }

    //printf("10B %1.2f  %1.2f\n",_10B60[44],_10B60[70]);
    _10B70(maxInd3);
    _10B70.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _10B70[i] = -2.03 + 0.0025 * (i - 5);
        }
    }

    //printf("10B %1.2f\n",_10B70[49]);
    _10B80(maxInd3);
    _10B80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _10B80[i] = -2.08 + 0.0032 * (i - 5);
        }
    }

    //printf("10B %1.2f\n",_10B80[39]);

    _05PB40(maxInd2);
    _05PB40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _05PB40[i] = -1.87 + 0.0022 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _05PB40[i] = -1.78 + 0.0015 * (i - 45);
        }
    }

    //printf("05PB %1.2f  %1.2f\n",_05PB40[44],_05PB40[74]);
    _05PB50(maxInd2);
    _05PB50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _05PB50[i] = -1.91 + 0.0022 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _05PB50[i] = -1.82 + 0.001 * (i - 45);
        }
    }

    //printf("05PB %1.2f  %1.2f\n",_05PB50[44],_05PB50[85]);
    _05PB60(maxInd2);
    _05PB60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _05PB60[i] = -1.96 + 0.0027 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _05PB60[i] = -1.85 + 0.0013 * (i - 45);
        }
    }

    //printf("05PB %1.2f  %1.2f\n",_05PB60[44],_05PB60[70]);
    _05PB70(maxInd2);
    _05PB70.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _05PB70[i] = -1.99 + 0.0027 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _05PB70[i] = -1.88 + 0.001 * (i - 45);
        }
    }

    //printf("05PB %1.2f  %1.2f\n",_05PB70[44],_05PB70[54]);
    _05PB80(maxInd3);
    _05PB80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _05PB80[i] = -2.03 + 0.003 * (i - 5);
        }
    }

    //printf("05PB %1.2f\n",_05PB80[39]);



    //blue purple correction
    //between 15PB to 4P
    //maximum deviation 75PB

    //15PB
    _15PB10(maxInd3);
    _15PB10.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _15PB10[i] = -1.66 + 0.0035 * (i - 5);
        }
    }

    //printf("15 %1.2f\n",_15PB10[49]);
    _15PB20(maxInd2);
    _15PB20.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _15PB20[i] = -1.71 + 0.00275 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _15PB20[i] = -1.60 + 0.0012 * (i - 45);
        }
    }

    //printf("15 %1.2f  %1.2f\n",_15PB20[44],_15PB20[89]);

    _15PB30(maxInd2);
    _15PB30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _15PB30[i] = -1.75 + 0.0025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _15PB30[i] = -1.65 + 0.002 * (i - 45);
        }
    }

    //printf("15 %1.2f  %1.2f\n",_15PB30[44],_15PB30[89]);

    _15PB40(maxInd2);
    _15PB40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _15PB40[i] = -1.79 + 0.002 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _15PB40[i] = -1.71 + 0.002 * (i - 45);
        }
    }

    //printf("15 %1.2f  %1.2f\n",_15PB40[44],_15PB40[89]);

    _15PB50(maxInd2);
    _15PB50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _15PB50[i] = -1.82 + 0.002 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _15PB50[i] = -1.74 + 0.0011 * (i - 45);
        }
    }

    //printf("15 %1.2f  %1.2f\n",_15PB50[44],_15PB50[89]);

    _15PB60(maxInd2);
    _15PB60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _15PB60[i] = -1.87 + 0.0025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _15PB60[i] = -1.77 + 0.001 * (i - 45);
        }
    }

    //printf("15 %1.2f  %1.2f\n",_15PB60[44],_15PB60[89]);
    _15PB70(maxInd3);
    _15PB70.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _15PB70[i] = -1.90 + 0.0027 * (i - 5);
        }
    }

    //    printf("15 %1.2f\n",_15PB70[49]);
    _15PB80(maxInd3);
    _15PB80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _15PB80[i] = -1.93 + 0.0027 * (i - 5);
        }
    }

    //printf("15 %1.2f %1.2f\n",_15PB80[38], _15PB80[49]);

    //3PB
    _3PB10(maxInd2);
    _3PB10.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _3PB10[i] = -1.56 + 0.005 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _3PB10[i] = -1.36 + 0.001 * (i - 45);
        }
    }

    //printf("30 %1.2f  %1.2f\n",_3PB10[44],_3PB10[89]);

    _3PB20(maxInd2);
    _3PB20.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _3PB20[i] = -1.59 + 0.00275 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _3PB20[i] = -1.48 + 0.003 * (i - 45);
        }
    }

    //printf("30 %1.2f  %1.2f\n",_3PB20[44],_3PB20[89]);

    _3PB30(maxInd2);
    _3PB30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _3PB30[i] = -1.62 + 0.00225 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _3PB30[i] = -1.53 + 0.0032 * (i - 45);
        }
    }

    //printf("30 %1.2f  %1.2f\n",_3PB30[44],_3PB30[89]);

    _3PB40(maxInd2);
    _3PB40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _3PB40[i] = -1.64 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _3PB40[i] = -1.58 + 0.0025 * (i - 45);
        }
    }

    //printf("30 %1.2f  %1.2f\n",_3PB40[44],_3PB40[89]);

    _3PB50(maxInd2);
    _3PB50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _3PB50[i] = -1.69 + 0.00175 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _3PB50[i] = -1.62 + 0.002 * (i - 45);
        }
    }

    //printf("30 %1.2f  %1.2f\n",_3PB50[44],_3PB50[89]);

    _3PB60(maxInd2);
    _3PB60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _3PB60[i] = -1.73 + 0.002 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _3PB60[i] = -1.65 + 0.0012 * (i - 45);
        }
    }

    //printf("30 %1.2f  %1.2f\n",_3PB60[44],_3PB60[89]);
    _3PB70(maxInd3);
    _3PB70.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _3PB70[i] = -1.76 + 0.002 * (i - 5);
        }
    }

    //printf("30 %1.2f\n",_3PB70[49]);
    _3PB80(maxInd3);
    _3PB80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _3PB80[i] = -1.78 + 0.0025 * (i - 5);
        }
    }

    //printf("30 %1.2f %1.2f\n",_3PB80[38], _3PB80[49]);

    //45PB
    _45PB10(maxInd2);
    _45PB10.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _45PB10[i] = -1.46 + 0.0045 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _45PB10[i] = -1.28 + 0.0025 * (i - 45);
        }
    }

    //printf("45 %1.2f  %1.2f\n",_45PB10[44],_45PB10[89]);

    _45PB20(maxInd2);
    _45PB20.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _45PB20[i] = -1.48 + 0.00275 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _45PB20[i] = -1.37 + 0.0025 * (i - 45);
        }
    }

    //printf("45 %1.2f  %1.2f\n",_45PB20[44],_45PB20[89]);

    _45PB30(maxInd2);
    _45PB30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _45PB30[i] = -1.51 + 0.00175 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _45PB30[i] = -1.44 + 0.0035 * (i - 45);
        }
    }

    //printf("45 %1.2f  %1.2f\n",_45PB30[44],_45PB30[89]);

    _45PB40(maxInd2);
    _45PB40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _45PB40[i] = -1.52 + 0.001 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _45PB40[i] = -1.48 + 0.003 * (i - 45);
        }
    }

    //printf("45 %1.2f  %1.2f\n",_45PB40[44],_45PB40[89]);

    _45PB50(maxInd2);
    _45PB50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _45PB50[i] = -1.55 + 0.001 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _45PB50[i] = -1.51 + 0.0022 * (i - 45);
        }
    }

    //printf("45 %1.2f  %1.2f\n",_45PB50[44],_45PB50[89]);

    _45PB60(maxInd2);
    _45PB60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _45PB60[i] = -1.6 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _45PB60[i] = -1.54 + 0.001 * (i - 45);
        }
    }

    //printf("45 %1.2f  %1.2f\n",_45PB60[44],_45PB60[89]);
    _45PB70(maxInd3);
    _45PB70.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _45PB70[i] = -1.63 + 0.0017 * (i - 5);
        }
    }

    //printf("45 %1.2f\n",_45PB70[49]);
    _45PB80(maxInd3);
    _45PB80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _45PB80[i] = -1.67 + 0.0025 * (i - 5);
        }
    }

    //printf("45 %1.2f %1.2f\n",_45PB80[38], _45PB80[49]);

    //_6PB
    _6PB10(maxInd);
    _6PB10.clear();

    for (int i = 0; i < maxInd; i++) { //i = chromaticity  0==>140
        if (i < 45 && i > 5) {
            _6PB10[i] = -1.33 + 0.005 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _6PB10[i] = -1.13 + 0.0045 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _6PB10[i] = -0.95 + 0.0015 * (i - 85);
        }
    }

    //printf("60 %1.2f  %1.2f %1.2f\n",_6PB10[44],_6PB10[84],_6PB10[139]);

    _6PB20(maxInd);
    _6PB20.clear();

    for (int i = 0; i < maxInd; i++) { //i = chromaticity  0==>140
        if (i < 45 && i > 5) {
            _6PB20[i] = -1.36 + 0.004 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _6PB20[i] = -1.20 + 0.00375 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _6PB20[i] = -1.05 + 0.0017 * (i - 85);
        }
    }

    //printf("60 %1.2f  %1.2f %1.2f\n",_6PB20[44],_6PB20[84],_6PB20[139]);

    _6PB30(maxInd);
    _6PB30.clear();

    for (int i = 0; i < maxInd; i++) { //i = chromaticity  0==>140
        if (i < 45 && i > 5) {
            _6PB30[i] = -1.38 + 0.00225 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _6PB30[i] = -1.29 + 0.00375 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _6PB30[i] = -1.14 + 0.002 * (i - 85);
        }
    }

    //printf("60 %1.2f  %1.2f %1.2f\n",_6PB30[44],_6PB30[84],_6PB30[139]);

    _6PB40(maxInd);
    _6PB40.clear();

    for (int i = 0; i < maxInd; i++) { //i = chromaticity  0==>140
        if (i < 45 && i > 5) {
            _6PB40[i] = -1.39 + 0.00125 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _6PB40[i] = -1.34 + 0.00275 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _6PB40[i] = -1.23 + 0.002 * (i - 85);
        }
    }

    //printf("60 %1.2f  %1.2f %1.2f\n",_6PB40[44],_6PB40[84],_6PB40[139]);

    _6PB50(maxInd2);//limits  -1.3   -1.11
    _6PB50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _6PB50[i] = -1.43 + 0.00125 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _6PB50[i] = -1.38 + 0.00225 * (i - 45);
        }
    }

    //printf("60 %1.2f  %1.2f \n",_6PB50[44],_6PB50[89]);

    _6PB60(maxInd2);//limits  -1.3   -1.11
    _6PB60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _6PB60[i] = -1.46 + 0.0012 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _6PB60[i] = -1.40 + 0.000875 * (i - 45);
        }
    }

    //printf("60 %1.2f  %1.2f\n",_6PB60[44],_6PB60[89]);
    _6PB70(maxInd3);
    _6PB70.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _6PB70[i] = -1.49 + 0.0018 * (i - 5);
        }
    }

    //printf("6 %1.2f\n",_6PB70[49]);
    _6PB80(maxInd3);
    _6PB80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _6PB80[i] = -1.52 + 0.0022 * (i - 5);
        }
    }

    //printf("6 %1.2f %1.2f\n",_6PB80[38], _6PB80[49]);


    //_75PB : notation Munsell for maximum deviation blue purple
    _75PB10(maxInd);//limits hue -1.23  -0.71  _75PBx   x=Luminance  eg_75PB10 for L >5 and L<=15
    _75PB10.clear();

    for (int i = 0; i < maxInd; i++) { //i = chromaticity  0==>140
        if (i < 45 && i > 5) {
            _75PB10[i] = -1.23 + 0.0065 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _75PB10[i] = -0.97 + 0.00375 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _75PB10[i] = -0.82 + 0.0015 * (i - 85);
        }
    }

    //printf("75 %1.2f  %1.2f %1.2f\n",_75PB10[44],_75PB10[84],_75PB10[139]);

    _75PB20(maxInd);//limits -1.24  -0.79  for L>15 <=25
    _75PB20.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _75PB20[i] = -1.24 + 0.004 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _75PB20[i] = -1.08 + 0.00425 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _75PB20[i] = -0.91 + 0.0017 * (i - 85);
        }
    }

    //printf("75 %1.2f  %1.2f %1.2f\n",_75PB20[44],_75PB20[84],_75PB20[139]);

    _75PB30(maxInd);//limits -1.25  -0.85
    _75PB30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _75PB30[i] = -1.25 + 0.00275 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _75PB30[i] = -1.14 + 0.004 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _75PB30[i] = -0.98 + 0.0015 * (i - 85);
        }
    }

    //printf("75 %1.2f  %1.2f %1.2f\n",_75PB30[44],_75PB30[84],_75PB30[139]);

    _75PB40(maxInd);//limits  -1.27  -0.92
    _75PB40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _75PB40[i] = -1.27 + 0.002 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _75PB40[i] = -1.19 + 0.003 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _75PB40[i] = -1.07 + 0.0022 * (i - 85);
        }
    }

    //printf("75 %1.2f  %1.2f %1.2f\n",_75PB40[44],_75PB40[84],_75PB40[139]);

    _75PB50(maxInd2);//limits  -1.3   -1.11
    _75PB50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _75PB50[i] = -1.3 + 0.00175 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _75PB50[i] = -1.23 + 0.0025 * (i - 45);
        }
    }

    //printf("75 %1.2f  %1.2f\n",_75PB50[44],_75PB50[89]);

    _75PB60(maxInd2);
    _75PB60.clear();

    for (int i = 0; i < maxInd2; i++) { //limits -1.32  -1.17
        if (i < 45 && i > 5) {
            _75PB60[i] = -1.32 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _75PB60[i] = -1.26 + 0.002 * (i - 45);
        }
    }

    //printf("75 %1.2f  %1.2f \n",_75PB60[44],_75PB60[89]);

    _75PB70(maxInd3);
    _75PB70.clear();

    for (int i = 0; i < maxInd3; i++) { //limits  -1.34  -1.27
        if (i < 50 && i > 5) {
            _75PB70[i] = -1.34 + 0.002 * (i - 5);
        }
    }

    _75PB80(maxInd3);
    _75PB80.clear();

    for (int i = 0; i < maxInd3; i++) { //limits -1.35  -1.29
        if (i < 50 && i > 5) {
            _75PB80[i] = -1.35 + 0.00125 * (i - 5);
        }
    }


    _9PB10(maxInd);
    _9PB10.clear();

    for (int i = 0; i < maxInd; i++) { //i = chromaticity  0==>140
        if (i < 45 && i > 5) {
            _9PB10[i] = -1.09 + 0.00475 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _9PB10[i] = -0.9 + 0.003 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _9PB10[i] = -0.78 + 0.0013 * (i - 85);
        }
    }

    //printf("90 %1.2f  %1.2f %1.2f\n",_9PB10[44],_9PB10[84],_9PB10[139]);

    _9PB20(maxInd);
    _9PB20.clear();

    for (int i = 0; i < maxInd; i++) { //i = chromaticity  0==>140
        if (i < 45 && i > 5) {
            _9PB20[i] = -1.12 + 0.0035 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _9PB20[i] = -0.98 + 0.00325 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _9PB20[i] = -0.85 + 0.0015 * (i - 85);
        }
    }

    //printf("90 %1.2f  %1.2f %1.2f\n",_9PB20[44],_9PB20[84],_9PB20[139]);

    _9PB30(maxInd);
    _9PB30.clear();

    for (int i = 0; i < maxInd; i++) { //i = chromaticity  0==>140
        if (i < 45 && i > 5) {
            _9PB30[i] = -1.14 + 0.0028 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _9PB30[i] = -1.03 + 0.003 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _9PB30[i] = -0.91 + 0.0017 * (i - 85);
        }
    }

    //printf("90 %1.2f  %1.2f %1.2f\n",_9PB30[44],_9PB30[84],_9PB30[139]);

    _9PB40(maxInd);
    _9PB40.clear();

    for (int i = 0; i < maxInd; i++) { //i = chromaticity  0==>140
        if (i < 45 && i > 5) {
            _9PB40[i] = -1.16 + 0.002 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _9PB40[i] = -1.08 + 0.00275 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _9PB40[i] = -0.97 + 0.0016 * (i - 85);
        }
    }

    //printf("90 %1.2f  %1.2f %1.2f\n",_9PB40[44],_9PB40[84],_9PB40[139]);

    _9PB50(maxInd2);
    _9PB50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _9PB50[i] = -1.19 + 0.00175 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _9PB50[i] = -1.12 + 0.00225 * (i - 45);
        }
    }

    //printf("90 %1.2f  %1.2f \n",_9PB50[44],_9PB50[84]);

    _9PB60(maxInd2);
    _9PB60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _9PB60[i] = -1.21 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _9PB60[i] = -1.15 + 0.002 * (i - 45);
        }
    }

    //printf("90 %1.2f  %1.2f \n",_9PB60[44],_9PB60[89]);
    _9PB70(maxInd3);
    _9PB70.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _9PB70[i] = -1.23 + 0.0018 * (i - 5);
        }
    }

    //printf("9 %1.2f\n",_9PB70[49]);
    _9PB80(maxInd3);
    _9PB80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _9PB80[i] = -1.24 + 0.002 * (i - 5);
        }
    }

    //printf("9 %1.2f %1.2f\n",_9PB80[38], _9PB80[49]);


    //10PB
    _10PB10(maxInd);
    _10PB10.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10PB10[i] = -1.02 + 0.00425 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10PB10[i] = -0.85 + 0.0025 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10PB10[i] = -0.75 + 0.0012 * (i - 85);
        }
    }

    //printf("10 %1.2f  %1.2f %1.2f\n",_10PB10[44],_10PB10[84],_10PB10[139]);

    _10PB20(maxInd);
    _10PB20.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10PB20[i] = -1.05 + 0.00325 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10PB20[i] = -0.92 + 0.00275 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10PB20[i] = -0.81 + 0.0014 * (i - 85);
        }
    }

    //printf("10 %1.2f  %1.2f %1.2f\n",_10PB20[44],_10PB20[84],_10PB20[139]);

    _10PB30(maxInd);
    _10PB30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10PB30[i] = -1.07 + 0.00275 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10PB30[i] = -0.96 + 0.0025 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10PB30[i] = -0.86 + 0.0015 * (i - 85);
        }
    }

    //printf("10 %1.2f  %1.2f %1.2f\n",_10PB30[44],_10PB30[84],_10PB30[139]);

    _10PB40(maxInd);
    _10PB40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10PB40[i] = -1.09 + 0.002 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10PB40[i] = -1.01 + 0.00225 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10PB40[i] = -0.92 + 0.0016 * (i - 85);
        }
    }

    //printf("10 %1.2f  %1.2f %1.2f\n",_10PB40[44],_10PB40[84],_10PB40[139]);

    _10PB50(maxInd2);
    _10PB50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10PB50[i] = -1.12 + 0.00175 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10PB50[i] = -1.05 + 0.00225 * (i - 45);
        }
    }

    //printf("10 %1.2f  %1.2f\n",_10PB50[44],_10PB50[84]);

    _10PB60(maxInd2);
    _10PB60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10PB60[i] = -1.14 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10PB60[i] = -1.08 + 0.00225 * (i - 45);
        }
    }

    //printf("10 %1.2f  %1.2f\n",_10PB60[44],_10PB60[89]);


    //1P
    _1P10(maxInd);
    _1P10.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _1P10[i] = -0.96 + 0.00375 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _1P10[i] = -0.81 + 0.00225 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _1P10[i] = -0.72 + 0.001 * (i - 85);
        }
    }

    //printf("1P %1.2f  %1.2f %1.2f\n",_1P10[44],_1P10[84],_1P10[139]);

    _1P20(maxInd);
    _1P20.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _1P20[i] = -1.0 + 0.00325 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _1P20[i] = -0.87 + 0.0025 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _1P20[i] = -0.77 + 0.0012 * (i - 85);
        }
    }

    //printf("1P %1.2f  %1.2f %1.2f\n",_1P20[44],_1P20[84],_1P20[139]);

    _1P30(maxInd);
    _1P30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _1P30[i] = -1.02 + 0.00275 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _1P30[i] = -0.91 + 0.00225 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _1P30[i] = -0.82 + 0.0011 * (i - 85);
        }
    }

    //printf("1P %1.2f  %1.2f %1.2f\n",_1P30[44],_1P30[84],_1P30[139]);

    _1P40(maxInd);
    _1P40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _1P40[i] = -1.04 + 0.00225 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _1P40[i] = -0.95 + 0.00225 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _1P40[i] = -0.86 + 0.0015 * (i - 85);
        }
    }

    //printf("1P %1.2f  %1.2f %1.2f\n",_1P40[44],_1P40[84],_1P40[139]);

    _1P50(maxInd2);
    _1P50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _1P50[i] = -1.06 + 0.002 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _1P50[i] = -0.98 + 0.00175 * (i - 45);
        }
    }

    //printf("1P %1.2f  %1.2f \n",_1P50[44],_1P50[89]);

    _1P60(maxInd2);
    _1P60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _1P60[i] = -1.07 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _1P60[i] = -1.01 + 0.00175 * (i - 45);
        }
    }

    //printf("1P %1.2f  %1.2f \n",_1P60[44],_1P60[84],_1P60[139]);

    //4P
    _4P10(maxInd);
    _4P10.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _4P10[i] = -0.78 + 0.002 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _4P10[i] = -0.7 + 0.00125 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _4P10[i] = -0.65 + 0.001 * (i - 85);
        }
    }

    //printf("4P %1.2f  %1.2f %1.2f\n",_4P10[44],_4P10[84],_4P10[139]);

    _4P20(maxInd);
    _4P20.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _4P20[i] = -0.84 + 0.0025 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _4P20[i] = -0.74 + 0.00175 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _4P20[i] = -0.67 + 0.00085 * (i - 85);
        }
    }

    //printf("4P %1.2f  %1.2f %1.2f\n",_4P20[44],_4P20[84],_4P20[139]);

    _4P30(maxInd);
    _4P30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _4P30[i] = -0.85 + 0.00225 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _4P30[i] = -0.76 + 0.00125 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _4P30[i] = -0.71 + 0.001 * (i - 85);
        }
    }

    //printf("4P %1.2f  %1.2f %1.2f\n",_4P30[44],_4P30[84],_4P30[139]);

    _4P40(maxInd);
    _4P40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _4P40[i] = -0.87 + 0.00175 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _4P40[i] = -0.8 + 0.00175 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _4P40[i] = -0.73 + 0.00075 * (i - 85);
        }
    }

    //printf("4P %1.2f  %1.2f %1.2f\n",_4P40[44],_4P40[84],_4P40[139]);

    _4P50(maxInd2);
    _4P50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _4P50[i] = -0.88 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _4P50[i] = -0.82 + 0.0015 * (i - 45);
        }
    }

    //printf("4P %1.2f  %1.2f \n",_4P50[44],_4P50[89]);

    _4P60(maxInd2);
    _4P60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _4P60[i] = -0.89 + 0.00125 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _4P60[i] = -0.84 + 0.00125 * (i - 45);
        }
    }

    //printf("4P %1.2f  %1.2f\n",_4P60[44],_4P60[89]);


    //red yellow correction
    _10YR20(maxInd2);
    _10YR20.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10YR20[i] = 1.22 + 0.002 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10YR20[i] = 1.30 + 0.006 * (i - 45);
        }
    }

    //printf("10YR  %1.2f  %1.2f\n",_10YR20[44],_10YR20[56]);
    _10YR30(maxInd2);
    _10YR30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10YR30[i] = 1.27 + 0.00175 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10YR30[i] = 1.34 + 0.0017 * (i - 45);
        }
    }

    //printf("10YR  %1.2f  %1.2f\n",_10YR30[44],_10YR30[75]);
    _10YR40(maxInd2);
    _10YR40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10YR40[i] = 1.32 + 0.00025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10YR40[i] = 1.33 + 0.0015 * (i - 45);
        }
    }

    //printf("10YR  %1.2f  %1.2f\n",_10YR40[44],_10YR40[85]);
    _10YR50(maxInd2);
    _10YR50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10YR50[i] = 1.35 + 0.000 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10YR50[i] = 1.35 + 0.0012 * (i - 45);
        }
    }

    //printf("10YR  %1.2f  %1.2f\n",_10YR50[44],_10YR50[80]);
    _10YR60(maxInd);
    _10YR60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10YR60[i] = 1.38 - 0.00025 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10YR60[i] = 1.37 + 0.0005 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10YR60[i] = 1.39 + 0.0013 * (i - 85);
        }
    }

    //printf("10YR  %1.2f  %1.2f %1.2f\n",_10YR60[44],_10YR60[85],_10YR60[139] );
    _10YR70(maxInd);
    _10YR70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10YR70[i] = 1.41 - 0.0005 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10YR70[i] = 1.39 + 0.000 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10YR70[i] = 1.39 + 0.0013 * (i - 85);
        }
    }

    //printf("10YR  %1.2f  %1.2f %1.2f\n",_10YR70[44],_10YR70[85],_10YR70[139] );
    _10YR80(maxInd);
    _10YR80.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10YR80[i] = 1.45 - 0.00125 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10YR80[i] = 1.40 + 0.000 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10YR80[i] = 1.40 + 0.00072 * (i - 85);    //1.436
        }
    }

    //printf("10YR  %1.2f  %1.2f %1.2f\n",_10YR80[44],_10YR80[84],_10YR80[139] );
    _10YR90(maxInd2);
    _10YR90.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10YR90[i] = 1.48 - 0.001 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10YR90[i] = 1.44 - 0.0009 * (i - 45);
        }
    }

    //printf("10YR  %1.2f  %1.2f\n",_10YR90[45],_10YR90[80]);
    _85YR20(maxInd3);
    _85YR20.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _85YR20[i] = 1.12 + 0.004 * (i - 5);
        }
    }

    //printf("85YR  %1.2f \n",_85YR20[44]);
    _85YR30(maxInd2);
    _85YR30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _85YR30[i] = 1.16 + 0.0025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _85YR30[i] = 1.26 + 0.0028 * (i - 45);
        }
    }

    //printf("85YR  %1.2f  %1.2f\n",_85YR30[44],_85YR30[75]);
    _85YR40(maxInd2);
    _85YR40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _85YR40[i] = 1.20 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _85YR40[i] = 1.26 + 0.0024 * (i - 45);
        }
    }

    //printf("85YR  %1.2f  %1.2f\n",_85YR40[44],_85YR40[75]);
    _85YR50(maxInd);
    _85YR50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _85YR50[i] = 1.24 + 0.0005 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _85YR50[i] = 1.26 + 0.002 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _85YR50[i] = 1.34 + 0.0015 * (i - 85);
        }
    }

    //printf("85YR  %1.2f  %1.2f %1.2f\n",_85YR50[44],_85YR50[85],_85YR50[110] );
    _85YR60(maxInd);
    _85YR60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _85YR60[i] = 1.27 + 0.00025 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _85YR60[i] = 1.28 + 0.0015 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _85YR60[i] = 1.34 + 0.0012 * (i - 85);
        }
    }

    //printf("85YR  %1.2f  %1.2f %1.2f\n",_85YR60[44],_85YR60[85],_85YR60[139] );

    _85YR70(maxInd);
    _85YR70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _85YR70[i] = 1.31 - 0.00025 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _85YR70[i] = 1.30 + 0.0005 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _85YR70[i] = 1.32 + 0.0012 * (i - 85);
        }
    }

    //printf("85YR  %1.2f  %1.2f %1.2f\n",_85YR70[44],_85YR70[85],_85YR70[139] );
    _85YR80(maxInd);
    _85YR80.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _85YR80[i] = 1.35 - 0.00075 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _85YR80[i] = 1.32 + 0.00025 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _85YR80[i] = 1.33 + 0.00125 * (i - 85);
        }
    }

    //printf("85YR  %1.2f  %1.2f %1.2f\n",_85YR80[44],_85YR80[85],_85YR80[139] );
    _85YR90(maxInd2);
    _85YR90.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _85YR90[i] = 1.39 - 0.00125 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _85YR90[i] = 1.34 + 0.00 * (i - 45);
        }
    }

    //printf("85YR  %1.2f  %1.2f\n",_85YR90[44],_85YR90[85]);

    //7YR
    _7YR30(maxInd2);
    _7YR30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _7YR30[i] = 1.06 + 0.0028 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _7YR30[i] = 1.17 + 0.0045 * (i - 45);
        }
    }

    //printf("7YR  %1.2f  %1.2f\n",_7YR30[44],_7YR30[66]);
    _7YR40(maxInd2);
    _7YR40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _7YR40[i] = 1.10 + 0.0018 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _7YR40[i] = 1.17 + 0.0035 * (i - 45);
        }
    }

    //printf("7YR  %1.2f  %1.2f\n",_7YR40[44],_7YR40[89]);
    _7YR50(maxInd2);
    _7YR50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _7YR50[i] = 1.14 + 0.00125 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _7YR50[i] = 1.19 + 0.002 * (i - 45);
        }
    }

    //printf("7YR  %1.2f  %1.2f\n",_7YR50[44],_7YR50[89] );
    _7YR60(maxInd);
    _7YR60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _7YR60[i] = 1.17 + 0.00075 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _7YR60[i] = 1.20 + 0.00175 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _7YR60[i] = 1.27 + 0.002 * (i - 85);
        }
    }

    //printf("7YR  %1.2f  %1.2f %1.2f\n",_7YR60[44],_7YR60[84],_7YR60[125] );

    _7YR70(maxInd);
    _7YR70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _7YR70[i] = 1.20 + 0.0005 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _7YR70[i] = 1.22 + 0.00125 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _7YR70[i] = 1.27 + 0.0015 * (i - 85);
        }
    }

    //printf("7YR  %1.2f  %1.2f %1.2f\n",_7YR70[44],_7YR70[84],_7YR70[125] );
    _7YR80(maxInd3);
    _7YR80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _7YR80[i] = 1.29 - 0.0008 * (i - 5);
        }
    }

    //printf("7YR  %1.2f \n",_7YR80[44] );
    _55YR30(maxInd3);
    _55YR30.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _55YR30[i] = 0.96 + 0.0038 * (i - 5);
        }
    }

    //printf("55YR  %1.2f \n",_55YR30[44] );
    _55YR40(maxInd2);
    _55YR40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _55YR40[i] = 1.01 + 0.0022 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _55YR40[i] = 1.10 + 0.0037 * (i - 45);
        }
    }

    //printf("55YR  %1.2f  %1.2f\n",_55YR40[44],_55YR40[89] );
    _55YR50(maxInd);
    _55YR50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _55YR50[i] = 1.06 + 0.0015 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _55YR50[i] = 1.12 + 0.00225 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _55YR50[i] = 1.21 + 0.0015 * (i - 85);
        }
    }

    //printf("55YR  %1.2f  %1.2f %1.2f\n",_55YR50[44],_55YR50[84],_55YR50[125] );
    _55YR60(maxInd);
    _55YR60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _55YR60[i] = 1.08 + 0.0012 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _55YR60[i] = 1.13 + 0.0018 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _55YR60[i] = 1.20 + 0.0025 * (i - 85);
        }
    }

    //printf("55YR  %1.2f  %1.2f %1.2f\n",_55YR60[44],_55YR60[84],_55YR60[125] );
    _55YR70(maxInd);
    _55YR70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _55YR70[i] = 1.11 + 0.00075 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _55YR70[i] = 1.14 + 0.0012 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _55YR70[i] = 1.19 + 0.00225 * (i - 85);
        }
    }

    //printf("55YR  %1.2f  %1.2f %1.2f\n",_55YR70[44],_55YR70[84],_55YR70[125] );
    _55YR80(maxInd);
    _55YR80.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _55YR80[i] = 1.16 + 0.00 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _55YR80[i] = 1.16 + 0.00075 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _55YR80[i] = 1.19 + 0.00175 * (i - 85);
        }
    }

    //printf("55YR  %1.2f  %1.2f %1.2f\n",_55YR80[44],_55YR80[84],_55YR80[125] );
    _55YR90(maxInd3);
    _55YR90.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _55YR90[i] = 1.19 - 0.0005 * (i - 5);
        }
    }

    //printf("55YR  %1.2f \n",_55YR90[44] );

    _4YR30(maxInd2);
    _4YR30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _4YR30[i] = 0.87 + 0.0035 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _4YR30[i] = 1.01 + 0.0043 * (i - 45);
        }
    }

    //printf("4YR  %1.2f  %1.2f\n",_4YR30[44],_4YR30[78] );
    _4YR40(maxInd2);
    _4YR40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _4YR40[i] = 0.92 + 0.0025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _4YR40[i] = 1.02 + 0.0033 * (i - 45);
        }
    }

    //printf("4YR  %1.2f  %1.2f\n",_4YR40[44],_4YR40[74] );
    _4YR50(maxInd2);
    _4YR50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _4YR50[i] = 0.97 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _4YR50[i] = 1.03 + 0.0025 * (i - 45);
        }
    }

    //printf("4YR  %1.2f  %1.2f\n",_4YR50[44],_4YR50[85] );
    _4YR60(maxInd);
    _4YR60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _4YR60[i] = 0.99 + 0.00125 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _4YR60[i] = 1.04 + 0.002 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _4YR60[i] = 1.12 + 0.003 * (i - 85);
        }
    }

    //printf("4YR  %1.2f  %1.2f %1.2f\n",_4YR60[44],_4YR60[84],_4YR60[125] );
    _4YR70(maxInd);
    _4YR70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _4YR70[i] = 1.02 + 0.00075 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _4YR70[i] = 1.05 + 0.00175 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _4YR70[i] = 1.12 + 0.002 * (i - 85);
        }
    }

    //printf("4YR  %1.2f  %1.2f %1.2f\n",_4YR70[44],_4YR70[84],_4YR70[125] );
    _4YR80(maxInd3);
    _4YR80.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 50 && i > 5) {
            _4YR80[i] = 1.09 - 0.0002 * (i - 5);
        }
    }

    //printf("4YR  %1.2f \n",_4YR80[41] );

    _25YR30(maxInd2);
    _25YR30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _25YR30[i] = 0.77 + 0.004 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _25YR30[i] = 0.94 + 0.004 * (i - 45);
        }
    }

    //printf("25YR  %1.2f  %1.2f\n",_25YR30[44],_25YR30[74] );
    _25YR40(maxInd2);
    _25YR40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _25YR40[i] = 0.82 + 0.003 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _25YR40[i] = 0.94 + 0.002 * (i - 45);
        }
    }

    //printf("25YR  %1.2f  %1.2f\n",_25YR40[44],_25YR40[84] );
    _25YR50(maxInd2);
    _25YR50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _25YR50[i] = 0.87 + 0.002 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _25YR50[i] = 0.95 + 0.003 * (i - 45);
        }
    }

    //printf("25YR  %1.2f  %1.2f\n",_25YR50[44],_25YR50[84] );
    _25YR60(maxInd2);
    _25YR60.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _25YR60[i] = 0.89 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _25YR60[i] = 0.95 + 0.004 * (i - 45);
        }
    }

    //printf("25YR  %1.2f  %1.2f\n",_25YR60[44],_25YR60[84] );
    _25YR70(maxInd2);
    _25YR70.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _25YR70[i] = 0.92 + 0.001 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _25YR70[i] = 0.96 + 0.003 * (i - 45);
        }
    }

    //printf("25YR  %1.2f  %1.2f\n",_25YR70[44],_25YR70[84] );

    _10R30(maxInd2);
    _10R30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10R30[i] = 0.62 + 0.00225 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10R30[i] = 0.71 + 0.003 * (i - 45);
        }
    }

    //printf("10R  %1.2f  %1.2f\n",_10R30[44],_10R30[84] );
    _10R40(maxInd2);
    _10R40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10R40[i] = 0.66 + 0.0025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10R40[i] = 0.76 + 0.0035 * (i - 45);
        }
    }

    //printf("10R  %1.2f  %1.2f\n",_10R40[44],_10R40[84] );
    _10R50(maxInd2);
    _10R50.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10R50[i] = 0.71 + 0.002 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10R50[i] = 0.79 + 0.0043 * (i - 45);
        }
    }

    //printf("10R  %1.2f  %1.2f\n",_10R50[44],_10R50[84] );
    _10R60(maxInd);
    _10R60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10R60[i] = 0.73 + 0.00175 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10R60[i] = 0.80 + 0.0033 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10R60[i] = 0.93 + 0.0018 * (i - 85);
        }
    }

    //printf("10R  %1.2f  %1.2f %1.2f\n",_10R60[44],_10R60[84],_10R60[125] );
    _10R70(maxInd);
    _10R70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10R70[i] = 0.75 + 0.0015 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10R70[i] = 0.81 + 0.0017 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10R70[i] = 0.88 + 0.0025 * (i - 85);
        }
    }

    //printf("10R  %1.2f  %1.2f %1.2f\n",_10R70[44],_10R70[84],_10R70[125] );

    _9R30(maxInd2);
    _9R30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _9R30[i] = 0.57 + 0.002 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _9R30[i] = 0.65 + 0.0018 * (i - 45);
        }
    }

    //printf("9R  %1.2f  %1.2f\n",_9R30[44],_9R30[84] );
    _9R40(maxInd2);
    _9R40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _9R40[i] = 0.61 + 0.002 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _9R40[i] = 0.69 + 0.0025 * (i - 45);
        }
    }

    //printf("9R  %1.2f  %1.2f\n",_9R40[44],_9R40[84] );
    _9R50(maxInd);
    _9R50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _9R50[i] = 0.66 + 0.00175 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _9R50[i] = 0.73 + 0.0025 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _9R50[i] = 0.83 + 0.0035 * (i - 85);
        }
    }

    //printf("9R  %1.2f  %1.2f %1.2f\n",_9R50[44],_9R50[84],_9R50[125] );
    _9R60(maxInd);
    _9R60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _9R60[i] = 0.68 + 0.0015 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _9R60[i] = 0.74 + 0.0022 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _9R60[i] = 0.93 + 0.0022 * (i - 85);
        }
    }

    //printf("9R  %1.2f  %1.2f %1.2f\n",_9R60[44],_9R60[84],_9R60[125] );
    _9R70(maxInd2);
    _9R70.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _9R70[i] = 0.70 + 0.0012 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _9R70[i] = 0.75 + 0.0013 * (i - 45);
        }
    }

    //printf("9R  %1.2f  %1.2f\n",_9R70[44],_9R70[84] );

    _7R30(maxInd2);
    _7R30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _7R30[i] = 0.48 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _7R30[i] = 0.54 - 0.0005 * (i - 45);
        }
    }

    //printf("7R  %1.2f  %1.2f\n",_7R30[44],_7R30[84] );
    _7R40(maxInd2);
    _7R40.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _7R40[i] = 0.51 + 0.0015 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _7R40[i] = 0.57 + 0.0005 * (i - 45);
        }
    }

    //printf("7R  %1.2f  %1.2f\n",_7R40[44],_7R40[84] );
    _7R50(maxInd);
    _7R50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _7R50[i] = 0.54 + 0.0015 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _7R50[i] = 0.60 + 0.0005 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _7R50[i] = 0.62 + 0.0025 * (i - 85);
        }
    }

    //printf("7R  %1.2f  %1.2f %1.2f\n",_7R50[44],_7R50[84],_7R50[125] );
    _7R60(maxInd);
    _7R60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _7R60[i] = 0.58 + 0.00075 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _7R60[i] = 0.61 + 0.00075 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _7R60[i] = 0.64 + 0.001 * (i - 85);
        }
    }

    //printf("7R  %1.2f  %1.2f %1.2f\n",_7R60[44],_7R60[84],_7R60[107] );
    _7R70(maxInd2);
    _7R70.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _7R70[i] = 0.59 + 0.00075 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _7R70[i] = 0.62 + 0.00075 * (i - 45);
        }
    }

    //printf("7R  %1.2f  %1.2f\n",_7R70[44],_7R70[84] );

    //5R 1 2 3

    //5R
    _5R10(maxInd2);
    _5R10.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _5R10[i] = 0.10 - 0.0018 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _5R10[i] = 0.035 - 0.003 * (i - 45);
        }
    }

    //printf("5R  %1.2f  %1.2f\n",_5R10[44],_5R10[51] );
    _5R20(maxInd2);
    _5R20.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _5R20[i] = 0.26 - 0.00075 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _5R20[i] = 0.023 - 0.0002 * (i - 45);
        }
    }

    //printf("5R  %1.2f  %1.2f\n",_5R20[44],_5R20[70] );
    _5R30(maxInd2);
    _5R30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _5R30[i] = 0.39 + 0.00075 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _5R30[i] = 0.42 - 0.0007 * (i - 45);
        }
    }

    //printf("5R  %1.2f  %1.2f\n",_5R30[44],_5R30[85] );

    //25R
    _25R10(maxInd3);
    _25R10.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 45 && i > 5) {
            _25R10[i] = -0.03 - 0.002 * (i - 5);
        }
    }

    //printf("25R  %1.2f \n",_25R10[44]);
    _25R20(maxInd2);
    _25R20.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _25R20[i] = 0.13 - 0.0012 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _25R20[i] = 0.08 - 0.002 * (i - 45);
        }
    }

    //printf("25R  %1.2f  %1.2f\n",_25R20[44],_25R20[69] );
    //25R30: 0.28, 0.26, 0.22
    _25R30(maxInd2);
    _25R30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _25R30[i] = 0.28 - 0.0005 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _25R30[i] = 0.26 - 0.0009 * (i - 45);
        }
    }

    //printf("25R  %1.2f  %1.2f\n",_25R30[44],_25R30[85] );


    _10RP10(maxInd3);
    _10RP10.clear();

    for (int i = 0; i < maxInd3; i++) {
        if (i < 45 && i > 5) {
            _10RP10[i] = -0.16 - 0.0017 * (i - 5);
        }
    }

    //printf("10RP  %1.2f \n",_10RP10[44]);
    _10RP20(maxInd2);
    _10RP20.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10RP20[i] = 0.0 - 0.0018 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10RP20[i] = -0.07 - 0.0012 * (i - 45);
        }
    }

    //printf("10RP  %1.2f  %1.2f\n",_10RP20[44],_10RP20[69] );
    _10RP30(maxInd2);
    _10RP30.clear();

    for (int i = 0; i < maxInd2; i++) {
        if (i < 45 && i > 5) {
            _10RP30[i] = 0.15 - 0.001 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _10RP30[i] = 0.11 - 0.0012 * (i - 45);
        }
    }

    //printf("10RP  %1.2f  %1.2f\n",_10RP30[44],_10RP30[85] );

    //7G
    _7G30(maxInd);
    _7G30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _7G30[i] = 2.90 + 0.0027 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _7G30[i] = 3.01 + 0.0005 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _7G30[i] = 3.03 + 0.00075 * (i - 85);
        }
    }

    //printf("7G  %1.2f  %1.2f %1.2f\n",_7G30[44],_7G30[84],_7G30[125] );
    _7G40(maxInd);
    _7G40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _7G40[i] = 2.89 + 0.00125 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _7G40[i] = 2.94 + 0.0015 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _7G40[i] = 3.0 + 0.001 * (i - 85);
        }
    }

    //printf("7G  %1.2f  %1.2f %1.2f\n",_7G40[44],_7G40[84],_7G40[125] );
    _7G50(maxInd);
    _7G50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _7G50[i] = 2.87 + 0.0015 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _7G50[i] = 2.93 + 0.00125 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _7G50[i] = 2.98 + 0.001 * (i - 85);
        }
    }

    //printf("7G  %1.2f  %1.2f %1.2f\n",_7G50[44],_7G50[84],_7G50[125] );
    _7G60(maxInd);
    _7G60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _7G60[i] = 2.86 + 0.00125 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _7G60[i] = 2.91 + 0.00125 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _7G60[i] = 2.96 + 0.00075 * (i - 85);
        }
    }

    //printf("7G  %1.2f  %1.2f %1.2f\n",_7G60[44],_7G60[84],_7G60[125] );
    _7G70(maxInd);
    _7G70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _7G70[i] = 2.85 + 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _7G70[i] = 2.89 + 0.00125 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _7G70[i] = 2.94 + 0.00075 * (i - 85);
        }
    }

    //printf("7G  %1.2f  %1.2f %1.2f\n",_7G70[44],_7G70[84],_7G70[125] );
    _7G80(maxInd);
    _7G80.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _7G80[i] = 2.84 + 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _7G80[i] = 2.88 + 0.001 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _7G80[i] = 2.92 + 0.001 * (i - 85);
        }
    }

    //printf("7G  %1.2f  %1.2f %1.2f\n",_7G80[44],_7G80[84],_7G80[125] );


    //5G
    _5G30(maxInd);
    _5G30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5G30[i] = 2.82 + 0.00175 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _5G30[i] = 2.89 + 0.0018 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _5G30[i] = 2.96 + 0.0012 * (i - 85);
        }
    }

    //printf("5G  %1.2f  %1.2f %1.2f\n",_5G30[44],_5G30[84],_5G30[125] );
    _5G40(maxInd);
    _5G40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5G40[i] = 2.80 + 0.0015 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _5G40[i] = 2.86 + 0.00175 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _5G40[i] = 2.93 + 0.00125 * (i - 85);
        }
    }

    //printf("5G  %1.2f  %1.2f %1.2f\n",_5G40[44],_5G40[84],_5G40[125] );
    _5G50(maxInd);
    _5G50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5G50[i] = 2.79 + 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _5G50[i] = 2.84 + 0.0015 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _5G50[i] = 2.90 + 0.0015 * (i - 85);
        }
    }

    //printf("5G  %1.2f  %1.2f %1.2f\n",_5G50[44],_5G50[84],_5G50[125] );
    _5G60(maxInd);
    _5G60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5G60[i] = 2.78 + 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _5G60[i] = 2.82 + 0.00175 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _5G60[i] = 2.89 + 0.001 * (i - 85);
        }
    }

    //printf("5G  %1.2f  %1.2f %1.2f\n",_5G60[44],_5G60[84],_5G60[125] );
    _5G70(maxInd);
    _5G70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5G70[i] = 2.77 + 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _5G70[i] = 2.81 + 0.00125 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _5G70[i] = 2.86 + 0.00125 * (i - 85);
        }
    }

    //printf("5G  %1.2f  %1.2f %1.2f\n",_5G70[44],_5G70[84],_5G70[125] );
    _5G80(maxInd);
    _5G80.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5G80[i] = 2.76 + 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _5G80[i] = 2.8 + 0.00125 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _5G80[i] = 2.85 + 0.00125 * (i - 85);
        }
    }

    //printf("5G  %1.2f  %1.2f %1.2f\n",_5G80[44],_5G80[84],_5G80[125] );

    //25G
    _25G30(maxInd);
    _25G30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _25G30[i] = 2.68 + 0.0015 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _25G30[i] = 2.74 + 0.0018 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _25G30[i] = 2.81 + 0.002 * (i - 85);
        }
    }

    //printf("25G  %1.2f  %1.2f %1.2f\n",_25G30[44],_25G30[84],_25G30[125] );
    _25G40(maxInd);
    _25G40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _25G40[i] = 2.68 + 0.00075 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _25G40[i] = 2.71 + 0.0015 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _25G40[i] = 2.77 + 0.00125 * (i - 85);
        }
    }

    //printf("25G  %1.2f  %1.2f %1.2f\n",_25G40[44],_25G40[84],_25G40[125] );
    _25G50(maxInd);
    _25G50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _25G50[i] = 2.65 + 0.00075 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _25G50[i] = 2.68 + 0.00125 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _25G50[i] = 2.73 + 0.00125 * (i - 85);
        }
    }

    //printf("25G  %1.2f  %1.2f %1.2f\n",_25G50[44],_25G50[84],_25G50[125] );
    _25G60(maxInd);
    _25G60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _25G60[i] = 2.64 + 0.0005 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _25G60[i] = 2.66 + 0.001 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _25G60[i] = 2.70 + 0.001 * (i - 85);
        }
    }

    //printf("25G  %1.2f  %1.2f %1.2f\n",_25G60[44],_25G60[84],_25G60[125] );
    _25G70(maxInd);
    _25G70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _25G70[i] = 2.64 + 0.00 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _25G70[i] = 2.64 + 0.00075 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _25G70[i] = 2.67 + 0.001 * (i - 85);
        }
    }

    //printf("25G  %1.2f  %1.2f %1.2f\n",_25G70[44],_25G70[84],_25G70[125] );
    _25G80(maxInd);
    _25G80.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _25G80[i] = 2.63 + 0.00 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _25G80[i] = 2.63 + 0.0005 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _25G80[i] = 2.65 + 0.0005 * (i - 85);
        }
    }

    //printf("25G  %1.2f  %1.2f %1.2f\n",_25G80[44],_25G80[84],_25G80[125] );


    //1G
    _1G30(maxInd);
    _1G30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _1G30[i] = 2.58 + 0.00025 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _1G30[i] = 2.59 + 0.001 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _1G30[i] = 2.63 + 0.00125 * (i - 85);
        }
    }

    //printf("1G  %1.2f  %1.2f %1.2f\n",_1G30[44],_1G30[84],_1G30[125] );
    _1G40(maxInd);
    _1G40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _1G40[i] = 2.56 - 0.00025 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _1G40[i] = 2.55 + 0.0005 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _1G40[i] = 2.57 + 0.0005 * (i - 85);
        }
    }

    //printf("1G  %1.2f  %1.2f %1.2f\n",_1G40[44],_1G40[84],_1G40[125] );
    _1G50(maxInd);
    _1G50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _1G50[i] = 2.55 - 0.00025 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _1G50[i] = 2.54 + 0.00025 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _1G50[i] = 2.55 + 0.0005 * (i - 85);
        }
    }

    //printf("1G  %1.2f  %1.2f %1.2f\n",_1G50[44],_1G50[84],_1G50[125] );
    _1G60(maxInd);
    _1G60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _1G60[i] = 2.54 - 0.0005 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _1G60[i] = 2.52 + 0.00025 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _1G60[i] = 2.53 + 0.00025 * (i - 85);
        }
    }

    //printf("1G  %1.2f  %1.2f %1.2f\n",_1G60[44],_1G60[84],_1G60[125] );
    _1G70(maxInd);
    _1G70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _1G70[i] = 2.53 - 0.0005 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _1G70[i] = 2.51 + 0.0 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _1G70[i] = 2.51 + 0.00025 * (i - 85);
        }
    }

    //printf("1G  %1.2f  %1.2f %1.2f\n",_1G70[44],_1G70[84],_1G70[125] );
    _1G80(maxInd);
    _1G80.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _1G80[i] = 2.52 - 0.0005 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _1G80[i] = 2.50 + 0.00 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _1G80[i] = 2.50 + 0.00 * (i - 85);
        }
    }

    //printf("1G  %1.2f  %1.2f %1.2f\n",_1G80[44],_1G80[84],_1G80[125] );


    //10GY
    _10GY30(maxInd);
    _10GY30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10GY30[i] = 2.52 - 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10GY30[i] = 2.48 - 0.002 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10GY30[i] = 2.40 + 0.0025 * (i - 85);
        }
    }

    //printf("10GY  %1.2f  %1.2f %1.2f\n",_10GY30[44],_10GY30[84],_10GY30[125] );
    _10GY40(maxInd);
    _10GY40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10GY40[i] = 2.48 - 0.0005 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10GY40[i] = 2.46 - 0.0005 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10GY40[i] = 2.44 - 0.0015 * (i - 85);
        }
    }

    //printf("10GY  %1.2f  %1.2f %1.2f\n",_10GY40[44],_10GY40[84],_10GY40[125] );
    _10GY50(maxInd);
    _10GY50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10GY50[i] = 2.48 - 0.00075 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10GY50[i] = 2.45 - 0.00075 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10GY50[i] = 2.42 - 0.00175 * (i - 85);
        }
    }

    //printf("10GY  %1.2f  %1.2f %1.2f\n",_10GY50[44],_10GY50[84],_10GY50[125] );
    _10GY60(maxInd);
    _10GY60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10GY60[i] = 2.47 - 0.00125 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10GY60[i] = 2.42 - 0.00025 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10GY60[i] = 2.41 - 0.0005 * (i - 85);
        }
    }

    //printf("10GY  %1.2f  %1.2f %1.2f\n",_10GY60[44],_10GY60[84],_10GY60[125] );
    _10GY70(maxInd);
    _10GY70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10GY70[i] = 2.46 - 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10GY70[i] = 2.42 + 0.0 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10GY70[i] = 2.42 - 0.001 * (i - 85);
        }
    }

    //printf("10GY %1.2f  %1.2f %1.2f\n",_10GY70[44],_10GY70[84],_10GY70[125] );
    _10GY80(maxInd);
    _10GY80.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _10GY80[i] = 2.45 - 0.00075 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _10GY80[i] = 2.42 - 0.0005 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _10GY80[i] = 2.40 - 0.0005 * (i - 85);
        }
    }

    //printf("10GY  %1.2f  %1.2f %1.2f\n",_10GY80[44],_10GY80[84],_10GY80[125] );


    //75GY
    _75GY30(maxInd2);
    _75GY30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _75GY30[i] = 2.36 - 0.0025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _75GY30[i] = 2.26 - 0.00175 * (i - 45);
        }
    }

    //printf("75GY  %1.2f  %1.2f\n",_75GY30[44],_75GY30[84] );
    _75GY40(maxInd2);
    _75GY40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _75GY40[i] = 2.34 - 0.00175 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _75GY40[i] = 2.27 - 0.00225 * (i - 45);
        }
    }

    //printf("75GY  %1.2f  %1.2f \n",_75GY40[44],_75GY40[84] );
    _75GY50(maxInd);
    _75GY50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _75GY50[i] = 2.32 - 0.0015 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _75GY50[i] = 2.26 - 0.00175 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _75GY50[i] = 2.19 - 0.00325 * (i - 85);
        }
    }

    //printf("75GY  %1.2f  %1.2f %1.2f %1.2f\n",_75GY50[44],_75GY50[84],_75GY50[125],_75GY50[139] );
    _75GY60(maxInd);
    _75GY60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _75GY60[i] = 2.30 - 0.00125 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _75GY60[i] = 2.25 - 0.001 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _75GY60[i] = 2.21 - 0.0027 * (i - 85);
        }
    }

    //printf("75GY  %1.2f  %1.2f %1.2f\n",_75GY60[44],_75GY60[84],_75GY60[125] );
    _75GY70(maxInd);
    _75GY70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _75GY70[i] = 2.29 - 0.00125 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _75GY70[i] = 2.24 - 0.0015 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _75GY70[i] = 2.18 - 0.00175 * (i - 85);
        }
    }

    //printf("75GY %1.2f  %1.2f %1.2f\n",_75GY70[44],_75GY70[84],_75GY70[125] );
    _75GY80(maxInd);
    _75GY80.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _75GY80[i] = 2.27 - 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _75GY80[i] = 2.23 - 0.001 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _75GY80[i] = 2.19 - 0.00175 * (i - 85);
        }
    }

    //printf("75GY  %1.2f  %1.2f %1.2f\n",_75GY80[44],_75GY80[84],_75GY80[125] );


    //55GY
    _5GY30(maxInd2);
    _5GY30.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5GY30[i] = 2.16 - 0.002 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _5GY30[i] = 2.07 - 0.0025 * (i - 45);
        }
    }

    //printf("5GY  %1.2f  %1.2f\n",_5GY30[44],_5GY30[84] );

    //5GY4: 2.14,2.04, 1.96, 1.91 //95

    _5GY40(maxInd2);
    _5GY40.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5GY40[i] = 2.14 - 0.0025 * (i - 5);
        } else if (i < 90 && i >= 45) {
            _5GY40[i] = 2.04 - 0.003 * (i - 45);
        }
    }

    //printf("5GY  %1.2f  %1.2f \n",_5GY40[44],_5GY40[84] );
    _5GY50(maxInd);
    _5GY50.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5GY50[i] = 2.13 - 0.00175 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _5GY50[i] = 2.06 - 0.002 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _5GY50[i] = 1.98 - 0.00225 * (i - 85);
        }
    }

    //printf("5GY  %1.2f  %1.2f %1.2f\n",_5GY50[44],_5GY50[84],_5GY50[125] );
    _5GY60(maxInd);
    _5GY60.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5GY60[i] = 2.11 - 0.0015 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _5GY60[i] = 2.05 - 0.002 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _5GY60[i] = 1.97 - 0.00275 * (i - 85);
        }
    }

    //printf("5GY  %1.2f  %1.2f %1.2f\n",_5GY60[44],_5GY60[84],_5GY60[125] );
    _5GY70(maxInd);
    _5GY70.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5GY70[i] = 2.09 - 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _5GY70[i] = 2.05 - 0.00175 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _5GY70[i] = 1.98 - 0.002 * (i - 85);
        }
    }

    //printf("5GY %1.2f  %1.2f %1.2f\n",_5GY70[44],_5GY70[84],_5GY70[125] );
    _5GY80(maxInd);
    _5GY80.clear();

    for (int i = 0; i < maxInd; i++) {
        if (i < 45 && i > 5) {
            _5GY80[i] = 2.07 - 0.001 * (i - 5);
        } else if (i < 85 && i >= 45) {
            _5GY80[i] = 2.03 - 0.00075 * (i - 45);
        } else if (i < 140 && i >= 85) {
            _5GY80[i] = 2.0 - 0.002 * (i - 85);
        }
    }

    //printf("5GY  %1.2f  %1.2f %1.2f\n",_5GY80[44],_5GY80[84],_5GY80[125] );

#ifdef _DEBUG
    t2e.set();

    if (settings->verbose) {
        printf("Lutf Munsell  %d usec\n", t2e.etime(t1e));
    }

#endif
}

}
