#ifndef MAPLE_533EAFC0950044BC959AE63DEA1DE1EA_HPP
#define MAPLE_533EAFC0950044BC959AE63DEA1DE1EA_HPP

#include <cstdint>

namespace maple {
    struct IGObj {
        virtual void Update() = 0;// +0
    };

    struct IUIMsgHandler {
        virtual void OnKey(unsigned int wParam, unsigned int lParam) = 0;// +0
    };

    struct CWnd : IGObj, IUIMsgHandler {
        virtual ~CWnd() = delete;

        virtual void Update() = 0;                                       // +0
        virtual int OnDragDrop(int state, void *ctx, int rx, int ry) = 0;// +4

        virtual void PreCreateWnd(int l, int t, int w, int h, int z, int coord, void *data) = 0;// +8
        virtual void OnCreate(void *data) = 0;                                                  // +c
        virtual void OnDestroy() = 0;                                                           // +10

        virtual void OnMoveWnd(int l, int t) = 0;// +14
        virtual void OnEndMoveWnd() = 0;         // +18

        virtual void OnChildNotify(unsigned int id, unsigned int a2, unsigned int a3) = 0;// +1c
        virtual void OnButtonClicked(unsigned int id) = 0;                                // +20
    };

    struct IVecCtrlOwner {
        virtual int GetType() = 0;
    };

    struct IWzShape2D {
        virtual ~IWzShape2D() = 0;
    };

    struct IWzVector2D : IWzShape2D {
        ~IWzVector2D() override = 0;
    };

    struct ZRefCounted {
        ZRefCounted() = delete;
        virtual ~ZRefCounted() = 0;


    };

    struct CVecCtrl : ZRefCounted, IGObj, IWzVector2D {
        ~CVecCtrl() override = 0;

        virtual int GetType() = 0;
        virtual int CollisionDetectWalk() = 0;
        virtual int CollisionDetectFloat() = 0;
        virtual int GetMoveAbility() = 0;


    };

    // size: 0x210
    struct CVecCtrlUser : CVecCtrl {

    };

    struct CLife : IGObj, IVecCtrlOwner {
        virtual ~CLife() = delete;
    };

    struct CAvatar {
        virtual ~CAvatar() = delete;
    };

    struct CUser : CLife, CAvatar {
        ~CUser() override = delete;

        // 0x004C81B5
        CVecCtrl *GetVecCtrl() {
            auto const obj = reinterpret_cast<uintptr_t *>(this);
            return *reinterpret_cast<CVecCtrl **>(obj + 0x244);
        }
    };

    // size: 0xDA8
    struct CUserLocal : CUser {
        ~CUserLocal() override = delete;

        static CUserLocal *get() {
            return *reinterpret_cast<CUserLocal **>(0x0067006C);
        }
    };

    struct CWvsContext {
        static CWvsContext *get() {
            return *reinterpret_cast<CWvsContext **>(0x0066F860);
        }
    };
}// namespace maple

#endif//MAPLE_533EAFC0950044BC959AE63DEA1DE1EA_HPP
