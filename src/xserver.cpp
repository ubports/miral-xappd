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

 // Hints taken from Xweston

#include "xserver.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

Xserver *Xserver::_this;
Xserver::Xserver(int display, XserverType type): _display(display), _type(type) { Xserver::_this = this;  }

void Xserver::_cleanup(int signal) {
  waitpid(_pid, NULL, 0);
  std::cout << "Xserver exited! wating for new event" << '\n';
  _alive = false;
}

bool Xserver::spawn(int ufd, int uafd) {
  int fd = dup(ufd);
		if (fd < 0)
			std::cout << "UFS < 0" << '\n';
  auto fd_str = std::to_string(fd);

  int afd = dup(uafd);
		if (afd < 0)
			std::cout << "AFD < 0" << '\n';
  auto afd_str = std::to_string(afd);

  std::ostringstream _dsp_str;
  _dsp_str << ":" << _display;
  std::string dsp_str = _dsp_str.str();

  signal(SIGCHLD, Xserver::cleanup);
  _pid = fork();
  switch (_pid) {
    case 0:
      switch (_type) {
        case XMIR:
          std::cout << "Staring Xmir!" << '\n';
          execl("/usr/bin/Xmir", "Xmir", dsp_str.c_str(), "-rootless", "-listen", afd_str.c_str(), "-listen", fd_str.c_str(), "-terminate", NULL);
          break;
        case XWAYLAND:
          std::cout << "Staring Xwayland!" << '\n';
          execl("/usr/bin/Xwayland", "Xwayland", dsp_str.c_str(), "-rootless", "-listen", afd_str.c_str(), "-listen", fd_str.c_str(), "-terminate", NULL);
          break;
      }
      break;
    case -1:
      std::cout << "Failed to start Xserver!" << std::endl;
      break;
    default:
      _alive = true;
      break;
  }
  return true;
}
