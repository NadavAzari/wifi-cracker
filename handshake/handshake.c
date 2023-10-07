#include "handshake.h"

char is_qos_data(uint16_t f_control) {
    uint8_t subtype = (f_control & FRAME_CONTROL_SUBTYPE_MASK) >> 12;
    uint8_t type = (f_control & FRAME_CONTROL_TYPE_MASK) >> 10;
    uint8_t version = (f_control & FRAME_CONTROL_PV_MASK);
    if(subtype == 8 && type == 0b10) {
        return 1;
    }
    return 0;
}



void sniff_handshake(char* iface, byte bssid[6],byte station[6], hcallback callback, char* ended) {

    int sock = sock_init(iface,0);
    byte buffer[MAX_BUFFER_SIZE];

    char i_ended = 0;

    int c = 1;
    struct response_chunk_ll* chunk = (struct response_chunk_ll*)malloc(sizeof(struct response_chunk_ll));
    memset(chunk,0,sizeof(chunk));
    memcpy(chunk->smac,station,6);
    memcpy(chunk->amac,bssid,6);
    while(!*ended) {
        int bytes_recieved = recv(sock,buffer,MAX_BUFFER_SIZE,0);

        byte* tmp_buff = buffer;

        uint16_t size = *((uint16_t*)(tmp_buff + 2));
        tmp_buff += size;

        struct iee_802_11_hdr* wifi_hdr = (struct iee_802_11_hdr*)(tmp_buff);
        wifi_hdr->frame_control = ntohs(wifi_hdr->frame_control);
        tmp_buff += sizeof(struct iee_802_11_hdr) + 2;



       

        if(is_qos_data(wifi_hdr->frame_control)) {
            if(ntohs(*((uint16_t*)(tmp_buff + 6))) == 0x888e) { 
                if(c % 2) {
                    if(memcmp(wifi_hdr->src,bssid,6) != 0 || memcmp(wifi_hdr->dst,station,6) != 0) {
                        continue;
                    }
                }else {
                    if(memcmp(wifi_hdr->dst,bssid,6) != 0 || memcmp(wifi_hdr->src,station,6) != 0) {
                        continue;
                    }
                }

                struct iee_eapol* auth = (struct iee_eapol*)(tmp_buff + LLC_SIZE - 1);
                if(c == 1) {
                    memcpy(chunk->anonce,auth->nonce,32);
                }else{
                    memcpy(chunk->snonce,auth->nonce,32);
                    memcpy(chunk->mic,auth->mic,16);
                    memcpy(chunk->iv,auth->iv,16);
                    chunk->ended = 1;
                }
                c++;
                
                if(c > 2) {
                    close(sock);
                    *ended = 1;
                    i_ended = 1;
                    callback(chunk);
                    return;
                }
            }
        }
    }

    if(!i_ended){
        free(chunk);
    }

}
