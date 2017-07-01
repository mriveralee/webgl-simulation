import * as THREE from 'three';


// A base class for the particleSystem that will hold a group of goodies

export default class Spring  {
  constructor(indexA, indexB, restLength, stiffness) {
    // Ensure its a number :)
    this.indexA = +indexA;
    this.indexB = +indexB;
    this.restLength = +restLength;
    this.k = +stiffness;
  }

  resolveConstraint(positions, forces) {
    let posA = positions[this.indexA];
    let posB = positions[this.indexB];

    let deltaPosA = posA.sub(posB);
    let lengthDeltaPosA = deltaPosA.length();
    deltaPosA = deltaPosA.normalize();
    let forceA = deltaPosA.multiply(-1 * this.stiffness * lengthDeltaPosA);
    let forceB = -forceA;

    forces[posA] += forceA;
    forces[posB] += forceB;
  }

}
