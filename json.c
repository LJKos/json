
#include "json.h"
#include "lib/ptrck.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



const char *json_value[] = { "null", "true", "false" };



// helpers
JSONType get_next_type(char *buffer, int *i) {
    JSONType type = NON;
    
    int j = *i;
   while (isspace(buffer[j])) j++;
    
    if (buffer[j] == '"')
        type = STRING;
    else if (buffer[j] == '[')
        type = ARRAY;
    else if (buffer[j] == '{')
        type = OBJECT;
    else if (buffer[j] == 'n' && buffer[j + 1] == 'u' && buffer[j + 2] == 'l' && buffer[j + 3] == 'l')
        type = NON;
    else if (buffer[j] == 't' && buffer[j + 1] == 'r' && buffer[j + 2] == 'u' && buffer[j + 3] == 'e')
        type = BOOLEAN;
    else if (buffer[j] == 'f' && buffer[j + 1] == 'a' && buffer[j + 2] == 'l' && buffer[j + 3] == 's' && buffer[j + 4] == 'e')
        type = BOOLEAN;
    // TODO: add number support
        
        return type;
}



char *get_next_string(char *buffer, int *i, void *pt) {
    int j = *i;
    while (buffer[j] != '"') j++;
    *i = ++j;
    while (buffer[*i] != '"') (*i)++;
    
    char *string = malloc(sizeof(char) * ((*i) - j + 1));
    if (!string)
        return NULL;
    
    ptrck_add(pt, string);
    
    memcpy(string, &buffer[j], (*i) - j);
    string[(*i) - j] = '\0';
    
    return string;
}



int *get_next_number(char *buffer, int *i, void *pt) {
    // TODO: add number support
    return NULL;
}



int *get_next_boolean(char *buffer, int *i, void *pt) {
    int *boolean = malloc(sizeof(int));
    if (!boolean)
        return NULL;
    
    while (isspace(buffer[*i])) (*i)++;
    if (buffer[*i] == 't' && buffer[(*i) + 1] == 'r' && buffer[(*i) + 2] == 'u' && buffer[(*i) + 3] == 'e') {
        *boolean = 1;
        ptrck_add(pt, boolean);
        return boolean;
    } else if (buffer[*i] == 'f' && buffer[(*i) + 1] == 'a' && buffer[(*i) + 2] == 'l' && buffer[(*i) + 3] == 's' && buffer[(*i) + 4] == 'e') {
        *boolean = 0;
        ptrck_add(pt, boolean);
        return boolean;
    }
    
    free(boolean);
    return NULL;
}



JSONArray *get_json_array(void *owner, char *buffer, int *i, Ptrck *pt);
JSONObject *get_json_object(void *owner, char *buffer, int *i, Ptrck *pt);
struct json_pair *get_json_pair(void *owner, char *buffer, int *i, Ptrck *pt);



JSONArray *get_json_array(void *owner, char *buffer, int *i, Ptrck *pt) {
    JSONType type = NON;
    int j = *i;
    
    JSONArray *ja = malloc(sizeof(JSONArray));
    if (!ja)
        return NULL;
    
    ptrck_add(pt, ja);
    
    // check JSONType
    while (isspace(buffer[(*i)++]));
    if (*i == ']') {
        ja->type = NON;
        ja->value = NULL;
        ja->value_c = 0;
        ja->owner = owner;
        return ja;
    }
    
    ja->value = NULL;
    ja->value_c = 0;
    ja->type = get_next_type(buffer, i);
    ja->owner = owner;
    
    while (1) {
        while (isspace(buffer[*i])) (*i)++;
        if (buffer[*i] == ']')
            break;
        
        ja->value_c++;
        void **tmp_value = realloc(ja->value, sizeof(void *) * ja->value_c);
        if (tmp_value == NULL) {
            // TODO: error
        }
        ja->value = tmp_value;
        
        switch (ja->type) {
            case OBJECT:
                ja->value[ja->value_c - 1] = get_json_object(ja, buffer, i, pt);
                break;
                
            case STRING:
                ja->value[ja->value_c - 1] = get_next_string(buffer, i, pt);
                break;
                
            case BOOLEAN:
                ja->value[ja->value_c - 1] = get_next_boolean(buffer, i, pt);
                break;
                
            default:
                printf(":|\n");
                break;
        } // TODO: add number support
        
        while (buffer[*i] != ',' && buffer[*i] != ']') (*i)++;
        if (buffer[*i] == ']')
            break;
        (*i)++;
    }
    
    if (ja->value != NULL)
        ptrck_add(pt, ja->value);
    
    return ja;
}



JSONObject *get_json_object(void *owner, char *buffer, int *i, Ptrck *pt) {
    int j = *i;
    JSONType type = NON;
    
    JSONObject *jo = malloc(sizeof(JSONObject));
    if (!jo)
        return NULL;
    
    ptrck_add(pt, jo);
    
    jo->pair = NULL;
    jo->pair_c = 0;
    jo->owner = owner;
    
    while (1) {
        while (isspace(buffer[*i])) (*i)++;
        if (buffer[*i] == '}')
            break;
        
        jo->pair_c++;
        struct json_pair **tmp_pairs = realloc(jo->pair, sizeof(struct json_pair *));
        if (tmp_pairs == NULL) {
            // TODO: error
        }
        jo->pair = tmp_pairs;
        
        jo->pair[jo->pair_c - 1] = get_json_pair(jo, buffer, i, pt);
        
        while (buffer[*i] != ',' && buffer[*i] != '}') (*i)++;
        if (buffer[*i] == '}')
            break;
        
        (*i)++;
    }
    
    ptrck_add(pt, jo->pair);
    
    return jo;
}



struct json_pair *get_json_pair(void *owner, char *buffer, int *i, Ptrck *pt) {
    while (buffer[*i] != ':' && buffer[*i] != '}') (*i)++;
    if (buffer[*i] == '}')
        return NULL;
    
    struct json_pair *pair = malloc(sizeof(struct json_pair));
    if (!pair)
        return NULL;
    
    int j = *i;
    while (buffer[--j] != '"');
    while (buffer[--j] != '"');
    
    pair->key = get_next_string(buffer, &j, pt);
    if (!pair->key) {
        free(pair);
        return NULL;
    }
    
    while (isspace(buffer[++(*i)]));
    pair->value_type = get_next_type(buffer, i);
    switch (pair->value_type) {
        case ARRAY:
            pair->value = get_json_array(owner, buffer, i, pt);
            break;
            
        case OBJECT:
            pair->value = get_json_object(owner, buffer, i, pt);
            break;
            
        case STRING:
            pair->value = get_next_string(buffer, i, pt);
            break;
            
        case BOOLEAN:
            pair->value = get_next_boolean(buffer, i, pt);
            break;
            
        case NON:
            pair->value = NULL;
            break;
            
        default:
            break;
    } // TODO: add number support
    
    ptrck_add(pt, pair);
    
    return pair;
}



JSONDocument *get_json_document(char *buffer) {
    Ptrck *pt = ptrck_init();
    if (!pt)
        return NULL;
    
    JSONDocument *jd = malloc(sizeof(JSONDocument));
    if (!jd) {
        ptrck_del(pt);
        return NULL;
    }
    
    ptrck_add(pt, jd);
    
    int i = 0;
    if (isspace(buffer[0]))
        while (isspace(buffer[i++]));
    
    switch (buffer[i]) {
        case '{':
            jd->content_type = OBJECT;
            jd->content = get_json_object(jd, buffer, &i, pt);
            break;
            
        case '[':
            jd->content_type = ARRAY;
            jd->content = get_json_array(jd, buffer, &i, pt);
            break;
            
        case '?':
            // TODO: get_json_?()
            break;
            
        default:
            break;
    }
    /*
    if (error) {
        ptrck_clear(pt);
        ptrck_del(pt);
        return NULL;
    }
    
    ptrck_del(pt);
    */
    //jd->ptrs = pt;
    
    return jd;
}



// user functions
JSONDocument *json_read_file(const char *file) {
    // Read source to buffer
    FILE *fp = fopen(file, "r");
    if (!fp)
        return NULL;
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = malloc(sizeof(char) * (size + 1));
    if (!buffer) {
        fclose(fp);
        return NULL;
    }
    
    if (fread(buffer, sizeof(char), size, fp) != size) {
        fclose(fp);
        free(buffer);
        return NULL;
    }
    
    fclose(fp);
    buffer[size] = '\0';
    
    JSONDocument *jd = get_json_document(buffer);
    
    free(buffer);
    return jd;
}



int json_write_file(JSONDocument *jd, const char *file) {
    // TODO: implement
    return 0;
}



JSONType json_value_type(JSONObject *jo, const char *key) {
    for (int i = 0; i < jo->pair_c; i++) {
        if (strcmp(jo->pair[i]->key, key) == 0)
            return jo->pair[i]->value_type;
    }
    
    return NON;
}



void *json_get_value(JSONObject *jo, const char *key) {
    for (int i = 0; i < jo->pair_c; i++) {
        if (strcmp(jo->pair[i]->key, key) == 0) {
            return jo->pair[i]->value;
        }
    }
    
    return NULL;
}



JSONType json_array_value_type(JSONArray *ja) {
    return ja->type;
}



int json_get_array_length(JSONArray *ja) {
    return ja->value_c;
}
