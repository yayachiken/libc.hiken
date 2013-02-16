#ifndef LIBCHIKEN_STDIO_COMMON_H
#define LIBCHIKEN_STDIO_COMMON_H

typedef enum {
    FLAG_ALTERNATE_FORM = 0x01,
    FLAG_ZERO_PADDING = 0x02,
    FLAG_LEFT_ADJUSTMENT = 0x04,
    FLAG_POSITIVE_BLANK = 0x08,
    FLAG_ALWAYS_SIGNED = 0x10,
    // TODO: Non C standard flags?
} PrintfFlags;

typedef enum {
    LENGTH_CHAR,
    LENGTH_SHORT,
    LENGTH_LONG,
    LENGTH_LONGLONG,
    LENGTH_INTMAX,
    LENGTH_SIZE_T,
    LENGTH_PTRDIFF_T,
} LengthModifier;

typedef union
{
    FILE *file;
    char *buffer;
} Output;

typedef enum
{
    OUTPUT_FILE,
    OUTPUT_BUFFER,
} OutputFlag;

// This struct is intended to pass the information gathered by printf_common()
// to the processing sub-functions
typedef struct
{
    PrintfFlags flags;
    int precision;
    int field_width;
    LengthModifier lm;
    char conversion_specifier;
    
    char *buf;
    size_t size;
    int *chars_printed;

    Output op;
    OutputFlag of;

    void (*output)(Output, int*, size_t, char);

} PrintfInfo;

#define is_flag(x) \
    ((x) == '#' || (x) == '0' || (x) == '-' || (x) == ' ' || (x) == '+')

#define is_digit(x) \
    ((x) >= '0' && (x) <= '9')

int printf_common(Output, const char*, va_list,
        void(*)(Output, int*, size_t, char), size_t);

#endif

