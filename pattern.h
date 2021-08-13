#pragma once

namespace pattern {
	// container
	using patterns_t     = std::vector< Address >;
	using pattern_byte_t = std::pair< uint8_t, bool >;

    static Address find( Address start, size_t len, const std::string &pat ) {
		uint8_t						  *scan_start, *scan_end;
		std::vector< pattern_byte_t > pattern{};
		std::stringstream			  stream{ pat };
		std::string				      w;

		if( !start || !len || pat.empty( ) )
			return{};

		// split spaces and convert to hex.
		while( stream >> w ) {
			// wildcard.
			if( w[ 0 ] == '?' )
				pattern.push_back( { 0, true } );

			// valid hex digits.
			else if( std::isxdigit( w[ 0 ] ) && std::isxdigit( w[ 1 ] ) )
				pattern.push_back( { ( uint8_t )std::strtoul( w.data( ), 0, 16 ), false } );
		}

		scan_start = start.as< uint8_t* >( );
		scan_end   = scan_start + len;

		// find match.
		auto result = std::search( scan_start, scan_end, pattern.begin( ), pattern.end( ),
		[]( const uint8_t b, const pattern_byte_t& p ) {
			// byte matches or it's a wildcard.
			return b == p.first || p.second;
		} );

		// nothing found.
		if( result == scan_end )
			return{};

		return ( uintptr_t )result;
	}

    static Address find( const PE::Module &module, const std::string &pat ) {
		return find( module.GetBase( ), module.GetImageSize( ), pat );
	}

    static patterns_t FindAll( Address start, size_t len, const std::string &pat ) {
		patterns_t out{};
		Address	   result;

		for( ;; ) {
			// find result.
			result = find( start, len, pat );
			if( !result )
				break;
			
			// if we arrived here we found something.
			out.push_back( result );

			// set new len.
			len = ( start + len ) - ( result + 1 );

			// new start point.
			start = result + 1;
		}

		return out;
	}

    static patterns_t FindAll( const PE::Module &module, const std::string &pat ) {
		return FindAll( module.GetBase( ), module.GetImageSize( ), pat );
	}

	static uintptr_t get_sig(std::string module_name, std::string pattern)
	{
		static auto pattern_to_byte = [](const char* pattern)
		{
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else
				{
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		const auto module = GetModuleHandleA(module_name.c_str());

		if (module)
		{
			const auto dosHeader = PIMAGE_DOS_HEADER(module);
			const auto ntHeaders = PIMAGE_NT_HEADERS(reinterpret_cast<std::uint8_t*>(module) + dosHeader->e_lfanew);

			const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
			auto patternBytes = pattern_to_byte(pattern.c_str());
			const auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

			const auto s = patternBytes.size();
			const auto d = patternBytes.data();

			for (auto i = 0ul; i < sizeOfImage - s; ++i)
			{
				auto found = true;
				for (auto j = 0ul; j < s; ++j)
				{
					if (scanBytes[i + j] != d[j] && d[j] != -1)
					{
						found = false;
						break;
					}
				}

				if (found)
					return uintptr_t(&scanBytes[i]);
			}
		}

		return 0;
	}
}