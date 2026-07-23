#include "ap_manager.h"

WebServer APManager::webServer(80);
DNSServer APManager::dnsServer;
bool APManager::apActive = false;
bool APManager::shutdownPending = false;
String APManager::apSsid = "";
