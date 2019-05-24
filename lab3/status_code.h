// File: status_code.h
// Author: iBug

#ifndef _STATUS_CODE_H
#define _STATUS_CODE_H

/*
#define STATUS_200 "200 OK"
#define STATUS_400 "400 Bad request"
#define STATUS_401 "401 Unauthorized"
#define STATUS_403 "403 Forbidden"
#define STATUS_404 "404 Not Found"
#define STATUS_500 "500 Interal Server Error"
#define STATUS_501 "501 Unsupported"
*/

#define STATUS_200 "200 OK"
#define STATUS_400 STATUS_500
#define STATUS_401 STATUS_500
#define STATUS_403 STATUS_500
#define STATUS_404 "404 Not Found"
#define STATUS_500 "500 Interal Server Error"
#define STATUS_501 STATUS_500

#endif
