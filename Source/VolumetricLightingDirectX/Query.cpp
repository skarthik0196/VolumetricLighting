#include "pch.h"
#include "Query.h"

namespace Rendering
{
	int8_t Query::CurrentCollection = -1;
	int8_t Query::CurrentQuery = 0;
	uint64_t Query::FrameCount = 0;
	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT Query::TimeStampDisjoint[2];
	Microsoft::WRL::ComPtr<ID3D11Query> Query::QueryDisjoint[2];

	Query::Query(ID3D11Device2* device)
	{
		D3D11_QUERY_DESC queryDescription;
		ZeroMemory(&queryDescription, sizeof(D3D11_QUERY_DESC));

		queryDescription.Query = D3D11_QUERY_TIMESTAMP;

		device->CreateQuery(&queryDescription, QueryObjectBegin[0].ReleaseAndGetAddressOf());
		device->CreateQuery(&queryDescription, QueryObjectBegin[1].ReleaseAndGetAddressOf());

		device->CreateQuery(&queryDescription, QueryObjectEnd[0].ReleaseAndGetAddressOf());
		device->CreateQuery(&queryDescription, QueryObjectEnd[1].ReleaseAndGetAddressOf());
	}

	void Query::BeginQuery(ID3D11DeviceContext2 * deviceContext)
	{
		deviceContext->End(QueryObjectBegin[CurrentQuery].Get());
	}

	void Query::EndQuery(ID3D11DeviceContext2 * deviceContext)
	{
		deviceContext->End(QueryObjectEnd[CurrentQuery].Get());
	}

	float Query::GetQueryTime(ID3D11DeviceContext2* deviceContext)
	{
		if (FrameCount < 1 || CurrentCollection < 0)
		{
			return 0;
		}
		
		if (TimeStampDisjoint[CurrentCollection].Disjoint)
		{
			return 0;
		}

		uint64_t beginTime, endTime;
		deviceContext->GetData(QueryObjectBegin[CurrentCollection].Get(), &beginTime, sizeof(uint64_t), 0);
		deviceContext->GetData(QueryObjectEnd[CurrentCollection].Get(), &endTime, sizeof(uint64_t), 0);

		float msTime = static_cast<float>(endTime - beginTime) / static_cast<float>(TimeStampDisjoint[CurrentCollection].Frequency) * 1000.0f;

		return msTime;
	}

	void Query::InitializeDisjointQuery(ID3D11Device2* device)
	{
		D3D11_QUERY_DESC queryDescription;
		ZeroMemory(&queryDescription, sizeof(D3D11_QUERY_DESC));

		queryDescription.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;

		device->CreateQuery(&queryDescription, QueryDisjoint[0].ReleaseAndGetAddressOf());
		device->CreateQuery(&queryDescription, QueryDisjoint[1].ReleaseAndGetAddressOf());

		ZeroMemory(&TimeStampDisjoint, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT));
	}

	void Query::GetDisjointTimeStamp(ID3D11DeviceContext2 * deviceContext)
	{
		if (FrameCount < 1 || CurrentCollection < 0)
		{
			return;
		}

		while (deviceContext->GetData(QueryDisjoint[CurrentCollection].Get(), NULL, 0, 0) == S_FALSE)
		{
			Sleep(1);
		}

		deviceContext->GetData(QueryDisjoint[CurrentCollection].Get(), &TimeStampDisjoint[CurrentCollection], sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT), 0);
	}

	void Query::SwapQueryBuffers()
	{
		++CurrentCollection &= 1;
		++CurrentQuery &= 1;
		++FrameCount;
	}

	void Query::BeginDisjointQuery(ID3D11DeviceContext2 * deviceContext)
	{
		deviceContext->Begin(QueryDisjoint[CurrentQuery].Get());
	}

	void Query::EndDisjointQuery(ID3D11DeviceContext2 * deviceContext)
	{
		deviceContext->End(QueryDisjoint[CurrentQuery].Get());
	}
}