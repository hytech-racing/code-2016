#ifndef arduinoMainHytech
#define arduinoMainHytech



namespace AM {
    //int MAIN_MESSAGE_GET = 0x08D;
    //int MAIN_MESSAGE_SEND = 0x0C8;
    int checkStatus(unsigned char*);
    bool getRegenButton(unsigned char*);
    const int MAIN_MESSAGE_SEND = 0x0C8;
    const int MAIN_MESSAGE_GET = 0x08C;
    

}

#endif
