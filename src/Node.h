/*
 *  Node.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_NODE
#define _TWS_NODE

#include <Quaternion.h>
#include <Model.h>

namespace TWS
{
	class Node {
	private:
		Vector3d _position, _velocity;
		Quatd _rotation;
		Model *_model;

	public:
		Node();

		Vector3d position();
        Quatd rotation();
		Model *model();

		void setModel(Model *aModel);
		void setPosition(Vector3d &newpos);
		void setVelocity(Vector3d &newvel);
		void step();
		void rotatex(double angle);
		void rotatey(double angle);
		void rotatez(double angle);
		void moveforward(double distance, bool accelerate);
		void moveupward(double distance, bool accelerate);
		void straferight(double distance, bool accelerate);
		void stop();
        void follownode(Node *node);
	};
};

#endif
