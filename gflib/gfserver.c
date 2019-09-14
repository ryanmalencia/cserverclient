
#include "gfserver-student.h"

/* 
 * Modify this file to implement the interface specified in
 * gfserver.h.
 */

void gfs_abort(gfcontext_t **ctx){
    gfcontext_t cont = **ctx;
    printf("gfs_abort\n");
    close(cont.socketNo);
    cont.socketNo = -1;
    return;
}

gfserver_t* gfserver_create(){
    gfserver_t *initgfs = (gfserver_t*)malloc(sizeof(gfserver_t));
    (*initgfs).handler_callBack = NULL;
    (*initgfs).handlerArg = NULL;
    return initgfs;
}

ssize_t gfs_send(gfcontext_t **ctx, const void *data, size_t len){
    ssize_t sent=0;
    ssize_t sentTot;
    const char * data2;
    gfcontext_t cont = **ctx;


        fprintf(stdout,"sending msg\n");

        sent = send(cont.socketNo,data,len,0);
        printf("len %ld\n",len);
        printf("sent %ld \n",sent);
        sentTot = sent;
        while (sentTot<len){
           data2 = data + sent;
           printf("before send\n");
           //printf("%u  %u\n",&data[0],&data2[0]);
           sent = send(cont.socketNo,data2,len-sent,0);
           sentTot = sent + sentTot;
           printf("sent %ld \n",sent);
           if (sent<0){
               perror("sent");
           }
        }

        fprintf(stdout, "returning sent %ld\n", sentTot);
    return sentTot;
}

ssize_t gfs_sendheader(gfcontext_t **ctx, gfstatus_t status, size_t file_len){

    ssize_t sent=0;
    gfcontext_t cont = **ctx;
    const char *err = "GETFILE ERROR";
    const char *invalid = "GETFILE INVALID";
    const char *FNF = "GETFILE FILE_NOT_FOUND";
    char ok[100];

    printf("status %d\n ",status);

    if(status==GF_OK){
        memset(&ok,0,100);
        fprintf(stdout,"GETFILE OK %zu\r\n\r\n",file_len);
        sprintf(ok,"GETFILE OK %zu\r\n\r\n",file_len);
        fprintf(stdout,"ok   %s\n",ok);
        sent = send(cont.socketNo,ok,strlen(ok),0);
        while (sent<strlen(ok)){
            sent = sent + send(cont.socketNo,&ok[sent],strlen(ok)-sent,0);
        }

    }
    else if(status==GF_FILE_NOT_FOUND){
        fprintf(stdout,"GETFILE FILE_NOT_FOUND");
        sent = send(cont.socketNo,FNF,strlen(FNF),0);
        while (sent<strlen(FNF)){
            sent = sent + send(cont.socketNo,&FNF[sent],strlen(FNF)-sent,0);
        }
    }
    else if(status==GF_INVALID){
        fprintf(stdout,"GETFILE INVALID");
        sent = send(cont.socketNo,invalid,strlen(invalid),0);
        while (sent<strlen(invalid)){
            sent = sent + send(cont.socketNo,&invalid[sent],strlen(invalid)-sent,0);
        }
    }
    else if(status==GF_ERROR){
        fprintf(stdout,"GETFILE ERROR");
        sent = send(cont.socketNo,err,strlen(err),0);
        while (sent<strlen(err)){
            sent = sent + send(cont.socketNo,&err[sent],strlen(err)-sent,0);
        }
    }

    fprintf(stdout, "returning sent %ld\n", sent);
    return sent;
}

void gfserver_serve(gfserver_t **gfs){

    gfcontext_t *pContext;
    //const char * path;
    int servSocket = 0;
    int opt = 1,err = 0;
    char msg[BUFSIZE];
    char path[PATH_BUFFER_SIZE];
    int leng,sendLen,j;
    char * temp;
    struct addrinfo hints, *pResult = NULL, *pInfo = NULL;
    struct sockaddr_in info;
    char port[10];

    memset(&msg, 0, BUFSIZE * sizeof(char));
    memset(&path, 0, PATH_BUFFER_SIZE * sizeof(char));
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    sprintf(port, "%d", (**gfs).port);
    err = getaddrinfo("localhost", port, &hints, &pResult);
    if(err != 0) {
        fprintf(stderr, "Failed to get addr info: %s\n", gai_strerror(err));
        return;
    }

    for(pInfo = pResult; pInfo != NULL; pInfo = pInfo->ai_next) {
	servSocket = socket(pInfo->ai_family, pInfo->ai_socktype, pInfo->ai_protocol);
	if(servSocket == -1) continue;
	setsockopt(servSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if(bind(servSocket, pInfo->ai_addr, pInfo->ai_addrlen) == 0) break;
	close(servSocket);
    }

    if(pInfo == NULL) {
        fprintf(stderr, "Could not bind to available addresses\n");
    }

    freeaddrinfo(pResult);

    void *arg = (**gfs).handlerArg;
    int addrSize = sizeof(struct sockaddr_in);
    int valid = 0;

    pContext = (gfcontext_t *)malloc(sizeof(gfcontext_t));

    setbuf(stdout, NULL); // disable buffering

    getsockname(servSocket,(struct sockaddr *)&info,(socklen_t *)&addrSize);

    err = listen(servSocket,(**gfs).maxconn);
    if(err) {
        fprintf(stderr, "Error listening on socket: %s\n", strerror(errno));
    }
    sendLen = 0;
    sendLen = sendLen;

    while(1){

        printf("waiting to accept new connection\n");

        //connect to client
        //printf("%d\n",servSocket);

        pContext->socketNo = accept(servSocket, (struct sockaddr*)&info, (socklen_t *)&addrSize);
        //end code reuse

        printf("conn accepted\n");

        if(pContext->socketNo < 0){
            perror("accept");
            return;
        }

        //recieve request string
        int totLen = 0;
        valid = 0;
        leng = recv(pContext->socketNo, msg, BUFSIZE-1, 0);
        totLen = leng;
        printf("leng %d totLen%d\n",leng,totLen);
        while (leng > 0 && totLen < 13){
            temp = msg + leng;
            leng = recv(pContext->socketNo, temp, BUFSIZE-1-totLen, 0);
            totLen = leng + totLen;
            printf("leng %d totLen%d\n",leng,totLen);

        }

	if(totLen < 13) {
            close(pContext->socketNo);
	    pContext->socketNo = -1;
	}

        msg[totLen]='\0';
        printf("leng %d totLen%d\n",leng,totLen);
        printf("%s\n",msg);
        if (strncmp("GETFILE GET /",msg,13)!=0){
            printf("invalid scheme\n");
            sendLen = send(pContext->socketNo,"GETFILE INVALID",15,0);
            //invalid request
            close(pContext->socketNo);
            pContext->socketNo = -1;
        }
        else{
            j = 0;
            memset(&path,0,PATH_BUFFER_SIZE);
            path[j++]='/';
            for(int i=13;i<totLen;i++){
                printf("%c %d\n",msg[i],i);
                if (msg[i]=='\r' && msg[i+1]=='\n' && msg[i+2]=='\r' && msg[i+3]=='\n'){
                    //valid
                    printf("valid1\n");
                    path[j]='\0';
                    valid = 1;
                    break;

                }
                else if (msg[i]=='\r'){
                    sendLen = send(pContext->socketNo,"GETFILE INVALID",15,0);
                    //invalid request
                    close(pContext->socketNo);
                    pContext->socketNo = -1;

                }
                else{
                    path[j++]=msg[i];
                }
            }
            while (valid==0 && leng!=0 && totLen<(PATH_BUFFER_SIZE)){
                leng = recv(pContext->socketNo, msg, BUFSIZE-1, 0);
                msg[leng]='\0';
                totLen = totLen + leng;
                printf("try again\n");
                printf("%s\n",msg);
                printf("leng %d totlen %d\n",leng,totLen);
                for(int i=0;i<leng;i++){
                    fprintf(stdout, "hey %c %d\n",msg[i],i);
                    if (msg[i]=='\r' && msg[i+1]=='\n' && msg[i+2]=='\r' && msg[i+3]=='\n'){
                        //valid
                        printf("valid2\n");
                        printf("j %d",j);
                        path[j]='\0';
                        valid = 1;
                        break;

                    }
                    else if (msg[i]=='\r'){
                      sendLen = send(pContext->socketNo,"GETFILE INVALID",15,0);
                     //invalid request
                        close(pContext->socketNo);
                        pContext->socketNo = -1;

                    }
                    else{
                        path[j++]=msg[i];
                    }
                }

            }
            if (valid==1){
                fprintf(stdout,"valid\n");
                fprintf(stdout,"%s\n",path);
                if ((**gfs).handler_callBack != NULL){
                   ((**gfs).handler_callBack)(&pContext,path,arg);
                   printf("exiting handler\n");
                }   
                close(pContext->socketNo);
                printf("closed socket\n");
                printf("set sock to -1\n");
                pContext->socketNo = -1;
            }
            else{
                sendLen = send(pContext->socketNo,"GETFILE INVALID",15,0);
                //invalid request
                close(pContext->socketNo);
                pContext->socketNo = -1;
            }
        }
    }
}

void gfserver_set_handlerarg(gfserver_t **gfs, void* arg){
    (**gfs).handlerArg = arg;
    return;
}

void gfserver_set_handler(gfserver_t **gfs, gfh_error_t (*handler)(gfcontext_t **, const char *, void*)){
    (**gfs).handler_callBack = handler;
    return;

}

void gfserver_set_maxpending(gfserver_t **gfs, int max_npending){
    (**gfs).maxconn = max_npending;
    return;

}

void gfserver_set_port(gfserver_t **gfs, unsigned short port){
    (**gfs).port = port;
    return;

}


