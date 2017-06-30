import * as THREE from 'three';

import Geometry from './../helpers/geometry';

// A base class for the particleSystem that will hold a group of goodies

export default class ParticleSystem extends Geometry {
  constructor(scene) {
    super(scene);
    this.scene = scene;
    this.geo = null;
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
    let dim = 20;
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
  }

}
