import * as THREE from 'three';

export default class Spring {
  constructor(particleIndexA, particleIndexB, restLength, stiffness = 1000) {
    // Ensure its a number :)
    this.indexA = +particleIndexA;
    this.indexB = +particleIndexB;
    this.restLength = +restLength;
    this.stiffness = +stiffness;
  }

  resolveConstraint(positions, forces) {
    const posA = positions[this.indexA].clone();
    const posB = positions[this.indexB].clone();
    let [forceA, forceB] = this._computeForces(posA, posB);
    forces[this.indexA].add(forceA);
    forces[this.indexB].add(forceB);
  }

  _computeForces(posA, posB) {
    let deltaPosA = posA.sub(posB);
    let lengthDeltaPosA = deltaPosA.length();
    deltaPosA = deltaPosA.normalize();
    let forceA = deltaPosA.multiplyScalar(
      -1 * this.stiffness * ((lengthDeltaPosA / this.restLength) -1));

      // -1 * this.stiffness * (lengthDeltaPosA - this.restLength));
    let forceB = forceA.clone().multiplyScalar(-1);
    return [forceA, forceB];
  }

}
