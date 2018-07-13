#pragma once
#include <wrl.h>

namespace Rendering
{
	class Query
	{
	public:
		Query(ID3D11Device2* device);
		~Query() = default;

		void BeginQuery(ID3D11DeviceContext2* deviceContext);
		void EndQuery(ID3D11DeviceContext2* deviceContext);
		float GetQueryTime(ID3D11DeviceContext2* deviceContext);

		static void InitializeDisjointQuery(ID3D11Device2* device);
		static void GetDisjointTimeStamp(ID3D11DeviceContext2* deviceContext);
		static void SwapQueryBuffers();
		static void BeginDisjointQuery(ID3D11DeviceContext2* deviceContext);
		static void EndDisjointQuery(ID3D11DeviceContext2* deviceContext);

	private:
		Microsoft::WRL::ComPtr<ID3D11Query> QueryObjectBegin[2];
		Microsoft::WRL::ComPtr<ID3D11Query> QueryObjectEnd[2];

		static Microsoft::WRL::ComPtr<ID3D11Query> QueryDisjoint[2];
		static D3D11_QUERY_DATA_TIMESTAMP_DISJOINT TimeStampDisjoint[2];

		static int8_t CurrentQuery;
		static int8_t CurrentCollection;
		static uint64_t FrameCount;
	};
}