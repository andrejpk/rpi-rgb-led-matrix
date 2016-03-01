// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Copyright (C) 2014 Henner Zeller <h.zeller@acm.org>
// Copyright (C) 2015 Christoph Friedrich <christoph.friedrich@vonaffenfels.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 2.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://gnu.org/licenses/gpl-2.0.txt>

#ifndef RPI_AK_TRANSFORMER_H
#define RPI_AK_TRANSFORMER_H

#include <vector>
#include <cstddef>

#include "canvas.h"

namespace rgb_matrix {

class Rect64x32Transformer : public CanvasTransformer {
public:
  Rect64x32Transformer();
  virtual ~Rect64x32Transformer();

  virtual Canvas *Transform(Canvas *output);

private:
  class TransformCanvas;

  TransformCanvas *const canvas_;
};

} // namespace rgb_matrix

#endif // RPI_AK_TRANSFORMER_H
