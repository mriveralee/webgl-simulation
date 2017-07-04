import * as THREE from 'three';


export default class SceneHelper {

  static createPrincipalAxes(scene, length = 100) {
      var axes = new THREE.Object3D();
      axes.add(
        SceneHelper.buildAxis(
          new THREE.Vector3(),
          new THREE.Vector3(length, 0, 0),
          0xFF0000,
          false )); // +X
      axes.add(
        SceneHelper.buildAxis(
          new THREE.Vector3(),
          new THREE.Vector3(-length, 0, 0),
          0xFF0000,
          true)); // -X
      axes.add(
        SceneHelper.buildAxis(
          new THREE.Vector3(),
          new THREE.Vector3(0, length, 0),
          0x00FF00,
          false)); // +Y
      axes.add(
        SceneHelper.buildAxis(
          new THREE.Vector3(),
          new THREE.Vector3( 0, -length, 0),
          0x00FF00,
          true)); // -Y
      axes.add(
        SceneHelper.buildAxis(
          new THREE.Vector3(),
          new THREE.Vector3(0, 0, length),
          0x0000FF,
          false)); // +Z
      axes.add(
        SceneHelper.buildAxis(
          new THREE.Vector3(),
          new THREE.Vector3(0, 0, -length),
          0x0000FF,
          true)); // -Z
      scene.add(axes);
  }

  static buildAxis(src, dst, colorHex, dashed) {
    let geom = new THREE.Geometry();
    var mat;

    if(dashed) {
      mat = new THREE.LineDashedMaterial({ linewidth: 3, color: colorHex, dashSize: 3, gapSize: 3 });
    } else {
      mat = new THREE.LineBasicMaterial({ linewidth: 3, color: colorHex });
    }
    geom.vertices.push(src.clone());
    geom.vertices.push(dst.clone());
    // dashed lines will appear as simple plain lines w/o this
    geom.computeLineDistances();
    var axis = new THREE.LineSegments( geom, mat, THREE.LinePieces );
    return axis;
  }


}
