#ifndef __LOADED_STATION__
    #define __LOADED_STATION__

    #include "../wifi/wifi.h"
    #include <pthread.h>


    struct station {
        byte mac[6];
        struct station* next;
    };


    void deep_station_free(struct station* station);
    struct station* find_station(char* iface, int freq, byte bssid[6]);
#endif

