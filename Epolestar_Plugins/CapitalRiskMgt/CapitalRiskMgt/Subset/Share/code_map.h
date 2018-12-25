#pragma once

#include <map>
#include "trade_struct.h"
#include "load_rc.h"
//#include "..\..\Include\pluginmgr_api.h"
#include "..\..\Include\PluginMgr_API.h"

#define CODE_MAP_FILE			"config\\tradeapi.codemapping.pub"
#define FIELD_SPLIT_STR			"\",\""

class CodeMapping
{
private:
	struct TCodeMapping
	{
		char			ContractNo[56];
		double			PriceMultiple;
	};

	typedef map<string, TCodeMapping>		mapCode;
	mapCode				m_to_local_mapping;	//远程品种到本地品种的映射
	mapCode				m_to_peer_mapping;	//本地品种到远程品种的映射

private:
	void load_param(const char * file_name, const char * plugin_name, const char * broker_no)
	{
		FILE * file = _fsopen(file_name, "rbS", _SH_DENYWR);
		if (!file)
			return;	
		fseek(file, 0, SEEK_SET);


		TCodeMapping data;
		const char * peer_code;
		const char * local_code;
		double price_factor;
		char line[MAX_PATH];
		while (fgets(line, sizeof(line), file) != nullptr)
		{
			StrFun::Trim(line, '"');
			if (line[0] == 0 || line[0] == '#')
				continue;

			peer_code = nullptr;
			local_code = nullptr;
			price_factor = 1;

			int col	 = 0;
			char * p = line;
			char * q = p;
			while (p && *p)
			{
				p = strstr(p, FIELD_SPLIT_STR);
				if (p)
				{
					*p = 0;
					p += strlen(FIELD_SPLIT_STR);
				}

				switch (col++)
				{
					case 0: if (strcmp(plugin_name, q)) *p = 0; break;
					case 1: if (strcmp(broker_no, q))	*p = 0; break;
					case 2: peer_code = q;						break;
					case 3: local_code = q;						break;
					case 4: price_factor = atof(q);				break;
				}
				q = p;
			}

			if (peer_code && local_code)
			{
				strncpy_s(data.ContractNo	, local_code);
				data.PriceMultiple			= price_factor;
				m_to_local_mapping.insert(mapCode::value_type(string(peer_code), data));

				strncpy_s(data.ContractNo	, peer_code);
				data.PriceMultiple			= 1 / price_factor;
				m_to_peer_mapping.insert(mapCode::value_type(string(local_code), data));
			}
		}

		fclose(file);
	}
public:
	~CodeMapping()
	{
		m_to_local_mapping.clear();
		m_to_peer_mapping.clear();
	}

	void init(const char * plugin_name, const char * broker_no)
	{
		m_to_local_mapping.clear();
		m_to_peer_mapping.clear();

		string map_file = FileFun::GetAbsolutePathForExe(CODE_MAP_FILE);
		if (PathFileExistsA(map_file.c_str()))
			load_param(map_file.c_str(), plugin_name, broker_no);
	}

	template<size_t _size>
	double to_local_commodity(char (&code)[_size], const char * peer_code)
	{
		code[0] = 0;
		mapCode::iterator it = m_to_local_mapping.find(string(peer_code));
		if (it == m_to_local_mapping.end())
			return 1;
		else
		{
			strncpy_s(code, it->second.ContractNo);
			return it->second.PriceMultiple;
		}
	}

	template<size_t _size>
	double to_peer_commodity(char (&code)[_size], const char * local_code)
	{
		code[0] = 0;
		mapCode::iterator it = m_to_peer_mapping.find(string(local_code));
		if (it == m_to_peer_mapping.end())
			return 1;
		else
		{
			strncpy_s(code, it->second.ContractNo);
			return it->second.PriceMultiple;
		}
	}

	template<size_t _size>
	double to_local_commodity(char(&local_exchange)[_size], char(&local_commodity)[_size], const char * exchangeno, const char * commodityno, const char commodity_type)
	{
		string peer;
		StrFun::format(peer, "%s|%c|%s", exchangeno, commodity_type, commodityno);
		char local[MAX_PATH];
		double ret = to_local_commodity(local, peer.c_str());

		if (local[0] == 0)
		{
			strncpy_s(local_exchange, exchangeno);
			strncpy_s(local_commodity, commodityno);
		}
		else
		{
			char * p = strrchr(local, '|');
			strncpy_s(local_commodity, ++p);
			p = strchr(local, '|');
			*p = 0;
			strncpy_s(local_exchange, local);
		}
		return ret;
	}
	template<size_t _size>
	double to_peer_commodity(char(&peer_commodity)[_size], const char * exchangeno, const char * commodityno, const char commodity_type)
	{
		string local;
		StrFun::format(local, "%s|%c|%s", exchangeno, commodity_type, commodityno);
		char peer[MAX_PATH];
		double ret = to_peer_commodity(peer, local.c_str());

		if (peer[0] == 0)
			strncpy_s(peer_commodity, commodityno);
		else
		{
			const char * p = strrchr(peer, '|');
			strncpy_s(peer_commodity, ++p);
		}
		return ret;
	}
	template<size_t _size>
	double to_peer_commodity(char(&peer_exchange)[_size], char(&peer_commodity)[_size], const char * exchangeno, const char * commodityno, const char commodity_type)
	{
		string local;
		StrFun::format(local, "%s|%c|%s", exchangeno, commodity_type, commodityno);
		char peer[MAX_PATH];
		double ret = to_peer_commodity(peer, local.c_str());

		if (peer[0] == 0)
		{
			strncpy_s(peer_exchange, exchangeno);
			strncpy_s(peer_commodity, commodityno);
		}
		else
		{
			char * p = strrchr(peer, '|');
			strncpy_s(peer_commodity, ++p);
			p = strchr(peer, '|');
			*p = 0;
			strncpy_s(peer_exchange, peer);
		}
		return ret;
	}
	double to_local_factor(const char * exchangeno, const char * commodityno, const char commodity_type)
	{
		string peer;
		StrFun::format(peer, "%s|%c|%s", exchangeno, commodity_type, commodityno);
		char local[MAX_PATH];
		return to_local_commodity(local, peer.c_str());
	}
	double to_peer_factor(const char * exchangeno, const char * commodityno, const char commodity_type)
	{
		string local;
		StrFun::format(local, "%s|%c|%s", exchangeno, commodity_type, commodityno);
		char peer[MAX_PATH];
		return to_peer_commodity(peer, local.c_str());
	}
};