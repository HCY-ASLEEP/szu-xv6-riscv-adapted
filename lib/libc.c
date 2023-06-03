#include "include/assert.h"
#include "include/ctype.h"
#include "include/limits.h"
#include <stdarg.h>
#include "include/stdbool.h"
#include "include/stddef.h"
#include "include/stdio.h"
#include "include/stdlib.h"
#include "include/string.h"
#include "include/unistd.h"


#define max(m, n)   ((m) > (n) ? (m) : (n))
#define min(m, n)   ((m) < (n) ? (m) : (n))


/* assert.h */

void __assert_fail(const char *expr, const char *file, int line)
{
  printf("%s:%d: Assertion `%s' failed.\n", file, line, expr);
  abort();
}


/* ctype.h */

int isalnum(int c) { return isalpha(c) || isdigit(c); }
int isalpha(int c) { return islower(c) || isupper(c); }
int isdigit(int c) { return '0' <= c && c <= '9'; }
int islower(int c) { return 'a' <= c && c <= 'z'; }
int isspace(int c) { return c == ' ' || ('\t' <= c && c <= '\r'); }
int isupper(int c) { return 'A' <= c && c <= 'Z'; }
int tolower(int c) { return isupper(c) ? c - 'A' + 'a' : c; }
int toupper(int c) { return islower(c) ? c - 'a' + 'A' : c; }


/* stdio.h */

FILE _iob[OPEN_MAX] = {     /* stdin, stdout, stderr */
  { 0, (char *) 0, (char *) 0, _READ, 0 },
  { 0, (char *) 0, (char *) 0, _WRITE | _LNBUF, 1 },
  { 0, (char *) 0, (char *) 0, _WRITE | _UNBUF, 2 }
};


int _fillbuf(FILE *fp) {
  int bufsize;

  if ((fp->flag & (_READ | _EOF | _ERR)) != _READ)
    return EOF;

  bufsize = (fp->flag & _UNBUF) ? 1: BUFSIZ;

  if  (fp->base == NULL)  /* no buffer yet */
    if ((fp->base = (char *) malloc(bufsize)) == NULL)
      return EOF;         /* can't get buffer */

  fp->ptr = fp->base;
  fp->cnt = read(fp->fd, fp->ptr, bufsize);

  if (--fp->cnt < 0) {
    if (fp->cnt == -1)
      fp->flag |= _EOF;
    else
      fp->flag |= _ERR;
    fp->cnt = 0;
    return EOF;
  }

  return (unsigned char) *fp->ptr++;
}

int _flushbuf(int x, FILE *fp) {

  int num_written=0, bufsize=0;
  unsigned char uc = x;

  if ((fp->flag & (_WRITE|_EOF|_ERR)) != _WRITE)
    return EOF;
  if (fp->base == NULL && ((fp->flag & _UNBUF) == 0)) {
    /* no buffer yet */
    if ((fp->base = malloc(BUFSIZ)) == NULL) {
      /* couldn't allocate a buffer, so try unbuffered */
      fp->flag |= _UNBUF;
    } else {
      fp->ptr = fp->base;
      fp->cnt = BUFSIZ - 1;
    }
  }
  if (fp->flag & _UNBUF) {
    /* unbuffered write */
    fp->ptr = fp->base = NULL;
    fp->cnt = 0;
    if (x == EOF)
      return EOF;
    num_written = write(fp->fd, &uc, 1);
    bufsize = 1;
  } else {
    /* buffered write */
    assert(fp->ptr);
    if (x != EOF) {
      *fp->ptr++ = uc;
    }
    bufsize = (int)(fp->ptr - fp->base);
    while(bufsize - num_written > 0) {
      int t;
      t = write(fp->fd, fp->base + num_written, bufsize - num_written);
      num_written += t;
    }

    fp->ptr = fp->base;
    fp->cnt = BUFSIZ - 1;
  }

  if (num_written == bufsize) {
    return x;
  } else {
    fp->flag |= _ERR;
    return EOF;
  }
}

/* fflush */
int fflush(FILE *f)
{
  int retval;
  int i;

  retval = 0;
  if (f == NULL) {
    /* flush all output streams */
    for (i = 0; i < OPEN_MAX; i++) {
      if ((_iob[i].flag & _WRITE) && (fflush(&_iob[i]) == -1))
        retval = -1;
    }
  } else {
    if ((f->flag & _WRITE) == 0)
      return -1;
    _flushbuf(EOF, f);
    if (f->flag & _ERR)
      retval = -1;
  }
  return retval;
}


int fputc(int x, FILE *fp)
{
  if (--fp->cnt >= 0) {
    return *(fp)->ptr++ = x;
  } else {
    return _flushbuf(x, fp);
  }
}


int fgetc(FILE *fp)
{
  if (--fp->cnt >= 0) {
    return (unsigned char) *(fp)->ptr++;
  } else {
    return _fillbuf(fp);
  }

}

int fputs(char *s, FILE *stream)
{

  while(*s != '\0') {

    if (fputc(*s, stream) == EOF)
      return EOF;

    ++s;

  }

  return 1;

}


int puts(char *s)
{
  int i = 0;

  while(*s != '\0') {

    if (putchar(*s) == EOF)
      return EOF;

    ++i; ++s;

  }

  putchar('\n');

  return i + 1;
}



char *fgets(char *s, int size, FILE *stream)
{
  int c = -1;
  char *buf;
  fflush(NULL);
  buf = s;

  if (size == 0)
    return NULL;

  while (--size > 0 && (c = fgetc(stream)) != EOF) {

    if ((*buf++ = c) == '\n')
      break;

  }

  *buf = '\0';

  return (c == EOF && buf == s) ? NULL : s;

}

char *gets(char *s)
{
  int c;
  char *buf;
  fflush(NULL);
  buf = s;

  while ((c = getc(stdin)) != EOF && c != '\n') {

    *buf++ = c;

  }

  *buf = '\0';

  return (c == EOF && buf == s) ? NULL : s;

}


#define LEFT      (1 << 0)
#define ZEROPAD   (1 << 1)
#define SIGN      (1 << 2)
#define PLUS      (1 << 3)
#define SPACE     (1 << 4)
#define OCTPRE    (1 << 5)
#define HEXPRE    (1 << 6)
#define CAP       (1 << 7)
#define DOT       (1 << 8)
#define TRUNC     (1 << 9)

static int prints(FILE *fp, const char *str, int w, int prec, int flag)
{
  int i, len;

  if (! str)
    str = "(null)";

  len = strlen(str);;
  if (prec >= 0 && len > prec)
    len = prec;

  if (! (flag & LEFT))
    for (i = 0; i < w - len; ++i)
      putc(' ', fp);

  for (i = 0; i < len; ++i)
    putc(str[i], fp);

  if (flag & LEFT)
    for (i = 0; i < w - len; ++i)
      putc(' ', fp);

  return max(len, w);
}

static int printi(FILE *fp, int x, int base, int w, int prec, int flag)
{
  char buf[12], pbuf[2];
  int i, n, len = 0, plen = 0;
  unsigned u = x;

  if (prec >= 0)
    flag &= ~ZEROPAD;

  if (flag & SIGN) {
    if (x < 0) {
      u = -x;
      pbuf[plen++] = '-';
    } else if (flag & (PLUS | SPACE)) {
      pbuf[plen++] = flag & PLUS ? '+' : ' ';
    }
  }
  if (x && flag & HEXPRE) {
    pbuf[plen++] = '0';
    pbuf[plen++] = flag & CAP ? 'X' : 'x';
  }

  while (u > 0) {
    int t = u % base;
    buf[len++] = (t < 10) ? (t + '0') : (t - 10 + (flag & CAP ? 'A' : 'a'));
    u /= base;
  }
  if (x == 0 || flag & OCTPRE)
    buf[len++] = '0';

  n = max(len, prec) + plen;

  if (! (flag & (LEFT | ZEROPAD)))
    for (i = 0; i < w - n; ++i)
      putc(' ', fp);

  for (i = 0; i < plen; ++i)
    putc(pbuf[i], fp);

  if (flag & ZEROPAD)
    for (i = 0; i < w - n; ++i)
      putc('0', fp);

  for (i = 0; i < prec - len; ++i)
    putc('0', fp);

  for (i = len - 1; i >= 0; --i)
    putc(buf[i], fp);

  if (flag & LEFT)
    for (i = 0; i < w - n; ++i)
      putc(' ', fp);

  return max(w, n);
}

int printf(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  return vfprintf(stdout, fmt, ap);
}

int fprintf(FILE *fp, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  return vfprintf(fp, fmt, ap);
}


int vprintf(const char *fmt, va_list ap)
{
  return vfprintf(stdout, fmt, ap);
}

int vfprintf(FILE *fp, const char *fmt, va_list ap)
{
  int ret = 0;

  if (! (fp->flag & _WRITE))
    return -1;

  for (; *fmt; ++fmt) {
    if (*fmt == '%') {
      int flag = 0;
      int alt = 0;
      int w = 0;
      int prec = -1;
      const char *bak = fmt++;
      if (*fmt == '%') {
        ++ret;
        putc('%', fp);
        continue;
      }
      while (strchr("-+ #0", *fmt)) {
        switch (*(fmt++)) {
          case '-': flag |= LEFT; break;
          case '+': flag |= PLUS; break;
          case ' ': flag |= SPACE; break;
          case '#': alt = -1; break;
          case '0': flag |= ZEROPAD; break;
        }
      }
      if (flag & LEFT)
        flag &= ~ZEROPAD;
      if (flag & PLUS)
        flag &= ~SPACE;
      if (*fmt == '*') {
        ++fmt;
        w = va_arg(ap, int);
      } else while (isdigit(*fmt)) {
        w = w * 10 + *(fmt++) - '0';
      }
      if (*fmt == '.') {
        ++fmt;
        prec = 0;
        if (*fmt == '*') {
          ++fmt;
          prec = va_arg(ap, int);
        } else while (isdigit(*fmt)) {
          prec = prec * 10 + *(fmt++) - '0';
        }
      }
      while (strchr("hlL", *fmt))
        ++fmt;
      switch (*fmt) {
        case 'c':
          ++ret;
          putc(va_arg(ap, int), fp);
          break;
        case 's':
          ret += prints(fp, va_arg(ap, char*), w, prec, flag);
          break;
        case 'd':
        case 'i':
          ret += printi(fp, va_arg(ap, int), 10, w, prec, flag | SIGN);
          break;
        case 'o':
          ret += printi(fp, va_arg(ap, int),  8, w, prec, flag | (alt & OCTPRE));
          break;
        case 'x':
          ret += printi(fp, va_arg(ap, int), 16, w, prec, flag | (alt & HEXPRE));
          break;
        case 'X':
          ret += printi(fp, va_arg(ap, int), 16, w, prec, flag | (alt & HEXPRE) | CAP);
          break;
        case 'u':
          ret += printi(fp, va_arg(ap, int), 10, w, prec, flag);
          break;
        case 'p':
          ret += printi(fp, va_arg(ap, int), 16, w, prec, flag | HEXPRE);
          break;
        // case 'e':
        //   ret += printef(fp, va_arg(ap, double), w, prec, flag | (alt & DOT));
        //   break;
        // case 'E':
        //   ret += printef(fp, va_arg(ap, double), w, prec, flag | (alt & DOT) | CAP);
        //   break;
        // case 'f':
        //   ret += printff(fp, va_arg(ap, double), w, prec, flag | (alt & DOT));
        //   break;
        // case 'F':
        //   ret += printff(fp, va_arg(ap, double), w, prec, flag | (alt & DOT) | CAP);
        //   break;
        // case 'g':
        //   ret += printgf(fp, va_arg(ap, double), w, prec, flag | (~alt & TRUNC));
        //   break;
        // case 'G':
        //   ret += printgf(fp, va_arg(ap, double), w, prec, flag | (~alt & TRUNC) | CAP);
        //   break;
        case 'n':
          *va_arg(ap, int*) = ret;
          break;
        default:
          ret += fmt - bak;
          for (; bak < fmt; ++bak){
            putc(*bak, fp);
          }
          --fmt;
          break;
      }
    } else {
      ++ret;
      putc(*fmt, fp);
    }
  }

  return ret;
}

/* stdlib.h */

void abort(void)
{
  fprintf(stderr, "abort!\n");
  exit(1);
}


long strtol(const char *str, char **end, int base)
{
  long l = 0;

  if (base > 36) {
    if (end)
      *end = (char *)str;
    return 0;
  }

  if (base == 0) {
    if (*str == '0') {
      ++str;
      if (*str == 'x' || *str == 'X') {
        ++str;
        base = 16;
      } else {
        base = 8;
      }
    } else {
      base = 10;
    }
  }

  while (1) {
    char c = *str;
    long n = base;

    if (isdigit(c)) n = c - '0';
    if (islower(c)) n = c - 'a' + 10;
    if (isupper(c)) n = c - 'A' + 10;

    if (base <= n)
      break;

    ++str;
    l = l * base + n;
  }

  if (end)
    *end = (char *)str;
  return l;
}


#define NALLOC 1024

struct header {
  struct header *next;
  size_tt size;
};

typedef struct header Header;

static Header base;
static Header *freep;

void free(void *ap)
{
  Header *bp, *p;

  if (! ap)
    return;

  bp = (Header *)ap - 1;
  for (p = freep; !(p < bp && bp < p->next); p = p->next)
    if (p->next <= p && (p < bp || bp < p->next))
      break;

  if (bp + bp->size == p->next) {
    bp->size += p->next->size;
    bp->next = p->next->next;
  } else {
    bp->next = p->next;
  }

  if (p + p->size == bp) {
    p->size += bp->size;
    p->next = bp->next;
  } else {
    p->next = bp;
  }

  freep = p;
}

static Header *morecore(size_tt nunits)
{
  Header *p;

  nunits = (nunits + NALLOC - 1) / NALLOC * NALLOC;
  p = (Header *)sbrk(nunits * sizeof(Header));
  if (p == (Header *)-1)
    return NULL;
  p->size = nunits;
  free(p + 1);
  return freep;
}

void *malloc(size_tt nbytes)
{
  Header *p, *prevp;
  size_tt nunits;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  if (! (prevp = freep)) {  /* no free list yet */
    base.next = freep = prevp = &base;
    base.size = 0;
  }

  for (p = prevp->next; ; prevp = p, p = p->next) {
    if (p->size >= nunits) {  /* big enough */
      if (p->size == nunits) {  /* exactly */
        prevp->next = p->next;
      } else {  /* allocate tail end */
        p->size -= nunits;
        p += p->size;
        p->size = nunits;
      }
      freep = prevp;
      return (void *)(p + 1);
    }
    if (p == freep)
      if (! (p = morecore(nunits)))
        return NULL;
  }

}

static size_tt malloc_size(void *ap)
{
  Header *p;

  p = (Header *)ap - 1;
  return (p->size - 1) * sizeof(Header);
}

void *realloc(void *ptr, size_tt size)
{
  void *new;

  if (! ptr)
    return malloc(size);

  if (size <= malloc_size(ptr))
    return ptr;

  new = malloc(size);
  if (! new)
    return NULL;

  memcpy(new, ptr, malloc_size(ptr));
  free(ptr);

  return new;
}

void *calloc(size_tt n, size_tt size)
{
  char *ptr = malloc(n * size);

  if (! ptr)
    return NULL;

  memset(ptr, 0, n * size);
  return (void *) ptr;
}


int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}


/* string.h */

size_tt strlen(const char *str)
{
  size_tt len = 0;

  while (*str++) {
    ++len;
  }
  return len;
}

int strcmp(const char *l, const char *r)
{
  while (*l) {
    if (*l++ != *r++) {
      return l[-1] < r[-1] ? -1 : 1;
    }
  }
  return *r ? -1 : 0;
}

int strncmp(const char *l, const char *r, size_tt n)
{
  while (*l && 0 < n) {
    if (*l++ != *r++) {
      return l[-1] < r[-1] ? -1 : 1;
    }
    --n;
  }
  return 0;
}

char *strchr(const char *str, int c)
{
  do {
    if (*str == c) {
      return (char *)str;
    }
  } while (*str++);
  return NULL;
}

char *strcpy(char *dst, const char *src)
{
  char *d = dst;

  while (*d++ == *src++);
  return dst;
}

char *strncpy(char *dst, const char *src, size_tt n)
{
  char *d = dst;

  while (n-- > 0 && (*d++ = *src++));
  return dst;
}

void *memset(void *dst, int c, size_tt n)
{
  char *d = dst;

  while (n-- > 0) {
    *d++ = c;
  }
  return dst;
}

void *memcpy(void *dst, const void *src, size_tt n)
{
  const char *s = src;
  char *d = dst;

  while (n-- > 0) {
    *d++ = *s++;
  }
  return dst;
}

void*
memmove(void *dst, const void *src, size_tt n)
{
  const char *s;
  char *d;

  s = src;
  d = dst;
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
      *d++ = *s++;

  return dst;
}
