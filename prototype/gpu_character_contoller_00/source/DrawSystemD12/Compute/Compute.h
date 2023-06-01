#pragma once

#include "DrawSystemD12/IResource.h"

struct ConstantBufferInfo;
struct ShaderResourceInfo;
struct ConstantBuffer;
struct UnorderedAccessInfo;
class HeapWrapperItem;

class Compute : public IResource
{
public:
	Compute(
		DrawSystem* const pDrawSystem,
		const std::shared_ptr< std::vector<uint8_t> >& pShaderData
		);
	virtual ~Compute();

	void SetDebugName( const std::string& name );

	void Dispatch(
		ID3D12GraphicsCommandList* const pCommandList,
		const int threadGroupCountX,
		const int threadGroupCountY,
		const int threadGroupCountZ
		);

private:
	virtual void OnDeviceLost() override;

	virtual void OnDeviceRestored(
		ID3D12GraphicsCommandList* const,
		ID3D12Device2* const pDevice
		) override;

private:
	std::shared_ptr< std::vector<uint8_t> > m_pShaderData;

	std::vector< std::shared_ptr< ConstantBuffer > > m_arrayConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

	std::string m_debugName;
};
