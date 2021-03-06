﻿#ifndef PLATFORM_REG_HPP_A1C836DB_556E_41F8_B04C_AF159E265315
#define PLATFORM_REG_HPP_A1C836DB_556E_41F8_B04C_AF159E265315
#pragma once

/*
platform.reg.hpp

*/
/*
Copyright © 2017 Far Group
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

namespace os::reg
{
	class value;

	class key: public conditional<key>
	{
	public:
		key() = default;

		static const key classes_root;
		static const key current_user;
		static const key local_machine;

		static key open(const key& Key, const string_view& SubKey, DWORD SamDesired);

		void close();
		HKEY native_handle() const;

		bool enum_keys(size_t Index, string& Name) const;
		bool enum_values(size_t Index, value& Value) const;

		bool get(const string_view& Name) const;
		bool get(const string_view& Name, string& Value) const;
		bool get(const string_view& Name, unsigned int& Value) const;
		bool get(const string_view& Name, unsigned long long& Value) const;

		template<class T>
		bool get(const string_view& SubKey, const string_view& Name, T& Value, REGSAM Sam = 0) const
		{
			using is_supported_type = is_one_of_t<T, string, unsigned int, unsigned long long>;
			static_assert(is_supported_type::value);

			const auto NewKey = open(*this, SubKey, KEY_QUERY_VALUE | Sam);
			if (!NewKey)
				return false;

			return NewKey.get(Name, Value);
		}

		bool operator!() const;

	private:
		explicit key(HKEY Key);

		struct hkey_deleter
		{
			void operator()(HKEY Key) const;
		};

		std::unique_ptr<std::remove_pointer_t<HKEY>, hkey_deleter> m_Key;
	};

	class value
	{
	public:
		const string& name() const;
		DWORD type() const;
		string get_string() const;
		unsigned int get_unsigned() const;
		unsigned long long get_unsigned_64() const;

	private:
		friend class key;

		string m_Name;
		DWORD m_Type{ REG_NONE };
		const key* m_Key{};
	};

	class enum_key: noncopyable, public enumerator<enum_key, string>
	{
		IMPLEMENTS_ENUMERATOR(enum_key);

	public:
		explicit enum_key(const key& Key);
		enum_key(const key& Key, const string_view& SubKey, REGSAM Sam = 0);

	private:
		bool get(size_t Index, value_type& Value) const;

		key m_Key;
		const key& m_KeyRef;
	};

	class enum_value: noncopyable, public enumerator<enum_value, value>
	{
		IMPLEMENTS_ENUMERATOR(enum_value);

	public:
		explicit enum_value(const key& Key);
		enum_value(const key& Key, const string_view& SubKey, REGSAM Sam = 0);

	private:
		bool get(size_t Index, value_type& Value) const;

		key m_Key;
		const key& m_KeyRef;
	};
}

#endif // PLATFORM_REG_HPP_A1C836DB_556E_41F8_B04C_AF159E265315
