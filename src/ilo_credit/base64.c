/***************************************************************************/
/*(C) Copyright [2016] Hewlett Packard Enterprise Development Company, L.P.*/

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdint.h>
#include "base64.h"

int base64_encode(const unsigned char* buffer, size_t length, char** b64text) { //Encodes a binary safe base 64 string
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
        if (buffer)
		BIO_write(bio, buffer, length);
	else
		return 1;//error
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);
	BIO_free_all(b64);

	if (b64text)
		*b64text=(*bufferPtr).data;
        else
		return 1;//error

	return (0); //success
}
