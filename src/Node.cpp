/*
 *  Node.cpp
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <Astro.h>
#include <Factory.h>

namespace TWS {
    Node::Node() {
        _rotation = Quatd(1.0, 0.0, 0.0, 0.0);
    }

    void Node::step() {
        // estimate gravitational pull
        Vector3d normalized_earth = _position - TWS::getEarth()->_position;
        normalized_earth.normalize();

        Vector3d normalized_mars = _position - TWS::getMars()->_position;
        normalized_mars.normalize();

        Vector3d g_earth = -((TWS_Astro::G*TWS::getEarth()->_mass) / pow((TWS::getEarth()->_position - _position).length(), 2)) * normalized_earth;
        Vector3d g_mars  = -((TWS_Astro::G*TWS::getMars()->_mass) / pow((TWS::getMars()->_position - _position).length(), 2)) * normalized_mars;
        Vector3d g = g_earth + g_mars;

        // estimate position increment
        _position += _velocity * TWS::getDt() + g * TWS::getDt() * TWS::getDt();
        _velocity += g * TWS::getDt();
    }

    void Node::rotatex(double angle) {
        Quaternion<double> nrot(Vector3d(1.0, 0.0, 0.0), angle);
        _rotation = _rotation * nrot;
    }

    void Node::rotatey(double angle) {
        Quaternion<double> nrot(Vector3d(0.0, 1.0, 0.0), angle);
        _rotation = _rotation * nrot;
    }

    void Node::rotatez(double angle) {
        Quaternion <double> nrot(Vector3d(0.0, 0.0, 1.0), angle);
        _rotation = _rotation * nrot;
    }

    void Node::moveforward(double distance, bool accelerate) {
        Vector3d delta = -Vector3d(0.0, 0.0, -1.0) * distance * (accelerate?TWS::getDt():1.0);
        _rotation.rotate(delta);
        _velocity += delta;
    }

    void Node::moveupward(double distance, bool accelerate) {
        Vector3d delta = Vector3d(0.0, 1.0, 0.0) * distance * (accelerate?TWS::getDt():1.0);
        _rotation.rotate(delta);
        _velocity += delta;
    }

    void Node::straferight(double distance, bool accelerate) {
        Vector3d delta = Vector3d(1.0, 0.0, 0.0) * distance * (accelerate?TWS::getDt():1.0);
        _rotation.rotate(delta);
        _velocity += delta;
    }

    void Node::follownode(Node *node) {
        _position = node->_position;
        _position.normalize();
        _position *= node->_position.length() + 2000.0;

        _velocity = node->_velocity;
    }

    void Node::stop() {_velocity = 0.0;}

    Vector3d Node::position() {return _position;}
    Quatd Node::rotation() {return _rotation;}
    Model *Node::model() {return _model;}

    void Node::setPosition(Vector3d &newpos) {
        _position = newpos;
        _velocity = Vector3d(0.0, 0.0, 0.0);
    }

    void Node::setVelocity(Vector3d &newvel) {_velocity = newvel;}
    void Node::setModel(Model *aModel) {_model = aModel;}
}
