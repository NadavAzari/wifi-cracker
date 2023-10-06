#ifndef __LOADED_TARGET__
    #define __LOADED_TARGET__
    #include "../wifi/wifi.h"
    #include "../deauth/deauth.h"
    #include "../handshake/handshake.h"
    #include <pthread.h>

    void aim_on(char* iface, int freq, byte client[6], byte bssid[6], struct response_chunk_ll* chunk, char* ended);
#endif