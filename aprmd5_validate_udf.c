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

my_bool aprmd5_validate_udf_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void aprmd5_validate_udf_deinit(UDF_INIT *initid __attribute__((unused)));
long long aprmd5_validate_udf(UDF_INIT *initid, UDF_ARGS *args,
              char *is_null, char *error); 
                 
#include <apr.h>
#include <apr_base64.h>
#include <apr_general.h>
#include <apr_md5.h>
#define APR_WANT_STDIO
#include <apr_want.h>   

   
   
my_bool aprmd5_validate_udf_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	if(!(args->arg_count == 2)) {
		strcpy(message, "Expected two argument: password to validate and password to validate against");
		return 1;
	}
 
	if (args->arg_type[0] != STRING_RESULT) {
 		strcpy(message,"First argument is wrong: string required");
 		return 1;
	}
	if (args->arg_type[1] != STRING_RESULT) {
 		strcpy(message,"Second argument is wrong: string required");
 		return 1;
	}
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	apr_initialize();
	
	return 0;
}
   
void aprmd5_udf_deinit(UDF_INIT *initid __attribute__((unused))) {
	apr_terminate();
}
   
long long aprmd5_validate_udf(UDF_INIT *initid, UDF_ARGS *args,
              char *is_null, char *error) {
                     
	const char* passwordToValidate = args->args[0];
	const char* passwordToValidateAgainst = args->args[1];
	if (passwordToValidate==NULL | passwordToValidateAgainst==NULL)
		return 0;
		
	fprintf(stderr, "passwordToValidate %s\n", passwordToValidate);
	fprintf(stderr, "passwordToValidateAgainst %s\n", passwordToValidateAgainst);
	apr_status_t rv;

	rv = apr_password_validate(passwordToValidate, passwordToValidateAgainst);
	if (rv == APR_SUCCESS) {		
		return 1;
  } 
	return 0;
}

#endif /* HAVE_DLOPEN */
