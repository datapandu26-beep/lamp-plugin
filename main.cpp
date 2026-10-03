#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

MYMODCFG(net.byth.lightscontrol, GTA SA Vehicle Lights Control, 1.1, Byth)

uintptr_t pGTASA = 0;

// Struct aman untuk membaca status tombol klakson pada GTA SA v2.10 64-bit
class CPad {
public:
    static CPad* GetPad(int player) {
        typedef CPad* (*GetPadFn)(int);
        static GetPadFn fn = (GetPadFn)aml->GetSym(pGTASA, "_ZN4CPad6GetPadEi");
        return fn ? fn(player) : nullptr;
    }

    // Offset tombol klakson pada CPad 64-bit
    bool GetHornJustDown() {
        return *(bool*)((uintptr_t)this + 0x120); 
    }
};

class CVehicle {
public:
    void ToggleOverrideLights() {
        // Offset override lights untuk GTA SA v2.10 64-bit
        unsigned char* pLightMode = (unsigned char*)((uintptr_t)this + 0x6A4); 
        
        if (*pLightMode == 2) {
            *pLightMode = 1; // Paksa Mati
        } else {
            *pLightMode = 2; // Paksa Menyala
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

    if (!FindPlayerPed) return;

    CPlayerPed* pPlayer = FindPlayerPed(-1);
    if (pPlayer && pPlayer->m_pMyVehicle == (CVehicle*)self) {
        CPad* pPad = CPad::GetPad(0);
        if (pPad && pPad->GetHornJustDown()) {
            pPlayer->m_pMyVehicle->ToggleOverrideLights();
        }
    }
}

extern "C" __attribute__((visibility("default"))) void OnModLoad() {
    logger->SetTag("LightsControl");
    pGTASA = aml->GetLib("libGTASA.so");
    if (!pGTASA) {
        logger->Error("libGTASA.so tidak ditemukan!");
        return;
    }

    FindPlayerPed = (CPlayerPed*(*)(int))aml->GetSym(pGTASA, "_Z13FindPlayerPedi");

    uintptr_t updateAddr = aml->GetSym(pGTASA, "_ZN11CAutomobile6UpdateEv");
    if (updateAddr) {
        aml->Hook((void*)updateAddr, (void*)hook_CAutomobile_Update, (void**)&orig_CAutomobile_Update);
        logger->Info("Berhasil hook CAutomobile::Update untuk GTA SA v2.10!");
    } else {
        logger->Error("Gagal menemukan symbol CAutomobile::Update");
    }
}
