#pragma once

#include "maoliao/levels.h"

#include <string>
#include <vector>

namespace maoliao {

// Occupancy grid: one char per tile. Solid ids 1-10 -> '#', scenery -> '.',
// coins -> 'o', food tile -> '*', enemy -> 'e', empty -> ' '.
std::vector<std::string> render_ascii(const Level& level, int min_x, int max_x,
                                      int min_y, int max_y);

std::string render_ascii_joined(const Level& level, int min_x, int max_x,
                                int min_y, int max_y);

}  // namespace maoliao
