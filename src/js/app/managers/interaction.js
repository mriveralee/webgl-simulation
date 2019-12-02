import Keyboard from '../../utils/keyboard';
import Helpers from '../../utils/helpers';
import Config from '../../data/config';

// Manages all input interactions
export default class Interaction {
  constructor(renderer, scene, camera, controls, onReset, getGeometry) {
    // Properties
    this.renderer = renderer;
    this.scene = scene;
    this.camera = camera;
    this.controls = controls;
    this.onReset = onReset;

    this.timeout = null;

    // Instantiate keyboard helper
    this.keyboard = new Keyboard();

    // Listeners
    // Mouse events
    this.renderer.domElement.addEventListener('mousemove', (event) => Helpers.throttle(this.onMouseMove(event), 250), false);
    this.renderer.domElement.addEventListener('mouseleave', (event) => this.onMouseLeave(event), false);
    this.renderer.domElement.addEventListener('mouseover', (event) => this.onMouseOver(event), false);

    // Keyboard events
    this.keyboard.domElement.addEventListener('keydown', (event) => {
      // Only once
      if(event.repeat) {
        return;
      }

      if(this.keyboard.eventMatches(event, 'escape')) {
        console.log('Escape pressed');
      }

      if (this.keyboard.eventMatches(event, 'space')) {
        console.log('Space pressed');
        Config.simulation.animate = !Config.simulation.animate;
      }

      if (this.keyboard.eventMatches(event, 'r')) {
        if (this.onReset) {
          this.onReset();
        }
      }
      if (this.keyboard.eventMatches(event, 'p')) {
          console.log("hello", Config.mesh.showPoints, !Config.mesh.showPoints)
          let val = !Config.mesh.showPoints;
          Config.mesh.showPoints = val;
      }
    });
  }

  onMouseOver(event) {
    event.preventDefault();

    Config.isMouseOver = true;
  }

  onMouseLeave(event) {
    event.preventDefault();

    Config.isMouseOver = false;
  }

  onMouseMove(event) {
    event.preventDefault();

    clearTimeout(this.timeout);

    this.timeout = setTimeout(function() {
      Config.isMouseMoving = false;
    }, 200);

    Config.isMouseMoving = true;
  }
}
