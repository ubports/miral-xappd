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

#include "socket.h"
#include "xserver.h"
#include <iostream>

int main(int argc, char const *argv[]) {
  XserverType type = XserverType::XMIR;
  if(argc>=2) {
    if (std::string(argv[1]) == "-xmir") {
      std::cout << "Using Xmir" << '\n';
    }
    else if (std::string(argv[1]) == "-xwayland") {
      std::cout << "Using Xwayland" << '\n';
      type = XserverType::XWAYLAND;
    }
    else {
      std::cout << "avalable options -xmir and -xwayland" << '\n';
      return 1;
    }
  } else {
    std::cout << "Using Xmir" << '\n';
  }

  Socket socket(0, type);
again:
  if (socket.createLockfile() < 0) {
		if (errno == EAGAIN) {
      goto again;
		} else if (errno == EEXIST) {
			std::cout << "X11 lock alredy exist!" << '\n';
      return -1;
		} else {
			return -1;
		}
  }

  socket.bindToAbstractSocket();
  socket.bindToSocket();
  socket.spawnXserverOnEvent();
  return 0;
}
