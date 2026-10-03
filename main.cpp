#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

MYMODCFG(net.byth.lightscontrol, GTA SA Vehicle Lights Control, 1.0, Byth)

uintptr_t pGTASA = 0;

class CVehicle {
public:
    void ToggleLights() {
        unsigned char* pFlags = (unsigned char*)((uintptr_t)this + 0x5A0); 
        if (*pFlags == 2) {
            *pFlags = 1;
        } else {
            *pFlags = 2;
        }
    }
};

class CPlayerPed {
public:
    CVehicle* m_pMyVehicle;
};

CPlayerPed* (*FindPlayerPed)(int id) = nullptr;

bool (*orig_IsHornPressed)(void* self);
bool hook_IsHornPressed(void* self) {
    static bool bWasPressed = false;
    bool bIsPressed = orig_IsHornPressed(self);

    if (bIsPressed && !bWasPressed) {
        if (FindPlayerPed) {
            CPlayerPed* pPlayer = FindPlayerPed(-1);
            if (pPlayer && pPlayer->m_pMyVehicle) {
                pPlayer->m_pMyVehicle->ToggleLights();
            }
        }
    }

    bWasPressed = bIsPressed;
    return bIsPressed;
}

extern "C" void OnModLoad() {
    logger->SetTag("LightsControl");
    pGTASA = aml->GetLib("libGTASA.so");
    if (!pGTASA) return;

    FindPlayerPed = (CPlayerPed*(*)(int))aml->GetSym(pGTASA, "_Z13FindPlayerPedi");

    uintptr_t hornFuncAddr = aml->GetSym(pGTASA, "_ZN11CWidgetHorn6UpdateEv"); 
    if (hornFuncAddr) {
        aml->Redirect(hornFuncAddr, (uintptr_t)hook_IsHornPressed, (uintptr_t*)&orig_IsHornPressed);
    }
}
