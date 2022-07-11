#pragma once

#include "core.h"

class Asset : public fclass
{
	F_CLASS(Asset) {}

	F_PROPERTY(filename, fstring) {}
};

extern fstorage<Asset> all_assets;

class LoadedFile
{
private:
	struct FileData
	{
		fstring name;
		int size;
		u8* data;

		explicit FileData(fstring const& name, int size) : name(name), size(size), data(new u8[size]) {}
		~FileData() { delete[] data; }
	};

	std::shared_ptr<FileData> m_data;

	static std::vector<std::weak_ptr<FileData>> all_file_data;

public:
	explicit LoadedFile(Asset* asset);

	int size() const { return m_data->size; }
	u8* data() const { return m_data->data; }
};

void InitializeAssets();
