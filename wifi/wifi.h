#ifndef __WIFI_LOADED__
    #define __WIFI_LOADED__

    #include <stdlib.h>
    #include <stdio.h>
    #include <stdint.h>
    #include <string.h>

    #include <sys/socket.h> //for socket();
    #include <sys/ioctl.h> //for ioctl()
    #include <net/if.h> //for ifreq, IFF_UP
    #include <unistd.h> //for close()
    #include <linux/wireless.h> //for iw_freq
    #include <linux/if_ether.h> //for ETH_P_...
    #include <arpa/inet.h> //for htons, ntoh - converting bytes to network/host order
    #include <netpacket/packet.h> //for sockaddr_ll && sockaddr
    #include <sys/time.h> //for struct timeval

    #define MAX_BUFFER_SIZE 500


    #define FRAME_CONTROL_PV_MASK 0x3
    #define FRAME_CONTROL_TYPE_MASK 0xc00
    #define FRAME_CONTROL_SUBTYPE_MASK 0xf000


    #define LLC_SIZE 8


    typedef uint8_t byte;

    struct packet {
        byte* data;
        uint16_t len;
    };

    struct wifi_network {
            uint8_t ap_hwaddr[6];
            char ssid[100];
            int freq;
            struct wifi_network* next;
        };

        

    struct iee_802_11_hdr {
        uint16_t frame_control;
        uint16_t duration;
        uint8_t dst[6];
        uint8_t src[6];
        uint8_t ap[6];
        uint16_t seq_ctrl;
    }__attribute__((packed));


    struct iee_802_11_radio_tap {
        uint8_t version;
        uint8_t pad;
        uint16_t len;
        uint32_t present;
        uint8_t data_rate;
    }__attribute__((__packed__));



    struct iee_tagged_param {
        uint8_t tag;
        uint8_t length;
    }__attribute__((packed));

    struct iee_eapol{
        uint8_t version;
        uint8_t type;
        uint16_t length;
        uint8_t key_type;
        uint16_t key_info;
        uint16_t key_length;
        uint8_t rc[8];
        uint8_t nonce[32];
        uint8_t iv[16];
        uint8_t key_rsc[8];
        uint8_t key_id[8];
        uint8_t mic[16];
        uint16_t key_data_length;
    };

    

    int sock_init(char* iname, char is_timeout);
    char channel_switch(char* iface,int freq);
    char toggle_monitor(char* iface, char on);
    char iface_toogle(char* iface, char on) ;
    void print_mac(uint8_t*);
    void deep_free(struct wifi_network* networks);

   unsigned char send_packet_multiple(char* iface, int channel, struct packet* _packet,int amount, int before_timeout, int timeout, char initialized, uint8_t* ended);
    // unsigned char send_packet(char* iface, int channel, struct packet* _packet);

#endif
