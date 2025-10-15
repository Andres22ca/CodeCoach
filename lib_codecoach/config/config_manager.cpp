//
// Created by andres on 5/10/25.
//

#include "config_manager.h"
#include "errors/exceptions.h"
#include "cstdlib"
#include "cctype"
#include "string"

namespace cc::config {
    //override para test
    static const Config* g_test_cfg = nullptr;
    //Loader de entorno y defaults
    static Config make_from_env_or_default();
    //validador de url
    static bool looks_like_url(const std::string& s);



    static int parse_int_or_throw(const std::string& raw, int min, int max, const char* key);









}