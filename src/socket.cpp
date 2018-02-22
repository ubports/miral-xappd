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

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>

#include "socket.h"

Socket::Socket(int display, XserverType type) : _display(display), _xserver(new Xserver(display, type)) {}

bool Socket::bindToSocket() {
  struct sockaddr_un addr;
  socklen_t size, name_size;
  addr.sun_family = AF_LOCAL;

  _fd = socket(PF_LOCAL, SOCK_STREAM, 0);

   name_size = snprintf(addr.sun_path, sizeof addr.sun_path,
                        "/tmp/.X11-unix/X%d", 0) + 1;
  size = offsetof(struct sockaddr_un, sun_path) + name_size;
  unlink(addr.sun_path);
  if (bind(_fd, (struct sockaddr *) &addr, size) < 0) {
    std::cout << "Failed to bind to socket" << std::endl;
    close(_fd);
    return false;
  }

  if (listen(_fd, 1) < 0) {
    std::cout << "Faled to listen to socket" << std::endl;
    unlink(addr.sun_path);
    close(_fd);
    return false;
  }

  return true;
}

bool Socket::bindToAbstractSocket() {
  struct sockaddr_un addr;
  socklen_t size, name_size;
  addr.sun_family = AF_LOCAL;

  _afd = socket(PF_LOCAL, SOCK_STREAM, 0);

   name_size = snprintf(addr.sun_path, sizeof addr.sun_path,
                      "%c/tmp/.X11-unix/X%d", 0, _display);
  size = offsetof(struct sockaddr_un, sun_path) + name_size;
  if (bind(_afd, (struct sockaddr *) &addr, size) < 0) {
    std::cout << "Failed to bind to abstract socket" << std::endl;
    close(_afd);
    return false;
  }

  if (listen(_afd, 1) < 0) {
    std::cout << "Failed to listen to abstract socket" << std::endl;
    close(_afd);
    return false;
  }

  return true;
}

int Socket::createLockfile()
{
  char lockfile[256];
	char pid[11];
	int fd, size;
	pid_t other;

	snprintf(lockfile, sizeof lockfile, "/tmp/.X%d-lock", _display);
	fd = open(lockfile, O_WRONLY | O_CLOEXEC | O_CREAT | O_EXCL, 0444);
	if (fd < 0 && errno == EEXIST) {
		fd = open(lockfile, O_CLOEXEC | O_RDONLY);
		if (fd < 0 || read(fd, pid, 11) != 11) {
			if (fd >= 0)
				close (fd);

			errno = EEXIST;
			return -1;
		}

		/* Trim the trailing LF, or at least ensure it's NULL. */
		pid[10] = '\0';

    other = std::stoi(pid);

		if (kill(other, 0) < 0 && errno == ESRCH) {
			/* stale lock file; unlink and try again */
			close(fd);
			if (unlink(lockfile))
				/* If we fail to unlink, return EEXIST
				   so we try the next display number.*/
				errno = EEXIST;
			else
				errno = EAGAIN;
			return -1;
		}

		close(fd);
		errno = EEXIST;
		return -1;
	} else if (fd < 0) {
		return -1;
	}

	size = dprintf(fd, "%10d\n", getpid());
	if (size != 11) {
		unlink(lockfile);
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

void Socket::spawnXserverOnEvent() {
  struct pollfd poll_set[32];
  int numfds = 2;
  poll_set[0].fd = _afd;
  poll_set[0].events = POLLIN;
  poll_set[1].fd = _fd;
  poll_set[1].events = POLLIN;

  while (true) {
    if (!_xserver->isAlive()) {
      if (poll(poll_set, numfds, 10000) > 0) {
        _xserver->spawn(_fd, _afd);
      }
    }
  }
}
