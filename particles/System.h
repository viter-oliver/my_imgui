/******************************************************
 * Georg Albrecht                                     *
 * Final Project for CMPS 161, Winter 2009            *
 *                                                    *
 * System.h                                           *
 *    This is the header file for System.cpp. It      *
 *    contains the particle structure and the particle*
 *    system manager class. The definition for        *
 *    MAX_PARTICLES is contained in this file         *
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>

#pragma once

#define MAX_PARTICLES 3000

typedef float fVector[3];

enum dimensions{X,Y,Z};
enum type{Smoke=1,Fountain=2,Fire=3,FireWithSmoke=4};

struct Particle
{
   int type; //used only for "fire with smoke" to tell between fire and smoke 0=fire, 1=smoke, 2=other
   float lifespan, age, scale, direction; // how long the particle will exist for, alpha blending variable; how old is it.
   fVector position; // initial onscreen position
   fVector movement; // movement vector
   fVector color; // color values
   fVector pull; // compounding directional pull in the x,y,z directions
}; 

class System
{
   int systemType; //1 = smoke, 2 = fountain, 3 = fire, 4 = fire with smoke
   fVector systemPull; //used to store global compounding system pull in x,y,z
   Particle particles[MAX_PARTICLES]; //initalizes and array of type Particle
public:
   System(void);
   virtual ~System(void);
   void createParticle(Particle *p); //creates and initalizes a single particle
   void createParticles(); //calls createParticle() to initalize all particles in system
   void updateParticles(); //updates particles according to forces being used
   void turnToSmoke(Particle *p); //called only durring fire with smoke system to turn dead fire into smoke

   void setSystemType(int systemType); //sets the particle system type
   int getNumOfParticles(); // returns the number of particles in the system (legacy)
   float getXPos(int i); //returns x position of particle i
   float getYPos(int i); //returns y position of particle i
   float getZPos(int i); //returns z position of particle i
   float getR(int i); //returns red component of particle i
   float getG(int i); //returns green component of particle i
   float getB(int i); //returns blue component of particle i
   float getScale(int i); //returns scale of particle
   float getDirection(int i); //returns direction of particle for texture rotation
   float getAlpha(int i); //returns how faded (according to age) the particle should be

   void modifySystemPull(float x, float y, float z); //used to modify x,y,z pull magnitudes
};
