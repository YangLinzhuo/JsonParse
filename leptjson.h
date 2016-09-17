#ifndef _LEPT_JSON_H__
#define _LEPT_JSON_H__

// value type
typedef enum{
    LEPT_NULL,
    LEPT_FALSE,
    LEPT_TRUE,
    LEPT_NUMBER,
    LEPT_STRING,
    LEPT_ARRAY,
    LEPT_OBJECT
} lept_type;


// node value
typedef struct {
    lept_type type;
} lept_value;


// result that parse func returns
enum{
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR
};

int lept_parse(lept_value* v, const char* json); //parse the JSON
lept_type lept_get_type(const lept_value* v);    //get the json type


#endif