import * as THREE from 'three';

import Config from './../../data/config'
import Geometry from './../helpers/geometry';
import Spring from './../physics/spring';
import ZeroLengthSpring from './../physics/zeroLengthSpring';


// A base class for the particleSystem that will hold a group of particles
const FABRIC_SPRING_STRUCTURAL = 1;
const FABRIC_SPRING_BEND = 2;
const FABRIC_SPRING_SHEAR = 3;
const HYDROGEL_TO_FABRIC_SPRING = 4;
const HYDROGEL_TO_HYDROGEL_SPRING = 5;


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
        this.prevAccelerations = [];

        this.velocities = [];
        this.fixedPointIndices = [];
        this.oldPositions = [];
        this.layerHeight = Config.simulation.hydrogel.layerHeight;
        // Positions are handled in our geometry
        let massFabric = Config.simulation.fabric.particleMass;
        let massHydrogel = Config.simulation.hydrogel.particleMass;
        let mass = 0;
        for (let i = 0; i < this.numParticles; i++) {
            if (i < this.gridParticles) {
                mass = massFabric;
            } else {
                mass = massHydrogel;

            }
            this.masses.push(mass);
            this.inverseMasses.push(1.0 / mass);
            this.forces.push(new THREE.Vector3());
            this.accelerations.push(new THREE.Vector3());
            this.prevAccelerations.push(new THREE.Vector3());
            this.velocities.push(new THREE.Vector3());
            //this.oldPositions.push(new THREE.Vector3());
        }

        this.constraints = [];

        this.constraintsGeometries = [];

    }

    computeForces() {
        let gravity = new THREE.Vector3(...Config.simulation.getGravityComponents());
        let particleArea = this.pointSpacingXY * this.pointSpacingXY / (1000 * 1000);
        let coeffOfDrag = Config.simulation.coefficientOfDrag; // For a flat plate; sphere is about 0.48
        for (let i = 0; i < this.numParticles; i++) {
            // Clear forces
            this.forces[i].multiplyScalar(0);
            // Gravity
            if (Config.simulation.useGravity) {
                this.forces[i].addScaledVector(gravity, this.masses[i]);
            }
            // Drag Forces
            if (Config.simulation.useDragForce) {
                let vel = this.velocities[i].clone();
                let dragForce = vel.multiply(this.velocities[i]).multiplyScalar(coeffOfDrag * particleArea * -1);
                this.forces[i].add(dragForce);

            }
        }
        // Resolve any spring constraints
        this.resolveConstraints();

    }

    computeAccelerations() {
        for (let i = 0; i < this.numParticles ; i++) {
            this.prevAccelerations[i].clone(this.accelerations[i]);
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
                // // x += x - x_prev + a * t^2
                let useVerletVelocities = false;
                if (!useVerletVelocities) {
                    let prevPos = this.oldPositions[i];
                    let currPos = this.geo.vertices[i].clone();
                    let copyCurrPos = currPos.clone();
                    let a = this.accelerations[i];

                    this.geo.vertices[i].add(copyCurrPos);
                    this.geo.vertices[i].sub(prevPos);
                    this.geo.vertices[i].addScaledVector(a, sqTimeStep);
                    this.oldPositions[i] = this.oldPositions[i].copy(copyCurrPos);
                    //
                } else {
                    let a = this.prevAccelerations[i];
                    let nextA = this.accelerations[i];
                    let vel = this.velocities[i];

                    let nextPos =
                        this.geo.vertices[i].addScaledVector(vel, timeStep)
                            .addScaledVector(a, 0.5 * sqTimeStep);

                    let nextVel = a.clone().add(nextA);
                    nextVel = nextVel.multiplyScalar(0.5 * timeStep);
                    nextVel = nextVel.add(vel);
                    // Accelerations happens after updating forces
                    // this.accelerations[i] = nextAcc
                    this.geo.vertices[i] = nextPos;
                    this.velocities[i] = nextVel;
                }
                //this.velocities[i].addScaledVector(this.accelerations[i], timeStep);
                //this.velocities[i].multiplyScalar(1 - dampingFactor);

            }
            // Make the simulation move like honey
            //this.velocities[i].multiplyScalar(0);

        }

        this.visualizeConstraints();

        this.geo.verticesNeedUpdate = true;
        this.geo.normalsNeedUpdate = true;
        this.geo.colorsNeedUpdate = true;
    }

    resolveConstraints() {
        if (Config.simulation.avoidSelfIntersections) {
            this._resolveSelfIntersections(this.geo.vertices, Config.simulation.fabricSelfIntersectionsMinDist);
        }

        // REsolve other constraints
        for (let i = 0; i < this.constraints.length; i++) {
            this.constraints[i].resolveConstraint(this.geo.vertices, this.forces);
        }

        if (Config.simulation.useFloorConstraint) {
            this._resolveFloorConstraint(this.geo.vertices, 0);

        }
    }

    visualizeConstraints() {
        for (let i = 0; i < this.constraintsGeometries.length; i++) {
            let constraintGeo = this.constraintsGeometries[i];
            constraintGeo.verticesNeedUpdate = true;
            constraintGeo.normalsNeedUpdate = true;
            constraintGeo.colorsNeedUpdate = true;
        }

    }

    _getColor(id) {
        switch (id) {
            case FABRIC_SPRING_BEND:
                return 0xff0000;
            case FABRIC_SPRING_SHEAR:
                return 0x00ff00;
            case HYDROGEL_TO_FABRIC_SPRING:
                return 0x0000ff;
            case HYDROGEL_TO_HYDROGEL_SPRING:
                return 0xffff00;
            case FABRIC_SPRING_STRUCTURAL:
                return 0xff00ff;
            default:
                return 0x000000;
        }
    }

    _resolveFloorConstraint(vertices, zPosition) {
        for (let i = 0; i < vertices.length; i++) {
            if (Math.abs(vertices[i].z - zPosition) <= 0.0001) {
                vertices[i].add(new THREE.Vector3(0,0, zPosition));
            }
        }

    }
    _resolveSelfIntersections(vertices, minIntersectionDistance) {
        const dim = this.gridDim;
        const particleCount = this.gridParticles; //vertices.length; //this.gridParticles;
        let deltaVec = new THREE.Vector3();
        const sqIntersectDist = minIntersectionDistance * minIntersectionDistance;
        for (let i = 0; i < particleCount; i++) {
            for (let j = 0; j < particleCount; j++) {
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
                secondPosition.z += this.layerHeight;
                let hydrogelPositionIndex = pointsLength + j;
                //secondPosition.x -= ptSpacing;
                positions.push(secondPosition);

                // Draw vertical line from fabric to gel pt
                //    gPt
                //    |
                //   fPt
                orderIndices.push(positionIndex);
                orderIndices.push(hydrogelPositionIndex);
                orderIndices.push(hydrogelPositionIndex);


                // Draw lines across the gel points sampels  (gPt-1) - gPt
                if ((pointsLength + j) % this.gridDim != 0) {
                    orderIndices.push(hydrogelPositionIndex - 1)
                    orderIndices.push(hydrogelPositionIndex)
                    orderIndices.push(hydrogelPositionIndex)
                }

                // Draw diagonal lines from  fabric
                //        gPt
                //     /   |   \
                //fPt-1  fPt0 fpt+1
                if (k - 1 >= 0) {
                    orderIndices.push(positionIndex - 1)
                    orderIndices.push(hydrogelPositionIndex)
                    orderIndices.push(hydrogelPositionIndex)
                }
                if (k + 1 < this.gridDim) {
                    orderIndices.push(positionIndex + 1)
                    orderIndices.push(hydrogelPositionIndex)
                    orderIndices.push(hydrogelPositionIndex)
                }

                // Draw springs from gel to adjacent columns
                if (i - 1 >= 0) {
                    let prevColumnTextileIndex = (i - 1) * dim + k;
                    orderIndices.push(prevColumnTextileIndex)
                    orderIndices.push(hydrogelPositionIndex)
                    orderIndices.push(hydrogelPositionIndex)
                }
                if (i + 1 < this.gridDim) {
                    let nextColumnTextileIndex = (i + 1) * dim + k;
                    orderIndices.push(nextColumnTextileIndex)
                    orderIndices.push(hydrogelPositionIndex)
                    orderIndices.push(hydrogelPositionIndex)
                }

                // Leave as last item in the loop
                j += 1;
            }
        }

        this.makeParticles(positions, orderIndices);

        let midPosition = (-this.gridDim/2 + this.pointSpacingXY/4)/2;
        let startPos = [midPosition, midPosition, 0];
        startPos = [0, 0, 0];
        let startRot = [0, 0, 0];
        this.place(startPos, startRot, Config.mesh.showPoints);


        // Make the springs!
        this.createSprings();
        // Setup visualization for constraints
        this._createConstraintVisualizations(startPos, startRot);

    }

    _createConstraintVisualizations() {
        let vertices = this.geo.vertices;
        if (this.constraintsGeometries.length == 0) {
            // init constraint geometries:

            for (let i = 0; i < this.constraints.length; i++) {
                let constraint = this.constraints[i];
                let indexA = constraint.indexA;
                let indexB = constraint.indexB;
                let posA = vertices[indexA];
                let posB = vertices[indexB];
                let identifier = constraint.id;
                let colorForId = this._getColor(identifier);
                let material = new THREE.LineBasicMaterial({color: colorForId});
                let constraintGeometry = new THREE.Geometry();
                constraintGeometry.vertices.push(posA);
                constraintGeometry.vertices.push(posB);
                let line = new THREE.Line(constraintGeometry, material);
                this.scene.add(line);
                this.constraintsGeometries.push(constraintGeometry)
            }
        }
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
            //
            // this._createHydrogelSprings(
            //     Config.simulation.hydrogel.springStiffnessZ,
            //     Config.simulation.hydrogel.springStiffnessXY,
            //     Config.simulation.hydrogel.springShrinkRatioZ,
            //     Config.simulation.hydrogel.springShrinkRatioXY);
        const halfGridDim = this.gridDim / 2;
        const seedPtIndex = halfGridDim + halfGridDim * this.gridDim;
        let zBiasOffset = Config.simulation.biasOffsetZ;
        this.geo.vertices[seedPtIndex] = this.geo.vertices[seedPtIndex].add(new THREE.Vector3(0,0, zBiasOffset));
        //his._createFixedPositionSprings(150);
    }


    _createHydrogelSprings(stiffnessZ, stiffnessXY, initialExtensionRatioZ=1.00, initialExtensionRatioXY=1.00) {
        const points = this.geo.vertices;
        const textileParticleCount = this.gridParticles;
        const dim = this.gridDim;

        const columnSpacing = this.hydrogelColumnSpacing;
        const startColumn = this.hydrogelStartColumn;
        let j = 0;
        let springLength = 0;
        for (let i = startColumn; i < dim; i += columnSpacing) {
            for (let k = 0; k < dim; k += 1) {
                let textileParticleIndex = i * dim + k;
                let hydrogelParticleIndex = textileParticleCount + j;


                // Textile to Hydrogel Spring (Vertical)
                //    gPt
                //    |
                //   fPt
                let baseLength = (points[textileParticleIndex].clone().sub(points[hydrogelParticleIndex])).length();
                springLength = initialExtensionRatioZ * baseLength;

                this.constraints.push(
                    new Spring(
                        textileParticleIndex,
                        hydrogelParticleIndex,
                        springLength,
                        stiffnessZ,
                        HYDROGEL_TO_FABRIC_SPRING));
                // Hydrogel-to-Hydrogel Spring (along gel line)
                // (gPt-1) - gPt
                if (k != 0) {
                    let prevHydrogelParticleIndex = hydrogelParticleIndex - 1;
                    baseLength = (points[hydrogelParticleIndex].clone().sub(points[prevHydrogelParticleIndex])).length();
                    springLength = initialExtensionRatioXY * baseLength;
                    this.constraints.push(
                        new Spring(
                            prevHydrogelParticleIndex,
                            hydrogelParticleIndex,
                            springLength,
                            stiffnessXY,
                            HYDROGEL_TO_FABRIC_SPRING));
                }

                // Hydrogel-to-Hydrogel Bend Spring (along gel line)
                // (gPt-2) - XXXX - gPt
                if (k > 1) {
                    let prevHydrogelParticleIndex = hydrogelParticleIndex - 2;
                    baseLength = (points[hydrogelParticleIndex].clone().sub(points[prevHydrogelParticleIndex])).length();
                    springLength = initialExtensionRatioXY * baseLength;
                    this.constraints.push(
                        new Spring(
                            prevHydrogelParticleIndex,
                            hydrogelParticleIndex,
                            springLength,
                            stiffnessXY,
                            HYDROGEL_TO_HYDROGEL_SPRING));
                }
                // Diagonal springs from  fabric to gel
                //        gPt
                //     /   |   \
                // fPt-1  fPt0 fpt+1
                if (k - 1 >= 0) {
                    baseLength = (points[textileParticleIndex-1].clone().sub(points[hydrogelParticleIndex])).length();
                    springLength = initialExtensionRatioZ * baseLength;
                    this.constraints.push(
                        new Spring(
                            textileParticleIndex,
                            hydrogelParticleIndex,
                            springLength,
                            stiffnessZ,
                            HYDROGEL_TO_FABRIC_SPRING));
                }
                if (k + 1 < this.gridDim) {
                    baseLength = (points[textileParticleIndex+1].clone().sub(points[hydrogelParticleIndex])).length();
                    springLength = initialExtensionRatioZ * baseLength;
                    this.constraints.push(
                        new Spring(
                            textileParticleIndex,
                            hydrogelParticleIndex,
                            springLength,
                            stiffnessZ,
                            HYDROGEL_TO_FABRIC_SPRING));
                }

                // Diagonal springs from  fabric to gel across columns
                //        gPt
                //     /   |   \
                // i-1*dim  fPt0 (i+1)*dim
                if (i - 1 >= 0) {
                    let prevColumnTextileIndex = (i - 1) * dim + k;
                    baseLength = (points[prevColumnTextileIndex].clone().sub(points[hydrogelParticleIndex])).length();
                    springLength = initialExtensionRatioZ * baseLength;
                    this.constraints.push(
                        new Spring(
                            prevColumnTextileIndex,
                            hydrogelParticleIndex,
                            springLength,
                            stiffnessZ,
                            HYDROGEL_TO_FABRIC_SPRING));
                }
                if (i + 1 < this.gridDim) {
                    let nextColumnTextileIndex = (i + 1) * dim + k;
                    baseLength = (points[nextColumnTextileIndex].clone().sub(points[hydrogelParticleIndex])).length();
                    springLength = initialExtensionRatioZ * baseLength;
                    this.constraints.push(
                        new Spring(
                            nextColumnTextileIndex,
                            hydrogelParticleIndex,
                            springLength,
                            stiffnessZ,
                            HYDROGEL_TO_FABRIC_SPRING));
                }


                // Last element of loop: Move to next Hydrogel Particle
                j += 1;
            }
        }
    }

    _createStructuralSprings(stiffnessX, stiffnessY) {
        const points = this.geo.vertices;
        const dim = this.gridDim;

        for (let i = 0; i < dim; i += 1) {
            for (let k = 0; k < dim; k += 1) {
                let currentIndex = i * dim + k;
                let springLength = 0;
                // NEIGHBORING STRUCTURAL SPRINGS
                // *(i-1)*dim+k --- *(i)*dim + k
                //            |
                //            * (i*dim + k)
                if (i > 0) {
                    let prevColIndex = (i - 1) * dim + k;
                    springLength = (points[currentIndex].clone().sub(points[prevColIndex])).length();
                    this.constraints.push(new Spring(
                        currentIndex,
                        prevColIndex,
                        springLength,
                        stiffnessX,
                        FABRIC_SPRING_STRUCTURAL));
                }
                if (k + 1 < dim) {
                    let nextIndex = currentIndex + 1;
                    springLength = (points[currentIndex].clone().sub(points[nextIndex])).length();
                    this.constraints.push(new Spring(
                        currentIndex,
                        nextIndex,
                        springLength,
                        stiffnessY,
                        FABRIC_SPRING_STRUCTURAL));
                }
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
                        new Spring(i, i - j * dim, springLength, stiffnessY, FABRIC_SPRING_BEND));
                }
                // before column
                if (i - j >= 0) {
                    springLength = points[i].clone().sub(points[i - j]).length();
                    this.constraints.push(
                        new Spring(i, i - j, springLength, stiffnessX, FABRIC_SPRING_BEND));
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
        let springLength = 0;

        for (let i = 0; i < dim; i += 1) {
            for (let k = 0; k < dim; k += 1) {
                let currentIndex = i * dim + k;
                if (i >= 1 && k >= 1) {
                    let beforeShearIndex = (i - 1) * dim + (k - 1);
                    springLength = points[currentIndex].clone().sub(points[beforeShearIndex]).length();
                    this.constraints.push(
                        new Spring(currentIndex, beforeShearIndex, springLength, stiffness, FABRIC_SPRING_SHEAR));
                }
                if (i >= 1 && (k + 1) < this.gridDim) {
                    let nextShearIndex = (i - 1) * dim + (k + 1);
                    springLength = points[currentIndex].clone().sub(points[nextShearIndex]).length();
                    this.constraints.push(
                        new Spring(currentIndex, nextShearIndex, springLength, stiffness, FABRIC_SPRING_SHEAR));
                }
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
