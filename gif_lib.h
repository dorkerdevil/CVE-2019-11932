/******************************************************************************
 
gif_lib.h - service library for decoding and encoding GIF images
                                                                             
*****************************************************************************/

#ifndef _GIF_LIB_H_
#define _GIF_LIB_H_ 1

#define GIF_ERROR   0
#define GIF_OK      1

#include <stddef.h>
#include <stdbool.h>

typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef short __int16_t;
typedef unsigned short __uint16_t;
typedef int __int32_t;
typedef unsigned int __uint32_t;
#if defined(__LP64__)
typedef long __int64_t;
typedef unsigned long __uint64_t;
#else
typedef long long __int64_t;
typedef unsigned long long __uint64_t;
#endif
#if defined(__LP64__)
typedef long __intptr_t;
typedef unsigned long __uintptr_t;
#else
typedef int __intptr_t;
typedef unsigned int __uintptr_t;
#endif
typedef __int8_t      int8_t;
typedef __uint8_t     uint8_t;
typedef __int16_t     int16_t;
typedef __uint16_t    uint16_t;
typedef __int32_t     int32_t;
typedef __uint32_t    uint32_t;
typedef __int64_t     int64_t;
typedef __uint64_t    uint64_t;
typedef __intptr_t    intptr_t;
typedef __uintptr_t   uintptr_t;
typedef int8_t        int_least8_t;
typedef uint8_t       uint_least8_t;
typedef int16_t       int_least16_t;
typedef uint16_t      uint_least16_t;
typedef int32_t       int_least32_t;
typedef uint32_t      uint_least32_t;
typedef int64_t       int_least64_t;
typedef uint64_t      uint_least64_t;
typedef int8_t        int_fast8_t;
typedef uint8_t       uint_fast8_t;
typedef int64_t       int_fast64_t;
typedef uint64_t      uint_fast64_t;
#if defined(__LP64__)
typedef int64_t       int_fast16_t;
typedef uint64_t      uint_fast16_t;
typedef int64_t       int_fast32_t;
typedef uint64_t      uint_fast32_t;
#else
typedef int32_t       int_fast16_t;
typedef uint32_t      uint_fast16_t;
typedef int32_t       int_fast32_t;
typedef uint32_t      uint_fast32_t;
#endif

#define GIF_STAMP "GIFVER"          /* First chars in file - GIF stamp.  */
#define GIF_STAMP_LEN sizeof(GIF_STAMP) - 1
#define GIF_VERSION_POS 3           /* Version first character in stamp. */

typedef unsigned char GifPixelType;
typedef unsigned char GifByteType;
typedef unsigned int GifPrefixType;
typedef uint_fast16_t GifWord;

typedef struct GifColorType {
	uint8_t Red, Green, Blue;
} GifColorType;

typedef struct ColorMapObject {
	uint_fast16_t ColorCount;
	uint_fast8_t BitsPerPixel;
//    bool SortFlag;
	GifColorType *Colors;    /* on malloc(3) heap */
} ColorMapObject;

typedef struct GifImageDesc {
	GifWord Left, Top, Width, Height;   /* Current image dimensions. */
	bool Interlace;
	/* Sequential/Interlaced lines. */
	ColorMapObject *ColorMap;           /* The local color map */
} GifImageDesc;

//typedef struct ExtensionBlock {
//    int ByteCount;
//    GifByteType *Bytes; /* on malloc(3) heap */
//    int Function;       /* The block function code */
#define CONTINUE_EXT_FUNC_CODE    0x00    /* continuation subblock */
#define COMMENT_EXT_FUNC_CODE     0xfe    /* comment */
#define GRAPHICS_EXT_FUNC_CODE    0xf9    /* graphics control (GIF89) */
#define PLAINTEXT_EXT_FUNC_CODE   0x01    /* plaintext */
#define APPLICATION_EXT_FUNC_CODE 0xff    /* application block */
//} ExtensionBlock;

typedef struct SavedImage {
	GifImageDesc ImageDesc;
//    GifByteType *RasterBits;         /* on malloc(3) heap */
//    int ExtensionBlockCount;         /* Count of extensions before image */
//    ExtensionBlock *ExtensionBlocks; /* Extensions before image */
} SavedImage;

#define EXTENSION_INTRODUCER      0x21
#define DESCRIPTOR_INTRODUCER     0x2c
#define TERMINATOR_INTRODUCER     0x3b

#define LZ_MAX_CODE         4095    /* Biggest code possible in 12 bits. */
#define LZ_BITS             12

#define FLUSH_OUTPUT        4096    /* Impossible code, to signal flush. */
#define FIRST_CODE          4097    /* Impossible code, to signal first. */
#define NO_SUCH_CODE        4098    /* Impossible code, to signal empty. */

//#define FILE_STATE_WRITE    0x01
//#define FILE_STATE_SCREEN   0x02
//#define FILE_STATE_IMAGE    0x04
//#define FILE_STATE_READ     0x08

//#define IS_READABLE(Private)    (Private->FileState & FILE_STATE_READ)


struct GifFileType;
/* func type to read gif data from arbitrary sources (TVT) */
typedef uint_fast8_t (*InputFunc)(struct GifFileType *, GifByteType *, uint_fast8_t);

typedef struct GifFilePrivateType {
	GifWord //FileState, /*FileHandle,*/  /* Where all this data goes to! */
			BitsPerPixel,     /* Bits per pixel (Codes uses at least this + 1). */
			ClearCode,   /* The CLEAR LZ code. */
			EOFCode,     /* The EOF LZ code. */
			RunningCode, /* The next code algorithm can generate. */
			RunningBits, /* The number of bits required to represent RunningCode. */
			MaxCode1,    /* 1 bigger than max. possible code, in RunningBits bits. */
			LastCode,    /* The code before the current code. */
            CrntCode,    /* Current algorithm code. */
			StackPtr,    /* For character stack (see below). */
			CrntShiftState;
	/* Number of bits in CrntShiftDWord. */
	unsigned long CrntShiftDWord;
	/* For bytes decomposition into codes. */
	uint_fast32_t PixelCount;
	/* Number of pixels in image. */
//    FILE *File;
	/* File as stream. */
	InputFunc Read;     /* function to read gif input (TVT) */
//    OutputFunc Write;   /* function to write gif output (MRB) */
	GifByteType Buf[256];
	unsigned char OutBuf[500];
	int OutBufLen;
	/* Compressed input is buffered here. */
	GifByteType Stack[LZ_MAX_CODE];
	/* Decoded pixels are stacked here. */
	GifByteType Suffix[LZ_MAX_CODE + 1];
	/* So we can trace the codes. */
	GifPrefixType Prefix[LZ_MAX_CODE + 1];
//    bool gif89;
} GifFilePrivateType;

typedef struct GifFileType {
	GifWord SWidth, SHeight;         /* Size of virtual canvas */
//    GifWord SColorResolution;        /* How many colors can we generate? */
	GifWord SBackGroundColor;        /* Background color for virtual canvas */
//    GifByteType AspectByte;	     /* Used to compute pixel aspect ratio */
	ColorMapObject *SColorMap;
	/* Global colormap, NULL if nonexistent. */
	uint_fast32_t ImageCount;
	/* Number of current image (both APIs) */
	GifImageDesc Image;
	/* Current image (low-level API) */
	SavedImage *SavedImages;         /* Image sequence (high-level API) */
//    int ExtensionBlockCount;         /* Count extensions past last image */
//    ExtensionBlock *ExtensionBlocks; /* Extensions past last image */
	int Error;
	/* Last error condition reported */
	void *UserData;
	/* hook to attach user data (TVT) */
	GifFilePrivateType *Private;                   /* Don't mess with this! */
} GifFileType;

//#define GIF_ASPECT_RATIO(n)    ((n)+15.0/64.0)

typedef enum {
	UNDEFINED_RECORD_TYPE,
	SCREEN_DESC_RECORD_TYPE,
	IMAGE_DESC_RECORD_TYPE, /* Begin with ',' */
			EXTENSION_RECORD_TYPE, /* Begin with '!' */
			TERMINATE_RECORD_TYPE   /* Begin with ';' */
} GifRecordType;

/* func type to read gif data from arbitrary sources (TVT) */
typedef uint_fast8_t (*InputFunc)(GifFileType *, GifByteType *, uint_fast8_t);

/******************************************************************************
 GIF89 structures
******************************************************************************/

typedef struct GraphicsControlBlock {
	uint_fast8_t DisposalMode;
#define DISPOSAL_UNSPECIFIED      0       /* No disposal specified. */
#define DISPOSE_DO_NOT            1       /* Leave image in place */
#define DISPOSE_BACKGROUND        2       /* Set area too background color */
#define DISPOSE_PREVIOUS          3       /* Restore to previous content */
//    bool UserInputFlag;      /* User confirmation required before disposal */
	uint_fast32_t DelayTime;
	/* pre-display delay in 0.01sec units */
	int TransparentColor;    /* Palette index for transparency, -1 if none */
#define NO_TRANSPARENT_COLOR    -1
} GraphicsControlBlock;

/******************************************************************************
 GIF decoding routines
******************************************************************************/

/* Main entry points */
GifFileType *DGifOpen(void *userPtr, InputFunc readFunc, int *Error);

/* new one (TVT) */
int DGifCloseFile(GifFileType *GifFile);

#define D_GIF_ERR_OPEN_FAILED    101    /* And DGif possible errors. */
#define D_GIF_ERR_READ_FAILED    102
#define D_GIF_ERR_NOT_GIF_FILE   103
#define D_GIF_ERR_NO_SCRN_DSCR   104
#define D_GIF_ERR_NO_IMAG_DSCR   105
#define D_GIF_ERR_NO_COLOR_MAP   106
#define D_GIF_ERR_WRONG_RECORD   107
#define D_GIF_ERR_DATA_TOO_BIG   108
#define D_GIF_ERR_NOT_ENOUGH_MEM 109
#define D_GIF_ERR_CLOSE_FAILED   110
#define D_GIF_ERR_NOT_READABLE   111
#define D_GIF_ERR_IMAGE_DEFECT   112
#define D_GIF_ERR_EOF_TOO_SOON   113

#define E_GIF_SUCCEEDED          0
#define E_GIF_ERR_OPEN_FAILED    1    /* And EGif possible errors. */
#define E_GIF_ERR_WRITE_FAILED   2
#define E_GIF_ERR_HAS_SCRN_DSCR  3
#define E_GIF_ERR_HAS_IMAG_DSCR  4
#define E_GIF_ERR_NO_COLOR_MAP   5
#define E_GIF_ERR_DATA_TOO_BIG   6
#define E_GIF_ERR_NOT_ENOUGH_MEM 7
#define E_GIF_ERR_DISK_IS_FULL   8
#define E_GIF_ERR_CLOSE_FAILED   9
#define E_GIF_ERR_NOT_WRITEABLE  10

/* These are legacy.  You probably do not want to call them directly */
int DGifGetScreenDesc(GifFileType *GifFile);

int DGifGetRecordType(GifFileType *GifFile, GifRecordType *GifType);

int DGifGetImageDesc(GifFileType *GifFile, bool changeImageCount);

int DGifGetLine(GifFileType *GifFile, GifPixelType *GifLine, uint_fast32_t GifLineLen);

int DGifGetExtension(GifFileType *GifFile, int *GifExtCode,
                     GifByteType **GifExtension);

int DGifGetExtensionNext(GifFileType *GifFile, GifByteType **GifExtension);

int DGifGetCodeNext(GifFileType *GifFile, GifByteType **GifCodeBlock);

int EGifCompressLine(GifFilePrivateType *Private, unsigned char *Line, const int LineLen);
/*****************************************************************************
 Everything below this point is new after version 1.2, supporting `slurp
 mode' for doing I/O in two big belts with all the image-bashing in core.
******************************************************************************/

/******************************************************************************
 Color map handling from gif_alloc.c
******************************************************************************/

extern ColorMapObject *GifMakeMapObject(uint_fast8_t BitsPerPixel,
                                        const GifColorType *ColorMap);

extern void GifFreeMapObject(ColorMapObject *Object);

//extern int GifBitSize(int n);
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

/******************************************************************************
 Support for the in-core structures allocation (slurp mode).              
******************************************************************************/

//extern void GifFreeExtensions(int *ExtensionBlock_Count,
//			      ExtensionBlock **ExtensionBlocks);
extern void GifFreeSavedImages(GifFileType *GifFile);

/******************************************************************************
 5.x functions for GIF89 graphics control blocks
******************************************************************************/

int DGifExtensionToGCB(const size_t GifExtensionLength,
                       const GifByteType *GifExtension,
                       GraphicsControlBlock *GCB);

#endif /* _GIF_LIB_H */

/* end */
