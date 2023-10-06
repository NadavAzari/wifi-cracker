#include "station.h"
#include "../target_client/target.h"


char sniffing = 1;

int check_and_add(struct station* s, byte mac[6]) {
    if(s->next == NULL) {
        byte z[6] = {0,0,0,0,0,0};
        if(!memcmp(s->mac,z,6)) {
            memcpy(s->mac,mac,6);
            s->next = NULL;
            return 1;
        }else {
            if(!memcmp(mac,s->mac,6)) {
                return 0;
            }
        }

    }
    struct station* tmp = s;
    while(tmp->next != NULL) {
        if(!memcmp(tmp->mac,mac,6)) {
            return 0;
        }
        tmp = tmp->next;
    }
    if(!memcmp(tmp->mac,mac,6)) {
        return 0;
    }

    struct station* st = (struct station*)malloc(sizeof(struct station));
    memcpy(st->mac,mac,6);
    tmp->next = st;
    st->next = NULL;
    return 1;
    

}

void* pthread_sleep(void* arg) {
    sleep(10);
    sniffing = 0;
    pthread_exit(NULL);
}

void start_thread() {
    pthread_t thread;

    if(pthread_create(&thread,NULL,pthread_sleep,NULL) != 0) {
        printf("Thread error\n");
    }
}

struct station* find_station(char* iface, int freq, byte bssid[6]) {
    iface_toogle(iface,0);
    toggle_monitor(iface,1);
    iface_toogle(iface,1);
    channel_switch(iface,freq);

    int sock = sock_init(iface,0);
    byte buffer[MAX_BUFFER_SIZE];

    struct station* stat = (struct station*)malloc(sizeof(struct station));

    memset(stat,0,sizeof stat);
    stat->next = NULL;


    byte station[6];

    start_thread();
    while(sniffing) {
        int bytes_recieved = recv(sock,buffer,MAX_BUFFER_SIZE,0);

        byte* tmp_buff = buffer;

        uint16_t size = *((uint16_t*)(tmp_buff + 2));
       // printf("%u\n",size);
        tmp_buff += size;

        struct iee_802_11_hdr* wifi_hdr = (struct iee_802_11_hdr*)(tmp_buff);
        byte broadcast[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
        if(!memcmp(wifi_hdr->src,broadcast,6) || !memcmp(wifi_hdr->dst,broadcast,6)) {
            continue;
        }
        if(!memcmp(wifi_hdr->dst,bssid,6) && memcmp(wifi_hdr->dst,wifi_hdr->src,6) != 0) {
            if(check_and_add(stat,wifi_hdr->src)) {

            }
        }else if(!memcmp(wifi_hdr->src,bssid,6)&& memcmp(wifi_hdr->dst,wifi_hdr->src,6) != 0) {
            if(check_and_add(stat,wifi_hdr->dst)) {
            }

        }
    }

    return stat;

}


void deep_station_free(struct station* station) {
    if(station->next) {
        deep_station_free(station->next);
    }
    free(station);
}