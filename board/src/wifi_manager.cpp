#include "wifi_manager.h"

WebServer WiFiManager::webServer(80);
DNSServer WiFiManager::dnsServer;
bool WiFiManager::apActive = false;