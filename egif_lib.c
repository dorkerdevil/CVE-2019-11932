#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gif_lib.h"

static int EGifBufferedOutput(GifFilePrivateType *Private, int c) {
    Private->Buf[0] = 0;
    Private->Buf[++(Private->Buf[0])] = c;
    Private->OutBuf[Private->OutBufLen++] = c;
    return GIF_OK;
}

static int EGifCompressOutput(GifFilePrivateType *Private, const int Code)
{
    int retval = GIF_OK;

    if (Code == FLUSH_OUTPUT) {
        while (Private->CrntShiftState > 0) {
            /* Get Rid of what is left in DWord, and flush it. */
            if (EGifBufferedOutput(Private, Private->CrntShiftDWord & 0xff) == GIF_ERROR)
                retval = GIF_ERROR;
            Private->CrntShiftDWord >>= 8;
            Private->CrntShiftState -= 8;
        }
        Private->CrntShiftState = 0;    /* For next time. */
        if (EGifBufferedOutput(Private, FLUSH_OUTPUT) == GIF_ERROR)
            retval = GIF_ERROR;
    } else {
        Private->CrntShiftDWord |= ((long)Code) << Private->CrntShiftState;
        Private->CrntShiftState += Private->RunningBits;
        while (Private->CrntShiftState >= 8) {
            /* Dump out full bytes: */
            if (EGifBufferedOutput(Private, Private->CrntShiftDWord & 0xff) == GIF_ERROR)
                retval = GIF_ERROR;
            Private->CrntShiftDWord >>= 8;
            Private->CrntShiftState -= 8;
        }
    }

    /* If code cannt fit into RunningBits bits, must raise its size. Note */
    /* however that codes above 4095 are used for special signaling.      */
    if (Private->RunningCode >= Private->MaxCode1 && Code <= 4095) {
        Private->MaxCode1 = 1 << ++Private->RunningBits;
    }

    return retval;
}

int EGifCompressLine(GifFilePrivateType *Private, unsigned char *Line, const int LineLen)
{
    int i = 0, CrntCode, NewCode;
    unsigned long NewKey;
    GifPixelType Pixel;

    if (Private->CrntCode == FIRST_CODE)    /* Its first time! */
        CrntCode = Line[i++];
    else
        CrntCode = Private->CrntCode;    /* Get last code in compression. */

    while (i < LineLen) {   /* Decode LineLen items. */
        Pixel = Line[i++];  /* Get next pixel from stream. */

        if (EGifCompressOutput(Private, CrntCode) == GIF_ERROR) {
            return GIF_ERROR;
        }
        CrntCode = Pixel;

        /* If however the HashTable if full, we send a clear first and
         * Clear the hash table.
         */
        if (Private->RunningCode >= LZ_MAX_CODE) {
            /* Time to do some clearance: */
            if (EGifCompressOutput(Private, Private->ClearCode)
                == GIF_ERROR) {
                return GIF_ERROR;
            }
            Private->RunningCode = Private->EOFCode + 1;
            Private->RunningBits = Private->BitsPerPixel + 1;
            Private->MaxCode1 = 1 << Private->RunningBits;
        }

    }

    /* Preserve the current state of the compression algorithm: */
    Private->CrntCode = CrntCode;

    if (Private->PixelCount == 0) {
        /* We are done - output last Code and flush output buffers: */
        if (EGifCompressOutput(Private, CrntCode) == GIF_ERROR) {
            return GIF_ERROR;
        }
        if (EGifCompressOutput(Private, Private->EOFCode) == GIF_ERROR) {
            return GIF_ERROR;
        }
        if (EGifCompressOutput(Private, FLUSH_OUTPUT) == GIF_ERROR) {
            return GIF_ERROR;
        }
    }

    return GIF_OK;
}
