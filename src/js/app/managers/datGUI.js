import Config from '../../data/config';

// Manages all dat.GUI interactions
export default class DatGUI {
  constructor(main, getGeometry) {
    const gui = new dat.GUI();

    this.camera = main.camera.threeCamera;
    this.controls = main.controls.threeControls;
    this.light = main.light;

    /* Global */
    //gui.close();

    /* Camera */
    const cameraFolder = gui.addFolder('Camera');
    const cameraFOVGui = cameraFolder.add(Config.camera, 'fov', 0, 180).name('Camera FOV');
    cameraFOVGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.camera.fov = value;
    });
    cameraFOVGui.onFinishChange(() => {
      this.camera.updateProjectionMatrix();

      this.controls.enableRotate = true;
    });
    const cameraAspectGui = cameraFolder.add(Config.camera, 'aspect', 0, 4).name('Camera Aspect');
    cameraAspectGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.camera.aspect = value;
    });
    cameraAspectGui.onFinishChange(() => {
      this.camera.updateProjectionMatrix();

      this.controls.enableRotate = true;
    });
    const cameraFogColorGui = cameraFolder.addColor(Config.fog, 'color').name('Fog Color');
    cameraFogColorGui.onChange((value) => {
      main.scene.fog.color.setHex(value);
    });
    const cameraFogNearGui = cameraFolder.add(Config.fog, 'near', 0.000, 0.010).name('Fog Near');
    cameraFogNearGui.onChange((value) => {
      this.controls.enableRotate = false;

      main.scene.fog.density = value;
    });
    cameraFogNearGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });


    /* Controls */
    const controlsFolder = gui.addFolder('Controls');
    controlsFolder.add(Config.controls, 'autoRotate').name('Auto Rotate').onChange((value) => {
      this.controls.autoRotate = value;
    });
    const controlsAutoRotateSpeedGui = controlsFolder.add(Config.controls, 'autoRotateSpeed', -5, 5).name('Rotation Speed');
    controlsAutoRotateSpeedGui.onChange((value) => {
      this.controls.enableRotate = false;
      this.controls.autoRotateSpeed = value;
    });
    controlsAutoRotateSpeedGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });

    controlsFolder.add(Config, 'showGrid', true).name('Show Grid');


    /* Mesh */
    const meshFolder = gui.addFolder('Mesh');
    meshFolder.add(Config.mesh, 'translucent', true).name('Translucent').onChange((value) => {
      let mesh = getGeometry().mesh;
      if(value) {
        mesh.material.transparent = true;
        mesh.material.opacity = 0.5;
      } else {
        mesh.material.opacity = 1.0;
      }
    });
    meshFolder.add(Config.mesh, 'wireframe', true).name('Wireframe').onChange((value) => {
      let mesh = getGeometry().mesh;
      mesh.material.wireframe = value;
    });

    meshFolder.add(Config.mesh, 'showPoints', true).name('Show Points').onChange((value) => {
      let geo = getGeometry();
      geo.showPoints(value);
    }).listen();



    meshFolder.add(Config.mesh, 'pointSize', 0.05, 1).name('Point Size');

    meshFolder.open();


    const simulationFolder = gui.addFolder('Simulation');
    simulationFolder.add(Config.simulation, 'animate', true).name('Animate').listen();
    simulationFolder.add(Config.simulation, 'useGravity', true).name('Use Gravity');
    simulationFolder.add(Config.simulation, 'useFloorConstraint', false).name('Use Floor Constraint');

    simulationFolder.add(Config.simulation, 'useVerletIntegration', true).name('Use Verlet Integration');

    simulationFolder.add(Config.simulation, 'useVelocityDamping', true).name('Damp Velocity');
    simulationFolder.add(Config.simulation, 'velocityDampingConstant', 0.001, 0.25).name('velocityDampingConstantDamping');
    simulationFolder.add(Config.simulation, 'timeStep', 0.001, 0.1).name('Time Step');
    simulationFolder.add(Config.simulation, 'biasOffsetZ', -20, 20).name('Bias Z Offset');
    simulationFolder.add(Config.simulation, 'avoidSelfIntersections', false).name('Avoid Self Intersections');
    simulationFolder.add(Config.simulation, 'fabricSelfIntersectionsMinDist', 0, 2, 0.1).name('Self Intersections Test Dist');
    simulationFolder.add(Config.simulation, 'useDragForce', false).name('Use Drag Forces');
    simulationFolder.add(Config.simulation, 'coefficientOfDrag', 0, 2, 0.01).name('Coefficient of Drag');

    simulationFolder.open();

    const fabricParametersFolder = gui.addFolder('Fabric');
    fabricParametersFolder.add(Config.simulation.fabric, 'particleMass', 0.00001, 100, 0.001).name('Mass');
    fabricParametersFolder.add(Config.simulation.fabric, 'structuralSpringStiffnessX', 0, 1000, 0.001).name('Str. Stiff X');
    fabricParametersFolder.add(Config.simulation.fabric, 'structuralSpringStiffnessY', 0, 1000, 0.001).name('Str. Stiff Y');
    fabricParametersFolder.add(Config.simulation.fabric, 'bendSpringStiffnessX', 0, 1000, 0.001).name('Bend Stiffness X');
    fabricParametersFolder.add(Config.simulation.fabric, 'bendSpringStiffnessY', 0, 1000, 0.001).name('Bend Stiffness Y');
    fabricParametersFolder.add(Config.simulation.fabric, 'shearSpringStiffness', 0, 1000, 0.1).name('Shear Stiffness');
    fabricParametersFolder.open();

    const hydrogelParametersFolder = gui.addFolder('Hydrogel');
    hydrogelParametersFolder.add(Config.simulation.hydrogel, 'particleMass', 0.00001, 100, 0.001).name('Mass');
    hydrogelParametersFolder.add(Config.simulation.hydrogel, 'layerHeight', 0.1, 15, 0.1).name('Layer Height');
    hydrogelParametersFolder.add(Config.simulation.hydrogel, 'springStiffnessZ', 0, 1000, 0.001).name('Stiffness Z');
    hydrogelParametersFolder.add(Config.simulation.hydrogel, 'springStiffnessXY', 0, 1000, 0.001).name('Stiffness XY');
    hydrogelParametersFolder.add(Config.simulation.hydrogel, 'springShrinkRatioZ', 0.01, 1.1, 0.001).name('Shrink Ratio Z');
    hydrogelParametersFolder.add(Config.simulation.hydrogel, 'springShrinkRatioXY', 0.01 , 1.1, 0.001).name('Shrink Ratio XY');
    hydrogelParametersFolder.add(Config.simulation.hydrogel, 'hydrogelColumns', 1, Config.simulation.gridDim.X, 1).name('Column Count');
    hydrogelParametersFolder.open();

    /* Lights */
    // Ambient Light
    const ambientLightFolder = gui.addFolder('Ambient Light');
    ambientLightFolder.add(Config.ambientLight, 'enabled').name('Enabled').onChange((value) => {
      this.light.ambientLight.visible = value;
    });
    ambientLightFolder.addColor(Config.ambientLight, 'color').name('Color').onChange((value) => {
      this.light.ambientLight.color.setHex(value);
    });

    // Directional Light
    const directionalLightFolder = gui.addFolder('Directional Light');
    directionalLightFolder.add(Config.directionalLight, 'enabled').name('Enabled').onChange((value) => {
      this.light.directionalLight.visible = value;
    });
    directionalLightFolder.addColor(Config.directionalLight, 'color').name('Color').onChange((value) => {
      this.light.directionalLight.color.setHex(value);
    });
    const directionalLightIntensityGui = directionalLightFolder.add(Config.directionalLight, 'intensity', 0, 2).name('Intensity');
    directionalLightIntensityGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.intensity = value;
    });
    directionalLightIntensityGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
    const directionalLightPositionXGui = directionalLightFolder.add(Config.directionalLight, 'x', -1000, 1000).name('Position X');
    directionalLightPositionXGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.position.x = value;
    });
    directionalLightPositionXGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
    const directionalLightPositionYGui = directionalLightFolder.add(Config.directionalLight, 'y', -1000, 1000).name('Position Y');
    directionalLightPositionYGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.position.y = value;
    });
    directionalLightPositionYGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
    const directionalLightPositionZGui = directionalLightFolder.add(Config.directionalLight, 'z', -1000, 1000).name('Position Z');
    directionalLightPositionZGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.position.z = value;
    });
    directionalLightPositionZGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });

    // Shadow Map
    const shadowFolder = gui.addFolder('Shadow Map');
    shadowFolder.add(Config.shadow, 'enabled').name('Enabled').onChange((value) => {
      this.light.directionalLight.castShadow = value;
    });
    shadowFolder.add(Config.shadow, 'helperEnabled').name('Helper Enabled').onChange((value) => {
      this.light.directionalLightHelper.visible = value;
    });
    const shadowNearGui = shadowFolder.add(Config.shadow, 'near', 0, 400).name('Near');
    shadowNearGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.shadow.camera.near = value;
    });
    shadowNearGui.onFinishChange(() => {
      this.controls.enableRotate = true;
      this.light.directionalLight.shadow.map.dispose();
      this.light.directionalLight.shadow.map = null;
      this.light.directionalLightHelper.update();
    });
    const shadowFarGui = shadowFolder.add(Config.shadow, 'far', 0, 1200).name('Far');
    shadowFarGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.shadow.camera.far = value;
    });
    shadowFarGui.onFinishChange(() => {
      this.controls.enableRotate = true;
      this.light.directionalLight.shadow.map.dispose();
      this.light.directionalLight.shadow.map = null;
      this.light.directionalLightHelper.update();
    });
    const shadowTopGui = shadowFolder.add(Config.shadow, 'top', -400, 400).name('Top');
    shadowTopGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.shadow.camera.top = value;
    });
    shadowTopGui.onFinishChange(() => {
      this.controls.enableRotate = true;
      this.light.directionalLight.shadow.map.dispose();
      this.light.directionalLight.shadow.map = null;
      this.light.directionalLightHelper.update();
    });
    const shadowRightGui = shadowFolder.add(Config.shadow, 'right', -400, 400).name('Right');
    shadowRightGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.shadow.camera.right = value;
    });
    shadowRightGui.onFinishChange(() => {
      this.controls.enableRotate = true;
      this.light.directionalLight.shadow.map.dispose();
      this.light.directionalLight.shadow.map = null;
      this.light.directionalLightHelper.update();
    });
    const shadowBottomGui = shadowFolder.add(Config.shadow, 'bottom', -400, 400).name('Bottom');
    shadowBottomGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.shadow.camera.bottom = value;
    });
    shadowBottomGui.onFinishChange(() => {
      this.controls.enableRotate = true;
      this.light.directionalLight.shadow.map.dispose();
      this.light.directionalLight.shadow.map = null;
      this.light.directionalLightHelper.update();
    });
    const shadowLeftGui = shadowFolder.add(Config.shadow, 'left', -400, 400).name('Left');
    shadowLeftGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.shadow.camera.left = value;
    });
    shadowLeftGui.onFinishChange(() => {
      this.controls.enableRotate = true;
      this.light.directionalLight.shadow.map.dispose();
      this.light.directionalLight.shadow.map = null;
      this.light.directionalLightHelper.update();
    });
    const shadowBiasGui = shadowFolder.add(Config.shadow, 'bias', -0.000010, 1).name('Bias');
    shadowBiasGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.directionalLight.shadow.bias = value;
    });
    shadowBiasGui.onFinishChange(() => {
      this.controls.enableRotate = true;
      this.light.directionalLight.shadow.map.dispose();
      this.light.directionalLight.shadow.map = null;
      this.light.directionalLightHelper.update();
    });


    // Point Light
    const pointLightFolder = gui.addFolder('Point Light');
    pointLightFolder.add(Config.pointLight, 'enabled').name('Enabled').onChange((value) => {
      this.light.pointLight.visible = value;
    });
    pointLightFolder.addColor(Config.pointLight, 'color').name('Color').onChange((value) => {
      this.light.pointLight.color.setHex(value);
    });
    const pointLightIntensityGui = pointLightFolder.add(Config.pointLight, 'intensity', 0, 2).name('Intensity');
    pointLightIntensityGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.pointLight.intensity = value;
    });
    pointLightIntensityGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
    const pointLightDistanceGui = pointLightFolder.add(Config.pointLight, 'distance', 0, 1000).name('Distance');
    pointLightDistanceGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.pointLight.distance = value;
    });
    pointLightDistanceGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
    const pointLightPositionXGui = pointLightFolder.add(Config.pointLight, 'x', -1000, 1000).name('Position X');
    pointLightPositionXGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.pointLight.position.x = value;
    });
    pointLightPositionXGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
    const pointLightPositionYGui = pointLightFolder.add(Config.pointLight, 'y', -1000, 1000).name('Position Y');
    pointLightPositionYGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.pointLight.position.y = value;
    });
    pointLightPositionYGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
    const pointLightPositionZGui = pointLightFolder.add(Config.pointLight, 'z', -1000, 1000).name('Position Z');
    pointLightPositionZGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.pointLight.position.z = value;
    });
    pointLightPositionZGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });


    // Hemi Light
    const hemiLightFolder = gui.addFolder('Hemi Light');
    hemiLightFolder.add(Config.hemiLight, 'enabled').name('Enabled').onChange((value) => {
      this.light.hemiLight.visible = value;
    });
    hemiLightFolder.addColor(Config.hemiLight, 'color').name('Color').onChange((value) => {
      this.light.hemiLight.color.setHex(value);
    });
    hemiLightFolder.addColor(Config.hemiLight, 'groundColor').name('ground Color').onChange((value) => {
      this.light.hemiLight.groundColor.setHex(value);
    });
    const hemiLightIntensityGui = hemiLightFolder.add(Config.hemiLight, 'intensity', 0, 2).name('Intensity');
    hemiLightIntensityGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.hemiLight.intensity = value;
    });
    hemiLightIntensityGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
    const hemiLightPositionXGui = hemiLightFolder.add(Config.hemiLight, 'x', -1000, 1000).name('Position X');
    hemiLightPositionXGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.hemiLight.position.x = value;
    });
    hemiLightPositionXGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
    const hemiLightPositionYGui = hemiLightFolder.add(Config.hemiLight, 'y', -500, 1000).name('Position Y');
    hemiLightPositionYGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.hemiLight.position.y = value;
    });
    hemiLightPositionYGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
    const hemiLightPositionZGui = hemiLightFolder.add(Config.hemiLight, 'z', -1000, 1000).name('Position Z');
    hemiLightPositionZGui.onChange((value) => {
      this.controls.enableRotate = false;

      this.light.hemiLight.position.z = value;
    });
    hemiLightPositionZGui.onFinishChange(() => {
      this.controls.enableRotate = true;
    });
  }
}
