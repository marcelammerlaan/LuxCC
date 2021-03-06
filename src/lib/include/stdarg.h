#ifndef _STDARG_H
#define _STDARG_H

#define _INTSIZEOF(n)  ((sizeof(n)+sizeof(int)-1) & ~(sizeof(int)-1))

#ifndef __x86_64__
typedef char *va_list;
#endif

#ifdef __i386__
#define va_start(ap, last)  (ap = (va_list)&last + _INTSIZEOF(last))
#define va_arg(ap, type)    (*(type *)((ap += _INTSIZEOF(type)) - _INTSIZEOF(type)))
#define va_copy(dest, src)  (dest) = (src)
#define va_end(ap)
#endif

#if defined __mips__ || defined __arm__
#define va_start(ap, last)  (ap = (va_list)&last + _INTSIZEOF(last))
#define va_arg(ap, type)    (*(type *)(ap = (va_list)(((unsigned)ap+__alignof__(type)-1)&~(__alignof__(type)-1)),\
                                       (ap += _INTSIZEOF(type)) - _INTSIZEOF(type)))
#define va_copy(dest, src)  (dest) = (src)
#define va_end(ap)
#endif

#ifdef __x86_64__
/*
 * Reference: AMD64-ABI, section 3.5.7.
 */

#define _LONGSIZE(n) ((n+sizeof(long)-1) & ~(sizeof(long)-1))

typedef struct {
    unsigned int gp_offset;
    unsigned int fp_offset;
    void *overflow_arg_area;
    void *reg_save_area;
} va_list[1];

static void *__va_arg(va_list ap, unsigned long siz)
{
    void *p;
    int num_gp;

    num_gp = (int)(_LONGSIZE(siz)/8);

    if (siz>16 || ap->gp_offset>48-num_gp*8) {
        p = ap->overflow_arg_area;
        ap->overflow_arg_area = (char *)ap->overflow_arg_area+_LONGSIZE(siz);
    } else {
        p = (char *)ap->reg_save_area+ap->gp_offset;
        ap->gp_offset += num_gp*8;
    }
    return p;
}

#define va_start(ap, last)  __builtin_va_start(ap)
#define va_arg(ap, type)    (*(type *)__va_arg(ap, sizeof(type)))
#define va_copy(dest, src)  (*(dest)) = (*(src))
#define va_end(ap)

#endif


#ifdef __GNUC__

/*
 * gcc is compiling this, but we are including our own headers.
 * This happens when using gcc to generate pic code for shared libraries.
 * Use gcc's intrinsics instead of our own definitions.
 */

#define va_list __builtin_va_list

#undef va_start
#undef va_arg
#undef va_end
#undef va_copy

#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)

#endif


#endif
