#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include <printf_common.h>

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

void process_int_argument(uintmax_t arg, PrintfInfo info)
{
    // TODO: Field width, precision
    size_t length = 0, prefix_length = 0;
    int base = 10;

    // A bit hackish: Allows to process signed and unsigned types in a single
    // function
    int negative = 0;
    if((info.conversion_specifier == 'd' || info.conversion_specifier == 'i')
            && arg > INTMAX_MAX)
    {
        negative = 1;
        arg = (~arg)+1;
    }

    //
    // Determine which base to use for output based on the conversion specifier
    //
    switch(info.conversion_specifier)
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
    if(info.flags & FLAG_ALWAYS_SIGNED || info.flags & FLAG_POSITIVE_BLANK
            || negative)
    {
        prefix_length += 1;
    }

    // Any prefixes need some as well
    if(info.flags & FLAG_ALTERNATE_FORM)
    {
        if(info.conversion_specifier == 'o')
        {
            prefix_length += 1;
        }
        else if(info.conversion_specifier == 'x'
                || info.conversion_specifier == 'X')
        {
            prefix_length +=2;
        }
    }

    //
    // Construct the string (prefix + actual number + NUL)
    //

    // Sign
    if(negative)
    {
        info.output(info.op, info.chars_printed, info.size, '-');
    }
    else if(info.flags & FLAG_ALWAYS_SIGNED)
    {
        info.output(info.op, info.chars_printed, info.size, '+');
    }
    else if(info.flags & FLAG_POSITIVE_BLANK)
    {
        info.output(info.op, info.chars_printed, info.size, ' ');
    }

    // Hexadecimal/Octal Prefixes
    if(info.flags & FLAG_ALTERNATE_FORM && 
            (info.conversion_specifier == 'o'
             || info.conversion_specifier == 'x'
             || info.conversion_specifier == 'X'))
    {
        info.output(info.op, info.chars_printed, info.size, '0');
        if(info.conversion_specifier != 'o')
        {
            info.output(info.op, info.chars_printed, info.size,
                    info.conversion_specifier);
        }
    }

    // Special case 0
    if(arg == 0)
    {
        info.output(info.op, info.chars_printed, info.size, '0');
    }

    // Write the number out
    char number_string[prefix_length + length + 1];
    size_t position = prefix_length + length - 1;
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
    char padding_character = (info.flags & FLAG_ZERO_PADDING) ? '0' : ' ';

    if(! (info.flags & FLAG_LEFT_ADJUSTMENT))
    {
        for(int i=info.field_width; i > length+prefix_length; i--)
        {
            info.output(info.op, info.chars_printed, info.size,
                    padding_character);
        }
    }

    for(int i=0; i < length+prefix_length; i++)
    {
        info.output(info.op, info.chars_printed, info.size,
                number_string[i]);
    }

    if(info.flags & FLAG_LEFT_ADJUSTMENT)
    {
        for(int i=info.field_width; i > length+prefix_length; i--)
        {
            info.output(info.op, info.chars_printed, info.size,
                    padding_character);
        }

    }
}

void process_char_argument(unsigned char arg, PrintfInfo info)
{
    if(info.lm != 'l')
    {
       info.output(info.op, info.chars_printed, info.size, arg); 
    }
    else
    {
        // Not implemented yet
    }
}

void process_string_argument(char *str, PrintfInfo info)
{
    if(info.lm != 'l')
    {
        // "Normal" right adjustment not implemented yet
        for(int i=0;
            str[i] != '\0' && (i < info.precision || info.precision == 0);
            i++)
        {
            info.field_width--;
            info.output(info.op, info.chars_printed, info.size, str[i]);
        }
        if(info.flags & FLAG_LEFT_ADJUSTMENT)
        {
            for(;info.field_width > 0; info.field_width--)
            {
                info.output(info.op, info.chars_printed, info.size, ' ');
            }
        }
    }
    else
    {
        // Not implemented yet
    }
}

void process_pointer_argument(void *ptr, PrintfInfo info)
{
    info.conversion_specifier = 'x';
    info.flags = FLAG_ALTERNATE_FORM;
    info.precision = sizeof(void*);

    process_int_argument((intmax_t)ptr, info);
}

int printf_common(Output op, const char *format, va_list args,
        void (*output)(Output, int*, size_t, char), size_t size)
{
    int chars_printed = 0;

    PrintfInfo info;
    info.size = size;
    info.op = op;
    info.output = output;
    info.chars_printed = &chars_printed;

    for(const char *format_iter = format; *format_iter != '\0'; format_iter++)
    {
        if(*format_iter != '%')
        {
            // This is a verbatim character, print it
            info.output(op, &chars_printed, size, *format_iter);
        }
        else
        {
            format_iter++;

            //
            // Extract flags
            //
            info.flags = 0;
            while(is_flag(*format_iter))
            {
                switch(*format_iter)
                {
                    case '#':
                        info.flags |= FLAG_ALTERNATE_FORM;
                        break;
                    case '0':
                        info.flags |= FLAG_ZERO_PADDING;
                        break;
                    case '-':
                        info.flags |= FLAG_LEFT_ADJUSTMENT;
                        break;
                    case ' ':
                        info.flags |= FLAG_POSITIVE_BLANK;
                        break;
                    case '+':
                        info.flags |= FLAG_ALWAYS_SIGNED;
                        break;
                    default:
                        return -1;
                }
                format_iter++;
            }

            //
            // Extract field width
            //
            info.field_width = 0;

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
                    info.field_width = va_arg(args, int);
                }
            }
            else
            {
                info.field_width =
                    parse_decimal_string(format_iter, &format_iter);
                if(info.field_width <= 0)
                {
                    info.field_width = 0;
                }
            }

            //
            // Extract precision
            //
            info.precision = 0;
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
                        info.precision = va_arg(args, int);
                    }
                }
                else
                {
                    info.precision = 
                        parse_decimal_string(format_iter, &format_iter);
                    if(info.precision <= 0)
                    {
                        info.precision = 0;
                    }
                }

            }

            //
            // Extract length modifier
            //
            switch(*format_iter)
            {
                case 'h':
                    if(*(format_iter+1) == 'h')
                    {
                        info.lm = LENGTH_CHAR;
                        format_iter++;
                    }
                    else
                    {
                        info.lm = LENGTH_SHORT;
                    }
                    break;

                case 'l':
                    if(*(format_iter+1) == 'l')
                    {
                        info.lm = LENGTH_LONGLONG;
                        format_iter++;
                    }
                    else
                    {
                        info.lm = LENGTH_LONG;
                    }
                    break;

                case 'q':
                    info.lm = LENGTH_LONGLONG;
                    break;

                case 'j':
                    info.lm= LENGTH_INTMAX;
                    break;

                case 'z':
                    info.lm = LENGTH_SIZE_T;
                    break;

                case 't':
                    info.lm = LENGTH_PTRDIFF_T;
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
                    switch(info.lm)
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
                    info.conversion_specifier = *format_iter;
                    process_int_argument(arg, info);
                    break;

                case 'o':
                case 'u':
                case 'x':
                case 'X':
                    switch(info.lm)
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
                    info.conversion_specifier = *format_iter;
                    process_int_argument(arg, info);
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
                    info.conversion_specifier = 'c';
                    process_char_argument((unsigned char) arg, info);
                    break;
                case 's':
                    arg = (uintmax_t) va_arg(args, char*);
                    info.conversion_specifier = 's';
                    process_string_argument((char*) arg, info);
                    break;
                case 'p':
                    arg = (uintmax_t) va_arg(args, void*);
                    info.conversion_specifier = 'p';
                    process_pointer_argument((void*) arg, info);
                    break;
                case 'n':
                    // not implemented
                    break;
                case 'm':
                    // not implemented
                    break;
                case '%':
                    info.output(op, &chars_printed, size, '%');
                    break;
                default:
                    return -1;
            }
        }
    }

    // NULL termination
    info.output(op, &chars_printed, size, '\0');

    return chars_printed;
}


