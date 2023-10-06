#include "../colors/colors.h"
#include "../network_scanner/scanner.h"

#include <stdlib.h>
#include <stdio.h>

void title();
void show_networks_table(struct wifi_network* networks);
struct wifi_network* menu(struct wifi_network* networks);
void clear_screen();