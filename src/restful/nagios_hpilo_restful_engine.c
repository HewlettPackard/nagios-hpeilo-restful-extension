/*****************************************************************************/
/* (C) Copyright [2016] Hewlett Packard Enterprise Development Company, L.P. */
#define CURL_STATICLIB

#include <dlfcn.h>
#include <stdio.h>
#include <curl/curl.h>
#include <openssl/pem.h>
#include "nagios_hpilo_restful_engine.h"
#include "ilo-credit-store.h"
#include "base64.h"

/* The official name of this program (e.g., no 'g' prefix).  */
#define PROGRAM_NAME    "nagios_hpeilo_restful_engine"
#define PROGRAM_VERSION "1.0"

#define ORIGIAL_OPTION_NUMBER 10
#define SERVICE_NUM 8
#define DEVICE_MAX 1024
#define MSG_MAX 1024
#define URL_MAX 128
//int count=1;

#define FANNAME_ID 	11
#define FANSTATE_ID 	12
#define ThermalNAME_ID 	31
#define ThermalSTATE_ID 32
#define MEMORYINFO_ID 	21
#define MEMORYSLOT_ID 	22
#define MEMORYSTATE_ID 	23
char root_iterates[] = "links:Member:href";
/* Array of HP iLO service table. */
static struct generic_service_info ilo_service_table[] =
{
  {"/rest/v1/Systems", "",		"Status:Health",		"System Health"},	//1
  {"/rest/v1/Chassis", "links:ThermalMetrics:href",	"Fans:Status:Health",		"Fan"}, 	//2
  {"/rest/v1/Systems", "",		"Memory:Status:HealthRollUp",	"Memory"},		//3
  {"/rest/v1/Managers", "links:EthernetInterfaces:href",	"Items:Status:Health", 	"Network"},	//4 
  {"/rest/v1/Chassis", "links:PowerMetrics:href",		"PowerSupplies:Status:Health",	"Power Supply"},//5
  {"/rest/v1/Systems", "",		"Processors:Status:HealthRollUp", "Processor"},		//6
  {"/rest/v1/Systems", "Oem:Hp:links:SmartStorage:href",	"Status:Health",		"Storage"},//7
  {"/rest/v1/Chassis", "links:ThermalMetrics:href",	"Temperatures:Status:Health",	"Temperature"}, //8
  {0,0,0,0},
  {0,0,0,0},
  {"","","Fans:FanName","Fan"},											//11
  {"","","Fans:Status:State","Fan"},										//12
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {"", "Oem:Hp:links:Memory:href", "links:Member:href", "Memory"},						//21
  {"", "", "SocketLocator","Memory"},										//22
  {"", "", "DIMMStatus","Memory"},										//23
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {"","","Temperatures:Name","Fan"},										//31
  {"","","Temperatures:Status:State","Fan"},									//32
};

/* Debug:  */
/* Print debug info.  */
#ifdef DFLAG
void
debug_print(char *debug_info) {
    printf("Debug:\n%s\n\n",debug_info);
}
#else
#define debug_print(debug_info) do{}while(0)
#endif

/* int to char */
char*
itoa(int num,char* str,int radix)
{
  char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  unsigned unum;
  int i=0,j,k;

  if(radix==10&&num<0)
  {
    unum=(unsigned)-num;
    str[i++]='-';
  } else unum=(unsigned)num;

  do{
    str[i++]=index[unum%(unsigned)radix];
    unum/=radix;
  }while(unum);
  str[i]='\0';

  if(str[0]=='-') k=1;
  else k=0;
  char temp;
  for(j=k;j<=(i-1)/2;j++) {
    temp=str[j];
    str[j]=str[i-1+k-j];
    str[i-1+k-j]=temp;
  }
  return str;
}



/* Clean:  */
/* Free the allocated the elements of the request cJSON list.  */
void
free_cJSON_list (struct restful_requests *request)
{
  request->head=NULL;
  struct ilo_cJSON_list *prev;
  prev=(request->cJSON_list).next;
  while (prev)
    {
      //printf("%d\n",count--);
      request->head = prev->next;
      free(prev);
      prev=request->head;
    }
}

/* response overflow */
char restful_response[4][16384];
int i_res=0;

void
reset_response_array() {
  memset(restful_response,'\0',sizeof(restful_response));
  i_res=0;
}

/* libcurl:  */
/* libcurl request writecallback.  */
size_t
write_callback(char *ptr, size_t size, size_t nmemb, void *stream){
  strcpy(restful_response[i_res++],ptr);

  return size*nmemb;
}

/* Call libcurl to send restful get.  */
int
call_libcurl(struct ilo_restful_options *option, struct restful_requests *request, char * part_url) {

  char *restful_url=(char *)malloc(sizeof(char)*URL_MAX);
  char *admin_passwd
       =(char *)malloc(sizeof(char)*(strlen(option->admin)));
       //=(char *)malloc(sizeof(char)*(strlen(option->admin)+strlen(option->passwd)+1));
  char *post_fields = NULL;

  CURL *curl;
  CURLcode res;
  struct curl_slist *headers=NULL; // init to NULL is important 

  strcpy(restful_url, "https://"); 
  strcat(restful_url, option->host); 
  strcat(restful_url, part_url); 
  //strcat(restful_url, ilo_service_table[option->service_idx-1].service_url); 
  debug_print(restful_url);

  strcpy(admin_passwd, option->admin);
  //strcat(admin_passwd, ":");
  //strcat(admin_passwd, option->passwd);
  debug_print(admin_passwd);

  headers = curl_slist_append( headers, "Accept: application/json");  
  headers = curl_slist_append( headers, "Content-Type: application/json");
  headers = curl_slist_append( headers, "charsets: utf-8"); 

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, restful_url);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

    curl_easy_setopt(curl, CURLOPT_USERPWD , admin_passwd);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);

    free(restful_url);
    free(admin_passwd);

    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    curl_slist_free_all(headers); /* free the list again */

    /* always cleanup */
    curl_easy_cleanup(curl);

    /* Return curl_easy_perform result*/
    return res;
  } else {
    /* Return curl initial failed*/
    return CURLE_FAILED_INIT;
  }

}


/* cJSON:  */
/* Trim "" in cJSON data */
char *
trim_cJSON_to_char(struct ilo_cJSON_list *iterate) {
  if (iterate) {
    char *out=cJSON_Print(iterate->root);
    if ( out && out[0] == '\"' ) {
      out[strlen(out)-1]='\0';
      return out+1;
    } else if (out)
      return out;
     
  } else
    return NULL;
}

/* Check if list->root is array or not.  */
/* return 1 for array, 0 for not array.  */
int
is_cJSON_array (struct ilo_cJSON_list *list) {
  if (list->root) {
     char *out = cJSON_Print(list->root); 
     char tmp=out[0];
     free(out);
     if ('\[' == tmp) {
       debug_print("ARRAY");
       return 1;
     } else {
       return 0;
     }
  } else {
    debug_print("is_cJSON_array: list->root is empty");
  }

}

/* get item in ilo_cJSON_list *list->root.  */
/* return ilo_cJSON_list *list->head.  */
struct ilo_cJSON_list *
get_cJSON_item (struct ilo_cJSON_list *list, char *item) {
  struct ilo_cJSON_list *ltmp=list;
  char *out;
  int is_array;

  /* deal with array */
  is_array=is_cJSON_array(list);
  if (is_array == 1) {
    int arr_size = cJSON_GetArraySize(list->root);
    int i = 0;

    /* find key item in array */
    while (i<arr_size) {
      cJSON *ctmp;
      /* add a new node and leave list no change */
      if (!ltmp->next) {
        ltmp->next=(struct ilo_cJSON_list *)malloc(sizeof(struct ilo_cJSON_list));
        //printf("%d:",++count);
      }
      ltmp = ltmp->next;
      ltmp->next = NULL;

      ctmp = cJSON_GetArrayItem(list->root,i++);
      ltmp->root = cJSON_GetObjectItem(ctmp,item);

      out = cJSON_Print(ltmp->root);
      //debug_print("get_cJSON_item");
      debug_print(out);
      free(out);
    }

    if (list->next) {
      return list->next;
    } else {
      return NULL;
    }
  /* deal with non array, find key item */
  } else if (is_array == 0){
    //Save the end of list
    struct ilo_cJSON_list *lend=ltmp->next;
    struct ilo_cJSON_list *rlist=ltmp;

    /* find the end from the list->next */
    while (lend) {
      rlist=lend;
      lend=lend->next;
    }
    lend=rlist;

    /* find key item in each list item */
    do {
        cJSON *ctmp;
        ctmp = cJSON_GetObjectItem(ltmp->root,item);
        if (ctmp) {
          lend->next=(struct ilo_cJSON_list *)malloc(sizeof(struct ilo_cJSON_list));
          lend=lend->next;
          lend->root = ctmp;
          lend->next = NULL;
          out = cJSON_Print(lend->root);
          //debug_print("get_cJSON_item");
          out[strlen(++out)-1]='\0';
          debug_print(out);
          free(--out);
        //} else {
          //ltmp->root=NULL;
        }

      if (ltmp!=rlist) ltmp=ltmp->next;
      else break;
    } while (ltmp);

    return rlist->next;
  } else {
    return NULL;
  }
}

/* recover list to the clean state */
void
recover_cJSON_list (struct restful_requests *request) {
  request->head=&(request->cJSON_list);
  (request->cJSON_list).next=NULL;
}

/* get service info in request.  */
/* return service .  */
static enum Nagios_status
get_service_info (struct ilo_restful_options *option, struct restful_requests *request, char *iterates) {
  const char *delim = ":"; 
  char *out, *iterate;
  int retflag=0;
  enum Nagios_status n_status = NAGIOS_OK;

  struct ilo_cJSON_list *cJSON_iterate;
  cJSON *root, *child;

  int service_idx=option->service_idx-1;

  int dev_item=1;
  char erritem[MSG_MAX]=": [";
  

  request->head=&(request->cJSON_list);
  cJSON_iterate = request->head;
  //  out = cJSON_Print(root);
  //  printf("%s\n",out);
  if (iterates == NULL)
    iterate = strtok(ilo_service_table[service_idx].service_type,delim);
  else {
    iterate = strtok(iterates,delim);
    retflag=1;
  }
 
  /* find all search key seperated by : */
  while (iterate != NULL) { 
    debug_print(iterate);
    request->head=get_cJSON_item(cJSON_iterate,iterate);
    cJSON_iterate=request->head;
    if(cJSON_iterate) {
      iterate = strtok (NULL, delim);
      out = cJSON_Print(cJSON_iterate->root);
      //debug_print("get_service_info");
      debug_print(out);
    } else {
      iterate = NULL;
      return NAGIOS_UNKNOWN;
    }
  } 
 
  /* deal with sub_rul */
  if ( retflag==1 )
    return NAGIOS_OK;

  if (option->service_idx > ORIGIAL_OPTION_NUMBER)
    return;

  /* deal with result */
  cJSON_iterate=request->head;
  while (cJSON_iterate && cJSON_iterate->root) {
    char *out = trim_cJSON_to_char(cJSON_iterate);
    char *c_ok = "OK";
    char *c_warning = "Warning";
    char *c_critical = "Critical";
    char err[4]="";

    debug_print(out);

    if (strcmp(c_ok,out) != 0) {
      itoa(dev_item,err,10); 
      strcat(erritem, ilo_service_table[service_idx].service_name); 
      strcat(erritem, " "); 
      strcat(erritem, err); 
    }

    if (strcmp(c_warning,out) == 0 && n_status != NAGIOS_CRITICAL) {
      n_status = NAGIOS_WARNING;
      strcat(erritem, " - Warning,");
    } else if (strcmp(c_critical,out) == 0) {
      n_status = NAGIOS_CRITICAL;
      strcat(erritem, " - Crititcal,"); 
    } else if (strcmp(c_ok,out) != 0 && n_status != NAGIOS_CRITICAL) {
      n_status = NAGIOS_UNKNOWN;
      strcat(erritem, " - Unknown,"); 
    }

    cJSON_iterate=cJSON_iterate->next;
    dev_item++;
    //printf("%d",dev_item);
  } 
  
  erritem[strlen(erritem)-1]=']';
//  printf("%s:\t\t",ilo_service_table[service_idx].service_name);
  switch(n_status) {
    case NAGIOS_OK:
      printf("OK");
      break;
    case NAGIOS_WARNING:
      printf("WARNING");
      break;
    case NAGIOS_CRITICAL:
      printf("CRITICAL");
      break;
    case NAGIOS_UNKNOWN:
      printf("UNKNOWN");
      break;
    default:
      printf("ERROR");
  }
  if (erritem[3] != '\0' && 
     (service_idx == 4 || service_idx == 5))
    printf("%s",erritem);

  return n_status;
}

static void
print_version (void)
{
  printf("%s version - %s\n", PROGRAM_NAME, PROGRAM_VERSION);
  return ;
}

/* Print the usage information */

static void
usage (void)
{
  struct option *opt_ptr = NULL;

  printf("Usage: %s ", PROGRAM_NAME);

  for (opt_ptr=long_options;opt_ptr->name != NULL;opt_ptr++)
    {
      if (opt_ptr->has_arg == no_argument)
          printf("[-%c] ", opt_ptr->val);
      else if (opt_ptr->has_arg == required_argument)
          printf("-%c <%s> ", opt_ptr->val, opt_ptr->name);
    }

  printf("\n");

  return ;
}

static void
print_help (void)
{
  int   i, j;

  usage();

  printf("\n");

  for (i=0;i<sizeof(help_string)/sizeof(help_string[0]);i++)
    {
      printf("%s\n", help_string[i]);

      /* Check if the string contains "sid" string.  */
      if (strstr(help_string[i], "sid"))
        {
          printf("\n\toid index\tOID info\n");
          printf("\t------------------------------------\n");

          for (j = 0; j < SERVICE_NUM; j++)
            printf("\t%4d\t\t%s\n", j + 1,
                ilo_service_table[j].service_name);

          printf("\t------------------------------------\n");
        }
    }

  return ;
}

static void
check_option_instruction (int instruction)
{
  switch (instruction)
    {
    case NAGIOS_OPTION_PRINT_VERSION:
      print_version();
      break;
    case NAGIOS_OPTION_PRINT_HELP:
      print_help();
      break;
    }

  exit(NAGIOS_UNKNOWN);
}

int
process_options (int argc, char **argv, struct ilo_restful_options *options,
                 int *option_inst)
{
  char option;
  int ret = NAGIOS_ILO_SUCCESS_STATUS;
  int num_argc_proceed = 0;
  int option_index = 0;

  /* Ignore the error message reported by unistd library. */
  opterr = 0;

  while((option = getopt_long(argc, argv, SHORT_OPTIONS, long_options,
                                &option_index)) != -1)
    {
      switch(option)
        {
        case 'H':
          options->host = optarg;
          num_argc_proceed++;
          break;
        case 'o':
          options->service_idx = atoi(optarg);
          num_argc_proceed++;
          break;
        case 'V':
          *option_inst = NAGIOS_OPTION_PRINT_VERSION;  
          break;
        case 'h':
          *option_inst = NAGIOS_OPTION_PRINT_HELP;
          break;
        default:
          ret = NAGIOS_ILO_FAIL_STATUS;
        }
    }

  if(num_argc_proceed != NAGIOS_ILO_REQUIRE_ARGC &&
                                        *option_inst == NAGIOS_OPTION_NOP)
    {
      ret = NAGIOS_ILO_FAIL_STATUS;
    }

  return ret;
}

/* Initial request->cJSON_list, call call_libcurl function, and save cJSON to request->cJSON_list.root */
/* Return Nagios_status, > 0 ok, < 0 error */
int
init_curl(struct ilo_restful_options *option, struct restful_requests *request, char *url, char *root_iterates, enum Nagios_status *n_status){
  reset_response_array();
  if (CURLE_OK == call_libcurl(option, request, url)) {
    if(restful_response) {
      request->cJSON_list.root = cJSON_Parse((const char *)restful_response);
      request->cJSON_list.next = NULL;

      if (NULL != request->cJSON_list.root) {
        char *out = cJSON_Print(request->cJSON_list.root);
        debug_print(out);
        free(out);

        (*n_status)=get_service_info(option, request, root_iterates);
        return 0;
      }
      return -1;
    }
    return -2;
  }
  return -3;
}

/* Get Detail Error of Fan or Temperature */
/* Return Nagios_status, > 0 ok, < 0 error */
void
get_detail_error(struct ilo_restful_options *option, struct restful_requests *request,int service_idx1, int service_idx2) {
  struct ilo_cJSON_list *iterate;
  iterate=request->head;
  int ft_item=0;
  int ft_state_item=0;
  char *fts[DEVICE_MAX],*fts_state[DEVICE_MAX],*out;
  char erritem[MSG_MAX]=": [";

  while (iterate && ft_item < DEVICE_MAX) {
    fts[ft_item++]=trim_cJSON_to_char(iterate);
    iterate=iterate->next;
  }//while

  recover_cJSON_list(request);
  option->service_idx=service_idx1;
  get_service_info(option, request, NULL);
  iterate=request->head;
  while (iterate && ft_state_item < DEVICE_MAX) {
    fts_state[ft_state_item++]=trim_cJSON_to_char(iterate);
    iterate=iterate->next;
  }//while

  recover_cJSON_list(request);
  option->service_idx=service_idx2;
  ft_item=0;
  ft_state_item=0;
  get_service_info(option, request, NULL);
  iterate=request->head;
  while (iterate && ft_state_item < DEVICE_MAX) {
    if (strcmp(fts_state[ft_state_item++],"Absent") != 0) {
      if (strcmp(fts[ft_item],"OK") != 0 ) {
        char *ft_value=trim_cJSON_to_char(iterate);
        strcat(erritem,ft_value);
        strcat(erritem," - ");
        strcat(erritem,fts[ft_item]);
        strcat(erritem,",");
        debug_print(erritem);
      }
      ft_item++;
    }
    iterate=iterate->next;
  }//while
  erritem[strlen(erritem)-1]=']';
  printf("%s",erritem);
}

/* Recover the cJSON list of request*/
/* Return Nagios_status of the service */
static enum Nagios_status
recover_and_get(struct ilo_restful_options *option, struct restful_requests *request, int service_idx) {
  recover_cJSON_list(request);
  option->service_idx=service_idx;
  return get_service_info(option, request, NULL);
}

int main(int argc, char **argv) {
  /* Options information.  */
  struct ilo_restful_options option;
  struct restful_requests request;
  char *defaulthost="default";
  char *value;
  option.host=NULL;
  //option.admin=NULL;
  option.passwd=NULL;

  request.cJSON_list.root=NULL;
  request.cJSON_list.next=NULL;
  request.head=NULL;

  enum Nagios_status n_status = NAGIOS_UNKNOWN;

  int   option_instruction = NAGIOS_OPTION_NOP;

  if (process_options(argc, argv, &option, &option_instruction)
                      != NAGIOS_ILO_SUCCESS_STATUS)
    {
      usage();
      exit(NAGIOS_UNKNOWN);
    }

  if (option_instruction != NAGIOS_OPTION_NOP)
    check_option_instruction(option_instruction);

  if (option.service_idx > SERVICE_NUM) {
    printf("OID index '%d' is out-of-range "
           "in iLO OID table. The OID index "
           "should be 1-%d\n",
           option.service_idx, SERVICE_NUM);

    exit(NAGIOS_CRITICAL);
  }

  value=get_cred(option.host);
  if (!value) {
    exit(NAGIOS_UNKNOWN);
  }

  int ret=base64_decode(value, &option.admin);
  if (ret != 0) {
    exit(NAGIOS_UNKNOWN);
  }

  /* Retrieve the OID information and store in oid_list of
     struct ilo_snmp_priv.  */
  char *sub_url[200];

  // Call root
  ret=init_curl(&option, &request, ilo_service_table[option.service_idx-1].service_root_url, root_iterates, &n_status);

  if ( n_status != NAGIOS_UNKNOWN && ret == 0 ) {
    struct ilo_cJSON_list *iterate;
    iterate=request.head;
    int sub_url_count=0;
    while (iterate) {
      sub_url[sub_url_count]=trim_cJSON_to_char(iterate);
      debug_print("main");
      debug_print(sub_url[sub_url_count]);

      /* Check sub url exist */
      if ((*ilo_service_table[option.service_idx-1].service_sub_url) != '\0') {
        struct restful_requests request1;

        char tmp_sub_url[ILO_SERVICE_SUB_URL_LEN];
        memcpy(tmp_sub_url,ilo_service_table[option.service_idx-1].service_sub_url, \
               sizeof(ilo_service_table[option.service_idx-1].service_sub_url));
        debug_print(tmp_sub_url);

        // Call sub
        ret=init_curl(&option, &request1, sub_url[sub_url_count], tmp_sub_url, &n_status);

        if ( n_status != NAGIOS_UNKNOWN && ret == 0 ) {
          char *long_url = trim_cJSON_to_char(request1.head);
          debug_print("Sub1 URL");
          debug_print(long_url);
          sub_url[sub_url_count]=long_url;
        }
        cJSON_Delete(request1.cJSON_list.root);
        free_cJSON_list(&request1);
      }

      debug_print("request2");
      struct restful_requests request2;

      //Call service url
      ret=init_curl(&option, &request2, sub_url[sub_url_count], NULL, &n_status);
      if ( n_status != NAGIOS_UNKNOWN && ret == 0 ) {
        switch (option.service_idx) {
        /* system health, memory and processor in one restful request */
        case 1:
          printf(";");
          recover_and_get(&option, &request2, 3);
          printf(";");
          recover_and_get(&option, &request2, 6);
          break;
        /* Fan and Temperature in one restful request */
        case 2:
          if (NAGIOS_OK != n_status) {
            get_detail_error(&option, &request2, FANSTATE_ID, FANNAME_ID);
          }
          printf(";");
          int t_status=recover_and_get(&option, &request2, 8);
          if (NAGIOS_OK != t_status) {
            get_detail_error(&option, &request2, ThermalSTATE_ID, ThermalNAME_ID);
          }
          break;
        /* Deal with Memory info */
        case 3:
          if (NAGIOS_OK != n_status) {
            recover_cJSON_list(&request2);
            option.service_idx=MEMORYINFO_ID;
            char tmp_sub_url[ILO_SERVICE_SUB_URL_LEN];
            memcpy(tmp_sub_url,ilo_service_table[option.service_idx-1].service_sub_url, \
            sizeof(ilo_service_table[option.service_idx-1].service_sub_url));
            debug_print(tmp_sub_url);

            get_service_info(&option, &request2, tmp_sub_url);
            char *memory_sub_url = trim_cJSON_to_char(request2.head);
            debug_print(memory_sub_url);
            
            debug_print("request3");
            struct restful_requests request3;

            enum Nagios_status n_status_ext = NAGIOS_UNKNOWN;

            //Get each memory url
            ret=init_curl(&option, &request3, memory_sub_url, NULL, &n_status_ext);
            if ( n_status_ext != NAGIOS_UNKNOWN && ret == 0 ) {
              struct ilo_cJSON_list *iterate3;
              iterate3=request3.head;
              int memory_sub_url_count=0;
              char erritem[MSG_MAX]=": [";

              while (iterate3) {
                char *memory_sub_url1 = trim_cJSON_to_char(iterate3);
                debug_print(memory_sub_url1);
            
                debug_print("request4");
                struct restful_requests request4;
                option.service_idx=MEMORYSLOT_ID;

                ret=init_curl(&option, &request4, memory_sub_url1, NULL, &n_status_ext);
                if ( n_status_ext != NAGIOS_UNKNOWN && ret == 0 ) {
                  char *memory_info = trim_cJSON_to_char(request4.head);
                  debug_print(memory_info);
                  strcat(erritem,memory_info);
                  strcat(erritem," - ");

                  recover_and_get(&option, &request4, MEMORYSTATE_ID);
                  char *memory_value = trim_cJSON_to_char(request4.head);
                  debug_print(memory_value);
                  strcat(erritem,memory_value);
                  strcat(erritem,",");
                 }
                iterate3=iterate3->next;
              }//while
              erritem[strlen(erritem)-1]=']';
              printf("%s",erritem);
            }
          }
          break;
        case 8:
          recover_cJSON_list(&request2);
          if (NAGIOS_OK != n_status) {
            get_detail_error(&option, &request2, ThermalSTATE_ID, ThermalNAME_ID);
          }
          break;
        }//switch
      } else {
        exit(NAGIOS_UNKNOWN);
      }

      cJSON_Delete(request2.cJSON_list.root);
      free_cJSON_list(&request2);

      sub_url_count++;
      iterate=iterate->next;
    }
  } else if ( ret == 0 )//First URL
    n_status=NAGIOS_UNKNOWN;

  cJSON_Delete(request.cJSON_list.root);
  free_cJSON_list(&request);

  return n_status;
}
