/* nagios_hpilo_restful_engine.h -- types and prototypes used by nagios iLO engine 
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

/* Written by Adrian Huang <adrian.huang@hp.com>.  */

#ifndef NAGIOS_HPILO_ENGINE_H
#define NAGIOS_HPILO_ENGINE_H

#include "nagios_hpilo_common.h"

/* Macro definition */
#define USAGE	"-H <IP Address> -o <service_index> [-h] [-V]"

/* Available options for this plugin.  */ 
#define SHORT_OPTIONS "hVH:o:"

/* The required the number of arguments for this plugin.  */
#define NAGIOS_ILO_REQUIRE_ARGC	2

#define IDX_OUT_OF_RANGE(idx, array) \
	((idx < 0 || idx > NUM_ELEMENTS(array)) ? TRUE : FALSE)

#define ILO_OID_NAME_LEN	24 

#define MAX_OID_INFO_ENTRIES	2

/* Health group OID.  
   For example: Fans, power supplies and so on.  */
#define HLTH_GROUP_OID	0

/* Health component OID info
   For example: Fan 1, Fan 2.....Fan n
		Sensor 1, Sensor 2.....Sensor n.  */
#define HLTH_COMP_OID	1

#define ILO_SERVICE_URL_LEN 50

#define ILO_SERVICE_SUB_URL_LEN 50

#define ILO_SERVICE_TYPE_LEN  35

#define ILO_SERVICE_NAME_LEN  15

/* Strings for help menu.  */
static char help_string[][128] =
  {"-H, --hostname=ip_address\n\tHost name or IP address to connect with",
   "-o, --sid=sid_index\n\tSevice index",
   "-V, --version\n\tprint the version number",
   "-h, --help\n\tprint this help menu"
  };

/* options for arguments.  */
static struct option long_options[] =
  {
    {"hostname",	required_argument,	0,	'H'}, 
    {"sid",		required_argument,	0,	'o'},
    {"version",		no_argument,		0,	'V'},
    {"help",		no_argument,		0,	'h'},
    {0,			0,			0,	  0}
  };


/* iLO health status.  */
enum 
{
  ILO_HLTH_STATUS_NA,
  ILO_HLTH_STATUS_OTHER,
  ILO_HLTH_STATUS_OK,
  ILO_HLTH_STATUS_DEGRADED,
  ILO_HLTH_STATUS_FAILED,
};

/* iLO Redundant status.  */
enum 
{
  ILO_RED_STATUS_OTHER = 1,
  ILO_RED_STATUS_NOT_RED,
  ILO_RED_STATUS_RED,
};

/* A variety of OID types.  */
enum 
{
  /* The array index of the system overall condition.  */
  ILO_SYS_OVERALL_COND, 

  ILO_HLTH_TYPE,

  ILO_HLTH_STR_TYPE,

  ILO_REDUNDANT_TYPE,

  /* The array index of NIC condition (232.18.1.3:cpqNicMibCondition).  */
  ILO_NIC_MIB_COND = 14,
};

/* Generic Service info.    */
struct generic_service_info
{
  /* Root URL.  */
  char service_root_url[ILO_SERVICE_URL_LEN]; 

  /* Sub URL.  */
  char service_sub_url[ILO_SERVICE_SUB_URL_LEN]; 

  /* Service type.  */
  char service_type[ILO_SERVICE_TYPE_LEN];

  /* OID Length.  */
  char service_name[ILO_SERVICE_NAME_LEN];
};

/* Data structure to store options via the argument list of the command 
   line.  */
struct ilo_restful_options 
{
  /* Pointer to host name (IP address).  */
  char *host;

  /* Pointer to admin name.  */
  char *admin;

  /* Pointer to admin passwd.  */
  char *passwd;

  /* Index to the iLO restful service number.  */
  int service_idx;	
};

/* Data structure to store OID information retrieved from a specific OID.  */
struct ilo_cJSON_list
{
  /* Json root.  */
  cJSON *root;
 
  /* Pointer to the next entry.  */
  struct ilo_cJSON_list *next;
 
};

/* Private data for this plugin.  */
struct restful_requests
{
  /* Pointer to Json array.  */
  struct ilo_cJSON_list cJSON_list;
  struct ilo_cJSON_list* head;

  /* Nagios status */
  enum Nagios_status n_status; 

};

#endif
