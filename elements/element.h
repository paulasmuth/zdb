/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <functional>
#include <signaltk.h>

namespace signaltk {
class Layer;
class Viewport;

struct Element {
  ~Element();
  void* config;
  std::function<void (Element* e)> destroy;
};

using ElementRef = std::unique_ptr<Element>;

} // namespace signaltk
