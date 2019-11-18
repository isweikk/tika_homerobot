#ifndef _DISP_MAIN_UI_H_
#define _DISP_MAIN_UI_H_

#include "os_type.h"
#include "disp_texture.h"

class DispMainUi{
public:
    DispMainUi(){};
    ~DispMainUi()
    {
        //FreeGalaxy();
    };

    UINT32 InitGalaxy(DispTexture &win);
    void DrawGalaxy(DispTexture &win);

//protected:
    void FreeGalaxy(DispTexture &win);
    void StartOver(DispTexture &win);
    void RefreshGalaxy(DispTexture &win);

private:
    class Universe;
    Universe *universe_;
};

#endif

