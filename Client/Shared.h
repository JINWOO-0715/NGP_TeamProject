#pragma once

#pragma warning(disable:4819)

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#undef main
#include <entt/entt.hpp>

#include <string>
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>
#include <functional>
#include <algorithm>

using std::string;
using std::shared_ptr;
using std::vector;
using std::array;
using std::unordered_map;

#include "Math.h"

#include "Log.h"

#include "Tags.h"
#include "Components.h"
#include "Game.h"
#include "Entity.h"