/******************************************************
 * Georg Albrecht                                     *
 * Final Project for CMPS 161, Winter 2009            *
 *                                                    *
 * System.cpp                                         *
 *    This is the source file for System.h. It        *
 *    contains the code necessary to initalize, update*
 *    and manage and array of particles               *
 ******************************************************/

#include "System.h"

System::System(void)
{
}

System::~System(void)
{
}

/*
 * initalizes a single particle according to its type
 */
void System::createParticle(Particle *p)
{
   if(systemType == Fire || systemType == FireWithSmoke)
   {
      p->lifespan = (((rand()%10+1)))/10.0f;
      if(systemType == FireWithSmoke)
         p->type = 0;
   }
   else
   {
      p->lifespan = (((rand()%125+1)/10.0f)+5);
      p->type = 2;
   }

   p->age = 0.0f;
   p->scale = 0.25f;
   p->direction = 0;
   
   if(systemType == Smoke || systemType == Fire || systemType == FireWithSmoke)
      p->position[X] = ((rand()%2)-(rand()%2));
   else
      p->position[X] = 0;

   if(systemType == Smoke)
      p->position[Y] = -30;
   else
      p->position[Y] = -15;

   p->position[Z] = 0;

   if(systemType == Smoke)
   {
      p->movement[X] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0035) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0035);
      p->movement[Y] = ((((((5) * rand()%11) + 3)) * rand()%11) + 7) * 0.015; 
      p->movement[Z] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0015) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0015);
   }
   else if(systemType == Fountain)
   {
      p->movement[X] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.005) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.005);   
      p->movement[Y] = ((((((5) * rand()%11) + 5)) * rand()%11) + 10) * 0.02;
      p->movement[Z] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.005) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.005);
   }
   else if(systemType == Fire || systemType == FireWithSmoke)
   { 
      p->movement[X] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007);   
      p->movement[Y] = ((((((5) * rand()%11) + 5)) * rand()%11) + 1) * 0.02;
      p->movement[Z] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007);
   }
   else //just in case
   { 
      p->movement[X] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.005) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.005);   
      p->movement[Y] = ((((((5) * rand()%11) + 5)) * rand()%11) + 1) * 0.02;
      p->movement[Z] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.005) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.005);
   }

   if(systemType == Smoke)
   {//greyish-white for smoke
      p->color[X] = 0.7f;
      p->color[Y] = 0.7f;
      p->color[Z] = 0.7f;
   }
   else if(systemType == Fountain)
   {//blue for water
      p->color[X] = 0.0f;
      p->color[Y] = 0.0f;
      p->color[Z] = 1.0f;
   }
   else if(systemType == Fire || systemType == FireWithSmoke)
   {//red with mix of yellow for fire
      p->color[X] = 1.0f;
      p->color[Y] = 0.95f;
      p->color[Z] = 0.8f;
   }

   p->pull[X] = 0.0f;
   p->pull[Y] = 0.0f;
   p->pull[Z] = 0.0f;
}

/*
 * initalizes a particle system according to its type
 * calls create particle() to initalize individual particles
 */
void System::createParticles(void)
{
   if(systemType == Fountain)
   {
      systemPull[Y] = -0.0025;
      systemPull[X] = systemPull[Z] = 0.0f;
   }
   else if(systemType == Fire)
   {
      systemPull[Y] = 0.005;
      systemPull[X] = systemPull[Z] = 0.0f;
   }
   else if(systemType == FireWithSmoke)
   {
      systemPull[Y] = 0.0005;
      systemPull[X] = systemPull[Z] = 0.0f;
   }
   else
      systemPull[X] = systemPull[Y] = systemPull[Z] = 0.0f;
   int i = 0;
   for(i; i < MAX_PARTICLES; i++)
   {
      createParticle(&particles[i]);
   }
}

/*
 * updates required particle attributes for all particles in a system
 * also responsible for killing and respawning (via createparticle()) individual particles
 */
void System::updateParticles(void)
{
   int i = 0;
   for(i; i < MAX_PARTICLES; i++)
   {
      particles[i].age = particles[i].age + 0.02;
      
      if(systemType == Smoke || particles[i].type == 1)
         particles[i].scale = particles[i].scale + 0.001; //increasing scale makes textures bigger over lifetime

      particles[i].direction = particles[i].direction + ((((((int)(0.5) * rand()%11) + 1)) * rand()%11) + 1);

      particles[i].position[X] = particles[i].position[X] + particles[i].movement[X] + particles[i].pull[X];
      particles[i].position[Y] = particles[i].position[Y] + particles[i].movement[Y] + particles[i].pull[Y];
      particles[i].position[Z] = particles[i].position[Z] + particles[i].movement[Z] + particles[i].pull[Z];
      
      particles[i].pull[X] = particles[i].pull[X] + systemPull[X];
      particles[i].pull[Y] = particles[i].pull[Y] + systemPull[Y]; // acleration due to gravity
      particles[i].pull[Z] = particles[i].pull[Z] + systemPull[Z];

      // color changing for fire particles light yellow -> red with age
      if(systemType == Fire || particles[i].type == 0)
      {
         float temp = particles[i].lifespan/particles[i].age;
         if((temp) < 1.75)
         {//red
            particles[i].color[X] = 1.0f;
            particles[i].color[Y] = 0.25f;
            particles[i].color[Z] = 0.0f;
         }
         else if((temp) < 3.0)
         {//gold
            particles[i].color[X] = 1.0f;
            particles[i].color[Y] = 0.9f;
            particles[i].color[Z] = 0.0f;
         }
         else if((temp) < 10.0)
         {//yellow
            particles[i].color[X] = 1.0f;
            particles[i].color[Y] = 1.0f;
            particles[i].color[Z] = 0.0f;
         }
         else
         {// initial light yellow
            particles[i].color[X] = 1.0f;
            particles[i].color[Y] = 0.95f;
            particles[i].color[Z] = 0.8f;
         }
      }

      if(systemType == Smoke)
      {
         if (particles[i].age > particles[i].lifespan || particles[i].position[Y] > 45 || particles[i].position[Y] < -35 || particles[i].position[X] > 80 || particles[i].position[X] < -80)
            createParticle(&particles[i]);
      }
      else if(systemType == Fountain)
      {
         if (particles[i].position[Y] > 35 || particles[i].position[Y] < -25 || particles[i].position[X] > 40 || particles[i].position[X] < -40)
            createParticle(&particles[i]);
      }
      else if(systemType == Fire)
      {
         if (particles[i].age > particles[i].lifespan || particles[i].position[Y] > 35 || particles[i].position[Y] < -25 || particles[i].position[X] > 40 || particles[i].position[X] < -40)
            createParticle(&particles[i]);
      }
      else if(systemType == FireWithSmoke)
      {
         if(particles[i].type == 0)
         {
            if (particles[i].age > particles[i].lifespan || particles[i].position[Y] > 35 || particles[i].position[Y] < -25 || particles[i].position[X] > 40 || particles[i].position[X] < -40)
            {
               int temp = rand()%100;
               if(temp < 10)
                  turnToSmoke(&particles[i]);
               else
                  createParticle(&particles[i]);
            }
         }
         else if(particles[i].type == 1)
         {
            if (particles[i].age > particles[i].lifespan || particles[i].position[Y] > 45 || particles[i].position[Y] < -35 || particles[i].position[X] > 80 || particles[i].position[X] < -80)
               createParticle(&particles[i]);
         }
      }
   }
}

/*
 * used only by updateparticles() and only when the fire and smoke system is active
 * used to turn selected fire particles into smoke
 */
void System::turnToSmoke(Particle *p)
{
   p->lifespan = (((rand()%125+1)/10.0f)+5);
   p->age = 0.0f;
   p->scale = 0.25f;
   p->direction = 0;
   p->type = 1;

   p->movement[X] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0035) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0035);
   p->movement[Y] = ((((((5) * rand()%11) + 3)) * rand()%11) + 7) * 0.015; 
   p->movement[Z] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0015) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.0015);

   p->color[X] = 0.7f;
   p->color[Y] = 0.7f;
   p->color[Z] = 0.7f;
}

void System::setSystemType(int type)
{
   systemType = type;
}

int System::getNumOfParticles(void)
{
   return MAX_PARTICLES;
}

float System::getXPos(int i)
{
   return particles[i].position[X];
}

float System::getYPos(int i)
{
   return particles[i].position[Y];
}
float System::getZPos(int i)
{
   return particles[i].position[Z];
}

float System::getR(int i)
{
   return particles[i].color[X];
}

float System::getG(int i)
{
   return particles[i].color[Y];
}
float System::getB(int i)
{
   return particles[i].color[Z];
}

float System::getScale(int i)
{
   return particles[i].scale;
}

float System::getDirection(int i)
{
   return particles[i].direction;
}

float System::getAlpha(int i)
{
   return (1 - particles[i].age/particles[i].lifespan);
}

void System::modifySystemPull(float x, float y, float z)
{
   systemPull[X] += x;
   systemPull[Y] += y;
   systemPull[Z] += z;
}
