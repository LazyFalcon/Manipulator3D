#include <Utils/Includes.h>
#include <Utils/Utils.h>

extern glm::mat4 identity;
typedef vector<unsigned int> ints;
typedef vector<float> floats;

#include "BulletWorld.h"
#define _DebugLine_ std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";


void BulletWorld::init(){
	btVector3 worldMin(-250,-250,-50);
	btVector3 worldMax(250,250,50);
	// broadphase = new btAxisSweep3(worldMin,worldMax);// precyzja pozycjonowania œwiata
	broadphase = new btDbvtBroadphase();


	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,0,-10));
	dynamicsWorld->getSolverInfo().m_minimumSolverBatchSize = 128;
	dynamicsWorld->getSolverInfo().m_numIterations = 100;
	// btContactSolverInfo& info = dynamicsWorld->getSolverInfo();
	// info.m_solverMode |= SOLVER_INTERLEAVE_CONTACT_AND_FRICTION_CONSTRAINTS;
	// info.m_erp = 0.8;
  // info.m_globalCfm = 0.1;
}
void BulletWorld::clear(){}

btRigidBody* BulletWorld::createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, float inertiaScalling){
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	btVector3 localInertia(0,0,0);
	if (mass > 0.f)
		shape->calculateLocalInertia(mass,localInertia);
	localInertia *= inertiaScalling;
	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	body->setUserPointer(NULL);
	dynamicsWorld->addRigidBody(body);
	bodies.push_back(body);
	shapes.push_back(shape);
	motion_states.push_back(motionState);

	return body;
}

void BulletWorld::deleteMotionState(btRigidBody* body){
	u32 i;
	for(i=0; i<motion_states.size(); i++){
		if(motion_states[i] == body->getMotionState())
			break;
	}
	delete motion_states[i];
	motion_states[i] = motion_states.back();
	motion_states.pop_back();
}

void BulletWorld::update(float step){
	dynamicsWorld->stepSimulation(step, 10);

	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for(u32 i=0; i<numManifolds; i++){
		btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		// btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		for(u32 j=0; j<numContacts; j++){
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if(pt.getDistance()<0.f){
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
			}
		}
	}
}

void BulletWorld::deleteBodies(){
	// for(int i=0; constraints.size(); i++){
		// btTypedConstraint *c = constraints[i];
		// dynamicsWorld->removeConstraint(c);
		// delete c;
	// }

	for(int i = dynamicsWorld->getNumConstraints()-1; i >= 0; i--){
		btTypedConstraint* constraint = dynamicsWorld->getConstraint(i);
		dynamicsWorld->removeConstraint(constraint);
		delete constraint;
	}

	//terrai zostawiamy
	for(int i = dynamicsWorld->getNumCollisionObjects()-1; i >= 1; i--){
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if(body && body->getMotionState()){
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	for(int j = 0; j < shapes.size(); j++){
		btCollisionShape* shape = shapes[j];
		delete shape;
	}
	shapes.clear();
}

btCollisionWorld::ClosestRayResultCallback BulletWorld::raycast(glm::vec4 from, glm::vec4 to){
    return raycast(btVector3(from.x, from.y, from.z), btVector3(to.x, to.y, to.z));
}

btCollisionWorld::ClosestRayResultCallback BulletWorld::raycast(btVector3 from, btVector3 to){
    btCollisionWorld::ClosestRayResultCallback closestResults(from,to);
    // closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
    dynamicsWorld->rayTest(from,to,closestResults);
    return closestResults;
}

