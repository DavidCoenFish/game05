#pragma once

struct ConstantBufferInfo
{
public:
	template < class TYPE >
	static std::shared_ptr<ConstantBufferInfo> Factory(
		const TYPE& in_data,
		const D3D12_SHADER_VISIBILITY in_visiblity = D3D12_SHADER_VISIBILITY_ALL
		)
	{
		const auto size = sizeof(TYPE);
		std::vector<uint8_t> data(size);
		for(int index = 0; index < size; ++index)
		{
			data[index] = ((uint8_t*)(&in_data))[index];
		}
		return std::make_shared<ConstantBufferInfo>(data, in_visiblity);
	}

	explicit ConstantBufferInfo(
		const std::vector<uint8_t>& in_data = std::vector<uint8_t>(),
		const D3D12_SHADER_VISIBILITY in_visiblity = D3D12_SHADER_VISIBILITY_ALL
		);

	const D3D12_SHADER_VISIBILITY GetVisiblity() const
	{
		return _visiblity;
	}

public:
	// Public for json reflection
	std::vector<uint8_t> _data;

private: 
	D3D12_SHADER_VISIBILITY _visiblity;

};
