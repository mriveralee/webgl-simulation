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
        this.gridParticles = this.gridDim * this.gridDim;
        this.pointSpacingXY = Config.simulation.gridDim.spacing;

        this.hydrogelColumns = Config.simulation.hydrogel.hydrogelColumns;
        let minSpaceCount = Math.max(1, this.hydrogelColumns - 1);
        this.hydrogelColumnSpacing = Math.max(
            1,
            Math.ceil((this.gridDim - this.hydrogelColumns) / (minSpaceCount)));
        let remainder = Math.floor(
            (this.gridDim - this.hydrogelColumnSpacing * (minSpaceCount)) % this.hydrogelColumns);

        this.hydrogelStartColumn = Math.floor(remainder / 2);
        this.hydrogelParticles =  this.gridDim * this.hydrogelColumns;
        console.log(this.hydrogelColumns, this.hydrogelColumnSpacing, this.hydrogelParticles);

        // Math.floor(this.gridDim * this.gridDim / this.hydrogelSpacing);
        this.numParticles = this.gridParticles + this.hydrogelParticles;
        this.masses = [];
        this.inverseMasses = [];
        this.forces = [];
        this.accelerations = [];
        this.velocities = [];
        this.fixedPointIndices = [];
        this.oldPositions = [];
        this.layerHeight = Config.simulation.hydrogel.layerHeight;
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
        if (Config.simulation.avoidSelfIntersections) {
            this._resolveSelfIntersections(this.geo.vertices, Config.simulation.fabricSelfIntersectionsMinDist);
        }
        for (let i = 0; i < this.constraints.length; i++) {
            this.constraints[i].resolveConstraint(this.geo.vertices, this.forces);
        }

    }

    _resolveSelfIntersections(vertices, minIntersectionDistance) {
        const dim = this.gridDim;
        const gridParticles = this.gridParticles;
        let deltaVec = new THREE.Vector3();
        const sqIntersectDist = minIntersectionDistance * minIntersectionDistance;
        for (let i = 0; i < gridParticles; i++) {
            for (let j = 0; j < gridParticles; j++) {
                if (i == j) {
                    continue;
                }
                let sqDist = vertices[j].distanceToSquared(vertices[i])
                if (sqDist > sqIntersectDist || sqIntersectDist == 0) {
                    // there's no intersection
                    continue;
                }
                // // Otherwise move particles away
                deltaVec.copy(vertices[j]).sub(vertices[i]);
                let dist = deltaVec.length();
                let correctionVec = deltaVec.multiplyScalar((dist - minIntersectionDistance) / dist);
                let halfCorrectionVec = correctionVec.multiplyScalar(0.5);
                vertices[i].add(halfCorrectionVec);
                vertices[j].sub(halfCorrectionVec);
            }
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
                positions.push(new THREE.Vector3( // swapped i and j here
                    i * this.pointSpacingXY,
                    j * this.pointSpacingXY,
                    0)); // todo add random jitter here
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

        // Now make hydrogel particles
        const pointsLength = positions.length;
        const columnSpacing = this.hydrogelColumnSpacing;
        const startColumn = this.hydrogelStartColumn;
        let j = 0;
        for (let i = startColumn; i < dim; i += columnSpacing) {
            for (let k = 0; k < dim; k += 1) {
                let positionIndex = i * dim + k;
                let firstPosition = positions[positionIndex];
                let secondPosition = firstPosition.clone();
                secondPosition.z += this.layerHeight / 2;
                //secondPosition.x -= ptSpacing;
                positions.push(secondPosition);
                orderIndices.push(positionIndex);
                orderIndices.push(pointsLength + j);
                orderIndices.push(pointsLength + j);
                j += 1;

                // Draw lines across the gel points sampels  o-o
                if ((pointsLength + j) % this.gridDim != 0) {
                    orderIndices.push(pointsLength + j - 1)
                    orderIndices.push(pointsLength + j)
                    orderIndices.push(pointsLength + j)
                }
            }
        }

        this.makeParticles(positions, orderIndices);

        let midPosition = (-this.gridDim/2 + this.pointSpacingXY/4)/2;
        this.place([midPosition, midPosition, 0], [0, 0, 0], Config.mesh.showPoints);
        // Make the springs!
        this.createSprings();
    }

    createSprings() {
        this._createStructuralSprings(
            Config.simulation.fabric.structuralSpringStiffnessX,
            Config.simulation.fabric.structuralSpringStiffnessY);
        this._createBendSprings(
            Config.simulation.fabric.bendSpringStiffnessX,
            Config.simulation.fabric.bendSpringStiffnessY,
            2, 3); // Bend extension (2 away )
        this._createShearSprings(Config.simulation.fabric.shearSpringStiffness);

        this._createHydrogelSprings(
            Config.simulation.hydrogel.springStiffnessZ,
            Config.simulation.hydrogel.springStiffnessXY,
            Config.simulation.hydrogel.springShrinkRatioZ,
            Config.simulation.hydrogel.springShrinkRatioXY);
        //const halfGridDim = this.gridDim / 2;
        //const seedPtIndex = halfGridDim + halfGridDim * halfGridDim;
        //this.geo.vertices[seedPtIndex] = this.geo.vertices[seedPtIndex].add(new THREE.Vector3(0,0, -4.0));
        //his._createFixedPositionSprings(150);
    }


    _createHydrogelSprings(stiffnessZ, stiffnessXY, initialExtensionRatioZ=1.00, initialExtensionRatioXY=1.00) {
        const points = this.geo.vertices;
        const textileParticleCount = this.gridParticles;
        const dim = this.gridDim;
        const spacing = this.hydrogelSpacing;

        let j = 0;
        let springLength = 0;
        for (let i = spacing; i < dim; i += spacing) {
            for (let k = 0; k < dim; k += 1) {
                let textileParticleIndex = i * dim + k;
                let hydrogelParticleIndex = textileParticleCount + j;


                // Textile to Hydrogel Spring (Vertical)
                let baseLength = (points[textileParticleIndex].clone().sub(points[hydrogelParticleIndex])).length();
                springLength = initialExtensionRatioZ * baseLength;

                this.constraints.push(
                    new Spring(
                        textileParticleIndex,
                        hydrogelParticleIndex,
                        springLength,
                        stiffnessZ));
                // Hydrogel-to-Hydrogel Spring (along gel line)
                if (k != 0) {
                    let prevHydrogelParticleIndex = hydrogelParticleIndex - 1;
                    baseLength = (points[hydrogelParticleIndex].clone().sub(points[prevHydrogelParticleIndex])).length();
                    springLength = initialExtensionRatioXY * baseLength;
                    this.constraints.push(
                        new Spring(
                            prevHydrogelParticleIndex,
                            hydrogelParticleIndex,
                            springLength,
                            stiffnessXY));
                }

                // Hydrogel-to-Hydrogel Bend Spring (along gel line)
                if (k > 1) {
                    let prevHydrogelParticleIndex = hydrogelParticleIndex - 2;
                    baseLength = (points[hydrogelParticleIndex].clone().sub(points[prevHydrogelParticleIndex])).length();
                    springLength = initialExtensionRatioXY * baseLength;
                    this.constraints.push(
                        new Spring(
                            prevHydrogelParticleIndex,
                            hydrogelParticleIndex,
                            springLength,
                            stiffnessXY));
                }
                // H
                // Move to next Hydrogel Particle
                j += 1;
            }
        }
        // for (let i = textileParticleCount; i < (points.length - dim); i += 1) {
        //     // Hydrogel-to-Hydrogel across lines
        //     let hydrogelPtIndex = i;
        //     let hydrogelPtIndex2 = i + dim;
        //     let baseLength = (points[hydrogelPtIndex].clone().sub(points[hydrogelPtIndex2])).length();
        //     springLength = initialExtensionRatioXY * baseLength;
        //
        //     this.constraints.push(
        //         new Spring(
        //             hydrogelPtIndex,
        //             hydrogelPtIndex2,
        //             springLength,
        //             stiffnessXY));
        // }
    }

    _createStructuralSprings(stiffnessX, stiffnessY) {
        const points = this.geo.vertices;
        const dim = this.gridDim;
        for (let i = 0; i < this.gridParticles; i++) {
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
        for (let i = 0; i < this.gridParticles; i += 1) {
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
        for (let i = 0; i < this.gridParticles; i += 2) {
            let springLength = 0;
            if (i - (1 + dim) >= 0) {
                springLength = points[i].clone().sub(points[i - (1 + dim)]).length();
                this.constraints.push(
                    new Spring(i, i - (1 + dim), springLength, stiffness));
            }
            if (i + dim - 1 < this.gridParticles) {
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
        for (let i = this.gridParticles - dim; i < this.gridParticles; i++) {
            this.constraints.push(new ZeroLengthSpring(i, points[i], stiffness));
        }
    }
}
