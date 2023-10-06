#include "wifi.h"


void set_timeout(int sock) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100 * 1000;
    setsockopt(sock,SOL_SOCKET, SO_RCVTIMEO,(const char*)&tv, sizeof tv);
}



int sock_init(char* iname, char is_timeout) {
    struct ifreq ifr;
    struct sockaddr_ll sock_opt;
    struct iwreq wreq;
    struct iw_freq freq;
    
    memset(&freq,0,sizeof(freq));


    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); 
    if(sock < 0) {
        printf("Unable to open socket on %s interface",iname);
        return sock;
    }

    strcpy(ifr.ifr_name, iname);
    if(ioctl(sock, SIOCGIFINDEX, &ifr) < -1) {
        printf("Unable to set interface to socket");
        return -1;
    }



    sock_opt.sll_family = AF_PACKET;
    sock_opt.sll_protocol = htons(ETH_P_ALL);
    sock_opt.sll_ifindex = ifr.ifr_ifindex;
    if(is_timeout)
        set_timeout(sock);

    bind(sock, (struct sockaddr*)&sock_opt, sizeof(sock_opt));


    return sock;

}

char toggle_monitor(char* iface, char on) {
    int sock = socket(AF_INET,SOCK_DGRAM,0);
    struct iwreq wreq;
    memset(&wreq,0,sizeof(struct iwreq));

    strncpy(wreq.ifr_name,iface,IFNAMSIZ);

    if(ioctl(sock,SIOCGIFFLAGS,&wreq) == -1){
        return -1;
    }

    wreq.u.mode = on ? IW_MODE_MONITOR : IW_MODE_INFRA;
    if(ioctl(sock,SIOCSIWMODE,&wreq) == -1){
        close(sock);
        return -1;
    }

    close(sock);
    return 0;
}

char iface_toogle(char* iface, char on) {
    struct ifreq ifr;


    int sock = socket(AF_INET, SOCK_DGRAM,0);
    if(sock == -1) {
        return -1;
    }

    memset(&ifr,0,sizeof(struct ifreq));
    strncpy(ifr.ifr_name,iface,IFNAMSIZ - 1);

    if(ioctl(sock,SIOCGIFFLAGS,&ifr) == -1) {
        close(sock);
        return -1;
    }

    if(on == 0)
        ifr.ifr_flags &= ~IFF_UP;
    else
        ifr.ifr_flags |= IFF_UP;

    if(ioctl(sock,SIOCSIFFLAGS,&ifr) == -1) {
        close(sock);
        return -1;
    }

    close(sock);

    return 0;

}

char channel_switch(char* iface, int freq){
    struct iwreq wreq;

    int sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock == -1) {
        return -1;
    }

    memset(&wreq,0,sizeof(struct iwreq));
    strncpy(wreq.ifr_name,iface,strlen(iface));

    if(ioctl(sock,SIOCGIFFLAGS,&wreq) == -1) {
        return -1;
    }


    wreq.u.freq.m = freq;
    wreq.u.freq.e = 6;

    if(ioctl(sock,SIOCSIWFREQ,&wreq) == -1) {
        close(sock);
        return -1;
    }


    close(sock);

    return 0;

}


void print_mac(uint8_t* mac) {
    printf("%2x:%2x:%2x:%2x:%2x:%2x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    
}

void deep_free(struct wifi_network* networks) {
    if(networks->next != NULL) {
        deep_free(networks->next);
    }
    free(networks);
}

unsigned char send_packet_multiple(char* iface, int channel, struct packet* _packet,int amount, int before_timeout, int timeout,char initialized, uint8_t* ended) {
    struct sockaddr_ll sock_opt;
    struct ifreq ifr; 

    if(!initialized) {
        iface_toogle(iface,0);
        toggle_monitor(iface,1);
        iface_toogle(iface,1);
        channel_switch(iface,channel);
    }




    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(sock < 0) {
        return 1;
    }

    strcpy(ifr.ifr_name,iface);

    if(ioctl(sock,SIOCGIFINDEX, &ifr) == -1) {
        close(sock);
        return 1;
    }


    sock_opt.sll_family = AF_PACKET;    
    sock_opt.sll_protocol = htons(ETH_P_ALL);
    sock_opt.sll_ifindex = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr*)&sock_opt, sizeof(sock_opt));


    for(int i =1; i <= amount; ++i) {
        if(ended && *ended) {
            break;
        }
        if(!(i % before_timeout)) {
            sleep(timeout);
        }
        usleep(200*1000);
        ssize_t bytes_sent = sendto(sock,_packet->data,_packet->len,0,NULL,0);
        //printf("1 packet sent\n");
        if(bytes_sent < 0) {
            close(sock);
            return 1;
        }
    }


    close(sock);

    iface_toogle(iface,0);
    toggle_monitor(iface,0);
    iface_toogle(iface,1);

    return 0;
}


// unsigned char send_packet(char* iface, int channel, struct packet* _packet) {
//     send_packet_multiple(iface,channel,_packet,1);
// }
