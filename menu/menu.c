#include "menu.h"

typedef void (*func_ptr)(void);

char check_name_validation(char* name) {
    for(int i =0; i < strlen(name); ++i) {
        if(name[i] == '\b' || name[i] == '\r' || name[i] == '\x1B' || name[i] == '\e' || name[i] == '\t') {
            return 0;
        }
    }
    return 1;
}

void clear_screen() {
    system("clear");
}

void print_multiple(char c, int l) {
    for(int i=0; i < l; ++i){
        printf("%c",c);
    }
}

void middle() {
    print_multiple(' ',30);
}

void line() {
    printf("\n");
}

void print_lines() {
    for(int i =0; i < 32*3 - 4; ++i) {
        printf("\u2500");
    }
}

void title() {
    clear_screen();
    CYAN();
    
    printf("░██╗░░░░░░░██╗██╗███████╗██╗  ░█████╗░██████╗░░█████╗░░█████╗░██╗░░██╗███████╗██████╗░\n");
    
    printf("░██║░░██╗░░██║██║██╔════╝██║  ██╔══██╗██╔══██╗██╔══██╗██╔══██╗██║░██╔╝██╔════╝██╔══██╗\n");
    
    printf("░╚██╗████╗██╔╝██║█████╗░░██║  ██║░░╚═╝██████╔╝███████║██║░░╚═╝█████═╝░█████╗░░██████╔╝\n");
        
    printf("░░████╔═████║░██║██╔══╝░░██║  ██║░░██╗██╔══██╗██╔══██║██║░░██╗██╔═██╗░██╔══╝░░██╔══██╗\n");
    
    printf("░░╚██╔╝░╚██╔╝░██║██║░░░░░██║  ╚█████╔╝██║░░██║██║░░██║╚█████╔╝██║░╚██╗███████╗██║░░██║\n");
    
    printf("░░░╚═╝░░░╚═╝░░╚═╝╚═╝░░░░░╚═╝  ░╚════╝░╚═╝░░╚═╝╚═╝░░╚═╝░╚════╝░╚═╝░░╚═╝╚══════╝╚═╝░░╚═╝\n");
    printf("\n\n");
    WHITE();
}


struct wifi_network* print_selection(struct wifi_network* networks){
    int i = 1;

    struct wifi_network* tmp = networks;

    RED();
    while(tmp != NULL) {
        printf("%d) %s\n",i,tmp->ssid);
        tmp = tmp->next;
        ++i;
    }
    line();
    PURPLE();
    printf("network number to crack > ");
    int n;
    GREEN();
    scanf("%d",&n);

    tmp = networks;
    i = 1;
    while(tmp != NULL) {
        if(i == n) {
            return tmp;
        }
        tmp = tmp->next;
        ++i;
    }
    return NULL;
}



void show_networks_table(struct wifi_network* networks) {
    title();
    char* titles[] = {"SSID", "MAC Address", "Frequency"};
    func_ptr colors[] = {GREEN,PURPLE,YELLOW};


    
    WHITE();
    print_lines();
    line();
    
    printf("|");

    for(int i = 0; i < 3; i++) {
        int size = (30 - strlen(titles[i])) / 2; 
       
        print_multiple(' ',size);
        colors[i]();
        printf("%s",titles[i]);
        WHITE();
        print_multiple(' ',size);

        printf("\u2502");
    }
    line();
    
    print_lines();
    line();

    while(networks != NULL){
        printf("\u2502");
        int size = (30 - strlen(networks->ssid)) / 2;
        print_multiple(' ',size);
        GREEN();
        printf("%s",networks->ssid);
        WHITE();
        print_multiple(' ',size);

        printf("\u2502");
        size = (30 - 17) / 2; 
        print_multiple(' ',size);
        PURPLE();
        print_mac(networks->ap_hwaddr);
        WHITE();
        print_multiple(' ',size);

        printf("\u2502");
        size = (30 - 4) / 2; 
        print_multiple(' ',size);
        YELLOW();
        printf("%d",networks->freq);
        WHITE();
        print_multiple(' ',size);
        printf("\u2502\n");


        networks = networks->next;
        
        print_lines();
        line();
    }

    printf("\n\n");

}

struct wifi_network* menu(struct wifi_network* networks) {
    show_networks_table(networks);
    struct wifi_network* net = print_selection(networks);
    if(net == NULL) {
        net = menu(networks);
    }
    return net;
}