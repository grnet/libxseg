/*
Copyright (C) 2010-2014 GRNET S.A.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

int xseg_posix_init(void);
int xseg_pthread_init(void);
int xseg_posixfd_init(void);

int __xseg_preinit(void)
{
    int r;
    if ((r = xseg_posix_init())) {
        goto out;
    }
    if ((r = xseg_pthread_init())) {
        goto out;
    }
    if ((r = xseg_posixfd_init())) {
        goto out;
    }
  out:
    return r;
}
