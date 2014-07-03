/*
 * Copyright (C) 2010-2014 GRNET S.A.
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

EXPORT_SYMBOL(xpool_init);
EXPORT_SYMBOL(xpool_clear);
EXPORT_SYMBOL(xpool_add);
EXPORT_SYMBOL(xpool_remove);
EXPORT_SYMBOL(xpool_peek);
EXPORT_SYMBOL(xpool_peek_idx);
EXPORT_SYMBOL(xpool_peek_and_fwd);
EXPORT_SYMBOL(xpool_set_idx);

EXPORT_SYMBOL(__xpool_clear);
EXPORT_SYMBOL(__xpool_add);
EXPORT_SYMBOL(__xpool_remove);
EXPORT_SYMBOL(__xpool_peek);
EXPORT_SYMBOL(__xpool_peek_idx);
EXPORT_SYMBOL(__xpool_peek_and_fwd);
EXPORT_SYMBOL(__xpool_set_idx);
