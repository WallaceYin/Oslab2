#include <ylib.h>

void *memset(void *arr, int val, size_t n)
{
	for (int i = 0; i < n; i++)
		((char *)arr)[i] = 0;
	return arr;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	for (int i = 0; i < n; i++)
		((char *)dest)[i] = ((char *)src)[i];
	return dest;
}

size_t strlen(const char *s)
{
	size_t i;
	for (i = 0; ;i++)
		if (s[i] == 0)
			break;
	return i;
}

char *strcpy(char *dest, const char *src)
{
	return (char *)memcpy(dest, src, strlen(src));
}

int strcmp(const char *str_1, const char *str_2)
{
	int i;
	for (i = 0; ; i++)
	{
		if (i == strlen(str_1))
			break;
		if (i == strlen(str_2))
			break;

		if (!( str_2[i] - str_1[i]))
			return str_2[i] - str_1[i];
	}
	return 0;
}

char *strcat(char *dest, const char *src)
{
	char *p;
	p = dest;
	while (*p)	p++;
	strcpy(p, src);
	return dest;
}

static char str_out[1024];
int printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	memset(str_out, 0, 1024);
	int n = vsprintf(str_out, fmt, ap);
	for (int i = 0; i < strlen(str_out); i++)
		_putc(str_out[i]);
	va_end(ap);
	return n;
}

//Format String: %[Flags][Width][.precision][Length][Type]
enum {NONE = 0x00, LEFT = 0x01, SIGN = 0x02, BLANK = 0x04, PREFIX = 0x08, ZERO = 0x10}; //Flags
enum {NON_TYPE = 0, CHAR = 1, DEX, DOUBLE, POINT, STRING, HEX, UNSIGNED}; //Types

static char buf_temp[64];
int vsprintf(char *buf, const char *fmt, va_list ap)
{
	int flags;
	int width;
	int ret = 0;

	//char buf_temp[64];
	char ch_temp;
	unsigned num_temp;
	char *str_temp = NULL;

	//-----------------------------------------------------------------
	int buf_len = 0;
	void add_char(char ch)
	{
		buf[buf_len] = ch;
		buf_len++;
	};
	void add_buf(char *str)
	{
		strcat(buf, str);
		buf_len+=strlen(str);
	};
	int c2dex(char c)
	{
		if ((c >= '0') && (c <= '9'))
			return (c - '0');
		return -1;
	};
	//------------------------------------------------------------------

	memset(buf_temp, 0, 64);

	char *p;
	for (p = (char *)fmt;;)
	{
		width = 0;
		flags = 0;
		if (*p == '\0')
			break;
		if (*p != '%')
		{
			add_char(*p);
			p++;
			continue;
		}

		ret ++;
		p++; //Skip '%'

		for (;;)
		{
			switch (*p)
			{
				case '-':
					flags |= LEFT; p++; continue;
				case '+':
					flags |= SIGN; p++; continue; //Not supported yet
				case ' ':
					flags |= BLANK; p++; continue;
				case '#':
					flags |= PREFIX; p++; continue; //Not supported yet
				case '0':
					flags |= ZERO; p++; continue;
			}
			break;
		} //Flags

		while (c2dex(*p) != -1)
		{
			width = width * 10 + c2dex(*p);
			p++;
		} //Width

		//Precision is not supported yet.
		//Length is not supported yet.
		
		int snum[16];//store the single num straced from int or unsigned
		int snum_len = 0; //the number of single num
		memset(snum, 0, 16);
		switch (*p)
		{
			case 'c':
				ch_temp = (char)va_arg(ap, int);
				if (width == 0) buf_temp[0] = ch_temp;
				else if (flags | LEFT) buf_temp[0] = ch_temp;
				else 
				{
					for (int i = 0; i < width; i++)
						buf_temp[i] = ' ';
					buf_temp[width - 1] = ch_temp;
				}
				add_buf(buf_temp);
				break;

			case 'u':
			case 'd': // negative numbers are not supported yet
				num_temp = va_arg(ap, unsigned);
				if (num_temp == 0)
				{
					snum[snum_len] = 0;
					snum_len ++;
				}
				while (num_temp > 0)
				{
					snum[snum_len] = num_temp % 10;
					num_temp /= 10;
					snum_len ++;
				}
				if (width > snum_len)
				{
					if (flags | LEFT)
					{
						for (int i = snum_len - 1; i >= 0; i--)
							add_char((char)(snum[i] + '0'));
						for (int i = snum_len; i < width; i++)
							add_char(' ');
					}
					else if (flags | ZERO)
					{
						for (int i = 0; i < width - snum_len; i++)
							add_char('0');
						for (int i = snum_len - 1; i >= 0; i--)
							add_char((char)(snum[i] + '0'));
					}
					else
					{
						for (int i = 0; i < width - snum_len; i++)
							add_char(' ');
						for (int i = snum_len - 1; i >= 0; i--)
							add_char((char)(snum[i] + '0'));
					}
				}
				else
					for (int i = snum_len - 1; i >= 0; i--)
						add_char((char)(snum[i] + '0'));
				break;

			case 'f': //Not supported yet
				break;
			case 'p': //Not supported yet
				break;

			case 's':
				str_temp = (char *)va_arg(ap, int);
				if (strlen(str_temp) >= width)
					add_buf(str_temp);
				else if (flags & LEFT)
				{
					add_buf(str_temp);
					for (int i = strlen(str_temp); i < width; i++)
						add_char(' ');
				}
				else
				{
					for (int i = strlen(str_temp); i < width; i++)
						add_char(' ');
					add_buf(str_temp);
				}
				break;

			case 'X':
			case 'x':
				num_temp = va_arg(ap, unsigned);
				if (num_temp == 0)
				{
					snum[snum_len] = 0;
					snum_len++;
				}
				while (num_temp > 0)
				{
					snum[snum_len] = num_temp % 16;
					num_temp /= 16;
					snum_len++;
				}
				if (width >= snum_len)
				{
					if (flags & LEFT)
					{
						for (int i = snum_len - 1; i >= 0; i--)
						{
							if (snum[i] > 9)
								add_char((char)(snum[i] - 10 + 'a'));
							else
								add_char((char)(snum[i] + '0'));
						}
						for (int i = snum_len; i < width; i++)
							add_char(' ');
					}
					else if (flags & ZERO)
					{
						for (int i = snum_len; i < width; i++)
							add_char('0');
						for (int i = snum_len - 1; i >= 0; i--)
						{
							if (snum[i] > 9)
								add_char((char)(snum[i] - 10 + 'a'));
							else
								add_char((char)(snum[i] + '0'));
						}
					}
				}
				else
					for (int i = snum_len - 1; i >= 0; i--)
					{
						if (snum[i] > 9)
							add_char((char)(snum[i] - 10 + 'a'));
						else
							add_char((char)(snum[i] + '0'));
					}
				break;

			default:
				break;
		} //Type
		
		p++;
	}	
	return ret; 
}

static unsigned int randnum = 1;
void srand(unsigned int seed)
{
	randnum = seed;
}

#define MAXRAND 32768
int rand(void)
{
	randnum = (randnum * 137422 + 417) % MAXRAND;
	return randnum;
}

