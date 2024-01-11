#include "Particles.h"
#include "Camera.h"

ParticleSystem::ParticleSystem()
{
	isInitialized = false;
	iCurReadBuffer = 0;
	uiQuery = 0;
}

bool ParticleSystem::InitalizeParticleSystem(){
	if (isInitialized)return false;

	const char* sVaryings[NUM_PARTICLE_ATTRIBUTES] =
	{
		"vPositionOut",
		"vVelocityOut",
		"vColorOut",
		"fLifeTimeOut",
		"fSizeOut",
		"iTypeOut",
	};

	spUpdateParticles = Shader("ParticlesUpdateVer.vert","ParticlesUpdateGeo.geom",sVaryings,NUM_PARTICLE_ATTRIBUTES);
	spRenderParticles = Shader("ParticleRenderVer.vert", "ParticleRenderFrag.frag", "ParticleRenderGeo.geom");
	//spRenderParticles = Shader("ParticleRenderVer.vert", "LightFragShader.frag");
	
	glGenTransformFeedbacks(1, &uiTransformFeedbackBuffer);
	glGenQueries(1, &uiQuery);

	glGenBuffers(2, uiParticleBuffer);
	glGenVertexArrays(2, uiVAO);

	Particle partInitialization;
	partInitialization.iType = PARTICLE_TYPE_GENERATOR;

	for(int i = 0; i < 2; i++)
	{
		glBindVertexArray(uiVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, uiParticleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * MAX_PARTICLES_ON_SCENE, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle), &partInitialization);

		for(int j =0; j < NUM_PARTICLE_ATTRIBUTES; j++)
			glEnableVertexAttribArray(j);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)0); // Position
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)12); // Velocity
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)24); // Color
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)36); // Lifetime
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)40); // Size
		glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(Particle), (const GLvoid*)44); // Type
	}
	iCurReadBuffer = 0;
	iNumParticles = 1;
	isInitialized = true;
	return true;
}

float grandf(float fMin, float fAdd)
{
	float fRandom = float(rand() % (RAND_MAX + 1)) / float(RAND_MAX);
	return fMin + fAdd * fRandom;
}

void ParticleSystem::UpdateParticles(float fTimePassed)
{
	if (!isInitialized)return;
	spUpdateParticles.use();

	glm::vec3 vUpload;
	spUpdateParticles.setFloat("fTimePassed", fTimePassed);
	spUpdateParticles.setVec3F("vGenPosition", vGenPosition);
	spUpdateParticles.setVec3F("vGenVelocityMin", vGenVelocityMin);
	spUpdateParticles.setVec3F("vGenVelocityRange", vGenVelocityRange);
	spUpdateParticles.setVec3F("vGenColor", vGenColor);
	spUpdateParticles.setVec3F("vGenGravityVector", vGenGravityVector);

	spUpdateParticles.setFloat("fGenLifeMin", fGenLifeMin);
	spUpdateParticles.setFloat("fGenLifeRange", fGenLifeRange);

	spUpdateParticles.setFloat("fGenSize", fGenSize);
	spUpdateParticles.setInt("iNumToGenerate", 0);

	fElapsedTime += fTimePassed;

	if (fElapsedTime > fNextGenerationTime)
	{
		spUpdateParticles.setInt("iNumToGenerate", iNumToGenerate);
		fElapsedTime -= fNextGenerationTime;

		glm::vec3 vRandomSeed = glm::vec3(grandf(-10.0f, 20.0f), grandf(-10.0f, 20.0f), grandf(-10.0f, 20.0f));
		spUpdateParticles.setVec3F("vRandomSeed", vRandomSeed);
	}

	glEnable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, uiTransformFeedbackBuffer);

	glBindVertexArray(uiVAO[iCurReadBuffer]);
	glEnableVertexAttribArray(1); // Re-enable velocity

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, uiParticleBuffer[1 - iCurReadBuffer]);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, uiQuery);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, iNumParticles);

	glEndTransformFeedback();

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glGetQueryObjectiv(uiQuery, GL_QUERY_RESULT, &iNumParticles);
	iCurReadBuffer = 1 - iCurReadBuffer;

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void ParticleSystem::RenderParticles()
{
	if (!isInitialized)return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(0);

	glDisable(GL_RASTERIZER_DISCARD);
	spRenderParticles.use();
	spRenderParticles.setMat4F("matrices.mProj", matProjection);
	spRenderParticles.setMat4F("matrices.mView", matView);
	spRenderParticles.setVec3F("vQuad1", vQuad1);
	spRenderParticles.setVec3F("vQuad2", vQuad2);
	spRenderParticles.setInt("gSampler", texture.getTextureIndex());
	glBindTexture(GL_TEXTURE_2D, texture.getTextureIndex());

	glBindVertexArray(uiVAO[iCurReadBuffer]);
	glDisableVertexAttribArray(1); // Disable velocity, because we don't need it for rendering

	glDrawArrays(GL_POINTS, 0, iNumParticles);

	glDepthMask(1);
	glDisable(GL_BLEND);
}

void ParticleSystem::SetMatrices(glm::mat4* a_matProjection, const Camera& camera)
{
	matProjection = *a_matProjection;
	matView = camera.GetViewMatrix();
	glm::vec3 vView;
	vView = camera.Front;
	vView = glm::normalize(vView);
	vQuad1 = glm::cross(vView, camera.Up);
	vQuad1 = glm::normalize(vQuad1);
	vQuad2 = glm::cross(vView, vQuad1);
	vQuad2 = glm::normalize(vQuad2);
}

void ParticleSystem::SetGeneratorProperties(glm::vec3 a_vGenPosition, glm::vec3 a_vGenVelocityMin, glm::vec3 a_vGenVelocityMax, glm::vec3 a_vGenGravityVector, glm::vec3 a_vGenColor, float a_fGenLifeMin, float a_fGenLifeMax, float a_fGenSize, float fEvery, int a_iNumToGenerate)
{
	vGenPosition = a_vGenPosition;
	vGenVelocityMin = a_vGenVelocityMin;
	vGenVelocityRange = a_vGenVelocityMax - a_vGenVelocityMin;

	vGenGravityVector = a_vGenGravityVector;
	vGenColor = a_vGenColor;
	fGenSize = a_fGenSize;

	fGenLifeMin = a_fGenLifeMin;
	fGenLifeRange = a_fGenLifeMax - a_fGenLifeMin;

	fNextGenerationTime = fEvery;
	fElapsedTime = 0.8f;

	iNumToGenerate = a_iNumToGenerate;
}

int ParticleSystem::GetNumParticles()
{
	return iNumParticles;
}

void ParticleSystem::SetTexture(const Texture& texture) {
	this->texture = texture;
}