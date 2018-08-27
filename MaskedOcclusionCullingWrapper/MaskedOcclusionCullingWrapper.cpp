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

void mulMatrix4x4(float* lhs, float* rhs, float* res)
{
	res[0] = lhs[0] * rhs[0] + lhs[4] * rhs[1] + lhs[8] * rhs[2] + lhs[12] * rhs[3];
	res[1] = lhs[1] * rhs[0] + lhs[5] * rhs[1] + lhs[9] * rhs[2] + lhs[13] * rhs[3];
	res[2] = lhs[2] * rhs[0] + lhs[6] * rhs[1] + lhs[10] * rhs[2] + lhs[14] * rhs[3];
	res[3] = lhs[3] * rhs[0] + lhs[7] * rhs[1] + lhs[11] * rhs[2] + lhs[15] * rhs[3];

	res[4] = lhs[0] * rhs[4] + lhs[4] * rhs[5] + lhs[8] * rhs[6] + lhs[12] * rhs[7];
	res[5] = lhs[1] * rhs[4] + lhs[5] * rhs[5] + lhs[9] * rhs[6] + lhs[13] * rhs[7];
	res[6] = lhs[2] * rhs[4] + lhs[6] * rhs[5] + lhs[10] * rhs[6] + lhs[14] * rhs[7];
	res[7] = lhs[3] * rhs[4] + lhs[7] * rhs[5] + lhs[11] * rhs[6] + lhs[15] * rhs[7];

	res[8] = lhs[0] * rhs[8] + lhs[4] * rhs[9] + lhs[8] * rhs[10] + lhs[12] * rhs[11];
	res[9] = lhs[1] * rhs[8] + lhs[5] * rhs[9] + lhs[9] * rhs[10] + lhs[13] * rhs[11];
	res[10] = lhs[2] * rhs[8] + lhs[6] * rhs[9] + lhs[10] * rhs[10] + lhs[14] * rhs[11];
	res[11] = lhs[3] * rhs[8] + lhs[7] * rhs[9] + lhs[11] * rhs[10] + lhs[15] * rhs[11];

	res[12] = lhs[0] * rhs[12] + lhs[4] * rhs[13] + lhs[8] * rhs[14] + lhs[12] * rhs[15];
	res[13] = lhs[1] * rhs[12] + lhs[5] * rhs[13] + lhs[9] * rhs[14] + lhs[13] * rhs[15];
	res[14] = lhs[2] * rhs[12] + lhs[6] * rhs[13] + lhs[10] * rhs[14] + lhs[14] * rhs[15];
	res[15] = lhs[3] * rhs[12] + lhs[7] * rhs[13] + lhs[11] * rhs[14] + lhs[15] * rhs[15];
}


void mocRenderObjects(void* moc, objectRenderData* data, int nObject, int* resultList, float* PVMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask)
{
	float MVPMatrix[16];
	for (int i = 0; i < nObject; i++)
	{
		mulMatrix4x4(PVMatrix, data[i].modelToClipMatrix, MVPMatrix);
		resultList[i] = (int)(((MaskedOcclusionCulling*)moc)->RenderTriangles(data[i].inVtx, data[i].inTris, data[i].nTris, MVPMatrix, nullptr, bfWinding, clipPlaneMask, VertexLayout(16, 4, 12)));
	}
}

void mocTestObjects(void* moc, objectRenderData* data, int nObject, int* resultList, float* PVMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask)
{
	float MVPMatrix[16];
	for (int i = 0; i < nObject; i++) 
	{
		mulMatrix4x4(PVMatrix, data[i].modelToClipMatrix, MVPMatrix);
		resultList[i] = (int)(((MaskedOcclusionCulling*)moc)->TestTriangles(data[i].inVtx, data[i].inTris, data[i].nTris, MVPMatrix, nullptr, bfWinding, clipPlaneMask, VertexLayout(16, 4, 12)));
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