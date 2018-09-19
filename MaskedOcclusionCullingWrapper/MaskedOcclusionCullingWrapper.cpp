#include "MaskedOcclusionCullingWrapper.h"
#include <Windows.h>
#include <algorithm>

#define clamp(x, lower, upper) (max(lower, min(x, upper)))

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

CullingResult mocRenderTriangles(void* moc, float *inVtx, int *inTris, int nTris, float *modelToClipMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask)
{
	return ((MaskedOcclusionCulling*)moc)->RenderTriangles(inVtx, inTris, nTris, modelToClipMatrix, bfWinding, clipPlaneMask);
}

CullingResult mocTestTriangles(void* moc, float *inVtx, int *inTris, int nTris, float *modelToClipMatrix, BackfaceWinding bfWinding, ClipPlanes clipPlaneMask)
{
	return ((MaskedOcclusionCulling*)moc)->TestTriangles(inVtx, inTris, nTris, modelToClipMatrix, bfWinding, clipPlaneMask);
}

CullingResult mocTestRect(void* moc, float xmin, float ymin, float xmax, float ymax, float wmin, float wmax)
{
	return ((MaskedOcclusionCulling*)moc)->TestRect(xmin, ymin, xmax, ymax, wmin, wmax);
}

void mocComputePixelDepthBuffer(void* moc, float *depthData, bool flipY)
{
	((MaskedOcclusionCulling*)moc)->ComputePixelDepthBuffer(depthData, flipY);
}

void mocRenderObjects(void* moc, int nObject, float* OccluderInVtx, int* OccluderVtxIndex,
	int* OccluderInTris, int* OccluderNTris, int* OccluderTrisIndex, float* OccluderCenter,
	float* OccluderRadius, int* resultList, float* PVMatrix, float* frustum,
	BackfaceWinding bfWinding, ClipPlanes clipPlaneMask)
{
	MaskedOcclusionCulling* _moc = ((MaskedOcclusionCulling*)moc);
	long long test_frustum_cull_time = 0;
	long long rasterize_time = 0;

	LARGE_INTEGER cpuFreq = { 0 };
	QueryPerformanceFrequency(&cpuFreq);

	LARGE_INTEGER ts = { 0 };
	QueryPerformanceCounter(&ts);

	for (int i = 0; i < nObject; i++)
	{
		_moc->AddOccluderCount(OccluderNTris[i]);

		LARGE_INTEGER t0 = { 0 };
		QueryPerformanceCounter(&t0);
		
		if (!frustumTest(frustum, OccluderCenter + 3 * i, OccluderRadius[i]))
		{
			resultList[i] = (int)MaskedOcclusionCulling::FRUSTUM_CULLED;
			continue;
		}

		LARGE_INTEGER t1 = { 0 };
		QueryPerformanceCounter(&t1);
		test_frustum_cull_time += (t1.QuadPart - t0.QuadPart) * 10000000 / cpuFreq.QuadPart;

		resultList[i] = (int)_moc->RenderTriangles(OccluderInVtx + OccluderVtxIndex[i], OccluderInTris + OccluderTrisIndex[i], OccluderNTris[i], PVMatrix, bfWinding, clipPlaneMask, VertexLayout(16, 4, 12));

		LARGE_INTEGER t2 = { 0 };
		QueryPerformanceCounter(&t2);
		rasterize_time += (t2.QuadPart - t1.QuadPart) * 10000000 / cpuFreq.QuadPart;
	}

	LARGE_INTEGER te = { 0 };
	QueryPerformanceCounter(&te);

	long long rasterizeFullTime = (te.QuadPart - ts.QuadPart) * 10000000 / cpuFreq.QuadPart;

	_moc->SetOccluderTimeStat(rasterizeFullTime, test_frustum_cull_time, rasterize_time);
}

void mocTestObjects(void* moc, const int nObject, float* OccludeeBounds, float* OccludeeCenter,
					float* OccludeeRadius, int* resultList, float* PVMatrix, float* frustum,
					BackfaceWinding bfWinding, ClipPlanes clipPlaneMask)
{
	MaskedOcclusionCulling* _moc = ((MaskedOcclusionCulling*)moc);
	float t[32];
	long long test_frustum_cull_time = 0;
	long long transform_time = 0;
	long long boundingbox_time = 0;
	long long test_time = 0;

	LARGE_INTEGER cpuFreq = { 0 };
	QueryPerformanceFrequency(&cpuFreq);

	LARGE_INTEGER ts = { 0 };
	QueryPerformanceCounter(&ts);

	for (int i = 0; i < nObject; i++) 
	{	
		_moc->AddOccludeeCount(1);

		LARGE_INTEGER t0 = { 0 };
		QueryPerformanceCounter(&t0);

		if(!frustumTest(frustum, OccludeeCenter + 3 * i, OccludeeRadius[i]))
		{
			resultList[i] = (int)MaskedOcclusionCulling::FRUSTUM_CULLED;
			continue;
		}

		LARGE_INTEGER t1 = { 0 };
		QueryPerformanceCounter(&t1);
		test_frustum_cull_time += (t1.QuadPart - t0.QuadPart) * 10000000 / cpuFreq.QuadPart;

		MaskedOcclusionCulling::TransformVector3(PVMatrix, OccludeeBounds + 24 * i, t, 8);
		for(int j=0; j<8; j++)
		{
			MaskedOcclusionCulling::DivW(t + 4 * j);
		}

		LARGE_INTEGER t2 = { 0 };
		QueryPerformanceCounter(&t2);
		transform_time += (t2.QuadPart - t1.QuadPart) * 10000000 / cpuFreq.QuadPart;

		const auto xmin = clamp(mocMin(t[0], t[4], t[8], t[12], t[16], t[20], t[24], t[28]), -1.0f, 1.0f);
		const auto xmax = clamp(mocMax(t[0], t[4], t[8], t[12], t[16], t[20], t[24], t[28]), -1.0f, 1.0f);
		const auto ymin = clamp(mocMin(t[1], t[5], t[9], t[13], t[17], t[21], t[25], t[29]), -1.0f, 1.0f);
		const auto ymax = clamp(mocMax(t[1], t[5], t[9], t[13], t[17], t[21], t[25], t[29]), -1.0f, 1.0f);
		const auto wmin = mocMin(t[3], t[7], t[11], t[15], t[19], t[23], t[27], t[31]);
		const auto wmax = mocMax(t[3], t[7], t[11], t[15], t[19], t[23], t[27], t[31]);

		LARGE_INTEGER t3 = { 0 };
		QueryPerformanceCounter(&t3);
		boundingbox_time += (t3.QuadPart - t2.QuadPart) * 10000000 / cpuFreq.QuadPart;
		
		resultList[i] = (int)_moc->TestRect(xmin, ymin, xmax, ymax, wmin, wmax);

		LARGE_INTEGER t4 = { 0 };
		QueryPerformanceCounter(&t4);
		test_time += (t4.QuadPart - t3.QuadPart) * 10000000 / cpuFreq.QuadPart;
	}

	LARGE_INTEGER te = { 0 };
	QueryPerformanceCounter(&te);

	long long testFullTime = (te.QuadPart - ts.QuadPart) * 10000000 / cpuFreq.QuadPart;

	_moc->SetOccludeeTimeStat(testFullTime, test_frustum_cull_time, transform_time, boundingbox_time, test_time);
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
	result[0] = stats.mOccluders.mNumInputTriangles;
	result[1] = stats.mOccluders.mNumProcessedTriangles;
	result[2] = stats.mOccluders.mNumRasterizedTriangles;
	result[3] = stats.mOccluders.mNumTilesTraversed;
	result[4] = stats.mOccluders.mNumTilesUpdated;
	result[5] = stats.mOccluders.mNumTilesMerged;
	result[6] = stats.mOccluders.rasterize_full_time;
	result[7] = stats.mOccluders.test_frustum_cull_time;
	result[8] = stats.mOccluders.rasterize_time;

	result[9] = stats.mOccludees.mNumInputRectangles;
	result[10] = stats.mOccludees.mNumProcessedRectangles;
	result[11] = stats.mOccludees.mNumProcessedTriangles;
	result[12] = stats.mOccludees.mNumRasterizedTriangles;
	result[13] = stats.mOccludees.mNumTilesTraversed;
	result[14] = stats.mOccludees.test_full_time;
	result[15] = stats.mOccludees.test_frustum_cull_time;
	result[16] = stats.mOccludees.transform_time;
	result[17] = stats.mOccludees.boundingbox_time;
	result[18] = stats.mOccludees.test_time;

}

bool frustumTest(const float* frustum, const float* center, const float radius)
{
	for (int i=0; i<5; i++)
	{
		if (frustum[i * 4] * center[0] + frustum[i * 4 + 1] * center[1] + frustum[i * 4 + 2] * center[2] + frustum[i * 4 + 3] + radius <= 0) return false;
	}
	return true;
}

float mocMin(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8)
{
	float f12 = min(f1, f2);
	float f34 = min(f3, f4);
	float f56 = min(f5, f6);
	float f78 = min(f7, f8);
	float f1234 = min(f12, f34);
	float f5678 = min(f56, f78);
	return min(f1234, f5678);
}

float mocMax(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8)
{
	float f12 = max(f1, f2);
	float f34 = max(f3, f4);
	float f56 = max(f5, f6);
	float f78 = max(f7, f8);
	float f1234 = max(f12, f34);
	float f5678 = max(f56, f78);
	return max(f1234, f5678);
}