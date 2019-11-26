import * as THREE from 'three';

import Config from './../../data/config'
import Geometry from './../helpers/geometry';
import Spring from './../physics/spring';
import ZeroLengthSpring from './../physics/zeroLengthSpring';


// A base class for the particleSystem that will hold a group of particles

export default class ParticleSystem extends Geometry {
    constructor(scene, gridDim = 15) {
        super(scene);
        this.scene = scene;
        this.geo = null;
        this.gridDim = Math.max(1, gridDim);
        this.numParticles = this.gridDim * this.gridDim;
        this.masses = [];
        this.inverseMasses = [];
        this.forces = [];
        this.accelerations = [];
        this.velocities = [];
        this.fixedPointIndices = [];
        this.oldPositions = []
        // Positions are handled in our geometry
        let mass = Config.simulation.fabricParticleMass;
        for (let i = 0; i < this.numParticles; i++) {
            this.masses.push(mass);
            this.inverseMasses.push(1.0 / mass);
            this.forces.push(new THREE.Vector3());
            this.accelerations.push(new THREE.Vector3());
            this.velocities.push(new THREE.Vector3());
            //this.oldPositions.push(new THREE.Vector3());

        }
        this.constraints = [];
    }

    computeForces() {
        let gravity = new THREE.Vector3(...Config.simulation.getGravityComponents());
        for (let i = 0; i < this.numParticles; i++) {
            // Clear forces
            this.forces[i].multiplyScalar(0);
            // Gravity
            if (Config.simulation.useGravity) {
                this.forces[i].addScaledVector(gravity, this.masses[i]);
            }
        }
        // Resolve any spring constraints
        this.resolveConstraints();
    }

    computeAccelerations() {
        for (let i = 0; i < this.numParticles ; i++) {
            this.accelerations[i].multiplyScalar(0);
            this.accelerations[i].addScaledVector(
                this.forces[i], this.inverseMasses[i]);
        }
        return this.accelerations;
    }


    // Integration using Verlet Integration
    // https://en.wikipedia.org/wiki/Verlet_integration
    integrate(timeStep) {
        this.computeForces();
        this.computeAccelerations();

        // Apply some damping to velocity for air resistance etc.
        let dampingFactor =
        Config.simulation.useVelocityDamping ? Config.simulation.velocityDampingConstant : 0;
        let sqTimeStep = timeStep * timeStep;
        for (let i = 0; i < this.numParticles; i++) {

            if (!Config.simulation.useVerletIntegration) {
                this.velocities[i].addScaledVector(this.accelerations[i], timeStep);
                this.velocities[i].multiplyScalar(1 - dampingFactor);
                this.geo.vertices[i].addScaledVector(this.velocities[i], timeStep);
                //this.positions[i].addScaledVector(this.velocities[i], timeStep);
            } else {
                // Use Verlet
                let prevPos = this.oldPositions[i];
                let currPos = this.geo.vertices[i].clone();
                let copyCurrPos = currPos.clone();
                let a = this.accelerations[i];
                //
                // // x += x - x_prev + a * t^2
                this.geo.vertices[i].add(copyCurrPos);
                this.geo.vertices[i].sub(prevPos);
                this.geo.vertices[i].addScaledVector(a, sqTimeStep);
                this.oldPositions[i] = this.oldPositions[i].copy(copyCurrPos);
                //this.velocities[i].addScaledVector(this.accelerations[i], timeStep);
                //this.velocities[i].multiplyScalar(1 - dampingFactor);

            }
        }
        this.geo.verticesNeedUpdate = true;
        this.geo.normalsNeedUpdate = true;
        this.geo.colorsNeedUpdate = true;
        // Make the simulation move like honey
        //this.velocities[i].multiplyScalar(0);
    }

    resolveConstraints() {
        for (let i = 0; i < this.constraints.length; i++) {
            this.constraints[i].resolveConstraint(this.geo.vertices, this.forces);
        }
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
        for (let j = 0; j < this.geo.vertices.length; j++) {
            this.oldPositions[j] = this.geo.vertices[j].clone();
        }
    }

    makeParticlesTest() {
        let positions = [];
        const dim = this.gridDim;
        for (let i = 0; i < dim; i++) {
            for (let j = 0; j < dim; j++) {
                positions.push(new THREE.Vector3(
                    j * 1.2,
                    i * 1.2,
                    30));
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
                orderIndices.push(i + dim);
                orderIndices.push(i + 1);
                orderIndices.push((i + 1) + dim);

            } else {
                // *____*
                // | \ C|
                // |D \ |
                // *----*
                // Triangle C
                orderIndices.push(i + dim);
                orderIndices.push(i);
                orderIndices.push((i + 1) + dim);
                // Triangle D
                orderIndices.push((i + 1) + dim);
                orderIndices.push(i);
                orderIndices.push(i + 1);

            }
        }
        this.makeParticles(positions, orderIndices);
        this.place([-25, -30, 0], [0, 0, 0], true, true);
        // Make the springs!
        this.createSprings();
    }

    createSprings() {
        this._createStructuralSprings(0.1, 0.5);
        this._createBendSprings(2, 2, 2, 5);
        this._createShearSprings(0.2);
        this._createHydrogelSprings(4, 0.3, Math.min(this.gridDim/8,2));
        const halfGridDim = this.gridDim / 2;
        const seedPtIndex = halfGridDim + halfGridDim * halfGridDim;
        this.geo.vertices[seedPtIndex] = this.geo.vertices[seedPtIndex].add(new THREE.Vector3(0,0, -4.0));
        //his._createFixedPositionSprings(150);
    }


    _createHydrogelSprings(stiffness, initialExtensionRatio=1.00, spacing=1) {
        const points = this.geo.vertices;
        const dim = this.gridDim;
        for (let i = 0; i < points.length; i += spacing) {
            let springLength = 0;
            if (i - dim >= 0) {
                // before rows
                springLength = initialExtensionRatio * ((points[i].clone().sub(points[i - dim])).length());
                this.constraints.push(new Spring(i, i - dim, springLength, stiffness));
            }
            // Same as structural but only along one dim
            // before column
            if (i - 1 >= 0) {
                springLength = initialExtensionRatio * ((points[i].clone().sub(points[i -  1])).length());
                this.constraints.push(new Spring(i, i - 1, springLength, stiffness));
            }
        }
    }

    _createStructuralSprings(stiffnessX, stiffnessY) {
        const points = this.geo.vertices;
        const dim = this.gridDim;
        for (let i = 0; i < points.length; i++) {
            let springLength = 0;
            // NEIGHBORING STRUCTURAL SPRINGS
            // *(i-1) --- *(i)
            //            |
            //            * (i-dim)
            // TODO pass a stiffness constant
            if (i - dim >= 0) {
                // before row
                springLength = (points[i].clone().sub(points[i - dim])).length();
                this.constraints.push(new Spring(i, i - dim, springLength, stiffnessY));
            }
            // before column
            if (i - 1 >= 0) {
                springLength = points[i].clone().sub(points[i - 1]).length();
                this.constraints.push(new Spring(i, i - 1, springLength, stiffnessX));
            }
        }
    }

    _createBendSprings(stiffnessX, stiffnessY, startBendDistance=2, endBendDistance=5) {
        const points = this.geo.vertices;
        const dim = this.gridDim;
        for (let i = 0; i < points.length; i += 1) {
            let springLength = 0;
            // BEND SPRINGS
            // * (i-2) -...- *(i)----
            //               .|.

            //                * (i-2*dim)
            // before column
            for (let j = startBendDistance; j < endBendDistance; j++) {
                if (i - j * dim >= 0) {
                    // before row
                    springLength = (points[i].clone().sub(points[i - j * dim])).length();
                    this.constraints.push(
                        new Spring(i, i - j * dim, springLength, stiffnessY));
                }
                // before column
                if (i - j >= 0) {
                    springLength = points[i].clone().sub(points[i - j]).length();
                    this.constraints.push(
                        new Spring(i, i - j, springLength, stiffnessX));
                    }
                }
            }
        }

    _createShearSprings(stiffness = 100) {
        // SHEAR SPRINGS
        //      * (i + dim - 1)
        //      | \
        //(i-1) * - * (i)
        //      | /
        //      * (i - dim - 1)
        // before column
        const points = this.geo.vertices;
        const dim = this.gridDim;
        for (let i = 0; i < points.length; i += 2) {
            let springLength = 0;
            if (i - (1 + dim) >= 0) {
                springLength = points[i].clone().sub(points[i - (1 + dim)]).length();
                this.constraints.push(
                    new Spring(i, i - (1 + dim), springLength, stiffness));
            }
            if (i + dim - 1 < points.length) {
                springLength = points[i].clone().sub(points[i + dim - 1]).length();
                this.constraints.push(
                    new Spring(i, i + dim - 1, springLength, stiffness));
                }
            }
        }

    _createFixedPositionSprings(stiffness = 1000) {
        // Fixed point springs for pinning
        const points = this.geo.vertices;
        const dim = this.gridDim;
        for (let i = points.length - dim; i < points.length; i++) {
            this.constraints.push(new ZeroLengthSpring(i, points[i], stiffness));
        }
    }
}
