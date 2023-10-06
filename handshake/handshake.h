#ifndef __LOADED_HANDSHAKE__
    #define __LOADED_HANDSHAKE__
    #include "../wifi/wifi.h"


    struct response_chunk_ll {
        byte ended;
        byte smac[6];
        byte amac[6];
        byte anonce[32];
        byte snonce[32];
        byte mic[16];
        byte iv[16];
    };

    typedef void (*hcallback)(struct response_chunk_ll*);
    void sniff_handshake(char* iface,byte bssid[6], byte station[6], hcallback,char* ended);
#endif