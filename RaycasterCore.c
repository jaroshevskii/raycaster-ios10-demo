#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "RaycasterCore.h"

#define MAP_W 24
#define MAP_H 24
#define TEX_W 64
#define TEX_H 64

static const int worldMap[MAP_W][MAP_H] = {
    {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
    {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
    {4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
    {4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
    {4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
    {4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
    {4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
    {4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
    {4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1},
    {4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
    {4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
    {4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
    {6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
    {6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
    {4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
    {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
    {4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
    {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
    {4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
    {4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
    {4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
    {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
    {4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
};

static unsigned char textures[8][TEX_W * TEX_H * 4];

static double posX, posY;
static double dirX, dirY;
static double planeX, planeY;

void rc_init(void) {
    posX = 22.0;  posY = 11.5;
    dirX = -1.0;  dirY = 0.0;
    planeX = 0.0; planeY = 0.66;

    for (int t = 0; t < 8; t++)
        for (int y = 0; y < TEX_H; y++)
            for (int x = 0; x < TEX_W; x++) {
                int xorcolor = (x * 256 / TEX_W) ^ (y * 256 / TEX_H);
                int ycolor = y * 256 / TEX_H;
                int xycolor = y * 128 / TEX_H + x * 128 / TEX_W;
                unsigned int c;
                switch (t) {
                    case 0: c = 65536 * 254 * (x != y && x != TEX_W - y); break;
                    case 1: c = xycolor + 256 * xycolor + 65536 * xycolor; break;
                    case 2: c = 256 * xycolor + 65536 * xycolor; break;
                    case 3: c = xorcolor + 256 * xorcolor + 65536 * xorcolor; break;
                    case 4: c = 256 * xorcolor; break;
                    case 5: c = 65536 * 192 * (x % 16 && y % 16); break;
                    case 6: c = 65536 * ycolor; break;
                    default: c = 128 + 256 * 128 + 65536 * 128; break;
                }
                unsigned char *dst = &textures[t][(TEX_H * y + x) * 4];
                dst[0] = (c >> 16) & 0xff;
                dst[1] = (c >> 8) & 0xff;
                dst[2] = c & 0xff;
                dst[3] = 255;
            }
}

void rc_set_texture(int slot, const unsigned char *rgba, int w, int h) {
    if (slot < 0 || slot > 7) return;
    unsigned char *dst = textures[slot];
    if (w == TEX_W && h == TEX_H) {
        memcpy(dst, rgba, TEX_W * TEX_H * 4);
        return;
    }
    for (int y = 0; y < TEX_H; y++) {
        int sy = (y * h) / TEX_H;
        for (int x = 0; x < TEX_W; x++) {
            int sx = (x * w) / TEX_W;
            const unsigned char *s = &rgba[(sy * w + sx) * 4];
            unsigned char *d = &dst[(y * TEX_W + x) * 4];
            d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = 255;
        }
    }
}

void rc_render(unsigned char *out, int w, int h) {
    unsigned int *px = (unsigned int *)out;
    for (int i = 0; i < w * h; i++) px[i] = 0xFF000000u;
    double rayDirX0 = dirX - planeX;
    double rayDirY0 = dirY - planeY;
    double rayDirX1 = dirX + planeX;
    double rayDirY1 = dirY + planeY;

    for (int y = h / 2 + 1; y < h; y++) {
        double p = y - h / 2;
        double rowDistance = (0.5 * h) / p;
        double floorStepX = rowDistance * (rayDirX1 - rayDirX0) / w;
        double floorStepY = rowDistance * (rayDirY1 - rayDirY0) / w;
        double floorX = posX + rowDistance * rayDirX0;
        double floorY = posY + rowDistance * rayDirY0;
        int yc = h - y - 1;
        for (int x = 0; x < w; x++) {
            int cellX = (int)floorX;
            int cellY = (int)floorY;
            int tx = ((int)(TEX_W * (floorX - cellX))) & (TEX_W - 1);
            int ty = ((int)(TEX_H * (floorY - cellY))) & (TEX_H - 1);
            floorX += floorStepX;
            floorY += floorStepY;
            if (cellX < 0 || cellX >= MAP_W || cellY < 0 || cellY >= MAP_H) continue;
            int texNum = worldMap[cellX][cellY] - 1;
            if (texNum < 0 || texNum > 7) continue;
            int ti = (TEX_H * ty + tx) * 4;
            const unsigned char *t = textures[texNum];
            unsigned char *fo = &out[(y * w + x) * 4];
            fo[0] = t[ti]; fo[1] = t[ti + 1]; fo[2] = t[ti + 2]; fo[3] = 255;
            unsigned char *co = &out[(yc * w + x) * 4];
            co[0] = t[ti]; co[1] = t[ti + 1]; co[2] = t[ti + 2]; co[3] = 255;
        }
    }

    for (int x = 0; x < w; x++) {
        double cameraX = 2 * x / (double)w - 1;
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        int mapX = (int)posX;
        int mapY = (int)posY;

        double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
        double perpWallDist;
        int stepX, stepY;
        int hit = 0, side = 0;
        double sideDistX, sideDistY;

        if (rayDirX < 0) { stepX = -1; sideDistX = (posX - mapX) * deltaDistX; }
        else             { stepX = 1;  sideDistX = (mapX + 1.0 - posX) * deltaDistX; }
        if (rayDirY < 0) { stepY = -1; sideDistY = (posY - mapY) * deltaDistY; }
        else             { stepY = 1;  sideDistY = (mapY + 1.0 - posY) * deltaDistY; }

        while (hit == 0) {
            if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }
            else                       { sideDistY += deltaDistY; mapY += stepY; side = 1; }
            if (worldMap[mapX][mapY] > 0) hit = 1;
        }

        perpWallDist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
        if (perpWallDist < 0.001) perpWallDist = 0.001;

        int lineHeight = (int)(h / perpWallDist);
        int drawStart = -lineHeight / 2 + h / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + h / 2;
        if (drawEnd >= h) drawEnd = h - 1;

        int texNum = worldMap[mapX][mapY] - 1;
        if (texNum < 0) texNum = 0;
        if (texNum > 7) texNum = 7;

        double wallX = (side == 0) ? (posY + perpWallDist * rayDirY) : (posX + perpWallDist * rayDirX);
        wallX -= floor(wallX);

        int texX = (int)(wallX * TEX_W);
        if (side == 0 && rayDirX > 0) texX = TEX_W - texX - 1;
        if (side == 1 && rayDirY < 0) texX = TEX_W - texX - 1;

        double step = 1.0 * TEX_H / lineHeight;
        double texPos = (drawStart - h / 2 + lineHeight / 2) * step;
        unsigned char *tex = textures[texNum];

        for (int y = drawStart; y < drawEnd; y++) {
            int texY = ((int)texPos) & (TEX_H - 1);
            texPos += step;
            int ti = (TEX_H * texY + texX) * 4;
            unsigned int r = tex[ti], g = tex[ti + 1], b = tex[ti + 2];
            if (side == 1) { r >>= 1; g >>= 1; b >>= 1; }
            unsigned char *o = &out[(y * w + x) * 4];
            o[0] = r; o[1] = g; o[2] = b; o[3] = 255;
        }
    }
}

void rc_tick(int fwd, int back, int left, int right, double moveSpeed, double rotSpeed, double extraTurn) {
    if (fwd) {
        if (worldMap[(int)(posX + dirX * moveSpeed)][(int)posY] == 0) posX += dirX * moveSpeed;
        if (worldMap[(int)posX][(int)(posY + dirY * moveSpeed)] == 0) posY += dirY * moveSpeed;
    }
    if (back) {
        if (worldMap[(int)(posX - dirX * moveSpeed)][(int)posY] == 0) posX -= dirX * moveSpeed;
        if (worldMap[(int)posX][(int)(posY - dirY * moveSpeed)] == 0) posY -= dirY * moveSpeed;
    }
    double angle = (right ? -rotSpeed : 0.0) + (left ? rotSpeed : 0.0) + extraTurn;
    if (angle != 0.0) {
        double oldDirX = dirX;
        dirX = dirX * cos(angle) - dirY * sin(angle);
        dirY = oldDirX * sin(angle) + dirY * cos(angle);
        double oldPlaneX = planeX;
        planeX = planeX * cos(angle) - planeY * sin(angle);
        planeY = oldPlaneX * sin(angle) + planeY * cos(angle);
    }
}