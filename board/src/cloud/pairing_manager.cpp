#include "cloud/pairing_manager.h"
#include "cloud/firebase_client.h"
#include "core/globals.h"
#include "core/logger.h"
#include <WiFi.h>
#include <esp_system.h>

String PairingManager::pairingCode = "";
bool PairingManager::deviceClaimed = false;
unsigned long PairingManager::lastOwnershipCheck = 0;
unsigned long PairingManager::lastPairingToggle = 0;
bool PairingManager::showingPairingCode = false;
unsigned long PairingManager::lastClaimedRecheck = 0;

bool PairingManager::isClaimed()
{
    return deviceClaimed;
}

void PairingManager::maintain()
{
    if (WiFi.status() != WL_CONNECTED)
        return;

    unsigned long currentMillis = millis();

    if (deviceClaimed)
    {
        if (lastClaimedRecheck == 0 || currentMillis - lastClaimedRecheck >= (unsigned long)config["OWNERSHIP_RECHECK_INTERVAL"])
        {
            lastClaimedRecheck = currentMillis;
            recheckClaimedStatus();
        }
        return;
    }

    if (lastOwnershipCheck == 0 || currentMillis - lastOwnershipCheck >= (unsigned long)config["PAIRING_CHECK_INTERVAL"])
    {
        lastOwnershipCheck = currentMillis;
        refreshOwnershipStatus();
    }

    if (deviceClaimed || pairingCode.length() == 0)
        return;

    if (currentMillis - lastPairingToggle >= 3000)
    {
        lastPairingToggle = currentMillis;
        showingPairingCode = !showingPairingCode;

        if (showingPairingCode)
        {
            display("Pairing code:").clear().print();
            display(pairingCode).bottom().print();
        }
        else
        {
            display("Enter in app").clear().print();
            display("to link plant").bottom().print();
        }
    }
}

void PairingManager::refreshOwnershipStatus()
{
    String response;
    String path = "/devices/" + FirebaseClient::deviceId() + "/owner";

    if (!FirebaseClient::get(path, response))
    {
        Logger::log(LogCategory::LOG_FIREBASE, "Ownership check failed");
        return;
    }

    response.trim();

    if (response == "null" || response.length() == 0)
    {
        if (pairingCode.length() == 0)
        {
            pairingCode = generatePairingCode();
            publishPairingCode();
        }
        return;
    }

    deviceClaimed = true;
    Logger::log(LogCategory::LOG_FIREBASE, "Device claimed by a user account");
    clearPairingCode();

    display("Paired!").clear().print();
    display("Setup complete").bottom().print();
    delay(2000);
}

void PairingManager::recheckClaimedStatus()
{
    String response;
    String path = "/devices/" + FirebaseClient::deviceId() + "/owner";

    if (!FirebaseClient::get(path, response))
    {
        Logger::log(LogCategory::LOG_FIREBASE, "Ownership recheck failed");
        return;
    }

    response.trim();

    if (response == "null" || response.length() == 0)
    {
        deviceClaimed = false;
        pairingCode = "";
        lastOwnershipCheck = 0;
        Logger::log(LogCategory::LOG_FIREBASE, "Device was unpaired - resuming pairing mode");

        display("Unpaired").clear().print();
        display("Ready to re-pair").bottom().print();
        delay(2000);
    }
}

String PairingManager::generatePairingCode()
{
    static const char charset[] = "ABCDEFGHJKMNPQRSTUVWXYZ23456789";
    const int codeLength = 6;

    randomSeed(esp_random());

    String code = "";
    for (int i = 0; i < codeLength; i++)
    {
        code += charset[random(0, (int)sizeof(charset) - 1)];
    }

    return code;
}

void PairingManager::publishPairingCode()
{
    String devicePath = "/devices/" + FirebaseClient::deviceId() + "/pairingCode";

    if (FirebaseClient::put(devicePath, "\"" + pairingCode + "\""))
    {
        Logger::log(LogCategory::LOG_FIREBASE, "Pairing code published");
    }

    String indexPath = "/pairingCodes/" + pairingCode;
    String indexPayload = "{";
    indexPayload += "\"deviceId\":\"" + FirebaseClient::deviceId() + "\",";
    indexPayload += "\"createdAt\":{\".sv\":\"timestamp\"}";
    indexPayload += "}";

    if (FirebaseClient::put(indexPath, indexPayload))
    {
        Logger::log(LogCategory::LOG_FIREBASE, "Pairing code indexed");
    }
}

void PairingManager::clearPairingCode()
{
    String devicePath = "/devices/" + FirebaseClient::deviceId() + "/pairingCode";
    FirebaseClient::del(devicePath);

    String indexPath = "/pairingCodes/" + pairingCode;
    FirebaseClient::del(indexPath);
}