#ifndef _UTF8_DECODE_H_
#define _UTF8_DECODE_H_

/* utf8_decode.h */

#define UTF8_END   -1
#define UTF8_ERROR -2

class utf8_decode
{
public:
	utf8_decode(char p[], int length);
	int  utf8_decode_next();
	void reset()
	{
		the_index = 0;
		the_char = 0;
		the_byte = 0;
	}
protected:
	int  utf8_decode_at_byte();
	int  utf8_decode_at_character();
	int	 cont();
	int  get();



	int  the_index;
	int  the_length;
	int  the_char;
	int  the_byte;
	char* the_input;
};

#endif // _UTF8_DECODE_H_
