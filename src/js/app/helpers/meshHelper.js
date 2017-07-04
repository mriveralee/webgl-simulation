import * as THREE from 'three';

// Simple mesh helper that shows edges, wireframes, and face and vertex normals
export default class MeshHelper {
  constructor(scene, mesh, geometry) {
    this.geometry = geometry;
    this.mesh = mesh;

    const wireframe = new THREE.WireframeGeometry(geometry);
    const wireLine = new THREE.LineSegments(wireframe);
    wireLine.material.depthTest = false;
    wireLine.material.opacity = 0.25;
    wireLine.material.transparent = true;
    mesh.add(wireLine);
    this.wireLine = wireLine;

    const edges = new THREE.EdgesGeometry(geometry);
    const edgesLine = new THREE.LineSegments(edges);
    edgesLine.material.depthTest = false;
    edgesLine.material.opacity = 0.25;
    edgesLine.material.transparent = true;
    mesh.add(edgesLine);
    this.edgesLine = edgesLine;

    scene.add(new THREE.BoxHelper(mesh));
    scene.add(new THREE.FaceNormalsHelper(mesh, 2));
    scene.add(new THREE.VertexNormalsHelper(mesh, 2));
  }

}
