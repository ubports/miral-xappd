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

#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "xserver.h"

class Xserver;
class Socket {
private:
  int _display;
  int _fd;
  int _afd;
  Xserver *_xserver;

  void onEvent(struct ev_loop* loop, struct ev_io* io, int r);

public:
  Socket(int display, XserverType type);

  bool bindToSocket();
  bool bindToAbstractSocket();
  void spawnXserverOnEvent();
  int createLockfile();
};

#endif /* end of include guard: SOCKET_H */
