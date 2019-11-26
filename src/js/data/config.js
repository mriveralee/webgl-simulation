import TWEEN from 'tween.js';

// This object contains the state of the app

var getGravity = () => {
  return -9.81;
};

var getUpComponents = () => {
  return [0, 0, 1];
};

var config = {
  isDev: false,
  isLoaded: false,
  isTweening: false,
  isRotating: true,
  isMouseMoving: false,
  isMouseOver: false,
  simulation: {
    gridDim: {
        X: 20,
        Y: 20,
    },
    fabricParticleMass: 10/1000,
    shape: 'SQUARE',

    animate: false,
    timeStep: 0.016, //0.016,
    useVerletIntegration: false,
    useVelocityDamping: true,
    velocityDampingConstant: 0.003,
    useGravity: false,
    gravity: () => {
      return getGravity();
    },
    getGravityComponents: () => {
      var up = getUpComponents();
      for (let i = 0; i < up.length; i++) {
        up[i] *= getGravity();
      }
      return up;
    },
  },
  getUpComponents: () => {
    return getUpComponents();
  },
  showStats: () => {
    return false && config.isDev;
  },
  maxAnisotropy: 1,
  dpr: window.devicePixelRatio,
  easing: TWEEN.Easing.Quadratic.InOut,
  duration: 500,
  model: {
    path: './assets/models/Teapot.json',
    scale: 20
  },
  texture: {
    path: './assets/textures/',
    imageFiles: [
      {name: 'UV', image: 'UV_Grid_Sm.jpg'}
    ]
  },
  mesh: {
    enableHelper: false,
    wireframe: false,
    translucent: false,
    showPoints: false,
    opacity: 1.0,
    material: {
      color: 0xffffff,
      emissive: 0xffffff
    }
  },
  fog: {
    color: 0xffffff,
    near: 0.0008
  },
  camera: {
    fov: 75,
    near: 10,
    far: 1000,
    aspect: 1, // updated using window.devicePixelRatio
    posX: 0,
    posY: -10,
    posZ: 200
  },
  controls: {
    autoRotate: false,
    autoRotateSpeed: -5,
    rotateSpeed: 0.5,
    zoomSpeed: 0.8,
    minDistance: 0,
    maxDistance: Infinity,
    minPolarAngle: 0,
    maxPolarAngle: Math.PI,
    minAzimuthAngle: -Infinity,
    maxAzimuthAngle: Infinity,
    enableDamping: true,
    dampingFactor: 0.5,
    enableZoom: true,
    target: {
      x: 20,
      y: 20,
      z: 30
    }
  },
  ambientLight: {
    enabled: false,
    color: 0x141414
  },
  directionalLight: {
    enabled: true,
    color: 0xf0f0f0,
    intensity: 0.4,
    x: -75,
    y: 280,
    z: 150
  },
  shadow: {
    enabled: false,
    helperEnabled: true,
    bias: 0,
    mapWidth: 2048,
    mapHeight: 2048,
    near: 250,
    far: 400,
    top: 100,
    right: 100,
    bottom: -100,
    left: -100
  },
  pointLight: {
    enabled: false,
    color: 0xffffff,
    intensity: 0.34,
    distance: 115,
    x: 0,
    y: 0,
    z: 0
  },
  hemiLight: {
    enabled: true,
    color: 0xc8c8c8,
    groundColor: 0xffffff,
    intensity: 0.55,
    x: 0,
    y: 0,
    z: 0
  },
};

export default config;
