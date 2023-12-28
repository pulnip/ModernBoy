#pragma once

#include <memory>

using game_ptr = std::shared_ptr<class Game>;
using game_ref = std::weak_ptr<class Game>;
using actor_ptr = std::shared_ptr<class Actor>;
using actor_ref = std::weak_ptr<class Actor>;
using comp_ptr = std::shared_ptr<class Component>;
using comp_ref = std::weak_ptr<class Component>;
