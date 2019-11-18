/*
 * 描述：主画面，Galaxy效果
 * 说明：参考地址https://linux.die.net/man/6/galaxy
 */
#include "disp_main_ui.h"

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

# define DEFAULTS	"*delay:  20000  \n"   \
					"*count:  -5     \n"   \
					"*cycles:  250   \n"   \
					"*ncolors:  64   \n" \
					"*fpsSolid:  true   \n" \
					"*ignoreRotation: True \n" \
				    "*lowrez: True \n" \

# define UNIFORM_COLORS
# define release_galaxy 0
# define reshape_galaxy 0
# define galaxy_handle_event 0

static bool tracks;
static bool spin;
static bool dbufp;

#define DEF_TRACKS "True"
#define DEF_SPIN   "True"
#define DEF_DBUF   "True"

#define VectorSize 55
static unsigned int a[VectorSize] = {
        035340171546, 010401501101, 022364657325, 024130436022, 002167303062, /*  5 */
        037570375137, 037210607110, 016272055420, 023011770546, 017143426366, /* 10 */
        014753657433, 021657231332, 023553406142, 004236526362, 010365611275, /* 14 */
        007117336710, 011051276551, 002362132524, 001011540233, 012162531646, /* 20 */
        007056762337, 006631245521, 014164542224, 032633236305, 023342700176, /* 25 */
        002433062234, 015257225043, 026762051606, 000742573230, 005366042132, /* 30 */
        012126416411, 000520471171, 000725646277, 020116577576, 025765742604, /* 35 */
        007633473735, 015674255275, 017555634041, 006503154145, 021576344247, /* 40 */
        014577627653, 002707523333, 034146376720, 030060227734, 013765414060, /* 45 */
        036072251540, 007255221037, 024364674123, 006200353166, 010126373326, /* 50 */
        015664104320, 016401041535, 016215305520, 033115351014, 017411670323  /* 55 */
};

static int i1, i2;

unsigned int ya_random (void)
{
    register int ret = a[i1] + a[i2];
    a[i1] = ret;
    if (++i1 >= VectorSize) i1 = 0;
    if (++i2 >= VectorSize) i2 = 0;
    return ret;
}

#define SINF(n)			((float)sin((double)(n)))
#define COSF(n)			((float)cos((double)(n)))
#define FABSF(n)		((float)fabs((double)(n)))

#undef MAX
#undef MIN
#undef ABS
#define MAX(a,b)((a)>(b)?(a):(b))
#define MIN(a,b)((a)<(b)?(a):(b))
#define ABS(a)((a)<0 ? -(a) : (a))

#define LRAND()         ((long) (ya_random() & 0x7fffffff))
#define NRAND(n)       ((int) (LRAND() % (n)))
#define MAXRAND         (2147483648.0) /* unsigned 1<<31 as a float */
#define FLOAT_RAND_NUM ((double) LRAND() / ((double) MAXRAND))

#if 0
#define WRAP       1  /* Warp around edges */
#define BOUNCE     1  /* Bounce from borders */
#endif

#define MINSIZE       1
#define MINGALAXIES    2
#define MAX_STARS    3000
#define MAX_IDELTAT    50
/* These come originally from the Cluster-version */
#define DEFAULT_GALAXIES  3
#define DEFAULT_STARS    1000
#define DEFAULT_HITITERATIONS  7500
#define DEFAULT_IDELTAT    200 /* 0.02 */
#define EPSILON 0.00000001

#define sqrt_EPSILON 0.0001

#define DELTAT (MAX_IDELTAT * 0.0001)

#define GALAXYRANGESIZE  0.1
#define GALAXYMINSIZE  0.15
#define QCONS    0.001


#define COLOR_BASE  16
/* colors per galaxy */
/* #define COLORSTEP  (NUMCOLORS/COLOR_BASE) */
# define COLORSTEP (MI_NCOLORS(mi)/COLOR_BASE)


typedef struct {
    double pos[3], vel[3];
} Star;

typedef struct {
    int x, y;
} Point;

typedef struct {
    int         mass;
    int         nstars;
    Star       *stars;
    Point      *oldPoints;
    Point      *newPoints;
    double      pos[3], vel[3];
    int         galcol;
} Galaxy;

//typedef struct {
//    double      mat[3][3]; // Movement of stars(?)
//    double      scale; // Scale
//    int         midx; // Middle of screen, x
//    int         midy; // Middle of screen, y
//    double      size; //
//    double      diff[3];
//    Galaxy     *galaxies; // the Whole Universe */
//    int         ngalaxies; // # galaxies
//    int         f_hititerations; // # iterations before restart
//    int         step;
//    double      rot_y; // rotation of eye around center of universe, around y-axis
//    double      rot_x; // rotation of eye around center of universe, around x-axis
//} Universe;

class DispMainUi::Universe {
public:
    double      mat[3][3]; // Movement of stars(?)
    double      scale; // Scale
    int         midx; // Middle of screen, x
    int         midy; // Middle of screen, y
    double      size; //
    double      diff[3];
    Galaxy     *galaxies; // the Whole Universe */
    int         ngalaxies; // # galaxies
    int         f_hititerations; // # iterations before restart
    int         step;
    double      rot_y; // rotation of eye around center of universe, around y-axis
    double      rot_x; // rotation of eye around center of universe, around x-axis

    long batchCount;
};

void DispMainUi::FreeGalaxy(DispTexture &win)
{
    if (universe_->galaxies != NULL) {
        for (int i = 0; i < universe_->ngalaxies; i++) {
            Galaxy *gt = &universe_->galaxies[i];

            if (gt->stars != NULL) {
                delete[] gt->stars;
            }
            if (gt->oldPoints != NULL) {
                delete[] gt->oldPoints;
            }
            if (gt->newPoints != NULL) {
                delete[] gt->newPoints;
            }
        }
        delete[] universe_->galaxies;
        universe_->galaxies = NULL;
    }
}

void DispMainUi::StartOver(DispTexture &win)
{
    int         i, j; /* more tmp */
    double      w1, w2; /* more tmp */
    double      d, v, w, h; /* yet more tmp */

    universe_->step = 0;
    universe_->rot_y = 0;
    universe_->rot_x = 0;

    if (universe_->batchCount < -MINGALAXIES) {
        FreeGalaxy(win);
    }
    universe_->ngalaxies = universe_->batchCount;
    if (universe_->ngalaxies < -MINGALAXIES) {
        universe_->ngalaxies = NRAND(-universe_->ngalaxies - MINGALAXIES + 1) + MINGALAXIES;
    } else if (universe_->ngalaxies < MINGALAXIES) {
        universe_->ngalaxies = MINGALAXIES;
    }

    if (universe_->galaxies == NULL) {
        universe_->galaxies = new Galaxy[universe_->ngalaxies]();
    }

    for (i = 0; i < universe_->ngalaxies; ++i) {
        Galaxy     *gt = &universe_->galaxies[i];
        double      sinw1, sinw2, cosw1, cosw2;

        gt->galcol = NRAND(COLOR_BASE - 2);
        if (gt->galcol > 1) {
            gt->galcol += 2; /* Mult 8; 16..31 no green stars */
        }
        /* Galaxies still may have some green stars but are not all green. */

        if (gt->stars != NULL) {
            delete[] gt->stars;
            gt->stars = NULL;
        }
        gt->nstars = (NRAND(MAX_STARS / 2)) + MAX_STARS / 2;
        gt->stars = new Star[gt->nstars]();
        gt->oldPoints = new Point[gt->nstars]();
        gt->newPoints = new Point[gt->nstars]();

        w1 = 2.0 * M_PI * FLOAT_RAND_NUM;
        w2 = 2.0 * M_PI * FLOAT_RAND_NUM;
        sinw1 = SINF(w1);
        sinw2 = SINF(w2);
        cosw1 = COSF(w1);
        cosw2 = COSF(w2);

        universe_->mat[0][0] = cosw2;
        universe_->mat[0][1] = -sinw1 * sinw2;
        universe_->mat[0][2] = cosw1 * sinw2;
        universe_->mat[1][0] = 0.0;
        universe_->mat[1][1] = cosw1;
        universe_->mat[1][2] = sinw1;
        universe_->mat[2][0] = -sinw2;
        universe_->mat[2][1] = -sinw1 * cosw2;
        universe_->mat[2][2] = cosw1 * cosw2;

        gt->vel[0] = FLOAT_RAND_NUM * 2.0 - 1.0;
        gt->vel[1] = FLOAT_RAND_NUM * 2.0 - 1.0;
        gt->vel[2] = FLOAT_RAND_NUM * 2.0 - 1.0;
        gt->pos[0] = -gt->vel[0] * DELTAT * universe_->f_hititerations + FLOAT_RAND_NUM - 0.5;
        gt->pos[1] = -gt->vel[1] * DELTAT * universe_->f_hititerations + FLOAT_RAND_NUM - 0.5;
        gt->pos[2] = -gt->vel[2] * DELTAT * universe_->f_hititerations + FLOAT_RAND_NUM - 0.5;

        gt->mass = (int) (FLOAT_RAND_NUM * 1000.0) + 1;

        universe_->size = GALAXYRANGESIZE * FLOAT_RAND_NUM + GALAXYMINSIZE;

        for (j = 0; j < gt->nstars; ++j) {
            Star       *st = &gt->stars[j];
            Point *oldp = &gt->oldPoints[j];
            Point *newp = &gt->newPoints[j];
            double      sinw, cosw;

            w = 2.0 * M_PI * FLOAT_RAND_NUM;
            sinw = SINF(w);
            cosw = COSF(w);
            d = FLOAT_RAND_NUM * universe_->size;
            h = FLOAT_RAND_NUM * exp(-2.0 * (d / universe_->size)) / 5.0 * universe_->size;
            if (FLOAT_RAND_NUM < 0.5) {
                h = -h;
            }
            st->pos[0] = universe_->mat[0][0] * d * cosw + universe_->mat[1][0] * d * sinw +
                         universe_->mat[2][0] * h + gt->pos[0];
            st->pos[1] = universe_->mat[0][1] * d * cosw + universe_->mat[1][1] * d * sinw +
                         universe_->mat[2][1] * h + gt->pos[1];
            st->pos[2] = universe_->mat[0][2] * d * cosw + universe_->mat[1][2] * d * sinw +
                         universe_->mat[2][2] * h + gt->pos[2];

            v = sqrt(gt->mass * QCONS / sqrt(d * d + h * h));
            st->vel[0] = -universe_->mat[0][0] * v * sinw + universe_->mat[1][0] * v * cosw + gt->vel[0];
            st->vel[1] = -universe_->mat[0][1] * v * sinw + universe_->mat[1][1] * v * cosw + gt->vel[1];
            st->vel[2] = -universe_->mat[0][2] * v * sinw + universe_->mat[1][2] * v * cosw + gt->vel[2];
            st->vel[0] *= DELTAT;
            st->vel[1] *= DELTAT;
            st->vel[2] *= DELTAT;
            oldp->x = 0;
            oldp->y = 0;
            newp->x = 0;
            newp->y = 0;
        }
    }

    win.ClearScreen();

#if 0
    (void) printf("ngalaxies=%d, f_hititerations=%d\n", universe_->ngalaxies, universe_->f_hititerations);
    (void) printf("f_deltat=%g\n", DELTAT);
    (void) printf("Screen: ");
#endif /*0 */
}

UINT32 DispMainUi::InitGalaxy(DispTexture &win)
{
    Universe  *universe_;

    if (universe_ == NULL_PTR) {
        universe_ = new (DispMainUi::Universe);
        if (universe_ == NULL_PTR) {
            return OS_ERROR;
        }
    }

    universe_->f_hititerations = 0;
    universe_->scale = (double) (win.GetWidth() + win.GetHeight()) / 8.0;
    universe_->midx =  win.GetWidth()  / 2;
    universe_->midy =  win.GetHeight() / 2;
    StartOver(win);

    return OS_OK;
}

void DispMainUi::DrawGalaxy(DispTexture &win)
{
    double      d, eps, cox, six, cor, sir;  /* tmp */
    int         i, j, k; /* more tmp */

    if(spin){
        universe_->rot_y += 0.01;
        universe_->rot_x += 0.004;
    }

    cox = COSF(universe_->rot_y);
    six = SINF(universe_->rot_y);
    cor = COSF(universe_->rot_x);
    sir = SINF(universe_->rot_x);

    eps = 1/(EPSILON * sqrt_EPSILON * DELTAT * DELTAT * QCONS);

    for (i = 0; i < universe_->ngalaxies; ++i) {
        Galaxy     *gt = &universe_->galaxies[i];

        for (j = 0; j < universe_->galaxies[i].nstars; ++j) {
            Star       *st = &gt->stars[j];
            Point *newp = &gt->newPoints[j];
            double      v0 = st->vel[0];
            double      v1 = st->vel[1];
            double      v2 = st->vel[2];

            for (k = 0; k < universe_->ngalaxies; ++k) {
                Galaxy     *gtk = &universe_->galaxies[k];
                double      d0 = gtk->pos[0] - st->pos[0];
                double      d1 = gtk->pos[1] - st->pos[1];
                double      d2 = gtk->pos[2] - st->pos[2];

                d = d0 * d0 + d1 * d1 + d2 * d2;
                if (d > EPSILON) {
                    d = gtk->mass / (d * sqrt(d)) * DELTAT * DELTAT * QCONS;
                } else {
                    d = gtk->mass / (eps * sqrt(eps));
                }
                v0 += d0 * d;
                v1 += d1 * d;
                v2 += d2 * d;
            }

            st->vel[0] = v0;
            st->vel[1] = v1;
            st->vel[2] = v2;
            st->pos[0] += v0;
            st->pos[1] += v1;
            st->pos[2] += v2;

            newp->x = (short) (((cox * st->pos[0]) - (six * st->pos[2])) * universe_->scale) + universe_->midx;
            newp->y = (short) (((cor * st->pos[1]) - (sir * ((six * st->pos[0]) + (cox * st->pos[2]))))
                               * universe_->scale) + universe_->midy;
        }

        for (k = i + 1; k < universe_->ngalaxies; ++k) {
            Galaxy     *gtk = &universe_->galaxies[k];
            double      d0 = gtk->pos[0] - gt->pos[0];
            double      d1 = gtk->pos[1] - gt->pos[1];
            double      d2 = gtk->pos[2] - gt->pos[2];

            d = d0 * d0 + d1 * d1 + d2 * d2;
            if (d > EPSILON) {
                d = 1 / (d * sqrt(d)) * DELTAT * QCONS;
            } else {
                d = 1 / (EPSILON * sqrt_EPSILON) * DELTAT * QCONS;
            }

            d0 *= d;
            d1 *= d;
            d2 *= d;
            gt->vel[0] += d0 * gtk->mass;
            gt->vel[1] += d1 * gtk->mass;
            gt->vel[2] += d2 * gtk->mass;
            gtk->vel[0] -= d0 * gt->mass;
            gtk->vel[1] -= d1 * gt->mass;
            gtk->vel[2] -= d2 * gt->mass;
        }

        gt->pos[0] += gt->vel[0] * DELTAT;
        gt->pos[1] += gt->vel[1] * DELTAT;
        gt->pos[2] += gt->vel[2] * DELTAT;

        // 如果使用双缓冲，执行下方4行
//        if (dbufp) {
//            XSetForeground(display, gc, MI_WIN_BLACK_PIXEL(mi));
//            XDrawPoints(display, window, gc, gt->oldPoints, gt->nstars, CoordModeOrigin);
//        }
//        XSetForeground(display, gc, MI_PIXEL(mi, COLORSTEP * gt->galcol));
//        XDrawPoints(display, window, gc, gt->newPoints, gt->nstars, CoordModeOrigin);
        win.DrawPoint((SDL_Point *)gt->newPoints, gt->nstars);

        // 新坐标点和旧坐标点交换存储。双缓冲机制
        Point *dummy = gt->oldPoints;
        gt->oldPoints = gt->newPoints;
        gt->newPoints = dummy;
    }

    universe_->step++;
    if (universe_->step > universe_->f_hititerations * 4) {
        StartOver(win);
    }
}

void DispMainUi::RefreshGalaxy(DispTexture &win)
{

}