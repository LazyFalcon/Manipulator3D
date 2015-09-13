
#include <list>
const float KF = 64.f/1024.f;
glm::vec4 gravity(0,0,-10,0);
glm::vec4 dustTex(0,0,128/1024.f,128/1024.f);
// glm::vec4 dustTex(0,384/1024.f,256/1024.f,256/1024.f);
glm::vec4 smokeJetTex(0,640/1024.f,128/1024.f,384 /1024.f);
glm::vec4 mainGroundExplosion(512.f/1024.f,512.f/1024.f,512.f/1024.f,512.f/1024.f);
glm::vec4 gunFire(768.f/1024.f,320.f/1024.f,256.f/1024.f,128.f/1024.f);
glm::vec4 fires[3];
glm::vec4 smokes[4];
glm::vec4 firePositions[40];

void initialAZSF(){
	fires[0] = glm::vec4(256.f/1024.f,0.f/1024.f,128.f/1024.f,128.f/1024.f);
	fires[1] = glm::vec4(384.f/1024.f,0.f/1024.f,128.f/1024.f,128.f/1024.f);
	fires[2] = glm::vec4(512.f/1024.f,0.f/1024.f,128.f/1024.f,128.f/1024.f);
	smokes[0] = glm::vec4(768.f/1024.f,0.f/1024.f,128.f/1024.f,128.f/1024.f);
	smokes[1] = glm::vec4(768.f/1024.f,128.f/1024.f,128.f/1024.f,128.f/1024.f);
	smokes[2] = glm::vec4(896.f/1024.f,0.f/1024.f,128.f/1024.f,128.f/1024.f);
	smokes[3] = glm::vec4(896.f/1024.f,128.f/1024.f,128.f/1024.f,128.f/1024.f);
	glm::vec4 vecx(1,0,0,0);
	glm::vec4 vecy(0,1,0,0);
	glm::vec4 vecz(0,0,1,0);
	glm::vec4 zero(0,0,0,0);
	for(int i=0; i<20; i++){
		float depth = abs(1-RNG())*2;
		float al = depth/13.f*6.f;
		firePositions[i] = zero + glm::vec4(15*depth, (1-RNG())*al, (1-RNG())*al,0);
		// firePositions[i] = zero + glm::vec4(RNG()*5, 0,0,0);
	}
	for(int i=20; i<40; i++){
		firePositions[i] = zero + glm::vec4(abs(RNG())*2, (1-RNG())*10, (1-RNG())*5,0);
	}
	
}



struct Particles {
	vector<glm::vec4> m_positions;
	vector<glm::vec4> m_info;
	vector<glm::vec4> m_texture;
	vector<float> m_livingTime;
	vector<glm::vec4> m_velocity;
	vector<glm::vec4> m_vector;
	vector<glm::vec4> m_sizeVelocity;
	vector<float> m_mass;
	

	float m_fading{0.98f};
	void update(float dt){
		vector<int> toDelete;
		for(int i=0; i<m_positions.size(); i++){
			if(m_livingTime[i]<0){
				toDelete.push_back(i);
				continue;
			}
			m_livingTime[i] -= dt;
			m_info[i] += m_sizeVelocity[i]*dt;
			m_info[i][0] = m_livingTime[i];
			m_sizeVelocity[i] *= m_fading;
			m_positions[i] += dt*m_velocity[i];
			m_vector[i] += gravity*m_mass[i]*dt;
			
		}
		for(auto i : toDelete){
			m_positions[i] = m_positions.back();
			m_positions.pop_back();
			
			m_info[i] = m_info.back();
			m_info.pop_back();
			
			m_livingTime[i] = m_livingTime.back();
			m_livingTime.pop_back();
			
			m_sizeVelocity[i] = m_sizeVelocity.back();
			m_sizeVelocity.pop_back();

			m_vector[i] = m_vector.back();
			m_vector.pop_back();

			m_texture[i] = m_texture.back();
			m_texture.pop_back();

			m_mass[i] = m_mass.back();
			m_mass.pop_back();

			m_velocity[i] = m_velocity.back();
			m_velocity.pop_back();
		}
		
		
	}
	void push(glm::vec4 position, glm::vec4 info, float living, float mass, glm::vec4 vector, glm::vec4 vel, float sizeVel, glm::vec4 tex){
		m_positions.push_back(position);
		m_mass.emplace_back(mass);
		m_info.emplace_back(info);
		m_livingTime.push_back(living);
		m_sizeVelocity.emplace_back(0,0,sizeVel,sizeVel);
		m_vector.push_back(glm::normalize(vector));
		m_texture.push_back(tex);
		m_velocity.push_back(vel);
	}
};

class Emiter {
public:
	float completeTime {0.f};
	Particles m_particlesVAP;
	Particles m_particlesNAP;
	Particles m_particlesSAP;
	virtual bool emit(float dt) = 0;
};
class ExplosionEmiter:public Emiter {
	float t1 {10.f}, t2{10.f};
	default_random_engine m_RNG;
	normal_distribution<float> m_distr;
public:
	glm::vec4 m_position;
	glm::vec4 m_vector {0,0,1,0};
	
	ExplosionEmiter(glm::vec4 _pos){
		m_position = _pos;
	}
	
	bool emit(float dt){
		m_particlesVAP.update(dt);
		m_particlesNAP.update(dt);
		if(completeTime > 1.0f)
			return true;
		t1 += dt;
		t2 += dt;
		if(t1 > 0.05f){
			t1 = 0.f;
			// emitDust(8);
			// emitHorizontalDust(10);
		}
		if(t2 > 0.5f){
			t2 = 0.f;
			// emitJet(3);
		}
		if(completeTime < 0.01f){
			initial();
		}
		completeTime += dt;
		return completeTime > 1.2f;
	}
	void initial(){
		glm::vec4 size(0,0,5,5);
		m_particlesVAP.push(
			m_position+glm::vec4((RNG()-1)*3,0,0,0)
			,size
			,1.3f*RNG()
			,0.005f*RNG()
			,m_vector
			,m_vector*0.f
			,26.5
			,mainGroundExplosion
		);			
		
	}
	void emitDust(int count){
		glm::vec4 dustSize(0,0,6,6);
		for(int i=0; i<count; i++){
			auto vec = glm::normalize((glm::vec4((RNG()-1)*5,0,1,0)));
			m_particlesVAP.push(
				m_position+glm::vec4((RNG()-1)*10,0,0,0)
				,dustSize
				,0.3f*RNG()
				,0.005f*RNG()
				,vec
				,vec*30.f
				,36.5
				,dustTex
			);
		}
	}
	void emitJet(int count){
		// glm::vec4 smokeSize(0,0,36,12);
		glm::vec4 smokeSize(0,0,18,6);
		for(int i=0; i<count; i++){
			auto vec = glm::normalize(m_vector+glm::vec4((RNG()-1)*10,0,0,0));
			m_particlesVAP.push(
				m_position+glm::vec4((RNG()-1)*3,-0.1f,0,0)
				,smokeSize
				,2*RNG()
				,0.0001f*RNG()
				,vec
				,vec*0.f
				,10
				,smokeJetTex
			);
		}
	}
	void emitHorizontalDust(int count){
		glm::vec4 dustSize(0,0,3,3);
		for(int i=0; i<count; i++){
			float angle = 2*PI*i/count+1-RNG();
			auto vec = glm::normalize((glm::vec4(cos(angle),sin(angle),0,0))*3.f);
			m_particlesVAP.push(
				m_position+glm::vec4((RNG()-1)*3,(RNG()-1)*3,0,0)
				,dustSize
				,1*RNG()
				,0.005f*RNG()
				,CameraUp
				,vec*7.f+glm::vec4(0,0,1,0)
				,36.5
				,dustTex
			);
		}
	}
	
	
};
 
class GunFireEmiter:public Emiter {
public:
	glm::vec4 m_position;
	glm::mat4 m_transform;
	float t1{0};
	int phase{0};
	
	GunFireEmiter(glm::vec4 _pos, glm::vec4 _vector_x, glm::vec4 _vector_y){
		m_position = _pos;
		m_transform = identity;
		setColumn(m_transform, 0, glm::normalize(_vector_x));
		setColumn(m_transform, 1, glm::normalize(_vector_y));
		
		glm::vec4 _vector_z = cross(_vector_x, _vector_y);
		setColumn(m_transform, 2, glm::normalize(_vector_z));
		// m_transform = glm::rotate(identity, -PI/3, glm::vec3(0,1,0));
		setColumn(m_transform, 3, _pos);
	}
	GunFireEmiter(const glm::mat4 &transform){
		m_transform = transform;
	}
	
	bool emit(float dt){
		m_particlesVAP.update(dt);
		m_particlesNAP.update(dt);
		m_particlesSAP.update(dt);
		if(completeTime > 5.3f)
			return true;
		if(completeTime < dt && phase == 0){
			initial();
			phase++;
		}
		if(completeTime > 0.03f && phase == 1){
			phaseTwo();
			phase++;
		}
		else if(completeTime > 0.05f && phase == 2){
			phaseThree();
			phase++;
		}
		completeTime += dt;
		return completeTime > 5.2f;
	}
	void initial(){
		glm::vec4 size(0,0,13,13);
		glm::vec4 firePosition(0,0,0,0);
		float lifetime = 0.3f;
		for(int i=0; i<90; i++){
			float depth = abs(1-RNG())*2;
			float al = (1-depth)*20.f;
			firePosition =  glm::vec4( (1-RNG())*15, 20*depth, (1-RNG())*15,1);
			m_particlesSAP.push(
				m_transform*firePosition
				,size*RNG(0.2)
				,lifetime*RNG(0.9)+depth/20
				,0.000f*RNG()
				,m_transform*firePosition
				,m_transform*firePosition*0.f     
				,3*RNG()
				,fires[i%3]
			);	
		}
		
	}
	void phaseTwo(){
		
		glm::vec4 size(0,0,13,13);
		glm::vec4 firePosition(0,0,0,0);
		float lifetime = 0.2f;
		for(int i=0; i<80; i++){
			float depth = abs(1-RNG())*2;
			float al = (depth)*20.f;
			firePosition =  glm::vec4( (1-RNG())*20, 20*depth+10, (1-RNG())*20,1);
			m_particlesSAP.push(
				m_transform*firePosition
				,size*RNG(0.2)
				,lifetime*RNG(0.9)+depth/20
				,0.000f*RNG()
				,m_transform*firePosition
				,m_transform*firePosition*1.f     
				,3*RNG()
				,fires[i%3]
			);	
		}
		
	}
	void phaseThree(){
		glm::vec4 size(0,0,11,11);
		glm::vec4 firePosition(0,0,0,0);
		float lifetime = 0.5f;
		for(int i=0; i<80; i++){
			float depth = abs(1-RNG())*2;
			float al = (depth)*20.f;
			firePosition =  glm::vec4( (1-RNG())*10, 20*depth+10, (1-RNG())*10,1);
			m_particlesSAP.push(
				m_transform*firePosition
				,size*RNG(0.2)
				,lifetime*RNG(0.9)+depth/20
				,0.000f*RNG()
				,m_transform*firePosition
				,m_transform*firePosition*1.f     
				,3*RNG()
				,fires[i%3]
			);	
		}
	}
	
};
class FireballEmiter:public Emiter {
public:
	glm::vec4 m_position;
	glm::mat4 m_transform;
	float t1{0};
	int phase{0};
	
	FireballEmiter(glm::vec4 _pos, glm::vec4 _vector_x, glm::vec4 _vector_y){
		m_position = _pos;
		m_transform = identity;
		setColumn(m_transform, 0, glm::normalize(_vector_x));
		setColumn(m_transform, 1, glm::normalize(_vector_y));
		
		glm::vec4 _vector_z = cross(_vector_x, _vector_y);
		setColumn(m_transform, 2, glm::normalize(_vector_z));
		// m_transform = glm::rotate(identity, -PI/3, glm::vec3(0,1,0));
		setColumn(m_transform, 3, _pos);
	}
	FireballEmiter(const glm::mat4 &transform){
		m_transform = transform;
	}
	
	bool emit(float dt){
		m_particlesVAP.update(dt);
		m_particlesNAP.update(dt);
		m_particlesSAP.update(dt);
		if(completeTime > 5.3f)
			return true;
		if(completeTime < dt && phase == 0){
			initial();
			phase++;
		}
		if(completeTime > 0.1f && phase == 1){
			phaseTwo();
			phase++;
		}
		else if(completeTime > 0.2f && phase == 2){
			phaseThree();
			phase++;
		}
		completeTime += dt;
		return completeTime > 5.2f;
	}
	void initial(){
		glm::vec4 size(0,0,13,13);
		glm::vec4 firePosition(0,0,0,0);
		float lifetime = 0.1f;
		for(int i=0; i<60; i++){
			float depth = abs(1-RNG())*2;
			float r = 5.f;
			firePosition =  glm::vec4( (1-RNG())*r, (1-RNG())*r, abs(1-RNG())*r-3,1);
			m_particlesSAP.push(
				m_transform*firePosition
				,size*RNG(0.2)
				,lifetime*RNG(0.9)+depth/20
				,0.000f*RNG()
				,m_transform*firePosition
				,m_transform*firePosition*50.f   
				,3*RNG()
				,fires[i%3]
			);	
		}
		
	}
	void phaseTwo(){
		glm::vec4 size(0,0,13,13);
		glm::vec4 firePosition(0,0,0,0);
		float lifetime = 0.4f;
		for(int i=0; i<160; i++){
			float depth = abs(1-RNG())*2;
			float r = 20.f;
			firePosition =  glm::vec4( (1-RNG())*r, (1-RNG())*r, abs(1-RNG())*r-3,1);
			m_particlesSAP.push(
				m_transform*firePosition
				,size*RNG(0.2)
				,lifetime*RNG(0.9)
				,0.000f*RNG()
				,m_transform*firePosition
				,m_transform*firePosition*2.f     
				,3*RNG()
				,fires[i%3]
			);	
		}
	
	}
	void phaseThree(){
		glm::vec4 size(0,0,10,10);
		glm::vec4 firePosition(0,0,0,0);
		float lifetime = 0.4f;
		for(int i=0; i<160; i++){
			float r = 20.f;
			firePosition =  glm::vec4( (1-RNG())*r, (1-RNG())*r, abs(1-RNG())/5*r-1,1);
			m_particlesSAP.push(
				m_transform*firePosition
				,size*RNG(0.2)
				,lifetime*RNG(0.9)
				,0.000f*RNG()
				,m_transform*firePosition
				,m_transform*firePosition*200.f     
				,3*RNG()
				,fires[i%3]
			);	
		}
	}
	
};
 
class GunSmokeEmiter:public Emiter {
public:
	glm::vec4 m_position;
	glm::mat4 m_transform;
	float t1{0};
	
	GunSmokeEmiter(glm::vec4 _pos, glm::vec4 _vector_x, glm::vec4 _vector_y){
		m_position = _pos;
		m_transform = identity;
		setColumn(m_transform, 0, glm::normalize(_vector_x));
		setColumn(m_transform, 1, glm::normalize(_vector_y));
		
		glm::vec4 _vector_z = cross(_vector_x, _vector_y);
		setColumn(m_transform, 2, glm::normalize(_vector_z));
		// m_transform = glm::rotate(identity, -PI/3, glm::vec3(0,1,0));
		setColumn(m_transform, 3, _pos);
	}
	
	bool emit(float dt){
		m_particlesVAP.update(dt);
		m_particlesNAP.update(dt);
		m_particlesSAP.update(dt);
		if(completeTime > 5.3f)
			return true;
		if(completeTime < dt){
			initial();
		}
		completeTime += dt;
		return completeTime > 5.2f;
	}
	void initial(){
		glm::vec4 size(0,0,16,16);
		glm::vec4 firePosition(0,0,0,0);
		glm::vec4 fireVector(0,0,0,0);
		float lifetime = 1.f;
		for(int i=0; i<90; i++){
			float depth = abs(1-RNG())*2;
			float al = (1-depth)*6.f;
			firePosition = fireVector =  glm::vec4(8*depth, (1-RNG())*al, (1-RNG())*al,1);
			fireVector.w = 0;
			fireVector = glm::normalize(fireVector);
			m_particlesSAP.push(
				m_transform*firePosition
				,size*RNG(0.2)
				,lifetime*RNG(0.3)+depth/4
				,0.000f*RNG()
				,m_transform*fireVector
				,m_transform*fireVector*5.f*RNG()     
				,3*RNG()
				,smokes[i%4]
			);	
		}
		for(int i=0; i<70; i++){
			float depth = (1-RNG())*2;
			firePosition = fireVector = glm::vec4((1-RNG())*5+2, depth*5, (1-RNG())*5,1);
			fireVector.w = 0;
			fireVector = glm::normalize(fireVector);
			m_particlesSAP.push(
				m_transform*firePosition
				,size*RNG(0.2)
				,lifetime*RNG(0.3) + abs(depth)/4
				,0.000f*RNG()
				,m_transform*fireVector
				,m_transform*fireVector*5.f*RNG()   
				,3*RNG()
				,smokes[i%4]
			);	
		}
		
	}

	
};
 
class ExplosionManger {
public:
	list<Emiter*> emiters;
	
	ExplosionManger()	{
		initialAZSF();
	}
	
	void update(float dt){
		// for(auto &emiter : emiters){
		for(auto emiter=emiters.begin(); emiter!=emiters.end(); emiter++){
			if((*emiter)->emit(dt)){
				delete *emiter;
				emiter=emiters.erase(emiter);
			}
		}
	}
	void spawnExplosion(glm::vec4 pos){
		emiters.emplace_back(new ExplosionEmiter(pos));

	}
	void spawnGunfire(glm::vec4 pos){
		emiters.emplace_back(new GunFireEmiter(pos, glm::vec4(0.5,-0.5,0.2,0), glm::vec4(0.5,0.5 ,0,0)));
	}
	void spawnGunsmoke(glm::vec4 pos){
		emiters.emplace_back(new GunSmokeEmiter(pos, glm::vec4(1,0,0,0), glm::vec4(0,1,0,0)));
	}
	
}explosions;

ExplosionManger expSmokes;

void spawnMuzzleFire(glm::vec4 pos, glm::vec4 vecX, glm::vec4 vecY){
	explosions.emiters.emplace_back(new GunFireEmiter(pos, vecX, vecY));
}
void spawnMuzzleFire(const glm::mat4 &transform){
	explosions.emiters.emplace_back(new GunFireEmiter(transform));
}

void spawnFireball(){}