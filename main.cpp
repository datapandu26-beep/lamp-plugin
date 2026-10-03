#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

// Registrasi modul AML
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

void (*orig_CAutomobile_Update)(void* self);
void hook_CAutomobile_Update(void* self) {
    if (orig_CAutomobile_Update) {
        orig_CAutomobile_Update(self);
    }

    if (FindPlayerPed) {
        CPlayerPed* pPlayer = FindPlayerPed(-1);
        if (pPlayer && pPlayer->m_pMyVehicle == (CVehicle*)self) {
            bool bHorn = *(bool*)((uintptr_t)self + 0x4D5); 
            static bool bWasHorn = false;
            
            if (bHorn && !bWasHorn) {
                pPlayer->m_pMyVehicle->ToggleLights();
            }
            bWasHorn = bHorn;
        }
    }
}

// Tambahkan attribute default visibility agar OnModLoad terbaca oleh AML
extern "C" __attribute__((visibility("default"))) void OnModLoad() {
    logger->SetTag("LightsControl");
    logger->Info("LightsControl plugin berhasil dimuat!");

    pGTASA = aml->GetLib("libGTASA.so");
    if (!pGTASA) {
        logger->Error("Gagal menemukan libGTASA.so");
        return;
    }

    FindPlayerPed = (CPlayerPed*(*)(int))aml->GetSym(pGTASA, "_Z13FindPlayerPedi");

    uintptr_t updateAddr = aml->GetSym(pGTASA, "_ZN11CAutomobile6UpdateEv");
    if (updateAddr) {
        aml->Hook((void*)updateAddr, (void*)hook_CAutomobile_Update, (void**)&orig_CAutomobile_Update);
        logger->Info("Hook CAutomobile::Update berhasil dipasang!");
    } else {
        logger->Error("Gagal menemukan symbol CAutomobile::Update");
    }
}
