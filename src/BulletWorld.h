#pragma once
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

class BulletWorld{
public:
	btBroadphaseInterface*                  broadphase {nullptr};
	btDefaultCollisionConfiguration*        collisionConfiguration {nullptr};
	btCollisionDispatcher*                  dispatcher {nullptr};
	btSequentialImpulseConstraintSolver*    solver {nullptr};
	btDiscreteDynamicsWorld*                dynamicsWorld {nullptr};
	btSequentialImpulseConstraintSolver*    constraintSolver {nullptr};
	btCompoundShape*                        groundCompound {nullptr};
	vector<btHeightfieldTerrainShape*>      groundShapes {nullptr};
	btDefaultMotionState*                   groundMotionState {nullptr};
	btRigidBody*                            groundRigidBody {nullptr};
    btOverlapFilterCallback*                filterCallback {nullptr};

	vector <btRigidBody*> bodies;
	vector <btCollisionShape*> shapes;
	vector <btDefaultMotionState*> motion_states;
	vector <btTypedConstraint*> constraints;

	void init();
	void clear();
	btRigidBody* createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, float inertiaScalling = 1.f);

    btCollisionWorld::ClosestRayResultCallback raycast(glm::vec4 from, glm::vec4 to);
    btCollisionWorld::ClosestRayResultCallback raycast(btVector3 from, btVector3 to);

	void deleteMotionState(btRigidBody* body);
	void update(float step);
	void deleteBodies();
};

