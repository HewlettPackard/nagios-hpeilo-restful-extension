#include<string.h>

#ifdef __cplusplus
extern "C" {
#endif
  int base64_encode(const unsigned char*, size_t, char**);
  int base64_decode(char*, char**);

#ifdef __cplusplus
}
#endif
