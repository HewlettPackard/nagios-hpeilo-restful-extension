/* nagios_hpilo_common.h -- types and prototypes used by nagios iLO plug-in 
   (C) Copyright [2016] Hewlett Packard Enterprise Development Company, L.P.

   This program is free software; you can redistribute it and/or modify 
   it under the terms of version 2 of the GNU General Public License as 
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

   See the GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License 
   along with this program; if not, write to:
    Free Software Foundation, Inc.
    51 Franklin Street, Fifth Floor
    Boston, MA 02110-1301, USA.  */

/* Written by Maple Yang <maple.yang@hpe.com>.  */

#ifndef NAGIOS_HPILO_COMMON_H
#define NAGIOS_HPILO_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "cJSON.h"

#define TRUE	1
#define FALSE	0

/* Internal status.  */
#define NAGIOS_ILO_SUCCESS_STATUS	0
#define NAGIOS_ILO_FAIL_STATUS		-1

#define NUM_ELEMENTS(array) \
        (sizeof(array) / sizeof(typeof(array[0])))

/* Return status defined by Nagios.  */
enum Nagios_status
{
  NAGIOS_OK, 
  NAGIOS_WARNING, 
  NAGIOS_CRITICAL,
  NAGIOS_UNKNOWN 
};

/* Option operation.  */
enum
{
  NAGIOS_OPTION_NOP,
  NAGIOS_OPTION_PRINT_VERSION,
  NAGIOS_OPTION_PRINT_HELP
};

/* Error Debug Macro.  */
#define ILO_ERR_DEBUG(ppError, fmt, args...) \
	if (*ppError == NULL) { \
		asprintf(ppError, "[Error]: " fmt, ## args); \
	} else { \
		char	buf[strlen(*ppError) + 1]; \
		sprintf(buf, "%s", *ppError); \
		free(*ppError); \
		asprintf(ppError, "%s[Error]: " fmt, buf, ##args); \
	}

#endif
