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

#ifndef MINSTACK_DEBUG_H_
#define MINSTACK_DEBUG_H_

int minstack_allow_print_debug(unsigned int value);
void minstack_set_debug_level(unsigned int value);

#define MINSTACK_DEBUG_LEVEL		(0)
#define MINSTACK_MESSAGE_LEVEL		(1)
#define MINSTACK_WARNING_LEVEL		(2)
#define MINSTACK_ERROR_LEVEL		(3)

#define minstack_print_debug(level, fmt, arg...)     if (minstack_allow_print_debug(level)) \
        printf("<%d> " fmt ,level, ## arg)

#define printdebug(fmt,arg...)		minstack_print_debug(MINSTACK_DEBUG_LEVEL,fmt,##arg)
#define printmessage(fmt,arg...)	minstack_print_debug(MINSTACK_MESSAGE_LEVEL,fmt,##arg)
#define printwarning(fmt,arg...)	minstack_print_debug(MINSTACK_WARNING_LEVEL,fmt,##arg)
#define printerror(fmt,arg...)		minstack_print_debug(MINSTACK_ERROR_LEVEL,fmt,##arg)
#define printmoreerror(msg)			perror(msg);

#define printline					printf("%s(%d)\n",__FUNCTION__,__LINE__);

#endif /* MINSTACK_DEBUG_H_ */
