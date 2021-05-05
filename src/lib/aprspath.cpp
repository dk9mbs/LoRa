#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "aprspath.h"


// unit tests 1
void testParse();

int main() {
    AprsPath pathClass;
    //
    // parse a package
    //
    puts("*** parse ***");
    char package[]="dk9mbs-1>XPRS,WIDE1-1,WIDE2-2,DL0GSH-L1:;AG5ZL    test{001";

    pathClass.parse(package, strlen(package));
    puts("out:");
    puts(pathClass.getSource());
    puts(pathClass.getPath());
    puts(pathClass.getPayloadType());
    puts(pathClass.getPayload());

    //show the destination
    puts(pathClass.getDestination());

    //
    // validate call in path
    //
    int call;
    call=pathClass.contain("DL0GSH-L1");
    if(call==0) puts("not found");
    if(call==1) puts("found");


    //
    // get the number of calls in path
    //
    puts("*** Number of calls in path ***");
    printf("Number of calls in path: %d\r\n", pathClass.count());

    //
    // startswith
    //
    puts("*** startsWith ***");
    int start;
    start=pathClass.startsWith("xl0gsh-L1","D");
    printf("%d\r\n", start);
    start=pathClass.startsWith("SL0GSH-L1","s");
    printf("%d\r\n", start);

}



// lib
int getCallByPos(int &pos, char *call){

    return 0;
}


int AprsPath::callInPath(const char *call) {
    int numberOfCalls=count();

    for(int x=0;x<numberOfCalls;x++){
        puts("");
    }

    return 0;
}



int AprsPath::parse(const char address[], const int len) {
    int section;
    int index;
    index=0;
    section=0; //0=source 1=path 2=type 3=payload

    for (int x=0;x<len;x++){
        char ch;
        ch=address[x];

        if(ch=='>' || ch==':') {
            index=0;
            if(ch=='>') {
                section=1;
            } else if (ch==':') {
                section=2;
            }
        } else {
            if(section==0){
                _source[index]=ch;
            } else if(section==1) {
                _path[index]=ch;
            } else if (section==2) {
                _payloadType[0]=ch;
                section=3;
                index=-1;
            } else if (section==3) {
                _payload[index]=ch;
            }
            index++;
        }
    }
    return -1;
}

char *AprsPath::getDestination() {
    for (int x=0;x<MAX_CALL_SIZE;x++){
        char ch;
        ch=_path[x];
        if(ch!=','){
            _destination[x]=ch;
        } else {
            _destination[x]='\0';
            return _destination;
        }
    }

    return NULL;
}

//
// get the number of calls in the path
//
int AprsPath::count() {
    int count=0;
    for (int x=0;x<MAX_PATH_SIZE;x++) {
        char ch;
        ch=_path[x];
        if(ch==',') {
            count++;
        }
    }
    if(count>0) count++;

    return count;
}

int AprsPath::contain(const char* call) {
    char* pch;

    pch=strstr((char*)_path, (char*)call);
    if(pch!=NULL) {
        return 1;
    } else {
        return 0;
    }
}


int AprsPath::startsWith(const char *item, const char *pre) {
    return _startsWith(item, pre);
}
int AprsPath::endsWith(const char *item, const char *post) {
    return _endsWith(item, post);
}

int AprsPath::_toUpper(const char *cstring, char* upperstr){
    strcpy(upperstr, cstring);
    for (int x=0;x<strlen(upperstr);x++){
        upperstr[x]=toupper(upperstr[x]);
    }

    return 0;
}

int AprsPath::_startsWith(const char *item, const char *pre) {
    int result=0;
    char argItem[MAX_CALL_SIZE];
    char argPre[MAX_CALL_SIZE];

    _toUpper(item, argItem);
    _toUpper(pre, argPre);

    puts(argItem);
    puts(argPre);


    result=strncmp(argItem, argPre, strlen(argPre));
    return result;
}

int AprsPath::_endsWith(const char *item, const char *post) {
    char * pch;
    char argItem[100];
    char argPost[100];

    strcpy(argItem, item);
    strcpy(argPost, post);

    pch = strstr (argItem, argPost);

    if(!pch) {
        return -1;
    }

    if(strcmp(pch,post)==0) {
        return 0;
    } else {
        return -1;
    }

}


// getter and setter
char* AprsPath::getSource(){
    return _source;
}
char* AprsPath::getPath(){
    return _path;
}
char* AprsPath::getPayload(){
    return _payload;
}
char* AprsPath::getPayloadType(){
    return _payloadType;
}



