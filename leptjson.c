#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */

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
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type)
{
    size_t i;
    EXPECT(c, literal[0]);

    for (i = 0; literal[i+1]; i++)
    {
        if (c->json[i] != literal[i + 1])
        {
            return LEPT_PARSE_INVALID_VALUE;
        }
    }
    c->json += i;
    v->type = type;

    return LEPT_PARSE_OK;
}

#define ISDIGIT(ch)     ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')
#define TRY_GETDIGITS(json) \
    while (ISDIGIT(*json))\
    {\
        json++;\
    }
#define GETDIGITS(json) \
    do\
    {\
        if (ISDIGIT(*json)) json++; \
        else return LEPT_PARSE_INVALID_VALUE; \
    }while (ISDIGIT(*json))

static int lept_chcek_number_myown(const lept_context* c)
{
    char* json = c->json;
    if (*json == '-')
    {
        json++;
    }

    if (ISDIGIT1TO9(*json))
    {
        json++;
        TRY_GETDIGITS(json);
    }
    else if (ISDIGIT(*json))
    {
        json++;
    }
    else
    {
        return LEPT_PARSE_INVALID_VALUE;
    }

    if (*json == '.')
    {
        json++;
        GETDIGITS(json);
    }

    if (*json == 'e' || *json == 'E')
    {
        json++;
        if (*json == '+' || *json == '-')
        {
            json++;
        }
        GETDIGITS(json);
    }
    else if (*json == 0)
    {
        return LEPT_PARSE_EXPECT_VALUE;
    }
    else
    {
        return LEPT_PARSE_INVALID_VALUE;
    }
}


/* value = "123" */
static int lept_parse_number(lept_context* c, lept_value* v)
{
    // todo check failed and return LEPT_PARSE_INVALID_VALUE
    const char* p = c->json;
    /* 负号 ... */
    if (*p == '-') p++;

    /* 整数 ... */
    if (*p == '0') p++;
    else
    {
        if (!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }

    /* 小数 ... */
    if (*p == '.')
    {
        p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }

    /* 指数 ... */
    if (*p == 'e' || *p == 'E')
    {
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }

    errno = 0;
    v->n = strtod(c->json, NULL);
    // todo check is too big and return LEPT_PARSE_NUMBER_TOO_BIG
    if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL)) //如果不判断HUGEVAL的话，测试案例中的1e-10000 会返回 LEPT_PARSE_NUMBER_TOO_BIG
    {
        v->n = 0;
        v->type = LEPT_NULL;
        return LEPT_PARSE_NUMBER_TOO_BIG;
    }

    c->json = p;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}


/* value = null / false / true / number */
static int lept_parse_value(lept_context*c, lept_value* v)
{
    switch(*c->json)
    {
        case 'n' : return lept_parse_literal(c, v, "null",  LEPT_NULL);
        case 't' : return lept_parse_literal(c, v, "true",  LEPT_TRUE);
        case 'f' : return lept_parse_literal(c, v, "false", LEPT_FALSE);
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
    assert(v != NULL);
    assert(v->type == LEPT_NUMBER);
    return v->n;
}