import * as THREE from 'three';

import Spring from './../physics/spring';

export default class ZeroLengthSpring extends Spring {
  constructor(particleIndex, restPosition, stiffness = 10000) {
    // There's no second index for the spring and the length is 0
    super(particleIndex, -1, 0, stiffness);
    this.restPosition = restPosition.clone();
  }

  resolveConstraint(positions, forces) {
    let posA = positions[this.indexA].clone();
    let posB = this.restPosition.clone();
    let forceA = this._computeForces(posA, posB);
    forces[this.indexA].add(forceA);
  }

  _computeForces(posA, posB) {
    let deltaPosA = posA.sub(posB);
    let lengthDeltaPosA = deltaPosA.length();
    deltaPosA = deltaPosA.normalize();
    let forceA = deltaPosA.multiplyScalar(-1 * this.stiffness * lengthDeltaPosA);
    return forceA;
  }

}
