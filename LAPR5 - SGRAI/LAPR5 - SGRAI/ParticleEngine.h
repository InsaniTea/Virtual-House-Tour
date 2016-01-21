#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <GL/glaux.h>
#endif

#include "imageloader.h"
#include "vec3f.h"

/////SISTEMA DE PARTICULAS/////
typedef struct Particle {
	Vec3f pos;
	Vec3f velocity;
	Vec3f color;
	float timeAlive; //The amount of time that this particle has been alive.
	float lifespan;  //The total amount of time that this particle is to live.
} Particle;

class ParticleEngine{
public:
	ParticleEngine(GLuint textureId);

	Vec3f curColor();


	Vec3f curVelocity();


	//Alters p to be a particle newly produced by the fountain.
	void createParticle(Particle* p);


	//Advances the particle fountain by STEP_TIME seconds.
	void step();

	//Advances the particle fountain by the specified amount of time.
	void advance(float dt);

	//Draws the particle fountain.
	void draw();
	
	float randomFloat();

	Vec3f adjParticlePos(Vec3f pos);

	//Returns whether particle1 is in back of particle2
	bool compareParticles(Particle* particle1, Particle* particle2);

	//Rotates the vector by the indicated number of degrees about the specified axis
	Vec3f rotate(Vec3f v, Vec3f axis, float degrees);
};