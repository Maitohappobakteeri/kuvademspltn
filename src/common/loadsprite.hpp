#ifndef LOADSPRITE_HPP
#define LOADSPRITE_HPP


#include "renderer/renderer.hpp"
#include "renderer/spritemodel.hpp"

#include <string>
#include <vector>


SpriteModel parse_spritemodel(Renderer* renderer, const std::string& spriteString);
SpriteModel load_spritemodel_from_file(Renderer* renderer, const std::string& filename);


#endif
