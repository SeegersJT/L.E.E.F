#include "firebase_manager.h"

String FirebaseManager::deviceId = "";
String FirebaseManager::idToken = "";
String FirebaseManager::refreshToken = "";

unsigned long FirebaseManager::tokenExpiresAt = 0;
unsigned long FirebaseManager::lastPush = 0;
unsigned long FirebaseManager::lastOtaCheck = 0;

String FirebaseManager::lastOtaResult = "";
String FirebaseManager::lastOtaCheckTime = "";

String FirebaseManager::apiKey = "";
String FirebaseManager::databaseUrl = "";
String FirebaseManager::deviceEmail = "";
String FirebaseManager::devicePassword = "";
String FirebaseManager::storageBucket = "";