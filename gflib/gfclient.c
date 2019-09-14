
#include "gfclient-student.h"

// optional function for cleaup processing.
void gfc_cleanup(gfcrequest_t **gfr){

    free(*gfr);
    *gfr = NULL;
    return;

}

/* 
 * This function must be the first one called as part of 
 * request.  It returns a gfcrequest_t handle which should be
 * passed into all subsequent library calls pertaining to
 * this requeest.
 */
gfcrequest_t *gfc_create(){
    // dummy for now - need to fill this part in
    gfcrequest_t *initgfc = malloc(sizeof(gfcrequest_t));
    (*initgfc).header_callBack = NULL;
    (*initgfc).write_callBack = NULL;
    (*initgfc).headerlen = 0;
    (*initgfc).bytesrec = 0;
    (*initgfc).servStat = -1;
    
    return initgfc;

}

size_t gfc_get_bytesreceived(gfcrequest_t **gfr){
    // not yet implemented
    return (**gfr).bytesrec;
}

size_t gfc_get_filelen(gfcrequest_t **gfr){
    // not yet implemented
    return (**gfr).headerlen;
}

gfstatus_t gfc_get_status(gfcrequest_t **gfr){
    // not yet implemented
    return (**gfr).servStat;
}

void gfc_global_init(){
}

void gfc_global_cleanup(){
}


/*
 * Performs the transfer as described in the options.  Returns a value of 0
 * if the communication is successful, including the case where the server
 * returns a response with a FILE_NOT_FOUND or ERROR response.  If the 
 * communication is not successful (e.g. the connection is closed before
 * transfer is complete or an invalid header is returned), then a negative 
 * integer will be returned.
 */
int gfc_perform(gfcrequest_t **gfr){
    // currently not implemented.  You fill this part in.

    //struct sockaddr_in IPAddress;
    //struct hostent* hostInfo;
    //long hostAddr;
    char msg[BUFSIZE];
    char request[300];
    int leng = 0;
    int err = 0;
    char status[20];
    char sze[21];
    int bytesread =0;
    char port[10];

    struct addrinfo hints, *pResult, *pInfo;

    fprintf(stdout,"My gfclient\n");

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    sprintf(port,"%d",(**gfr).portno);

    err = getaddrinfo((**gfr).server,port,&hints, &pResult);
    if(err) {
	fprintf(stderr, "Error getting address info: %s\n", gai_strerror(err));
	return err;
    }

    for(pInfo = pResult; pInfo != NULL; pInfo = pInfo->ai_next) {
        (**gfr).gfcSocket = socket(pInfo->ai_family, pInfo->ai_socktype, pInfo->ai_protocol);
	if((**gfr).gfcSocket == -1) continue;
	if(connect((**gfr).gfcSocket, pInfo->ai_addr, pInfo->ai_addrlen) != -1) break;
	close((**gfr).gfcSocket);
    }

    if(pInfo == NULL) {
       fprintf(stderr, "Could not connect\n");
       return -1;
    }
    freeaddrinfo(pResult);
    
    strcpy(request,"GETFILE GET ");
    strcat(request,(**gfr).filename);
    strcat(request,"\r\n\r\n\0");
    //*request = strcat(strcat("GETFILE GET ",(**gfr).filename),"\r\n\r\n\0");
    //fprintf(stdout,"%s\n",request);
    leng = send ((**gfr).gfcSocket,request,strlen(request),0);

    //header receive
    leng = recv((**gfr).gfcSocket,msg,BUFSIZE-1,0);
    //fprintf(stdout,"leng %d\n",leng);
    if (leng>0) {
        //parse header
        if (strncmp("GETFILE ",msg,8)!=0){
            fprintf(stdout,"Wrong scheme response\n");
            close((**gfr).gfcSocket);
            (**gfr).gfcSocket = -1;
            (**gfr).servStat = GF_INVALID;
            return -1;
        }
        for (int i=0;i<19;i++){
            status[i]=msg[8+i];
            status[i+1]='\0';

            if (msg[8+i]==32){
                break;
            }
            
        }
        //fprintf(stdout,"%s\n",status);
        //fprintf(stdout,"%s\n",msg);
        if (strncmp("OK ",status,3)==0){
            int i;
           for (i=0;i<20;i++){
              sze[i]=msg[11+i];
              sze[i+1]='\0';

            if (msg[11+i]==32){
                i++;
                if(msg[11+i]!='\r'){
                    fprintf(stdout,"Invalid header response\n");
                    close((**gfr).gfcSocket);
                    (**gfr).gfcSocket = -1;
                    (**gfr).servStat = GF_INVALID;
                    return -1;
                }
                if(msg[11+i+1]!='\n'){
                    fprintf(stdout,"Invalid header response\n");
                    close((**gfr).gfcSocket);
                    (**gfr).gfcSocket = -1;
                    (**gfr).servStat = GF_INVALID;
                    return -1;
                }
                if(msg[11+i+2]!='\r'){
                    fprintf(stdout,"Invalid header response\n");
                    close((**gfr).gfcSocket);
                    (**gfr).gfcSocket = -1;
                    (**gfr).servStat = GF_INVALID;
                    return -1;
                }
                if(msg[11+i+3]!='\n'){
                    fprintf(stdout,"Invalid header response\n");
                    close((**gfr).gfcSocket);
                    (**gfr).gfcSocket = -1;
                    (**gfr).servStat = GF_INVALID;
                    return -1;
                }
                break;
            }
            else if (msg[11+i]=='\r'){
                if(msg[11+i+1]!='\n'){
                    fprintf(stdout,"Invalid header response\n");
                    close((**gfr).gfcSocket);
                    (**gfr).gfcSocket = -1;
                    (**gfr).servStat = GF_INVALID;
                    return -1;
                }
                if(msg[11+i+2]!='\r'){
                    fprintf(stdout,"Invalid header response\n");
                    close((**gfr).gfcSocket);
                    (**gfr).gfcSocket = -1;
                    (**gfr).servStat = GF_INVALID;
                    return -1;
                }
                if(msg[11+i+3]!='\n'){
                    fprintf(stdout,"Invalid header response\n");
                    close((**gfr).gfcSocket);
                    (**gfr).gfcSocket = -1;
                    (**gfr).servStat = GF_INVALID;
                    return -1;
                }
                break;
            }
            
        }
        if ((**gfr).header_callBack != NULL){
            ((**gfr).header_callBack)(msg,15+i,(**gfr).headerarg);
        }

        if (leng>(11+i+4)){
            if ((**gfr).write_callBack!=NULL){
              ((**gfr).write_callBack)(&(msg[i+15]),leng-(i+15),(**gfr).writerarg);
              bytesread = leng-(i+15);
            } 

        }
        //fprintf(stdout,"%s\n",sze);
        //fprintf(stdout,"OK\n");
        (**gfr).headerlen = atoi(sze);
        (**gfr).servStat = GF_OK;
        //fprintf(stdout,"%ld\n",(**gfr).headerlen);
        //fprintf(stdout,"%zu\n",(**gfr).headerlen);

        }
        else if (strncmp("FILE_NOT_FOUND",status,14)==0)
        {
            (**gfr).servStat = GF_FILE_NOT_FOUND;
            if ((**gfr).header_callBack != NULL){
               ((**gfr).header_callBack)(msg,leng,(**gfr).headerarg);
            }
            close((**gfr).gfcSocket);
            (**gfr).gfcSocket = -1;
            return 0;
        }
        else if (strncmp("ERROR",status,5)==0){
            (**gfr).servStat = GF_ERROR;
            if ((**gfr).header_callBack != NULL){
               ((**gfr).header_callBack)(msg,leng,(**gfr).headerarg);
            }
            close((**gfr).gfcSocket);
            (**gfr).gfcSocket = -1;
            return 0;
        }
        else if (strncmp("INVALID",status,7)==0){
            (**gfr).servStat = GF_INVALID;
            if ((**gfr).header_callBack != NULL){
               ((**gfr).header_callBack)(msg,leng,(**gfr).headerarg);
            }
            close((**gfr).gfcSocket);
            (**gfr).gfcSocket = -1;
            return -1;

        }
        else{
            fprintf(stdout,"Invalid status returned\n");
            close((**gfr).gfcSocket);
            (**gfr).gfcSocket = -1;
            (**gfr).servStat = GF_INVALID;
            return -1;
        }


    }
    //fprintf(stdout,"leng %d\n",leng);
    //fprintf(stdout,"Entering msg content\n");
    while (leng!= 0 && bytesread<(**gfr).headerlen){
        leng = recv((**gfr).gfcSocket,msg,BUFSIZE-1,0);
        bytesread = bytesread + leng;
           // fprintf(stdout,"leng %d\n",leng);
            //fprintf(stdout,"tot %d\n",bytesread);
        if ((**gfr).write_callBack!=NULL){
            ((**gfr).write_callBack)(msg,leng,(**gfr).writerarg);
        }
        if (leng<0){
            perror("recv");
            return leng;
        }
    }



    (**gfr).bytesrec = bytesread;
    close((**gfr).gfcSocket);
    (**gfr).gfcSocket = -1;

    if (bytesread!=(**gfr).headerlen){
        fprintf(stdout,"File incomplete\n");
        return -1;
    }

    return 0;
}

void gfc_set_headerarg(gfcrequest_t **gfr, void *headerarg){
    (**gfr).headerarg = headerarg;
    return;

}

void gfc_set_headerfunc(gfcrequest_t **gfr, void (*headerfunc)(void*, size_t, void *)){

    (**gfr).header_callBack = headerfunc;
    return;

}

void gfc_set_path(gfcrequest_t **gfr, const char* path){
    (**gfr).filename = path;
    return;

}

void gfc_set_port(gfcrequest_t **gfr, unsigned short port){
    (**gfr).portno = port;
    return;

}

void gfc_set_server(gfcrequest_t **gfr, const char* server){

    //set some value in gfr that the server is server
   (**gfr).server = server;
   return;

  
}

void gfc_set_writearg(gfcrequest_t **gfr, void *writearg){
    (**gfr).writerarg = writearg;
    return;

}

void gfc_set_writefunc(gfcrequest_t **gfr, void (*writefunc)(void*, size_t, void *)){

    (**gfr).write_callBack = writefunc;
    return;
  
}

const char* gfc_strstatus(gfstatus_t status){
    const char *strstatus = NULL;

    switch (status)
    {
        default: {
            strstatus = "UNKNOWN";
        }
        break;

        case GF_INVALID: {
            strstatus = "INVALID";
        }
        break;

        case GF_FILE_NOT_FOUND: {
            strstatus = "FILE_NOT_FOUND";
        }
        break;

        case GF_ERROR: {
            strstatus = "ERROR";
        }
        break;

        case GF_OK: {
            strstatus = "OK";
        }
        break;
        
    }

    return strstatus;
}

