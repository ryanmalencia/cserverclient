/*
 *  This file is for use by students to define anything they wish.  It is used by the gf server implementation
 */
#ifndef __GF_SERVER_STUDENT_H__
#define __GF_SERVER_STUDENT_H__

#include "gf-student.h"
#include "gfserver.h"

#define PATH_BUFFER_SIZE 2048

struct gfserver_t{
    unsigned short port;
    int maxconn;
    gfh_error_t (*handler_callBack)(gfcontext_t **, const char *, void*);
    void * handlerArg;
};

struct gfcontext_t{
        int socketNo;
     
};

#endif // __GF_SERVER_STUDENT_H__