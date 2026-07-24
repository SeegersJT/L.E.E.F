#ifndef PAIRING_MANAGER_H
#define PAIRING_MANAGER_H

#include <Arduino.h>

class PairingManager
{
public:
    static void maintain();
    static bool isClaimed();

private:
    static String pairingCode;
    static bool deviceClaimed;
    static unsigned long lastOwnershipCheck;
    static unsigned long lastPairingToggle;
    static bool showingPairingCode;
    static unsigned long lastClaimedRecheck;

    static void refreshOwnershipStatus();
    static void recheckClaimedStatus();
    static String generatePairingCode();
    static void publishPairingCode();
    static void clearPairingCode();
};

#endif // PAIRING_MANAGER_H