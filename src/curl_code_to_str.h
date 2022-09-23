/*
 * curl_code_to_str.h
 *
 *  Created on: Sep 23, 2022
 *      Author: mateusz
 */

#ifndef CURL_CODE_TO_STR_H_
#define CURL_CODE_TO_STR_H_

#include <string>
#include "curl/curl.h"

static std::string curlCodeToStr(CURLcode code) {

	switch (code) {
	case CURLE_OK: return "CURLE_OK";
	case CURLE_UNSUPPORTED_PROTOCOL: return "CURLE_UNSUPPORTED_PROTOCOL";   /* 1 */
	case  CURLE_FAILED_INIT: return "CURLE_FAILED_INIT";             /* 2 */
	case   CURLE_URL_MALFORMAT: return "CURLE_URL_MALFORMAT";           /* 3 */
	case  CURLE_NOT_BUILT_IN: return "CURLE_NOT_BUILT_IN";            /* 4 - [was obsoleted in August 2007 for
	                                    7.17.0, reused in April 2011 for 7.21.5] */
	case   CURLE_COULDNT_RESOLVE_PROXY: return "CURLE_COULDNT_RESOLVE_PROXY";   /* 5 */
	case   CURLE_COULDNT_RESOLVE_HOST: return "CURLE_COULDNT_RESOLVE_HOST";    /* 6 */
	case   CURLE_COULDNT_CONNECT: return "CURLE_COULDNT_CONNECT";         /* 7 */
	case  CURLE_WEIRD_SERVER_REPLY: return "CURLE_WEIRD_SERVER_REPLY";      /* 8 */
	case CURLE_REMOTE_ACCESS_DENIED: return "CURLE_REMOTE_ACCESS_DENIED";    /* 9 a service was denied by the server
	                                    due to lack of access - when login fails
	                                    this is not returned. */
	case CURLE_FTP_ACCEPT_FAILED: return "CURLE_FTP_ACCEPT_FAILED";       /* 10 - [was obsoleted in April 2006 for
	                                    7.15.4, reused in Dec 2011 for 7.24.0]*/
	case CURLE_FTP_WEIRD_PASS_REPLY: return "CURLE_FTP_WEIRD_PASS_REPLY";    /* 11 */
	case CURLE_FTP_ACCEPT_TIMEOUT: return "CURLE_FTP_ACCEPT_TIMEOUT";      /* 12 - timeout occurred accepting server
	                                    [was obsoleted in August 2007 for 7.17.0,
	                                    reused in Dec 2011 for 7.24.0]*/
	case  CURLE_FTP_WEIRD_PASV_REPLY: return "CURLE_FTP_WEIRD_PASV_REPLY";    /* 13 */
	case  CURLE_FTP_WEIRD_227_FORMAT: return "CURLE_FTP_WEIRD_227_FORMAT";    /* 14 */
	case  CURLE_FTP_CANT_GET_HOST: return "CURLE_FTP_CANT_GET_HOST";       /* 15 */
	case  CURLE_HTTP2: return "CURLE_HTTP2";                   /* 16 - A problem in the http2 framing layer.
	                                    [was obsoleted in August 2007 for 7.17.0,
	                                    reused in July 2014 for 7.38.0] */
	case  CURLE_FTP_COULDNT_SET_TYPE: return "CURLE_FTP_COULDNT_SET_TYPE";    /* 17 */
	case CURLE_PARTIAL_FILE: return "CURLE_PARTIAL_FILE";            /* 18 */
	case CURLE_FTP_COULDNT_RETR_FILE: return "CURLE_FTP_COULDNT_RETR_FILE";   /* 19 */
	case CURLE_OBSOLETE20: return "CURLE_OBSOLETE20";              /* 20 - NOT USED */
	case CURLE_QUOTE_ERROR: return "CURLE_QUOTE_ERROR";             /* 21 - quote command failure */
	case  CURLE_HTTP_RETURNED_ERROR: return "CURLE_HTTP_RETURNED_ERROR";     /* 22 */
	case  CURLE_WRITE_ERROR: return "CURLE_WRITE_ERROR";             /* 23 */
	case  CURLE_OBSOLETE24: return "CURLE_OBSOLETE24";              /* 24 - NOT USED */
	case CURLE_UPLOAD_FAILED: return "CURLE_UPLOAD_FAILED";           /* 25 - failed upload "command" */
	case  CURLE_READ_ERROR: return "CURLE_READ_ERROR";              /* 26 - couldn't open/read from file */
	case  CURLE_OUT_OF_MEMORY: return "CURLE_OUT_OF_MEMORY";           /* 27 */
	  /* Note: CURLE_OUT_OF_MEMORY may sometimes indicate a conversion error
	           instead of a memory allocation error if CURL_DOES_CONVERSIONS
	           is defined
	  */
	case CURLE_OPERATION_TIMEDOUT: return "CURLE_OPERATION_TIMEDOUT";      /* 28 - the timeout time was reached */
	case CURLE_OBSOLETE29: return "CURLE_OBSOLETE29";              /* 29 - NOT USED */
	case CURLE_FTP_PORT_FAILED: return "CURLE_FTP_PORT_FAILED";         /* 30 - FTP PORT operation failed */
	case CURLE_FTP_COULDNT_USE_REST: return "CURLE_FTP_COULDNT_USE_REST";    /* 31 - the REST command failed */
	case CURLE_OBSOLETE32: return "CURLE_OBSOLETE32";              /* 32 - NOT USED */
	case CURLE_RANGE_ERROR: return "CURLE_RANGE_ERROR";             /* 33 - RANGE "command" didn't work */
	case  CURLE_HTTP_POST_ERROR: return "CURLE_HTTP_POST_ERROR";         /* 34 */
	case CURLE_SSL_CONNECT_ERROR: return "CURLE_SSL_CONNECT_ERROR";       /* 35 - wrong when connecting with SSL */
	case  CURLE_BAD_DOWNLOAD_RESUME: return "CURLE_BAD_DOWNLOAD_RESUME";     /* 36 - couldn't resume download */
	case  CURLE_FILE_COULDNT_READ_FILE: return "CURLE_FILE_COULDNT_READ_FILE";  /* 37 */
	case CURLE_LDAP_CANNOT_BIND: return "CURLE_LDAP_CANNOT_BIND";        /* 38 */
	case CURLE_LDAP_SEARCH_FAILED: return "CURLE_LDAP_SEARCH_FAILED";      /* 39 */
	case  CURLE_OBSOLETE40: return "CURLE_OBSOLETE40";              /* 40 - NOT USED */
	case  CURLE_FUNCTION_NOT_FOUND: return "CURLE_FUNCTION_NOT_FOUND";      /* 41 - NOT USED starting with 7.53.0 */
	case  CURLE_ABORTED_BY_CALLBACK: return "CURLE_ABORTED_BY_CALLBACK";     /* 42 */
	case  CURLE_BAD_FUNCTION_ARGUMENT: return "CURLE_BAD_FUNCTION_ARGUMENT";   /* 43 */
	case CURLE_OBSOLETE44: return "CURLE_OBSOLETE44";              /* 44 - NOT USED */
	case  CURLE_INTERFACE_FAILED: return "CURLE_INTERFACE_FAILED";        /* 45 - CURLOPT_INTERFACE failed */
	case  CURLE_OBSOLETE46: return "CURLE_OBSOLETE46";              /* 46 - NOT USED */
	case  CURLE_TOO_MANY_REDIRECTS: return "CURLE_TOO_MANY_REDIRECTS";      /* 47 - catch endless re-direct loops */
	case  CURLE_UNKNOWN_OPTION: return "CURLE_UNKNOWN_OPTION";          /* 48 - User specified an unknown option */
	case  CURLE_TELNET_OPTION_SYNTAX: return "CURLE_TELNET_OPTION_SYNTAX";    /* 49 - Malformed telnet option */
	case  CURLE_OBSOLETE50: return "CURLE_OBSOLETE50";              /* 50 - NOT USED */
	case  CURLE_OBSOLETE51: return "CURLE_OK";              /* 51 - NOT USED */
	case  CURLE_GOT_NOTHING: return "CURLE_OBSOLETE51";             /* 52 - when this is a specific error */
	case  CURLE_SSL_ENGINE_NOTFOUND: return "CURLE_SSL_ENGINE_NOTFOUND";     /* 53 - SSL crypto engine not found */
	case  CURLE_SSL_ENGINE_SETFAILED: return "CURLE_SSL_ENGINE_SETFAILED";    /* 54 - can not set SSL crypto engine as
	                                    default */
	case CURLE_SEND_ERROR: return "CURLE_SEND_ERROR";              /* 55 - failed sending network data */
	case CURLE_RECV_ERROR: return "CURLE_RECV_ERROR";              /* 56 - failure in receiving network data */
	case CURLE_OBSOLETE57: return "CURLE_OBSOLETE57";              /* 57 - NOT IN USE */
	case CURLE_SSL_CERTPROBLEM: return "CURLE_SSL_CERTPROBLEM";         /* 58 - problem with the local certificate */
	case CURLE_SSL_CIPHER: return "CURLE_SSL_CIPHER";              /* 59 - couldn't use specified cipher */
	case  CURLE_PEER_FAILED_VERIFICATION: return "CURLE_PEER_FAILED_VERIFICATION"; /* 60 - peer's certificate or fingerprint
	                                     wasn't verified fine */
	case  CURLE_BAD_CONTENT_ENCODING: return "CURLE_BAD_CONTENT_ENCODING";    /* 61 - Unrecognized/bad encoding */
	case  CURLE_LDAP_INVALID_URL: return "CURLE_LDAP_INVALID_URL";        /* 62 - Invalid LDAP URL */
	case  CURLE_FILESIZE_EXCEEDED: return "CURLE_FILESIZE_EXCEEDED";       /* 63 - Maximum file size exceeded */
	case  CURLE_USE_SSL_FAILED: return "CURLE_USE_SSL_FAILED";          /* 64 - Requested FTP SSL level failed */
	case CURLE_SEND_FAIL_REWIND: return "CURLE_SEND_FAIL_REWIND";        /* 65 - Sending the data requires a rewind
	                                    that failed */
	case  CURLE_SSL_ENGINE_INITFAILED: return "CURLE_SSL_ENGINE_INITFAILED";   /* 66 - failed to initialise ENGINE */
	case  CURLE_LOGIN_DENIED: return "CURLE_LOGIN_DENIED";           /* 67 - user, password or similar was not
	                                    accepted and we failed to login */
	case  CURLE_TFTP_NOTFOUND: return "CURLE_TFTP_NOTFOUND";           /* 68 - file not found on server */
	case  CURLE_TFTP_PERM: return "CURLE_TFTP_PERM";               /* 69 - permission problem on server */
	case  CURLE_REMOTE_DISK_FULL: return "CURLE_REMOTE_DISK_FULL";        /* 70 - out of disk space on server */
	case  CURLE_TFTP_ILLEGAL: return "CURLE_OK";            /* 71 - Illegal TFTP operation */
	case  CURLE_TFTP_UNKNOWNID: return "CURLE_TFTP_UNKNOWNID";          /* 72 - Unknown transfer ID */
	case  CURLE_REMOTE_FILE_EXISTS: return "CURLE_REMOTE_FILE_EXISTS";      /* 73 - File already exists */
	case  CURLE_TFTP_NOSUCHUSER: return "CURLE_TFTP_NOSUCHUSER";         /* 74 - No such user */
	case  CURLE_CONV_FAILED: return "CURLE_CONV_FAILED";             /* 75 - conversion failed */
	case  CURLE_CONV_REQD: return "CURLE_CONV_REQD";               /* 76 - caller must register conversion
	                                    callbacks using curl_easy_setopt options
	                                    CURLOPT_CONV_FROM_NETWORK_FUNCTION,
	                                    CURLOPT_CONV_TO_NETWORK_FUNCTION, and
	                                    CURLOPT_CONV_FROM_UTF8_FUNCTION */
	case  CURLE_SSL_CACERT_BADFILE: return "CURLE_SSL_CACERT_BADFILE";      /* 77 - could not load CACERT file, missing
	                                    or wrong format */
	case  CURLE_REMOTE_FILE_NOT_FOUND: return "CURLE_REMOTE_FILE_NOT_FOUND";   /* 78 - remote file not found */
	case  CURLE_SSH: return "CURLE_SSH";                     /* 79 - error from the SSH layer, somewhat
	                                    generic so the error message will be of
	                                    interest when this has happened */

	case  CURLE_SSL_SHUTDOWN_FAILED: return "CURLE_SSL_SHUTDOWN_FAILED";     /* 80 - Failed to shut down the SSL
	                                    connection */
	case CURLE_AGAIN: return "CURLE_AGAIN";                   /* 81 - socket is not ready for send/recv,
	                                    wait till it's ready and try again (Added
	                                    in 7.18.2) */
	case  CURLE_SSL_CRL_BADFILE: return "CURLE_SSL_CRL_BADFILE";         /* 82 - could not load CRL file, missing or
	                                    wrong format (Added in 7.19.0) */
	case  CURLE_SSL_ISSUER_ERROR: return "CURLE_SSL_ISSUER_ERROR";        /* 83 - Issuer check failed.  (Added in
	                                    7.19.0) */
	case  CURLE_FTP_PRET_FAILED: return "CURLE_FTP_PRET_FAILED";         /* 84 - a PRET command failed */
	case CURLE_RTSP_CSEQ_ERROR: return "CURLE_RTSP_CSEQ_ERROR";         /* 85 - mismatch of RTSP CSeq numbers */
	case  CURLE_RTSP_SESSION_ERROR: return "CURLE_RTSP_SESSION_ERROR";      /* 86 - mismatch of RTSP Session Ids */
	case CURLE_FTP_BAD_FILE_LIST: return "CURLE_FTP_BAD_FILE_LIST";       /* 87 - unable to parse FTP file list */
	case  CURLE_CHUNK_FAILED: return "CURLE_OK";            /* 88 - chunk callback reported error */
	case  CURLE_NO_CONNECTION_AVAILABLE: return "CURLE_NO_CONNECTION_AVAILABLE"; /* 89 - No connection available, the
	                                    session will be queued */
	case  CURLE_SSL_PINNEDPUBKEYNOTMATCH: return "CURLE_SSL_PINNEDPUBKEYNOTMATCH"; /* 90 - specified pinned public key did not
	                                     match */
	case CURLE_SSL_INVALIDCERTSTATUS: return "CURLE_SSL_INVALIDCERTSTATUS";   /* 91 - invalid certificate status */
	case CURLE_HTTP2_STREAM: return "CURLE_HTTP2_STREAM";            /* 92 - stream error in HTTP/2 framing layer
	                                    */
	case CURLE_RECURSIVE_API_CALL: return "CURLE_RECURSIVE_API_CALL";      /* 93 - an api function was called from
	                                    inside a callback */
	default: return "";
	}

}


#endif /* CURL_CODE_TO_STR_H_ */
