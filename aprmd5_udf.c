#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;/* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#if defined(MYSQL_SERVER)
#include <m_string.h>/* To get strmov() */
#else
/* when compiled as standalone */
#include <string.h>
#define strmov(a,b) stpcpy(a,b)
#define bzero(a,b) memset(a,0,b)
#define memcpy_fixed(a,b,c) memcpy(a,b,c)
#endif
#endif
#include <mysql.h>
#include <ctype.h>

#ifdef HAVE_DLOPEN

#if !defined(HAVE_GETHOSTBYADDR_R) || !defined(HAVE_SOLARIS_STYLE_GETHOST)
static pthread_mutex_t LOCK_hostname;
#endif

my_bool aprmd5_udf_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void aprmd5_udf_deinit(UDF_INIT *initid __attribute__((unused)));
char* aprmd5_udf(UDF_INIT *initid, UDF_ARGS *args,
          char *result, unsigned long *length,
          char *is_null, char *error);
                     
#include <apr.h>
#include <apr_base64.h>
#include <apr_general.h>
#include <apr_md5.h>
#define APR_WANT_STDIO
#include <apr_want.h>   

#define MAX_STRING_LEN 256
#define SALT_LEN 9        

void randomizeSalt(char *resultSalt) {
  apr_status_t rv;
  char salt[SALT_LEN];
  char b64Salt[apr_base64_encode_len(SALT_LEN)];

  apr_base64_encode(b64Salt, salt, SALT_LEN);
  printf("Salt array before randomization:\t%s\n", b64Salt);
  rv = apr_generate_random_bytes(salt, SALT_LEN - 1);
  salt[SALT_LEN -1] = '\0';

  apr_base64_encode(b64Salt, salt, SALT_LEN);
  printf("Salt array  after randomization:\t%s\n", b64Salt);
  /* As we do just need 8 random characters, we just 
   * copy them (remember the 9th char is set to '\0' earlier
   */
  apr_cpystrn(resultSalt, b64Salt, SALT_LEN);
  return;
}          
   
my_bool aprmd5_udf_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	if(!(args->arg_count == 1)) {
		strcpy(message, "Expected one argument: password to salt");
		return 1;
	}
 
	if (args->arg_type[0] != STRING_RESULT) {
 		strcpy(message,"Wrong argument type: string required");
 		return 1;
	}

	args->arg_type[0] = STRING_RESULT;    
	apr_initialize();
	
	return 0;
}
   
void aprmd5_udf_deinit(UDF_INIT *initid __attribute__((unused))) {
	apr_terminate();
}
   
char* aprmd5_udf(UDF_INIT *initid, UDF_ARGS *args,char *result, unsigned long *length,
          char *is_null, char *error) {
                     
	unsigned char md5Digest[APR_MD5_DIGESTSIZE];
	char salt[9];
	const char* passwordToHash = args->args[0];
	
	if (passwordToHash==NULL)
		return NULL;
	
	unsigned char md5DigestSalted[200];
	apr_status_t rv;
	randomizeSalt(salt);
	rv = apr_md5_encode(passwordToHash, salt, md5DigestSalted, sizeof(md5DigestSalted));
	if (rv == APR_SUCCESS) {
		apr_cpystrn(result,md5DigestSalted,sizeof(md5DigestSalted));

		*length = (uint) strlen(md5DigestSalted);
		return result;
  } else {
    strcpy(error,"Error in apr md5 digest salt ");
  }
	return NULL;
}

#endif /* HAVE_DLOPEN */
