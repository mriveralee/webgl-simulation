// Global imports -
import * as THREE from 'three';
import TWEEN from 'tween.js';

// Local imports -
// Components
import Renderer from './components/renderer';
import Camera from './components/camera';
import Light from './components/light';
import Controls from './components/controls';

// Helpers
import Geometry from './helpers/geometry';
import SceneHelper from './helpers/SceneHelper';

// Model
import Texture from './model/texture';
import Model from './model/model';

// Managers
import Interaction from './managers/interaction';
import DatGUI from './managers/datGUI';

// data
import Config from './../data/config';

// Stretched
import ParticleSystem from './stretched/particleSystem';
// -- End of imports

// Local vars for rStats
let rS, bS, glS, tS;


// This class instantiates and ties all of the components together, starts the loading process and renders the main loop
export default class Main {
  constructor(container) {
    // Set container property to container element
    this.container = container;

    // Start Three clock
    this.clock = new THREE.Clock();

    // Main scene creation
    this.scene = new THREE.Scene();
    this.scene.fog = new THREE.FogExp2(Config.fog.color, Config.fog.near);

    // Get Device Pixel Ratio first for retina
    if(window.devicePixelRatio) {
      Config.dpr = window.devicePixelRatio;
    }

    // Main renderer instantiation
    this.renderer = new Renderer(this.scene, container);

    // Components instantiation
    this.camera = new Camera(this.renderer.threeRenderer);
    this.controls = new Controls(this.camera.threeCamera, container);
    this.light = new Light(this.scene);

    // Create and place lights in scene
    const lights = ['ambient', 'directional', 'point', 'hemi'];
    for(let i = 0; i < lights.length; i++) {
      this.light.place(lights[i]);
    }

    // Create and place geo in scene
    //this.geometry = new Geometry(this.scene);
    //this.geometry.make('plane')(150, 150, 10, 10);
    //this.geometry.place([0, -20, 0], [Math.PI/2, 0, 0]);

    // TODO: add things to the scene
    this.particleSystem = new ParticleSystem(this.scene, 30);
    this.particleSystem.makeParticlesTest();

    // Draw axes in scene
    SceneHelper.createPrincipalAxes(this.scene);

    // Set up rStats if dev environment
    if(Config.showStats()) {
      bS = new BrowserStats();
      glS = new glStats();
      tS = new threeStats(this.renderer.threeRenderer);

      rS = new rStats({
        CSSPath: './assets/css/',
        userTimingAPI: true,
        values: {
          frame: { caption: 'Total frame time (ms)', over: 16, average: true, avgMs: 100 },
          fps: { caption: 'Framerate (FPS)', below: 30 },
          calls: { caption: 'Calls (three.js)', over: 3000 },
          raf: { caption: 'Time since last rAF (ms)', average: true, avgMs: 100 },
          rstats: { caption: 'rStats update (ms)', average: true, avgMs: 100 },
          texture: { caption: 'GenTex', average: true, avgMs: 100 }
        },
        groups: [
          { caption: 'Framerate', values: [ 'fps', 'raf' ] },
          { caption: 'Frame Budget', values: [ 'frame', 'texture', 'setup', 'render' ] }
        ],
        fractions: [
          { base: 'frame', steps: [ 'texture', 'setup', 'render' ] }
        ],
        plugins: [bS, tS, glS]
      });

    }

    // Instantiate texture class
    this.texture = new Texture();

    // Start loading the textures and then go on to load the model after the texture Promises have resolved
    this.texture.load().then(() => {
      this.manager = new THREE.LoadingManager();

      // Textures loaded, load model
      // this.model = new Model(this.scene, this.manager, this.texture.textures);
      // this.model.load();

      // onProgress callback
      this.manager.onProgress = (item, loaded, total) => {
        console.log(`${item}: ${loaded} ${total}`);
      };
      // All loaders done now
      this.manager.onLoad = () => {
        // Set up interaction manager with the app now that the model is finished loading
        new Interaction(
          this.renderer.threeRenderer,
          this.scene,
          this.camera.threeCamera,
          this.controls.threeControls);

        // Add dat.GUI controls if dev
        let controls = new DatGUI(this, this.model.obj);

        // Everything is now fully loaded
        Config.isLoaded = true;
        this.container.querySelector('#loading').style.display = 'none';
      };

      // TODO REMOVE when we actually use loading
      if (!this.model) {
        // TODO: REMOVE
        this.manager.onLoad();
      }

    });
    this.container.querySelector('#loading').style.display = 'none';

    // Start render which does not wait for model fully loaded
    this.render();
  }

  render() {
    // Render rStats if Dev
    if(Config.showStats()) {
      rS('frame').start();
      glS.start();

      rS('rAF').tick();
      rS('FPS').frame();

      rS('render').start();
    }

    // Call render function and pass in created scene and camera
    if (Config.animate) {
      // Take the correct number of steps to appear as if
      // we are running in real time
      let numSteps = (1.0 / 30.0) / Config.timeStep;
      if (numSteps < 1) {
        numSteps = 1;
      }
      for (let i = 0; i < numSteps; i++) {
        this.particleSystem.integrate(Config.timeStep);
      }
    }
    this.renderer.render(this.scene, this.camera.threeCamera);

    // rStats has finished determining render call now
    if(Config.showStats()) {
      rS('render').end(); // render finished
      rS('frame').end(); // frame finished

      // Local rStats update
      rS('rStats').start();
      rS().update();
      rS('rStats').end();
    }

    // Delta time is sometimes needed for certain updates
    //const delta = this.clock.getDelta();

    // Call any vendor or module updates here
    TWEEN.update();
    this.controls.threeControls.update();

    // RAF
    requestAnimationFrame(this.render.bind(this)); // Bind the main class instead of window object
  }
}
