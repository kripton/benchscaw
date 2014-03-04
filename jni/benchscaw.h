/*
 * benchscaw.h
 *
 *  Created on: 15/04/2013
 *      Author: brooss
 */

#ifndef BENCHSCAW_H_
#define BENCHSCAW_H_

//ffmpeg
static const struct {
    int flag;
    const char *name;
} cpu_flag_tab[] = {
#ifdef __arm__
    { AV_CPU_FLAG_ARMV5TE,   "armv5te"    },
    { AV_CPU_FLAG_ARMV6,     "armv6"      },
    { AV_CPU_FLAG_ARMV6T2,   "armv6t2"    },
    { AV_CPU_FLAG_VFP,       "vfp"        },
    { AV_CPU_FLAG_VFPV3,     "vfpv3"      },
    { AV_CPU_FLAG_NEON,      "neon"       },
#else
    { AV_CPU_FLAG_MMX,       "mmx"        },
    { AV_CPU_FLAG_MMXEXT,    "mmxext"     },
    { AV_CPU_FLAG_SSE,       "sse"        },
    { AV_CPU_FLAG_SSE2,      "sse2"       },
    { AV_CPU_FLAG_SSE2SLOW,  "sse2(slow)" },
    { AV_CPU_FLAG_SSE3,      "sse3"       },
    { AV_CPU_FLAG_SSE3SLOW,  "sse3(slow)" },
    { AV_CPU_FLAG_SSSE3,     "ssse3"      },
    { AV_CPU_FLAG_ATOM,      "atom"       },
    { AV_CPU_FLAG_SSE4,      "sse4.1"     },
    { AV_CPU_FLAG_SSE42,     "sse4.2"     },
    { AV_CPU_FLAG_AVX,       "avx"        },
    { AV_CPU_FLAG_XOP,       "xop"        },
    { AV_CPU_FLAG_FMA4,      "fma4"       },
    { AV_CPU_FLAG_3DNOW,     "3dnow"      },
    { AV_CPU_FLAG_3DNOWEXT,  "3dnowext"   },
    { AV_CPU_FLAG_CMOV,      "cmov"       },
#endif
    { 0 }
};
//ffmpeg ugly globals
AVFormatContext *pFormatCtx = NULL;
AVCodecContext  *pCodecCtx = NULL;
AVCodec         *pCodec = NULL;
AVDictionary    *optionsDict = NULL;
AVFrame *pFrame = NULL;
AVFrame *pFrameRGB = NULL;
AVPacket packet;
struct SwsContext      *sws_ctx = NULL;
int             i, videoStream;
int frameFinished;
uint8_t *buffer = NULL;

#endif /* BENCHSCAW_H_ */
