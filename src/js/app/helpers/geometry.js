import * as THREE from 'three';

import Material from './material';

import Config from '../../data/config';

// This helper class can be used to create and then place geometry in the scene
export default class Geometry {
  constructor(scene) {
    this.scene = scene;
    this.geo = null;
  }

  make(type) {
    if(type == 'plane') {
      return (width, height, widthSegments = 1, heightSegments = 1) => {
        this.geo = new THREE.PlaneGeometry(width, height, widthSegments, heightSegments);
      };
    }

    if(type == 'sphere') {
      return (radius, widthSegments = 32, heightSegments = 32) => {
        this.geo = new THREE.SphereGeometry(radius, widthSegments, heightSegments);
      };
    }
  }

  place(position, rotation, showWireframe=false, showPoints=false) {
    //const material = new Material(0xeeeeee).standard;
    const material = new Material(0xffffff).standard;

    const mesh = new THREE.Mesh(this.geo, material);

    // Use ES6 spread to set position and rotation from passed in array
    mesh.position.set(...position);
    mesh.rotation.set(...rotation);

    if(Config.shadow.enabled) {
      mesh.receiveShadow = true;
    }

    // Optional Wireframe
    if (showWireframe) {
      mesh.material.wireframe = showWireframe;
    }

    // Add main mesh to the scene!
    this.scene.add(mesh);

    // Optional Points
    if (showPoints) {
      const pointMaterial = new THREE.PointsMaterial({
        color: 0x800080
      });
      const pointCloudMesh = new THREE.Points(this.geo, pointMaterial);
      pointCloudMesh.position.set(...position);
      pointCloudMesh.rotation.set(...rotation);
      this.scene.add(pointCloudMesh);
    }
  }
}
