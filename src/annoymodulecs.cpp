// dllmain.cpp : Defines the entry point for the DLL application.
#include <cstring>
#include <typeinfo>
#include "annoylib.h"
#include "kissrandom.h"
#define __cdecl __attribute__((__cdecl__))

#if defined(_MSC_VER) && _MSC_VER == 1500
typedef signed __int32    int32_t;
#else
#include <stdint.h>
#endif

#if defined(ANNOYLIB_USE_AVX512)
#define AVX_INFO "Using 512-bit AVX instructions"
#elif defined(ANNOYLIB_USE_AVX128)
#define AVX_INFO "Using 128-bit AVX instructions"
#else
#define AVX_INFO "Not using AVX instructions"
#endif

#if defined(_MSC_VER)
#define COMPILER_INFO "Compiled using MSC"
#define EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define COMPILER_INFO "Compiled on GCC"
#define EXPORT __attribute__((visibility("default")))
#else
#define COMPILER_INFO "Compiled on unknown platform"
#endif

#define ANNOY_DOC (COMPILER_INFO ". " AVX_INFO ".")




extern "C"
{
	using namespace Annoy;

	EXPORT AnnoyIndexInterface<int32_t, float>* Init(int32_t f, int32_t distType, const char* path)
	{
		AnnoyIndexInterface<int32_t, float>* index = nullptr;

		switch (distType)
		{
		case 0:
		{
			index = new AnnoyIndex<int32_t, float, Angular, Kiss64Random, AnnoyIndexSingleThreadedBuildPolicy>(f);
		}break;
		case 1:
		{
			index = new AnnoyIndex<int32_t, float, DotProduct, Kiss64Random, AnnoyIndexSingleThreadedBuildPolicy>(f);
		}break;
		case 2:
		{
			index = new AnnoyIndex<int32_t, float, Euclidean, Kiss64Random, AnnoyIndexSingleThreadedBuildPolicy>(f);
		}break;
		case 3:
		{
			index = new AnnoyIndex<int32_t, float, Manhattan, Kiss64Random, AnnoyIndexSingleThreadedBuildPolicy>(f);
		}break;
		default:
		{
			index = new AnnoyIndex<int32_t, float, Angular, Kiss64Random, AnnoyIndexSingleThreadedBuildPolicy>(f);
		}break;
		};

		if (path != nullptr)
		{
			index->load(path);
		}

		return index;
	}

	EXPORT void AddItem(AnnoyIndexInterface<int32_t, float>* index, int32_t item, const float* w)
	{
		index->add_item(item, w);
	}

	EXPORT void BuildAndSave(AnnoyIndexInterface<int32_t, float>* index, const char* path, const int trees)
	{
		index->build(trees);
		index->save(path);
	}

	EXPORT void NNS_Search(AnnoyIndexInterface<int32_t, float>* index, const float* w, const int32_t n, int32_t* res, float* dists)
	{
		std::vector<int32_t>* result = new std::vector<int32_t>();
		std::vector<float>* distances = new std::vector<float>();

		index->get_nns_by_vector(w, n, -1, result, distances);

		int32_t* const arrayRes = new int32_t[n];
		std::copy(result->begin(), result->end(), arrayRes);

		float* const arrayDist = new float[n];
		std::copy(distances->begin(), distances->end(), arrayDist);

		for (int i = 0; i < n; i++)
		{
			res[i] = arrayRes[i];
			dists[i] = arrayDist[i];
		}

		delete[] arrayDist;
		delete[] arrayRes;
	}

	EXPORT void NNS_SearchByItem(AnnoyIndexInterface<int32_t, float>* index, int32_t item, int32_t n, int32_t* res, float* dists)
	{
		std::vector<int32_t>* result = new std::vector<int32_t>();
		std::vector<float>* distances = new std::vector<float>();

		index->get_nns_by_item(item, n, -1, result, distances);

		int32_t* const arrayRes = new int32_t[n];
		std::copy(result->begin(), result->end(), arrayRes);

		float* const arrayDist = new float[n];
		std::copy(distances->begin(), distances->end(), arrayDist);

		for (int i = 0; i < n; i++)
		{
			res[i] = arrayRes[i];
			dists[i] = arrayDist[i];
		}

		delete[] arrayDist;
		delete[] arrayRes;
	}

	EXPORT void GetItemVector(AnnoyIndexInterface<int32_t, float>* index, int32_t item, float* vector)
	{
		(*index).get_item(item, vector);
	}

	EXPORT void GetDistance(AnnoyIndexInterface<int32_t, float>* index, int32_t itemA, int32_t itemB, float* distance)
	{
		*distance = (*index).get_distance(itemA, itemB);
	}

	EXPORT int ReleaseMemory(AnnoyIndexInterface<int32_t, float>* index)
	{
		index->~AnnoyIndexInterface();
		delete[] index;
		return 0;
	}
}


