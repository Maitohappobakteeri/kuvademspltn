// Generated with script: script/generate_res_id.py
// Fri 14.07.2017 - 13:32:43


#ifndef RESID_HPP
#define RESID_HPP


#include "residgenerator.hpp"

#include <map>
#include <string>


const unsigned int RES_COMMON_PLACEHOLDER = 0;
const unsigned int RES_COMMON_FONT_INCONSOLATA_REGULAR = 1;
const unsigned int RES_COMMON_SHADER_COLOR_VERTEXSHADER = 2;
const unsigned int RES_COMMON_SHADER_COLOR_FRAGMENTSHADER = 3;
const unsigned int RES_COMMON_SHADER_TEXTURE_FRAGMENTSHADER = 4;
const unsigned int RES_COMMON_SHADER_TEXTURE_VERTEXSHADER = 5;


const std::map<std::string, unsigned int> RESOURCE_ID_MAP = {
    {"res/common/placeholder.png", RES_COMMON_PLACEHOLDER},
    {"res/common/font/Inconsolata-Regular.ttf", RES_COMMON_FONT_INCONSOLATA_REGULAR},
    {"res/common/shader/color.vertexshader", RES_COMMON_SHADER_COLOR_VERTEXSHADER},
    {"res/common/shader/color.fragmentshader", RES_COMMON_SHADER_COLOR_FRAGMENTSHADER},
    {"res/common/shader/texture.fragmentshader", RES_COMMON_SHADER_TEXTURE_FRAGMENTSHADER},
    {"res/common/shader/texture.vertexshader", RES_COMMON_SHADER_TEXTURE_VERTEXSHADER},
};


#endif
