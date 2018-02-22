/*
 * Copyright (C) 2018 Marius Gripsgard <marius@ubports.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XSERVER_H
#define XSERVER_H

#include <spawn.h>

enum XserverType {
  XMIR,
  XWAYLAND
};

class Xserver {
private:
  static Xserver *_this;
  int _display;
  pid_t _pid;
  bool _alive;
  XserverType _type;

public:
  Xserver(int display, XserverType type);

  void _cleanup(int signal);
  static void cleanup(int signal) { _this->_cleanup(signal); }

  bool spawn(int fd, int afd);
  bool isAlive() { return _alive; }
};

#endif /* end of include guard: XSERVER_H */
