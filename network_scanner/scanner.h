#ifndef __LOADED_SCANNER__

    #define __LOADED_SCANNER__


    #include <stdint.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <pthread.h>


    #include "../wifi/wifi.h"

    #define FRAME_CONTROL_BEACON 0x8

    #define FIXED_PARAMETERS_SIZE 12
    #define SSID_TAG 0


   
    struct wifi_network* wifi_scan(char* iname);


#endif