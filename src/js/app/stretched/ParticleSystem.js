import * as THREE from 'three';

import Config from './../../data/config'
import Geometry from './../helpers/geometry';
import Spring from './../physics/spring';
import ZeroLengthSpring from './../physics/zeroLengthSpring';


// A base class for the particleSystem that will hold a group of particles

export default class ParticleSystem extends Geometry {
  constructor(scene, gridDim = 20) {
    super(scene);
    this.scene = scene;
    this.geo = null;
    this.gridDim = 20;
    this.numParticles = this.gridDim * this.gridDim;
    this.masses = [];
    this.forces = [];
    this.velocities = []
    // Positions are handled in our geometry
    for (let i = 0; i < this.numParticles; i++) {
      this.masses.push(1);
      this.forces.push(new THREE.Vector3());
      this.velocities.push(new THREE.Vector3());
    }
    this.constraints = [];
  }

  computeForces() {
    let gravity = new THREE.Vector3(...Config.getGravityComponents());
    for (let i = 0; i < this.numParticles; i++) {
      // Clear forces
      if (!this.forces[i].isVector3) {
        console.log("nana");
        this.forces[i] = new THREE.Vector3();
      }
      this.forces[i].multiplyScalar(0);
      // Gravity?
      if (Config.useGravity) {
        this.forces[i].addScaledVector(gravity, this.masses[i]);
      }
    }
    // TODO enable constraints solving
    this.resolveConstraints();
  }

  computeAccelerations() {
    var accelerations = [];
    for (let i = 0; i < this.numParticles ; i++) {
      accelerations.push(new THREE.Vector3());
      accelerations[i].addScaledVector(this.forces[i], 1/ this.masses[i]);
    }
    return accelerations;
  }


  // Integration using Sympletic euler
  // https://en.wikipedia.org/wiki/Semi-implicit_Euler_method
  integrate(timeStep) {
    this.computeForces();
    let accelerations = this.computeAccelerations();
    for (let i = 0; i < this.numParticles; i++) {
      this.velocities[i].addScaledVector(accelerations[i], timeStep);
      this.geo.vertices[i].addScaledVector(this.velocities[i], timeStep);
      //this.positions[i].addScaledVector(this.velocities[i], timeStep);
      this.geo.verticesNeedUpdate = true;
      // this.geo.normalsNeedUpdate = true;
      // this.geo.colorsNeedUpdate = true;
    }
  }

  resolveConstraints() {
    for (let i = 0; i < this.constraints.length; i++) {
      this.constraints[i].resolveConstraint(this.geo.vertices, this.forces);
    }
  }

  makeParticles(positions, orderIndices) {
    this.geo = new THREE.Geometry();
    this.geo.vertices.push(...positions);
    for (let i = 0; i < orderIndices.length; i += 3) {
      this.geo.faces.push(
        new THREE.Face3(orderIndices[i],
        orderIndices[i + 1],
        orderIndices[i + 2]));
    }
  }

  makeParticlesTest() {
    let positions = [];
    const dim = this.gridDim;
    for (let i = 0; i < dim; i++) {
      for (let j = 0; j < dim; j++) {
        positions.push(new THREE.Vector3(
          1 + j * 8,
          1 + i * 3,
          2));
      }
    }
    let orderIndices = [];
    for (let i = 0; i < positions.length - dim - 1; i++) {
      // Prevent the last triangle from
      // wrapping to the first in each row
      if ((i + 1) % dim == 0) {
        continue;
      }

      if (i % 2 == 0) {
        // *____*
        // | A /|
        // | / B|
        // *----*
        // Triangle A
        orderIndices.push(i);
        orderIndices.push(i + 1);
        orderIndices.push(i + dim);
        // Triangle B
        orderIndices.push(i + 1);
        orderIndices.push((i + 1) + dim);
        orderIndices.push(i + dim);
      } else {
        // *____*
        // | \ C|
        // |D \ |
        // *----*
        // Triangle C
        orderIndices.push(i);
        orderIndices.push((i + 1) + dim);
        orderIndices.push(i + dim);
        // Triangle D
        orderIndices.push(i);
        orderIndices.push(i + 1);
        orderIndices.push((i + 1) + dim);
      }
    }
    this.makeParticles(positions, orderIndices);
    this.place([0, -20, 0], [0, 0, 0], true, true);
    // Make the springs!
    this.createSprings();
    this.createZeroLengthSprings();
  }

  createSprings() {
    const points = this.geo.vertices;
    const dim = this.gridDim;
    let maxColumn = points.length - dim;
    let minColumn = dim;
    for (let i = 0; i < points.length; i++) {
      let div = Math.floor(i / dim);
      // TODO compute rest length for these points
      // TODO pass a stiffness constant :/
      if (div >= 1) {
        // before column
        this.constraints.push(new Spring(i, i - dim));
      }
      if (div + 1 < dim) {
        this.constraints.push(new Spring(i, i + 1));
      }
    }
  }

  createZeroLengthSprings() {
    const points = this.geo.vertices;
    const dim = this.gridDim;
    for (let i = points.length - dim; i < points.length; i++) {
      this.constraints.push(new ZeroLengthSpring(i, points[i]));
    }
  }

}
