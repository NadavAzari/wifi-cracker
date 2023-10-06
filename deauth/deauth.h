#ifndef __LOADED_DEAUTH__
    #define __LOADED_DEAUTH__
    #include "../wifi/wifi.h"

    struct packet* create_deauth_frame(byte bssid[6], byte dst[6]);
#endif