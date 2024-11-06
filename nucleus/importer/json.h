#ifndef NU_IMPORTER_JSON_H
#define NU_IMPORTER_JSON_H

#include <nucleus/importer/api.h>
#include <nucleus/external/jsmn/jsmn.h>

static jsmntok_t *nu__json_parse(nu_str_t   json,
                                 nu_size_t *size,
                                 nu_size_t *count);
static nu_str_t   nu__json_value(nu_str_t json, const jsmntok_t *tok);
static nu_bool_t  nu__json_eq(nu_str_t json, const jsmntok_t *tok, nu_str_t s);
static const jsmntok_t *nu__json_skip(const jsmntok_t *token);
static const jsmntok_t *nu__json_object_member(nu_str_t         json,
                                               const jsmntok_t *object,
                                               nu_str_t         name);

#endif
