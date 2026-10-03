#include <android/log.h>
#include <mod/amlmod.h>

#define LOG_TAG "LightsControl"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Pengganti MYMODCFG manual agar tidak butuh library Config
extern "C" {
    const char* GUID = "net.byth.lightscontrol";
    const char* Name = "LightsControl";
    const char* Version = "1.0";
    const char* Author = "Byth";
}

uintptr_t pGTASA = 0;

class CPad {
public:
    static CPad* GetPad(int player) {
        typedef CPad* (*GetPadFn)(int);
        static GetPadFn fn = (GetPadFn)aml->GetSym(pGTASA, "_ZN4CPad6GetPadEi");
        return fn ? fn(player) : nullptr;
    }

    bool GetHornJustDown() {
        return *(bool*)((uintptr_t)this + 0x120);
    }
};

class CVehicle {
public:
    void ToggleOverrideLights() {
        unsigned char* pLightMode = (unsigned char*)((uintptr_t)this + 0x6A4);
        if (*pLightMode == 2) {
            *pLightMode = 1;
        } else {
            *pLightMode = 2;
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
    LOGI("Plugin LightsControl dimuat!");

    pGTASA = aml->GetLib("libGTASA.so");
    if (!pGTASA) {
        LOGE("libGTASA.so tidak ditemukan!");
        return;
    }

    FindPlayerPed = (CPlayerPed*(*)(int))aml->GetSym(pGTASA, "_Z13FindPlayerPedi");

    uintptr_t updateAddr = aml->GetSym(pGTASA, "_ZN11CAutomobile6UpdateEv");
    if (updateAddr) {
        aml->Hook((void*)updateAddr, (void*)hook_CAutomobile_Update, (void**)&orig_CAutomobile_Update);
        LOGI("Hook berhasil!");
    } else {
        LOGE("Gagal menemukan symbol CAutomobile::Update");
    }
}
