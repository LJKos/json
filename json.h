#ifndef JSON_H
#define JSON_H

typedef enum { NON, DOCUMENT, ARRAY, OBJECT, NUMBER, STRING, BOOLEAN } JSONType;
//const char json_value[] = { "null", "true", "false" }; // should be private -> c file

struct json_pair {
    char *key;
    JSONType value_type;
    void *value;
};

typedef struct json_object {
    struct json_pair **pair;
    int pair_c;
    JSONType owner_type;    // Might not be needed.
    void *owner;
} JSONObject;

typedef struct json_array {
    JSONType type;
    void **value;
    int value_c;
    JSONType owner_type;    // Might not be needed.
    void *owner;
} JSONArray;

typedef struct json_document {
    JSONType content_type;
    void *content;
} JSONDocument;



JSONDocument *json_read_file(const char *file);
int json_write_file(JSONDocument *jd, const char *file);

JSONType json_value_type(JSONObject *jo, const char *key);
void *json_get_value(JSONObject *jo, const char *key);

JSONType json_array_value_type(JSONArray *ja);
int json_get_array_length(JSONArray *ja);

#endif
