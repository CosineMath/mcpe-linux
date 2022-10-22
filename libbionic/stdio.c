
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <stdarg.h>
#include <stdlib.h>

// typedef unsigned long fpos_t;		 stdio file position type

#define	BIONIC__SLBF	0x0001		/* line buffered */
#define	BIONIC__SNBF	0x0002		/* unbuffered */
#define	BIONIC__SRD	0x0004		/* OK to read */
#define	BIONIC__SWR	0x0008		/* OK to write */
	/* RD and WR are never simultaneously asserted */
#define	BIONIC__SRW	0x0010		/* open for reading & writing */
#define	BIONIC__SEOF	0x0020		/* found EOF */
#define	BIONIC__SERR	0x0040		/* found error */
#define	BIONIC__SMBF	0x0080		/* _buf is from malloc */
#define	BIONIC__SAPP	0x0100		/* fdopen()ed in append mode */
#define	BIONIC__SSTR	0x0200		/* this is an sprintf/snprintf string */
#define	BIONIC__SOPT	0x0400		/* do fseek() optimization */
#define	BIONIC__SNPT	0x0800		/* do not do fseek() optimization */
#define	BIONIC__SOFF	0x1000		/* set iff _offset is in fact correct */
#define	BIONIC__SMOD	0x2000		/* true => fgetln modified _p text */
#define	BIONIC__SALC	0x4000		/* allocate string space dynamically */
#define	BIONIC__SIGN	0x8000		/* ignore this file in _fwalk */

/* stdio buffers */
struct __sbuf {
	unsigned char *_base;
	int	_size;
};

/*
 * stdio state variables.
 *
 * The following always hold:
 *
 *	if (_flags&(__SLBF|__SWR)) == (__SLBF|__SWR),
 *		_lbfsize is -_bf._size, else _lbfsize is 0
 *	if _flags&__SRD, _w is 0
 *	if _flags&__SWR, _r is 0
 *
 * This ensures that the getc and putc macros (or inline functions) never
 * try to write or read from a file that is in `read' or `write' mode.
 * (Moreover, they can, and do, automatically switch from read mode to
 * write mode, and back, on "r+" and "w+" files.)
 *
 * _lbfsize is used only to make the inline line-buffered output stream
 * code as compact as possible.
 *
 * _ub, _up, and _ur are used when ungetc() pushes back more characters
 * than fit in the current _bf, or when ungetc() pushes back a character
 * that does not match the previous one in _bf.  When this happens,
 * _ub._base becomes non-nil (i.e., a stream has ungetc() data iff
 * _ub._base!=NULL) and _up and _ur save the current values of _p and _r.
 *
 * NOTE: if you change this structure, you also need to update the
 * std() initializer in findfp.c.
 */
typedef	struct __sFILE {
	unsigned char *_p;	/* current position in (some) buffer */
	int	_r;		/* read space left for getc() */
	int	_w;		/* write space left for putc() */
	short	_flags;		/* flags, below; this FILE is free if 0 */
	short	_file;		/* fileno, if Unix descriptor, else -1 */
	struct	__sbuf _bf;	/* the buffer (at least 1 byte, if !NULL) */
	int	_lbfsize;	/* 0 or -_bf._size, for inline putc */

	/* operations */
	void	*_cookie;	/* cookie passed to io functions */
	int	(*_close)(void *);
	int	(*_read)(void *, char *, int);
	unsigned long	(*_seek)(void *, unsigned long, int);
	int	(*_write)(void *, const char *, int);

	/* extension data, to avoid further ABI breakage */
	struct	__sbuf _ext;
	/* data for long sequences of ungetc() */
	unsigned char *_up;	/* saved _p when _p is doing ungetc data */
	int	_ur;		/* saved _r when _r is counting ungetc data */

	/* tricks to meet minimum requirements even when malloc() fails */
	unsigned char _ubuf[3];	/* guarantee an ungetc() buffer */
	unsigned char _nbuf[1];	/* guarantee a getc() buffer */

	/* separate buffer for fgetln() when line crosses buffer boundary */
	struct	__sbuf _lb;	/* buffer for fgetln() */

	/* Unix stdio files get aligned to block boundaries on fseek() */
	int	_blksize;	/* stat.st_blksize (may be != _bf._size) */
	unsigned long	_offset;	/* current lseek offset */
} bionic_FILE;

bionic_FILE __sF[3];

static bionic_FILE* bionic_stdin = &__sF[0];
static bionic_FILE* bionic_stdout = &__sF[1];
static bionic_FILE* bionic_stderr = &__sF[2];

static FILE* bionic_get_fp(bionic_FILE* bionic_fp) {
    if (bionic_fp == bionic_stdin) {
        bionic_fp->_cookie = stdin;
    }
    else if (bionic_fp == bionic_stdout) {
        bionic_fp->_cookie = stdout;
    }
    else if (bionic_fp == bionic_stderr) {
        bionic_fp->_cookie = stderr;
    }
    return (FILE*)(bionic_fp->_cookie);
}

static FILE* bionic_set_fp(bionic_FILE* bionic_fp, FILE* fp) {
    bionic_fp->_cookie = fp;
    return fp;
}

static bionic_FILE* bionic_wrap_fp(FILE* fp) {
    if (fp == NULL) {
        return NULL;
    }
    bionic_FILE* retval = malloc(sizeof(bionic_FILE));
    memset(retval, 0, sizeof(*retval));
    bionic_set_fp(retval, fp);
    retval->_file = fileno(fp);
    return retval;
}

static void bionic_update_fp_flags(bionic_FILE* bionic_fp) {
    if (feof(bionic_get_fp(bionic_fp))) {
        bionic_fp->_flags |= BIONIC__SEOF;
    }
    else {
        bionic_fp->_flags &= ~BIONIC__SEOF;
    }
}

bionic_FILE* bionic_fopen(const char* pathname, const char* mode) {
    return bionic_wrap_fp(fopen(pathname, mode));
}

bionic_FILE* bionic_fdopen(int fd, const char* mode) {
    return bionic_wrap_fp(fdopen(fd, mode));
}

int bionic_fclose(bionic_FILE *stream) {
    int ret = fclose(bionic_get_fp(stream));
    if (stream == bionic_stdin ||
        stream == bionic_stdout ||
        stream == bionic_stderr) {
        return ret;
    }
    free(stream);
    return ret;
}

int bionic_fflush(bionic_FILE* stream) {
    return fflush(bionic_get_fp(stream));
}

int bionic_fread(void* ptr, size_t size, size_t nmemb, bionic_FILE* stream) {
    int ret = fread(ptr, size, nmemb, bionic_get_fp(stream));
    bionic_update_fp_flags(stream);
    return ret;
}

int bionic_fwrite(const void* ptr, size_t size, size_t nmemb, bionic_FILE* stream) {
    return fwrite(ptr, size, nmemb, bionic_get_fp(stream));
}

int bionic_fputs(const char* s, bionic_FILE* stream) {
    return fputs(s, bionic_get_fp(stream));
}

char *bionic_fgets(char *s, int size, bionic_FILE *stream) {
    char* ret = fgets(s, size, bionic_get_fp(stream));
    bionic_update_fp_flags(stream);
    return ret;
}

int bionic_fprintf(bionic_FILE *stream, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vfprintf(bionic_get_fp(stream), format, ap);
    va_end(ap);
    return ret;
}

int bionic_fscanf(bionic_FILE *stream, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vfscanf(bionic_get_fp(stream), format, ap);
    va_end(ap);
    bionic_update_fp_flags(stream);
    return ret;
}

int bionic_fgetc(bionic_FILE *stream) {
    return fgetc(bionic_get_fp(stream));
}

int bionic_fputc(int c, bionic_FILE *stream) {
    return fputc(c, bionic_get_fp(stream));
}

int bionic_fseek(bionic_FILE *stream, long offset, int whence) {
    return fseek(bionic_get_fp(stream), offset, whence);
}

int bionic_fseeko(bionic_FILE *stream, off_t offset, int whence) {
    return fseeko(bionic_get_fp(stream), offset, whence);
}

long bionic_ftell(bionic_FILE *stream) {
    return ftell(bionic_get_fp(stream));
}

off_t bionic_ftello(bionic_FILE *stream) {
    return ftello(bionic_get_fp(stream));
}

int bionic_feof(bionic_FILE *stream) {
    return feof(bionic_get_fp(stream));
}

int bionic_vfprintf(bionic_FILE *stream, const char *format, va_list ap) {
    return vfprintf(bionic_get_fp(stream), format, ap);
}

int bionic_vfscanf(bionic_FILE *stream, const char *format, va_list ap) {
    int ret = vfscanf(bionic_get_fp(stream), format, ap);
    bionic_update_fp_flags(stream);
    return ret;
}

void bionic_rewind(bionic_FILE *stream) {
    return rewind(bionic_get_fp(stream));
}

int bionic_setvbuf(bionic_FILE *stream, char *buf, int mode, size_t size) {
    return setvbuf(bionic_get_fp(stream), buf, mode, size);
}

int bionic_getc(bionic_FILE *stream) {
    return getc(bionic_get_fp(stream));
}

int bionic_ungetc(int c, bionic_FILE *stream) {
    return ungetc(c, bionic_get_fp(stream));
}

int bionic_putc(int c, bionic_FILE *stream) {
    return putc(c, bionic_get_fp(stream));
}

wint_t bionic_getwc(bionic_FILE *stream) {
    return getwc(bionic_get_fp(stream));
}

wint_t bionic_ungetwc(wint_t wc, bionic_FILE *stream) {
    return ungetwc(wc, bionic_get_fp(stream));
}

wint_t bionic_putwc(wchar_t wc, bionic_FILE *stream) {
    return putwc(wc, bionic_get_fp(stream));
}


