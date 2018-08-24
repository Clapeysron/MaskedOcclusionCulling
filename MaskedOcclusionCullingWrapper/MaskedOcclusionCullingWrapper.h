#pragma once

#define DLL_API __declspec(dllexport) 
#include "../MaskedOcclusionCulling.h"

extern "C" {

	typedef MaskedOcclusionCulling::CullingResult   CullingResult;
	typedef MaskedOcclusionCulling::ClipPlanes      ClipPlanes;
	typedef MaskedOcclusionCulling::BackfaceWinding BackfaceWinding;
	typedef MaskedOcclusionCulling::ScissorRect     ScissorRect;
	typedef MaskedOcclusionCulling::VertexLayout    VertexLayout;
	typedef MaskedOcclusionCulling::TriList         TriList;
	typedef MaskedOcclusionCulling::Implementation  Implementation;

	class MaskedOcclusionCullingWrapper;

	struct objectRenderData {
		float *inVtx;
		int *inTris;
		int nTris;
		float *modelToClipMatrix;
	};

	DLL_API void* mocCreate(Implementation RequestedSIMD);

	DLL_API void mocDestroy(void* moc);

	DLL_API void mocSetResolution(void* moc, unsigned int width, unsigned int height);

	DLL_API void mocClearBuffer(void* moc);

	DLL_API void mocSetNearClipPlane(void* moc, float nearDist);

	DLL_API CullingResult mocRenderTriangles(void* moc, float *inVtx, int *inTris, int nTris, float *modelToClipMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask);

	DLL_API CullingResult mocTestTriangles(void* moc, float *inVtx, int *inTris, int nTris, float *modelToClipMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask);

	DLL_API CullingResult mocTestRect(void* moc, float xmin, float ymin, float xmax, float ymax, float wmin);

	DLL_API void mocComputePixelDepthBuffer(void* moc, float *depthData, bool flipY);

	DLL_API void mocRenderObjects(void* moc, objectRenderData* data, int nObject, int* resultList, float* PxVMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask);

	DLL_API void mocTestObjects(void* moc, objectRenderData* data, int nObject, int* resultList, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask);

	DLL_API void mocTestRects(void* moc, float *data, int nRect, int* resultList);

	DLL_API void mocGetStatistics(void* moc, long long* result);

}
