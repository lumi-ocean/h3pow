#include "pch.h"
#include "asset.h"

#include "patcher.h"
#include "package.h"

#include <h3api/h3api.hpp>

fstorage<Asset> all_assets;
flist<fstring> asset_extensions;

std::vector<std::weak_ptr<LoadedFile::FileData>> LoadedFile::all_file_data;

LoadedFile::LoadedFile(Asset* asset) : m_data()
{
	for (auto& file_data : all_file_data)
	{
		auto data = file_data.lock();
		if (data)
		{
			if (data->name == asset->id)
			{
				m_data = data;
				return;
			}
			break;
		}
		else
		{
			std::vector<std::weak_ptr<LoadedFile::FileData>> new_data;
			for (auto& file_data_1 : all_file_data)
			{
				if (!file_data_1.expired())
				{
					new_data.push_back(file_data_1);
				}
			}
			all_file_data = new_data;
			break;
		}
	}

	std::ifstream file(asset->filename.c_str(), std::ios_base::in | std::ios_base::binary);

	file.seekg(0, std::ios::end);
	int size = (int)file.tellg();
	file.seekg(0, std::ios::beg);

	m_data.reset(new FileData(asset->id, size));
	file.read(reinterpret_cast<char*>(m_data->data), size);

	all_file_data.push_back(m_data);
}

Asset* GetAsset(fstring const& name)
{
	fstring h3name = name.lower();
	Asset* asset = all_assets.by_id[h3name];
	if (asset != nullptr)
	{
		return asset;
	}
	else if (h3name.starts_with("pw"))
	{
		int num = h3name.substring(2, h3name.rfind(".") - 2).parse_int();
		return all_assets.by_native_slot[num];
	}
	else
	{
		return nullptr;
	}
}

u8* CreateBufferFromFile(fstring const& name)
{
	Asset* asset = GetAsset(name);
	if (asset != nullptr)
	{
		LoadedFile file(asset);
		u8* buffer = (u8*)h3::H3Malloc(file.size());
		memcpy(buffer, file.data(), file.size());
		return buffer;
	}
	else
	{
		return nullptr;
	}
}

struct
{
	int slot = -1;
	flist<fstring> m;
} current_def;

struct GameAssetHooks : Hooks
{
	LOHOOK(0x55CA26)
	{
		u8* buffer = CreateBufferFromFile(ebx);
		if (buffer != nullptr)
		{
			*reinterpret_cast<u8**>(context->ebp - 4 * 8) = buffer;
			edi = reinterpret_cast<int>(buffer);
			context->return_address = 0x55CB64;
			exec = false;

			current_def.slot = GetAsset(ebx)->native_slot;
			current_def.m.clear();
		}
		else
		{
			current_def.slot = -1;
		}
	}

	LOHOOK(0x55CDC4)
	{
		if (current_def.slot >= 0)
		{
			fstring s = fstring(eax).lower();
			int index = -1;
			for (int i = 0; i < int(current_def.m.count()); i++)
			{
				if (current_def.m[i] == s)
				{
					index = i;
					break;
				}
			}
			if (index < 0)
			{
				index = current_def.m.count();
				current_def.m.add(s);
			}
			eax = fstring("pw") + std::to_string(current_def.slot) + "." + std::to_string(index);
		}
	}
};

void AddAssetsRecursive(std::filesystem::directory_entry const& dir)
{
	if (!dir.is_directory()) return;

	for (auto const& entry : std::filesystem::directory_iterator(dir))
	{
		if (entry.is_regular_file())
		{
			fstring ext = fstring(entry.path().extension().string()).substring(1).lower();
			for (fstring const& asset_extension : asset_extensions)
			{
				if (ext == asset_extension)
				{
					Asset* asset = all_assets.create(fstring(entry.path().filename().string()).lower());
					asset->filename = entry.path().string();
				}
			}
			if (ext == "zip")
			{
				// todo: zip support
			}
			else if (ext == "lod" || ext == "pac")
			{
				// todo: lod support
			}
			else if (ext == "snd")
			{
				// todo: snd support
			}
			else if (ext == "vid")
			{
				// todo: vid support
			}
		}

		AddAssetsRecursive(entry);
	}
}

void InitializeAssets()
{
	asset_extensions.add("def");
	asset_extensions.add("wav");
	asset_extensions.add("mp3");

	if (exe_version != ExeVersion::pow)
	{
		asset_extensions.add("txt");
		asset_extensions.add("msk");
	}

	for (Package const& package : all_packages)
	{
		AddAssetsRecursive(std::filesystem::directory_entry(package.dir / "assets"));
	}

	if (exe_type == ExeType::main && exe_version == ExeVersion::pow)
	{
		GameAssetHooks hooks;
		hooks.Write();
	}
}
