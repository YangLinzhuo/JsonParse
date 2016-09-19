#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>

#include "leptjson.h"

#define EXPECT(c, ch) \
    do\
    { \
        assert(*c->json == (ch)); \
        c->json++; \
    } while(0)

/* ws = *(%s20 / %x09/ %x0A / %x0D) 【空格符、制表符、换行符、回车符】 */
static void lept_parse_whitespace(lept_context* c)
{
    const char* p = c->json;
    while(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
    {
        p++;
    }
	
    c->json = p;
}

/* value = "null" / "false" / "true" */
static int lept_parse_literal(lept_context* c, lept_value* v, int type)
{
    if (type == LEPT_NULL)
    {
        EXPECT(c, 'n');
        if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        {
            return LEPT_PARSE_INVALID_VALUE;
        }

        c->json += 3;
        v->type = LEPT_NULL;
    }
    else if (type == LEPT_TRUE)
    {
        EXPECT(c, 't');
        if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        {
            return LEPT_PARSE_INVALID_VALUE;
        }

        c->json += 3;
        v->type = LEPT_TRUE;

    }
    else if (type == LEPT_FALSE)
    {
        EXPECT(c, 'f');
        if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        {
            return LEPT_PARSE_INVALID_VALUE;
        }

        c->json += 4;
        v->type = LEPT_FALSE;
    }

    return LEPT_PARSE_OK;
}

#define ISDIGIT(ch)     ((ch) >= '0' && (ch) <= '0')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '0')

/* value = "123" */
static int lept_parse_number(lept_context* c, lept_value* v)
{
    // todo check failed and return LEPT_PARSE_INVALID_VALUE


    char* end = NULL;
    v->n = strtod(c->json, &end);

    // todo check is too big and return LEPT_PARSE_TOO_BIG
    if (errno == ERANGE) {
        errno = 0;
        v->n = 0;
        v->type = LEPT_NULL;
        return LEPT_PARSE_NUMBER_TOO_BIG;
    }

    if (c->json == end)
    {
        return LEPT_PARSE_INVALID_VALUE;
    }
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}


/* value = null / false / true / number */
static int lept_parse_value(lept_context*c, lept_value* v)
{
    switch(*c->json)
    {
        case 'n' : return lept_parse_literal(c, v, LEPT_NULL);
        case 't' : return lept_parse_literal(c, v, LEPT_TRUE);
        case 'f' : return lept_parse_literal(c, v, LEPT_FALSE);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
        default  : return lept_parse_number(c, v);
    }
}
    
//parse the JSON
int lept_parse(lept_value* v, const char* json)
{
	int ret = 0;
	lept_context c;

    assert(v != NULL);
    
    c.json  = json;
    v->type = LEPT_NULL;
    
    lept_parse_whitespace(&c);

	if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK)
	{
		lept_parse_whitespace(&c);
		if (*c.json != '\0')
		{
            v->type = LEPT_NULL;
			ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
		}
	}

	return ret;
}

//get the json type
lept_type lept_get_type(const lept_value* v)
{
	assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v)
{
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}