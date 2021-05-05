#ifndef APRSPATH_H
#define APRSPATH_H

#define MAX_PATH_SIZE 100
#define MAX_CALL_SIZE 20
#define MAX_PAYLOAD_SIZE 250
#define MAX_PAYLOADTYPE_SIZE 2

class AprsPath {
    char _source[MAX_CALL_SIZE]={};
    char _destination[MAX_CALL_SIZE]={};
    char _path[MAX_PATH_SIZE]={};
    char _payload[MAX_PAYLOAD_SIZE]={};
    char _payloadType[MAX_PAYLOADTYPE_SIZE]={};
    //char _pathBackup[MAX_PATH_SIZE]={};

    private:
        int _startsWith(const char *item, const char *pre);
        int _endsWith(const char *item, const char *post);
        int _toUpper(const char *cstring, char *upperstr);

    public:
        char* getSource();
        char* getPath();
        char* getPayload();
        char* getPayloadType();
        char* getDestination();

        int startsWith(const char *item, const char *pre);
        int endsWith(const char *item, const char *post);

        int contain(const char* call);
        int count();
        int parse(const char *fullcall, const int len);

        int callInPath(const char *call);
        int getCallByPos(int &pos, char *call);
};

extern AprsPath AprsPathClass;

#endif
