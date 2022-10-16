#ifndef MAPLE_533EAFC0950044BC959AE63DEA1DE1EA_HPP
#define MAPLE_533EAFC0950044BC959AE63DEA1DE1EA_HPP

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
}// namespace maple

#endif//MAPLE_533EAFC0950044BC959AE63DEA1DE1EA_HPP
