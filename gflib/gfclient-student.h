/*
 *  This file is for use by students to define anything they wish.  It is used by the gf client implementation
 */
 #ifndef __GF_CLIENT_STUDENT_H__
 #define __GF_CLIENT_STUDENT_H__
 
 #include "gfclient.h"
 #include "gf-student.h"

 struct gfcrequest_t {
  const char* server;
  unsigned short portno;
  int gfcSocket;
  const char* filename;
  void (*write_callBack) (void*, size_t, void *);
  void (*header_callBack)(void*, size_t, void *);
  void *writerarg;
  void *headerarg;
  unsigned long headerlen;
  int bytesrec;
  gfstatus_t servStat;

};

#define MAX_REQUEST_LEN 128

 
 #endif // __GF_CLIENT_STUDENT_H__