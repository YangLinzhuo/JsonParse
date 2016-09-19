#ifndef _LEPT_JSON_H__
#define _LEPT_JSON_H__

// value type
typedef enum
{
    LEPT_NULL,
    LEPT_FALSE,
    LEPT_TRUE,
    LEPT_NUMBER,
    LEPT_STRING,
    LEPT_ARRAY,
    LEPT_OBJECT
} lept_type;


// node value
typedef struct 
{
    double n;       //number
    lept_type type; //true/false/null
} lept_value;


// result that parse func returns
enum
{
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR
};

typedef struct 
{
	const char* json;
} lept_context;

int       lept_parse(lept_value* v, const char* json); //parse the JSON
lept_type lept_get_type(const lept_value* v);    //get the json type
double    lept_get_number(const lept_value* v);     //get the json value

#endif