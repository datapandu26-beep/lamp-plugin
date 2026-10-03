#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

MYMODCFG(net.byth.lightscontrol, GTA SA Vehicle Lights Control, 1.0, Byth)

uintptr_t pGTASA = 0;

class CVehicle {
public:
    void ToggleLights() {
        // Offset flag lampu kendaraan GTA SA 64-bit
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

// Hook proses klakson
void (*orig_CAutomobile_Update)(void* self);
void hook_CAutomobile_Update(void* self) {
    orig_CAutomobile_Update(self);

    if (FindPlayerPed) {
        CPlayerPed* pPlayer = FindPlayerPed(-1);
        if (pPlayer && pPlayer->m_pMyVehicle == (CVehicle*)self) {
            // Cek status klakson kendaraan
            bool bHorn = *(bool*)((uintptr_t)self + 0x4D5); 
            static bool bWasHorn = false;
            
            if (bHorn && !bWasHorn) {
                pPlayer->m_pMyVehicle->ToggleLights();
            }
            bWasHorn = bHorn;
        }
    }
}

extern "C" void OnModLoad() {
    logger->SetTag("LightsControl");
    pGTASA = aml->GetLib("libGTASA.so");
    if (!pGTASA) return;

    FindPlayerPed = (CPlayerPed*(*)(int))aml->GetSym(pGTASA, "_Z13FindPlayerPedi");

    uintptr_t updateAddr = aml->GetSym(pGTASA, "_ZN11CAutomobile6UpdateEv");
    if (updateAddr) {
        aml->Redirect(updateAddr, (uintptr_t)hook_Automobile_Update);
    }
}
