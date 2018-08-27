#include "MaskedOcclusionCullingWrapper.h"

void* mocCreate(Implementation RequestedSIMD)
{
	return MaskedOcclusionCulling::Create(RequestedSIMD);
}

void mocDestroy(void* moc)
{
	MaskedOcclusionCulling::Destroy((MaskedOcclusionCulling*)moc);
}

void mocSetResolution(void* moc, unsigned int width, unsigned int height)
{
	((MaskedOcclusionCulling*)moc)->SetResolution(width, height);
}

void mocClearBuffer(void* moc)
{
	((MaskedOcclusionCulling*)moc)->ClearBuffer();
}

void mocSetNearClipPlane(void* moc, float nearDist)
{
	((MaskedOcclusionCulling*)moc)->SetNearClipPlane(nearDist);
}

CullingResult mocRenderTriangles(void* moc, float *inVtx, int *inTris, int nTris, float *modelToClipMatrix, float *PVMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask)
{
	return ((MaskedOcclusionCulling*)moc)->RenderTriangles(inVtx, inTris, nTris, modelToClipMatrix, PVMatrix, bfWinding, clipPlaneMask);
}

CullingResult mocTestTriangles(void* moc, float *inVtx, int *inTris, int nTris, float *modelToClipMatrix, float *PVMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask)
{
	return ((MaskedOcclusionCulling*)moc)->TestTriangles(inVtx, inTris, nTris, modelToClipMatrix, PVMatrix, bfWinding, clipPlaneMask);
}

CullingResult mocTestRect(void* moc, float xmin, float ymin, float xmax, float ymax, float wmin, float wmax)
{
	return ((MaskedOcclusionCulling*)moc)->TestRect(xmin, ymin, xmax, ymax, wmin, wmax);
}

void mocComputePixelDepthBuffer(void* moc, float *depthData, bool flipY)
{
	((MaskedOcclusionCulling*)moc)->ComputePixelDepthBuffer(depthData, flipY);
}

void mocRenderObjects(void* moc, objectRenderData* data, int nObject, int* resultList, float* PVMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask)
{
	for (int i = 0; i < nObject; i++)
	{
		resultList[i] = (int)(((MaskedOcclusionCulling*)moc)->RenderTriangles(data[i].inVtx, data[i].inTris, data[i].nTris, data[i].modelToClipMatrix, PVMatrix, bfWinding, clipPlaneMask, VertexLayout(16, 4, 12)));
	}
}

void mocTestObjects(void* moc, objectRenderData* data, int nObject, int* resultList, float* PVMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask)
{

	for (int i = 0; i < nObject; i++) 
	{
		resultList[i] = (int)(((MaskedOcclusionCulling*)moc)->TestTriangles(data[i].inVtx, data[i].inTris, data[i].nTris, data[i].modelToClipMatrix, PVMatrix, bfWinding, clipPlaneMask, VertexLayout(16, 4, 12)));
	}
}

void mocTestRects(void* moc, float *data, int nRect, int* resultList)
{
	for (int i = 0; i < nRect; i++)
	{
		resultList[i] = (int)(((MaskedOcclusionCulling*)moc)->TestRect(data[i * 6], data[i * 6 + 1], data[i * 6 + 2], data[i * 6 + 3], data[i * 6 + 4], data[i * 6 + 5]));
	}
}

void mocGetStatistics(void* moc, long long* result)
{
	MaskedOcclusionCulling::OcclusionCullingStatistics stats = ((MaskedOcclusionCulling*)moc)->GetStatistics();
	result[0] = stats.mOccluders.mNumProcessedTriangles;
	result[1] = stats.mOccluders.mNumRasterizedTriangles;
	result[2] = stats.mOccluders.mNumTilesTraversed;
	result[3] = stats.mOccluders.mNumTilesUpdated;
	result[4] = stats.mOccluders.mNumTilesMerged;
	result[5] = stats.mOccludees.mNumProcessedRectangles;
	result[6] = stats.mOccludees.mNumProcessedTriangles;
	result[7] = stats.mOccludees.mNumRasterizedTriangles;
	result[8] = stats.mOccludees.mNumTilesTraversed;
}