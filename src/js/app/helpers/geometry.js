import * as THREE from 'three';

import Material from './material';
import MeshHelper from './../helpers/meshHelper';


import Config from '../../data/config';

// This helper class can be used to create and then place geometry in the scene
export default class Geometry {
  constructor(scene) {
    this.scene = scene;
    this.geo = null;
    this.mesh = null;
    this.pointsMesh = null;
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

  place(position, rotation, showPoints=false) {
    //const material = new Material(0xeeeeee).standard;
    const material = new Material(0xffffff).standard;
    const mesh = new THREE.Mesh(this.geo, material);
    this.mesh = mesh;

    // Use ES6 spread to set position and rotation from passed in array
    mesh.position.set(...position);
    mesh.rotation.set(...rotation);

    if(Config.shadow.enabled) {
      mesh.receiveShadow = true;
    }

    // Add main mesh to the scene!
    this.scene.add(mesh);
    this.showPoints(showPoints)
  }

  showPoints(value=false) {
    if (!value || (value && this.pointsMesh != null)) {
      this.scene.remove(this.pointsMesh);
      this.pointsMesh = null;
      return;
    }
    const pointMaterial = new THREE.PointsMaterial({
      color: 0x800080
    });
    this.pointsMesh = new THREE.Points(this.geo, pointMaterial);
    this.pointsMesh.position.set(...this.mesh.position.toArray());
    this.pointsMesh.rotation.set(...this.mesh.rotation.toArray());
    this.scene.add(this.pointsMesh);
  }
}
