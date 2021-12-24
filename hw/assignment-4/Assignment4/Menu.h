#include "Angel-yjc.h"
#include "Globals.h"
#include <iostream>
#include <string>

void main_menu(int id) {
    switch (id)
    {
    case 0:
        // default view, so restart everything
        animationFlag = 0;
        eye = VRP;
        // Make sure to restore shadow if we were under the floor
        if (tempShadowFlag == 1) {
            shadowFlag = tempShadowFlag;
            tempShadowFlag = 0;
        }
        break;
    case 1:
        exit(EXIT_SUCCESS);
        break;
    case 2:
        solidOrWireFlag = 0;
        sphereTextureFlag = 0;
        break;
    case 3:
        floorTextureFlag = 1 - floorTextureFlag;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void shadow_menu(int id) {
    shadowFlag = id;
    glutPostRedisplay();
}

void light_menu(int id) {
    switch (id)
    {
    case 0:
        lightFlag = 0;
        spotLightFlag = 0;
        pointSourceFlag = 0;
        program = nonshading_program;
        break;
    case 1:
        lightFlag = 1;
        program = shading_program;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void shading_menu(int id) {
    shadingFlag = id;
    solidOrWireFlag = 1;
    glutPostRedisplay();
}

void position_light_menu(int id) {
    if (lightFlag == 0) return;
    switch (id)
    {
    case 0:
        spotLightFlag = 1;
        pointSourceFlag = 0;
        break;
    case 1:
        spotLightFlag = 0;
        pointSourceFlag = 1;
        break;
    default:
        break;
    }
}

void fog_menu(int id) {
    fogFlag = id;
}

void blend_menu(int id) {
    shadowBlendFlag = id;
}

void sphere_texture(int id) {
    switch (id) {
    case 1:
        verOrSlantFlag = 0;
        objectOrEyeFlag = 0;
        break;
    case 2:
        verOrSlantFlag = 0;
        objectOrEyeFlag = 0;
        break;
    }
    if (solidOrWireFlag == 0)
        sphereTextureFlag = 0;
    else
        sphereTextureFlag = id;
}

void firework_menu(int id) {
    switch (id)
    {
    case 0:
        fireworkFlag = 0;
        break;
    case 1:
        fireworkFlag = 1;
        T = (float) glutGet(GLUT_ELAPSED_TIME);
        Tsub = (float) glutGet(GLUT_ELAPSED_TIME);
        break;
    default:
        break;
    }
}

class Menu {
public:
    //----------------------------------------------------------------------------
// Handling the menus

    void setupMenu() {
        int smenu1 = glutCreateMenu(shadow_menu);
        glutSetMenuFont(smenu1, GLUT_BITMAP_HELVETICA_18);
        glutAddMenuEntry(" No ", 0);
        glutAddMenuEntry(" Yes ", 1);

        int lmenu = glutCreateMenu(light_menu);
        glutSetMenuFont(lmenu, GLUT_BITMAP_HELVETICA_18);
        glutAddMenuEntry(" No ", 0);
        glutAddMenuEntry(" Yes ", 1);

        int smenu2 = glutCreateMenu(shading_menu);
        glutSetMenuFont(smenu2, GLUT_BITMAP_HELVETICA_18);
        glutAddMenuEntry(" Flat Shading ", 0);
        glutAddMenuEntry(" Smooth Shading ", 1);

        int poslightmenu = glutCreateMenu(position_light_menu);
        glutSetMenuFont(poslightmenu, GLUT_BITMAP_HELVETICA_18);
        glutAddMenuEntry(" Spot Light ", 0);
        glutAddMenuEntry(" Point Source ", 1);

        int fogmenu = glutCreateMenu(fog_menu);
        glutSetMenuFont(fogmenu, GLUT_BITMAP_HELVETICA_18);
        glutAddMenuEntry(" No Fog ", 0);
        glutAddMenuEntry(" Linear ", 1);
        glutAddMenuEntry(" Exponential ", 2);
        glutAddMenuEntry(" Exponential Square ", 3);

        int blendmenu = glutCreateMenu(blend_menu);
        glutSetMenuFont(blendmenu, GLUT_BITMAP_HELVETICA_18);
        glutAddMenuEntry(" No ", 0);
        glutAddMenuEntry(" Yes ", 1);

        int stexture = glutCreateMenu(sphere_texture);
        glutSetMenuFont(stexture, GLUT_BITMAP_HELVETICA_18);
        glutAddMenuEntry(" No ", 0);
        glutAddMenuEntry(" Yes - Contour Lines ", 1);
        glutAddMenuEntry(" Yes - Checkerboard ", 2);

        int fireworkmenu = glutCreateMenu(firework_menu);
        glutSetMenuFont(fireworkmenu, GLUT_BITMAP_HELVETICA_18);
        glutAddMenuEntry(" No ", 0);
        glutAddMenuEntry(" Yes ", 1);

        int menu_ID;
        menu_ID = glutCreateMenu(main_menu);
        glutSetMenuFont(menu_ID, GLUT_BITMAP_HELVETICA_18);
        glutAddMenuEntry(" Default View Point ", 0);
        glutAddMenuEntry(" Quit ", 1);
        glutAddMenuEntry(" Wire Frame Sphere ", 2);
        glutAddSubMenu(" Shadow ", smenu1);
        glutAddSubMenu(" Enable Lighting ", lmenu);
        glutAddSubMenu(" Shading ", smenu2);
        glutAddSubMenu(" Light Source ", poslightmenu);
        glutAddSubMenu(" Fog Options ", fogmenu);
        glutAddSubMenu(" Blending Shadow ", blendmenu);
        glutAddMenuEntry(" Texture Mapped Ground ", 3);
        glutAddSubMenu(" Texture Mapped Sphere ", stexture);
        glutAddSubMenu(" Firework ", fireworkmenu);
        glutAttachMenu(GLUT_LEFT_BUTTON);
    }
};