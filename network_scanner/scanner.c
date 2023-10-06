#include "scanner.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>



char is_sniffing = 1;

char is_beacon(uint16_t f_control) {
    uint8_t subtype = (f_control & FRAME_CONTROL_SUBTYPE_MASK) >> 12;
    uint8_t type = (f_control & FRAME_CONTROL_TYPE_MASK) >> 10;


    if(subtype == FRAME_CONTROL_BEACON && type == 0) {
        return 1;
    }

    return 0;
}



void trim(char* ssid) {
    int len = strlen(ssid);
    for(int i = len - 1; i >=0; --i) {
        if(ssid[i] == ' ' || ssid[i] == '\n' || ssid[i] == '\t') {
            ssid[i] = '\0';
        }else {
            return;
        }
    }
}

void* pthread_change(void* arg) {
    usleep(500 * 1000);
    is_sniffing = 0;
    pthread_exit(NULL);
}


void create_thread() {
    pthread_t thread;

    if(pthread_create(&thread,NULL,pthread_change,NULL) != 0) {
        printf("Thread error\n");
    }
}

char contains_network(struct wifi_network* networks, char* ssid) {
    while(networks != NULL) {
        if(strcmp(networks->ssid,ssid) == 0){
            return 1;
        }
        networks = networks->next;
    }
    return 0;

}

void add_network(struct wifi_network* networks, struct wifi_network* network) {
    while(networks->next != NULL) {
        networks = networks->next;
    }
    networks->next = network;
}

void sniff_beacons(char* iname, struct wifi_network* networks, int freq) {
    byte buffer[MAX_BUFFER_SIZE];    
    int sock = sock_init(iname,1);

    is_sniffing = 1;
    create_thread();
    while(is_sniffing) {
        //printf("%d\n",freq);
        int bytes_recieved = recv(sock,buffer,MAX_BUFFER_SIZE,0);

        byte* tmp_buff = buffer;

        uint16_t size = *((uint16_t*)(tmp_buff + 2));
        tmp_buff += size;

        struct iee_802_11_hdr* wifi_hdr = (struct iee_802_11_hdr*)(tmp_buff);
        wifi_hdr->frame_control = ntohs(wifi_hdr->frame_control);
        tmp_buff += sizeof(struct iee_802_11_hdr);
        tmp_buff += FIXED_PARAMETERS_SIZE;

        
        if(is_beacon(wifi_hdr->frame_control)) {
            while(tmp_buff < buffer+ bytes_recieved) {
                struct iee_tagged_param* param = (struct iee_tagged_param*)tmp_buff;
            
                tmp_buff += 2;
                if(param->tag != SSID_TAG) {
                    tmp_buff += param->length;
                }else {
                    char* ssid = (char*)malloc(param->length);
                    strncpy(ssid,tmp_buff,param->length);
                    //printf("%s\n",ssid);
                    if(!contains_network(networks,ssid)) {
                        struct wifi_network* network = (struct wifi_network*)malloc(sizeof(struct wifi_network));
                        strcpy(network->ssid,ssid);
                        for(int i=0; i < 6; i++) {
                            network->ap_hwaddr[i] = wifi_hdr->ap[i];
                        }
                        network->freq = freq;
                        trim(network->ssid);
                        add_network(networks,network);
                    }
                    free(ssid);
                }
            }
        }
    }

    close(sock);
}

struct wifi_network* wifi_scan(char* iname) {
    iface_toogle(iname,0);
    toggle_monitor(iname,1);
    iface_toogle(iname,1);
    struct wifi_network* networks = (struct wifi_network*)malloc(sizeof(struct wifi_network));
    memset(networks,0,sizeof networks);
    for(int i=2412; i <=2484; i+=5 ){
        if(channel_switch(iname,i) == -1) {
            continue;
        }
        sniff_beacons(iname,networks,i); 
    }
    for(int i = 5180; i <= 5680; i += 20) {
        if(channel_switch(iname,i) == -1) {
            continue;
        }

        sniff_beacons(iname,networks,i);
    }

    iface_toogle(iname,0);
    toggle_monitor(iname,0);
    iface_toogle(iname,1);
    add_network(networks,NULL);

    struct wifi_network* tmp = networks->next;
    free(networks);

    return tmp;
}