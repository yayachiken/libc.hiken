#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <libchiken/console.h>

//
// FLAGS
//
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

#define is_flag(x) \
    ((x) == '#' || (x) == '0' || (x) == '-' || (x) == ' ' || (x) == '+')

#define is_digit(x) \
    ((x) >= '0' && (x) <= '9')

int parse_decimal_string(const char *str, const char **end)
{
    int ret = 0;

    // Look for end of decimal string
    while(is_digit(*str))
    {
        str++;
    }

    // Back up one character and start conversion
    str--;
    for(int i=0; is_digit(*(str-i)); i++)
    {
        int power = 1;
        for(int j=0; j<i; j++)
        {
            power *= 10; 
        }
        ret += (*(str-i)-'0') * power;
    }
    *end = str+1;

    return ret;
}

int process_int_argument(uintmax_t arg, char conversion_specifier,
        PrintfFlags flags, int field_width, int precision)
{
    // TODO: Field width, precision
    size_t length = 0, prefix_length = 0;
    int base = 10;
    int chars_printed = 0;

    // A bit hackish: Allows to process signed and unsigned types in a single
    // function
    int negative = 0;
    if((conversion_specifier == 'd' || conversion_specifier == 'i')
            && arg > INTMAX_MAX)
    {
        negative = 1;
        arg = (~arg)+1;
    }

    //
    // Determine which base to use for output based on the conversion specifier
    //
    switch(conversion_specifier)
    {
        case 'o':
            base = 8;
            break;
        case 'x':
        case 'X':
            base = 16;
            break;
        case 'u':
        default:
            base = 10;
    }

    //
    // Determine the length of the number
    //
    intmax_t value = arg;
    while(value != 0)
    {
        value /= base;
        length++;    
    }

    // The sign needs a character, too
    if(flags & FLAG_ALWAYS_SIGNED || flags & FLAG_POSITIVE_BLANK || negative)
    {
        prefix_length += 1;
    }

    // Any prefixes need some as well
    if(flags & FLAG_ALTERNATE_FORM)
    {
        if(conversion_specifier == 'o')
        {
            prefix_length += 1;
        }
        else if(conversion_specifier == 'x' || conversion_specifier == 'X')
        {
            prefix_length +=2;
        }
    }

    //
    // Construct the string (prefix + actual number + NUL)
    //
    char number_string[prefix_length + length + 1];
    size_t position = 0;

    // Sign
    if(negative)
    {
        number_string[position++] = '-';
    }
    else if(flags & FLAG_ALWAYS_SIGNED)
    {
        number_string[position++] = '+';
    }
    else if(flags & FLAG_POSITIVE_BLANK)
    {
        number_string[position++] = ' ';
    }

    // Hexadecimal/Octal Prefixes
    if(flags & FLAG_ALTERNATE_FORM && 
            (conversion_specifier == 'o' || conversion_specifier == 'x'
             || conversion_specifier == 'X'))
    {
        number_string[position++] = '0';
        if(conversion_specifier != 'o')
        {
            number_string[position++] = conversion_specifier;
        }
    }

    // Special case 0
    if(arg == 0)
    {
        number_string[position++] = '0';
    }

    // Write the number out
    position = prefix_length + length - 1;
    while(arg != 0)
    {
        int digit = arg % base;
        if(digit < 10)
        {
            number_string[position--] = digit + '0';
        }
        else
        {
            number_string[position--] = (digit - 10) + 'A';
        }
        arg /= base;
    }

    // Add NUL
    number_string[prefix_length + length] = '\0';

    //
    // Do padding and print
    //
    char padding_character = (flags & FLAG_ZERO_PADDING) ? '0' : ' ';

    if(! (flags & FLAG_LEFT_ADJUSTMENT))
    {
        for(int i=field_width; i > length+prefix_length; i--)
        {
            CONSOLE_WRITE(padding_character);
            chars_printed++;
        }
    }

    for(int i=0; i < length+prefix_length; i++)
    {
        CONSOLE_WRITE(number_string[i]);
        chars_printed++;
    }

    if(flags & FLAG_LEFT_ADJUSTMENT)
    {
        for(int i=field_width; i > length+prefix_length; i--)
        {
            CONSOLE_WRITE(padding_character);
            chars_printed++;
        }

    }
    return chars_printed;
}

int process_char_argument(unsigned char arg, LengthModifier lm)
{
    if(lm != 'l')
    {
        CONSOLE_WRITE(arg);
    }
    else
    {
        // Not implemented yet
    }
    return 1;
}

int process_string_argument(char *str, PrintfFlags flags, int field_width,
        LengthModifier lm, int precision)
{
    int chars_printed;
    if(lm != 'l')
    {
        // "Normal" right adjustment not implemented yet
        for(int i=0; str[i] != '\0' && (i < precision || precision == 0); i++)
        {
            chars_printed++;
            field_width--;
            CONSOLE_WRITE(str[i]);
        }
        if(flags & FLAG_LEFT_ADJUSTMENT)
        {
            for(;field_width > 0; field_width--)
            {
                CONSOLE_WRITE(' ');
            }
        }
    }
    else
    {
        // Not implemented yet
    }
    return chars_printed;
}

int process_pointer_argument(void *ptr)
{
    return process_int_argument((intmax_t)ptr, 'x', FLAG_ALTERNATE_FORM,
            sizeof(void*), 0);
}

int vprintf(const char *format, va_list args)
{
    int chars_printed = 0;

    for(const char *format_iter = format; *format_iter != '\0'; format_iter++)
    {
        if(*format_iter != '%')
        {
            // This is a verbatim character, print it
            chars_printed++;
            CONSOLE_WRITE(*format_iter);
        }
        else
        {
            format_iter++;

            //
            // Extract flags
            //
            PrintfFlags flags = 0;
            while(is_flag(*format_iter))
            {
                switch(*format_iter)
                {
                    case '#':
                        flags |= FLAG_ALTERNATE_FORM;
                        break;
                    case '0':
                        flags |= FLAG_ZERO_PADDING;
                        break;
                    case '-':
                        flags |= FLAG_LEFT_ADJUSTMENT;
                        break;
                    case ' ':
                        flags |= FLAG_POSITIVE_BLANK;
                        break;
                    case '+':
                        flags |= FLAG_ALWAYS_SIGNED;
                        break;
                    default:
                        return -1;
                }
                format_iter++;
            }

            //
            // Extract field width
            //
            int field_width = 0;

            // If a star (*) appears here, the field width is either given as
            // the m-th argument (if a decimal string plus a $ follows) or
            // otherwise simply the next argument.
            if(*format_iter == '*')
            {
                int arg_num = parse_decimal_string(format_iter, &format_iter);
                if(arg_num >= 0 && *format_iter == '$')
                {
                    // TODO: Not implemented
                }
                else
                {
                    field_width = va_arg(args, int);
                }
            }
            else
            {
                field_width = parse_decimal_string(format_iter, &format_iter);
                if(field_width <= 0)
                {
                    field_width = 0;
                }
            }

            //
            // Extract precision
            //
            int precision = 0;
            if(*format_iter == '.')
            {
                format_iter++;
                if(*format_iter == '*')
                {
                    int arg_num = parse_decimal_string(format_iter,
                            &format_iter);
                    if(arg_num >= 0 && *format_iter == '$')
                    {
                        // TODO: Not implemented
                    }
                    else
                    {
                        precision = va_arg(args, int);
                    }
                }
                else
                {
                    precision = parse_decimal_string(format_iter, &format_iter);
                    if(precision <= 0)
                    {
                        precision = 0;
                    }
                }

            }

            //
            // Extract length modifier
            //
            LengthModifier length_modifier;
            switch(*format_iter)
            {
                case 'h':
                    if(*(format_iter+1) == 'h')
                    {
                        length_modifier = LENGTH_CHAR;
                        format_iter++;
                    }
                    else
                    {
                        length_modifier = LENGTH_SHORT;
                    }
                    break;

                case 'l':
                    if(*(format_iter+1) == 'l')
                    {
                        length_modifier = LENGTH_LONGLONG;
                        format_iter++;
                    }
                    else
                    {
                        length_modifier = LENGTH_LONG;
                    }
                    break;

                case 'q':
                    length_modifier = LENGTH_LONGLONG;
                    break;

                case 'j':
                    length_modifier = LENGTH_INTMAX;
                    break;

                case 'z':
                    length_modifier = LENGTH_SIZE_T;
                    break;

                case 't':
                    length_modifier = LENGTH_PTRDIFF_T;
                    break;

                default:
                    format_iter--;
            }
            format_iter++;

            //
            // Do conversions
            //
            uintmax_t arg;

            switch(*format_iter)
            {
                case 'd':
                case 'i':
                    switch(length_modifier)
                    {
                        case LENGTH_CHAR:
                            arg = (uintmax_t) va_arg(args, int);
                            break;
                        case LENGTH_SHORT:
                            arg = (uintmax_t) va_arg(args, int);
                            break;
                        case LENGTH_LONG:
                            arg = (uintmax_t) va_arg(args, long);
                            break;
                        case LENGTH_LONGLONG:
                            arg = (uintmax_t) va_arg(args, long long);
                            break;
                        case LENGTH_INTMAX:
                            arg = (uintmax_t) va_arg(args, intmax_t);
                            break;
                        case LENGTH_SIZE_T:
                            arg = (uintmax_t) va_arg(args, size_t);
                            break;
                        case LENGTH_PTRDIFF_T:
                            arg = (uintmax_t) va_arg(args, ptrdiff_t);
                            break;
                        default:
                            arg = (uintmax_t) va_arg(args, int);
                            break;
                    }
                    process_int_argument(arg, *format_iter, flags, field_width,
                            precision);
                    break;

                case 'o':
                case 'u':
                case 'x':
                case 'X':
                    switch(length_modifier)
                    {
                        case LENGTH_CHAR:
                            arg = va_arg(args, unsigned int);
                            break;
                        case LENGTH_SHORT:
                            arg = va_arg(args, unsigned int);
                            break;
                        case LENGTH_LONG:
                            arg = va_arg(args, unsigned long);
                            break;
                        case LENGTH_LONGLONG:
                            arg = va_arg(args, unsigned long long);
                            break;
                        case LENGTH_INTMAX:
                            arg = va_arg(args, uintmax_t);
                            break;
                        case LENGTH_SIZE_T:
                            arg = va_arg(args, size_t);
                            break;
                        case LENGTH_PTRDIFF_T:
                            arg = (uintmax_t) va_arg(args, ptrdiff_t);
                            break;
                        default:
                            arg = va_arg(args, unsigned int);
                            break;
                    }
                    process_int_argument(arg, *format_iter, flags, field_width,
                            precision);
                    break;

                case 'e':
                case 'E':
                case 'f':
                case 'F':
                case 'g':
                case 'G':
                case 'a':
                case 'A':
                    // Not implemented
                    break;
                case 'c':
                    arg = (uintmax_t) va_arg(args, int);
                    chars_printed += process_char_argument((unsigned char) arg,
                            length_modifier);
                    break;
                case 's':
                    arg = (uintmax_t) va_arg(args, char*);
                    chars_printed += process_string_argument((char*) arg,
                            flags, field_width, length_modifier, precision);
                    break;
                case 'p':
                    arg = (uintmax_t) va_arg(args, void*);
                    chars_printed += process_pointer_argument((void*) arg);
                    break;
                case 'n':
                    // not implemented
                    break;
                case 'm':
                    // not implemented
                    break;
                case '%':
                    chars_printed++;
                    CONSOLE_WRITE('%');
                    break;
                default:
                    return -1;
            }
        }
    }

    return chars_printed;
}


