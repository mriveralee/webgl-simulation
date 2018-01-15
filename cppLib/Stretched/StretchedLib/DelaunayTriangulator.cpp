#include "DelaunayTriangulator.h"


#include "Utils/Logger.h"
#include "MathLib/ConvexHull3D.h"


extern "C" {
	#include "triangle.h"
	#include <stdio.h>
	#include <stdlib.h>
}

static const int POINTS_IN_SEGMENT = 2;


static const int VALUES_PER_POINT2D = 2;
static const int VALUES_PER_POINT = VALUES_PER_POINT2D;
static const double ERROR_MULTIPLIER = 1;

void report(struct triangulateio *io, int markers, int reporttriangles, int reportneighbors, int reportsegments,
	int reportedges, int reportnorms) {
	int i, j;

	for (i = 0; i < io->numberofpoints; i++) {
		printf("Point %4d:", i);
		for (j = 0; j < VALUES_PER_POINT; j++) {
			printf("  %.6g", io->pointlist[i * VALUES_PER_POINT + j]);
		}
		if (io->numberofpointattributes > 0) {
			printf("   attributes");
		}
		for (j = 0; j < io->numberofpointattributes; j++) {
			printf("  %.6g",
				io->pointattributelist[i * io->numberofpointattributes + j]);
		}
		if (markers) {
			printf("   marker %d\n", io->pointmarkerlist[i]);
		}
		else {
			printf("\n");
		}
	}
	printf("\n");

	if (reporttriangles || reportneighbors) {
		for (i = 0; i < io->numberoftriangles; i++) {
			if (reporttriangles) {
				printf("Triangle %4d points:", i);
				for (j = 0; j < io->numberofcorners; j++) {
					printf("  %4d", io->trianglelist[i * io->numberofcorners + j]);
				}
				if (io->numberoftriangleattributes > 0) {
					printf("   attributes");
				}
				for (j = 0; j < io->numberoftriangleattributes; j++) {
					printf("  %.6g", io->triangleattributelist[i *
						io->numberoftriangleattributes + j]);
				}
				printf("\n");
			}
			if (reportneighbors) {
				printf("Triangle %4d neighbors:", i);
				for (j = 0; j < 3; j++) {
					printf("  %4d", io->neighborlist[i * 3 + j]);
				}
				printf("\n");
			}
		}
		printf("\n");
	}

	if (reportsegments) {
		for (i = 0; i < io->numberofsegments; i++) {
			printf("Segment %4d points:", i);
			for (j = 0; j < 2; j++) {
				printf("  %4d", io->segmentlist[i * 2 + j]);
			}
			if (markers) {
				printf("   marker %d\n", io->segmentmarkerlist[i]);
			}
			else {
				printf("\n");
			}
		}
		printf("\n");
	}

	if (reportedges) {
		for (i = 0; i < io->numberofedges; i++) {
			printf("Edge %4d points:", i);
			for (j = 0; j < 2; j++) {
				printf("  %4d", io->edgelist[i * 2 + j]);
			}
			if (reportnorms && (io->edgelist[i * 2 + 1] == -1)) {
				for (j = 0; j < 2; j++) {
					printf("  %.6g", io->normlist[i * 2 + j]);
				}
			}
			if (markers) {
				printf("   marker %d\n", io->edgemarkerlist[i]);
			}
			else {
				printf("\n");
			}
		}
		printf("\n");
	}
}



// The direction that is considered up is non-zero in that component
DelaunayTriangulatorPlane2DType planeTypeFromPlaneDir(P3D planeDir) {
	if (planeDir[0] != 0) {
		return YZ_PLANE;
	} else if (planeDir[1] != 0) {
		return XZ_PLANE;
	} else {
		return YZ_PLANE;
	}
}

void convertPointTo2DWithPlaneType(P3D p, DelaunayTriangulatorPlane2DType planeType, REAL &a, REAL &b) {
	switch (planeType) {
		case XY_PLANE:
			a = p[0];
			b = p[1];
			break;
		case YZ_PLANE:
			a = p[1];
			b = p[2];
			break;
		case XZ_PLANE:
		default:
			a = p[0];
			b = p[2];
	}
}

P3D convertPoint2DTo3DWithPlaneTypeAndHeight(REAL a, REAL b, DelaunayTriangulatorPlane2DType planeType, REAL planeHeight) {
	auto pt = P3D();
	switch (planeType) {
		case XY_PLANE:
			pt[0] = a;
			pt[1] = b;
			pt[2] = planeHeight;
			break;
		case YZ_PLANE:
			pt[0] = planeHeight;
			pt[1] = a;
			pt[2] = b;
			break;
		case XZ_PLANE:
		default:
			pt[0] = a;
			pt[1] = planeHeight;
			pt[2] = b;
	}
	return pt;
}

void convertPointsToPointArrayInPlane2D(std::vector<P3D> pts, DelaunayTriangulatorPlane2DType planeType, REAL ptsArray[], int ptArraySize) {
	if (ptArraySize < (int) pts.size()) {
		// We need the right number of points..
		return;
	}
	REAL a, b;
	for (int i = 0; i < (int) pts.size(); i += VALUES_PER_POINT) {
		// Put the right coordinates in a, b
		convertPointTo2DWithPlaneType(pts[i], planeType, a, b);
		ptsArray[i] = ERROR_MULTIPLIER * a;
		ptsArray[i + 1] = ERROR_MULTIPLIER * b;
	}
}

std::vector<P3D> convertPointArray2DToPointsIn3DFromPlaneHeight(REAL ptsArray[], int ptArraySize, DelaunayTriangulatorPlane2DType planeType, REAL planeHeight) {
	std::vector<P3D> pts = std::vector<P3D>();
	pts.reserve((ptArraySize + 1) / 2);
	for (int i = 0; i < ptArraySize * VALUES_PER_POINT; i += VALUES_PER_POINT) {
		// Put the right coordinates in a, b
		P3D pt = convertPoint2DTo3DWithPlaneTypeAndHeight(ptsArray[i] / ERROR_MULTIPLIER, ptsArray[i + 1] / ERROR_MULTIPLIER, planeType, planeHeight);
		pts.push_back(pt);
	}
	return pts;
}

DelaunayTriangulator::DelaunayTriangulator() {
	// Nothing to see here
}

DelaunayTriangulator::~DelaunayTriangulator() {
	// Nothing to see here
}

DelaunayTriangulation DelaunayTriangulator::triangulatePoints(std::vector<P3D> pts, DelaunayTriangulatorPlane2DType planeType) {
	if (pts.size() == 0) {
		return DelaunayTriangulation();
	}

	struct triangulateio in, mid;
	int ptsArraySize = (int) pts.size();

	/* Define input points. */
	in.numberofpoints = ptsArraySize;
	in.numberofpointattributes = 0;
	in.pointlist = (REAL *)malloc(in.numberofpoints * VALUES_PER_POINT * sizeof(REAL));
	in.pointattributelist = (REAL *)NULL;
	in.pointmarkerlist = (int *)NULL;// (int *)malloc(in.numberofpoints * sizeof(int));
	in.numberofsegments = 0;// ptsArraySize;
	in.segmentlist = (int *)malloc(in.numberofsegments * 2 * sizeof(int)); // Each segment has 2 pts
	in.segmentmarkerlist = (int *)malloc(in.numberofsegments * sizeof(int));
	in.numberofholes = 0;
	in.numberofregions = 0;
	printf("Input point set:\n\n");
	/* Make necessary initializations so that Triangle can return a */
	/*   triangulation in `mid' and a voronoi diagram in `vorout'.  */


	mid.pointlist = (REAL *)NULL;            /* Not needed if -N switch used. */
											 /* Not needed if -N switch used or number of point attributes is zero: */
	mid.pointattributelist = (REAL *)NULL;
	mid.pointmarkerlist = (int *)NULL; /* Not needed if -N or -B switch used. */
	mid.trianglelist = (int *)NULL;          /* Not needed if -E switch used. */
											 /* Not needed if -E switch used or number of triangle attributes is zero: */
	mid.triangleattributelist = (REAL *)NULL;
	mid.neighborlist = (int *)NULL;         /* Needed only if -n switch used. */
											/* Needed only if segments are output (-p or -c) and -P not used: */
	mid.segmentlist = (int *)NULL;
	/* Needed only if segments are output (-p or -c) and -P and -B not used: */
	mid.segmentmarkerlist = (int *)NULL;
	mid.edgelist = (int *)NULL;             /* Needed only if -e switch used. */
	mid.edgemarkerlist = (int *)NULL;   /* Needed if -e used and -B not used. */
	// This is the fun part...
	Logger::consolePrint("Converting of 3D -> 2D Points in Plane");
	convertPointsToPointArrayInPlane2D(pts, planeType, in.pointlist, ptsArraySize);
	Logger::consolePrint("Creating segment list from points");
	//for (int i = 0; i < in.numberofsegments; i++) {
	//	int a = i;
	//	int b = i + 1;
	//	if (b >= in.numberofsegments) {
	//		// Wrap around to the first one
	//		b = 0;
	//	}
	//	in.segmentlist[2 * i] = a;
	//	in.segmentlist[2 * i + 1] = b;

	//	// Set the segment marker as a boundary (because they all should be);
	//	in.segmentmarkerlist[i] = 0;
	//}
	

	/* Triangulate the points.  Switches are chosen to read and write a  */
	/*   PSLG (p), preserve the convex hull (c), number everything from  */
	/*   zero (z), assign a regional attribute to each element (A), and  */
	/*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
	/*   neighbor list (n).
	*/
	Logger::consolePrint("Starting Delaunay Triangulation");
	// Best So far : triangulate("qDYzeX", &in, &mid, NULL);
	// Removed the XsD //pjYq30zsCVL
	triangulate("pcq28.6zOXYeD", &in, &mid, NULL);
	Logger::consolePrint("Finished Delaunay Triangulation");
	Logger::consolePrint("Converting of 2D -> 3D Points in Plane");
	P3D firstPt = pts[0];
	double planeHeight = 0;
	if (planeType == XY_PLANE) {
		planeHeight = firstPt[2];
	} else if (planeType == XZ_PLANE) {
		planeHeight = firstPt[1];
	} else {
		// planeType == YZ_PLANE
		planeHeight = firstPt[0];
	}

	printf("Initial triangulation:\n\n");
	report(&mid, 0, 1, 0, 1, 0, 0);

	// Construct a usable triangulation for our design window
	std::vector<P3D> triangulationPts = convertPointArray2DToPointsIn3DFromPlaneHeight(mid.pointlist, mid.numberofpoints, planeType, planeHeight);
	for (int i = 0; i < (int) triangulationPts.size(); i++) {
		if (isnan(triangulationPts[i][0]) || isnan(triangulationPts[i][2])) {
			Logger::consolePrint("Error: Triangulation returned NAN value!");
			//triangulationPts[i] = P3D(0, triangulationPts[i][1], 0);
		}
	}

	//ConvexHull2D;
	std::vector<ConvexHull2D_Vertex> chull = std::vector<ConvexHull2D_Vertex>();
	ConvexHull3D::planarConvexHullFromSetOfPoints(triangulationPts, V3D(0, 1, 0), chull);
	std::vector<P3D> hull = std::vector<P3D>();
	for (int i = 0; i < (int) chull.size(); i++) {
		hull.push_back(chull[i].projectedPoint);
	}


	// Get the indices for the triangulation
	std::vector<int> triangulationIndices = std::vector<int>();
	for (int i = 0; i < mid.numberoftriangles; i++) {
		for (int j = 0; j < mid.numberofcorners; j++) {
			int ptIndex = mid.trianglelist[i * mid.numberofcorners + j];
			triangulationIndices.push_back(ptIndex);
		}
	}

	// Get the segments for the convex hull
	std::vector<int> convexHullIndices = std::vector<int>();
	//for (int i = 0; i < mid.numberofsegments; i++) {
	//	for (int j = 0; j < POINTS_IN_SEGMENT; j++) {
	//		int ptIndex = mid.segmentlist[i * POINTS_IN_SEGMENT + j];
	//		convexHullIndices.push_back(ptIndex);
	//	}
	//}

	// Get the indices for the new hull points
	convexHullIndices = std::vector<int>();
	for (int i = 0; i < (int) hull.size(); i++) {
		P3D h = hull[i];
		for (int j = 0; j < (int) triangulationPts.size(); j++) {
			P3D t = triangulationPts[j];
			if (abs(h[0] - t[0]) <= EPSILON 
				&& abs(h[1] - t[1]) <= EPSILON
				&& abs(h[2] - t[2]) <= EPSILON) {
				convexHullIndices.push_back(j);
				break;
			}
		}
	}

	//// Get the edges for the convex hull
	//convexHullIndices = std::vector<int>();
	//for (int i = 0; i < mid.numberofedges; i++) {
	//	for (int j = 0; j < POINTS_IN_SEGMENT; j++) {
	//		int ptIndex = mid.edgelist[i * POINTS_IN_SEGMENT + j];
	//		convexHullIndices.push_back(ptIndex);
	//	}
	//}

	DelaunayTriangulation output = DelaunayTriangulation(triangulationPts, triangulationIndices, convexHullIndices);

	// Free any memory
	free(in.pointlist);
	free(in.pointattributelist);
	free(in.pointmarkerlist);
	free(in.segmentlist);
	free(mid.pointlist);
	free(mid.pointattributelist);
	free(mid.pointmarkerlist);
	free(mid.trianglelist);
	free(mid.triangleattributelist);
	free(mid.neighborlist);
	free(mid.segmentlist);
	free(mid.segmentmarkerlist);
	free(mid.edgelist);
	free(mid.edgemarkerlist);

	// Now finish and return our triangulation
	return output;
}
