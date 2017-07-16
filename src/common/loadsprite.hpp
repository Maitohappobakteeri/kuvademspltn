#ifndef LOADSPRITE_HPP
#define LOADSPRITE_HPP


#include "renderer/renderer.hpp"
#include "renderer/spritemodel.hpp"

#include <string>
#include <vector>


SpriteModel parse_spritedata(Renderer* renderer, const std::string& spriteString);
SpriteModel read_spritedata_from_file(Renderer* renderer, const std::string& filePath);
Sprite create_sprite(const SpriteModel& SpriteModel);

#endif
