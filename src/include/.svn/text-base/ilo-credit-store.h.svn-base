#ifndef _ILOCREDITSO_H
#define _ILOCREDITSO_H

#include <getopt.h>

#ifdef __cplusplus
extern "C" {
#endif
  int set_cred(char * host, char * user, char * passwd);
  char *get_cred(char * host);
  int exist_cred(char * host);
  typedef int set_cred_t(char *, char *, char *);
  typedef char *get_cred_t(char *);
  typedef int exist_cred_t(char *);
#ifdef __cplusplus
}
#endif

#endif
