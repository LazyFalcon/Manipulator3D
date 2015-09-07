#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/BaseStructs.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>
#include "Helper.h"

extern glm::mat4 identity;
typedef vector<unsigned int> ints;
typedef vector<float> floats;

#include "BulletWorld.h"
#define _DebugLine_ std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";


// http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=7772&hilit=setCollisionFlags

struct RobotObjectFilterCallback : public btOverlapFilterCallback
{
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0,btBroadphaseProxy* proxy1) const
	{
		bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
		collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);

		// btCollisionObject* collisionObject = (btCollisionObject*) proxy0->m_clientObject;
		// EntityPayload &userData_0 = *((EntityPayload*)collisionObject->getUserPointer());
		// collisionObject = (btCollisionObject*) proxy1->m_clientObject;
		// EntityPayload &userData_1 = *((EntityPayload*)collisionObject->getUserPointer());

		// if(collides && ( userData_0.ownerType == OwnerType::Robot || userData_1.ownerType == OwnerType::Robot) )
			// cout<<"---------------> COLLSISION"<<endl;
		// Helper::collidingPair(proxy0->m_clientObject->getUserPointer()->backPointer, proxy1->m_clientObject->getUserPointer()->backPointer);

		return true;
	}
};

void BulletWorld::init(){
	btVector3 worldMin(-250,-250,-50);
	btVector3 worldMax(250,250,50);
	// broadphase = new btAxisSweep3(worldMin,worldMax);// precyzja pozycjonowania œwiata
	broadphase = new btDbvtBroadphase();
	btOverlappingPairCallback *ghostCallback = new btGhostPairCallback();
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(ghostCallback);

	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,0,-10));
	dynamicsWorld->getSolverInfo().m_minimumSolverBatchSize = 128;
	dynamicsWorld->getSolverInfo().m_numIterations = 100;

    filterCallback = new RobotObjectFilterCallback();
    dynamicsWorld->getPairCache()->setOverlapFilterCallback(filterCallback);


	// btContactSolverInfo& info = dynamicsWorld->getSolverInfo();
	// info.m_solverMode |= SOLVER_INTERLEAVE_CONTACT_AND_FRICTION_CONSTRAINTS;
	// info.m_erp = 0.8;
  // info.m_globalCfm = 0.1;
}
void BulletWorld::clear(){
	deleteBodies();
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
	delete filterCallback;
}

btRigidBody* BulletWorld::createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, float inertiaScalling){
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	btVector3 localInertia(0,0,0);
	if (mass > 0.f)
		shape->calculateLocalInertia(mass,localInertia);
	localInertia *= inertiaScalling;
	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);

    body->setActivationState(DISABLE_DEACTIVATION);
    body->setSleepingThresholds(0.001f, 0.001f);

	// body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	body->setUserPointer(nullptr);
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
	/// remove rgBodies from world and delete
	for(int i = dynamicsWorld->getNumCollisionObjects()-1; i >= 1; i--){
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if(body && body->getMotionState()){
			while (body->getNumConstraintRefs()){
				btTypedConstraint* constraint = body->getConstraintRef(0);
				dynamicsWorld->removeConstraint(constraint);
				delete constraint;
			}
			delete body->getMotionState();
			dynamicsWorld->removeRigidBody(body);
		}
		else {
			dynamicsWorld->removeCollisionObject( obj );
		}
		EntityPayload *payload = (EntityPayload*)(obj->getUserPointer());
		delete payload;
		delete obj;
	}

	/// delete cllision shapes
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

