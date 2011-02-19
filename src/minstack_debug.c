/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              minstack                                       *
 * Copyright (C) 2010,2011  Aurelien BOUIN (a_bouin@yahoo.fr)                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  This program is free software; you can redistribute it and/or              *
 *  modify it under the terms of the GNU General Public License                *
 *  as published by the Free Software Foundation; either version 3             *
 *  of the License, or (at your option) any later version.                     *
 *                                                                             *
 *  This program is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 *  GNU General Public License for more details.                               *
 *                                                                             *
 *  You should have received a copy of the GNU General Public License          *
 *  along with this program; if not, write to the Free Software                *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "minstack_debug.h"

#ifndef DEFAULT_DEBUG_LEVEL
#define	DEFAULT_DEBUG_LEVEL	(MINSTACK_WARNING_LEVEL)
#endif

unsigned int debug_level=DEFAULT_DEBUG_LEVEL;

/**
 * \brief return if it is allowed or not to print the message depending of the level
 * \param level the level of the message
 * \return 0 if it cannot been displayed
 */
int minstack_allow_print_debug(unsigned int level){
	if (level >= debug_level)
		return 1;
	return 0;
}

/**
 * \brief set the debug level
 * \param level the level to reach before displaying
 */
void minstack_set_debug_level(unsigned int value)
{
	debug_level=value;
}
