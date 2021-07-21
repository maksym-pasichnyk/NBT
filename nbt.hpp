#pragma once

#include <optional>
#include <variant>
#include <vector>
#include <string>
#include <array>
#include <span>
#include <list>
#include <map>
#include <bit>

struct Tag;

struct EndTag {};

struct ByteTag {
	int8_t value;
};

struct ShortTag {
	int16_t value;
};

struct IntTag {
	int32_t value;
};

struct LongTag {
	int64_t value;
};

struct FloatTag {
	float value;
};

struct DoubleTag {
	double value;
};

struct StringTag {
	std::string value;
};

template <typename T>
struct ArrayTag {
	ArrayTag() = default;
	explicit ArrayTag(std::vector<T> value) : value(std::move(value)) {}

	size_t size() const {
		return value.size();
	}

	size_t capacity() const {
		return value.capacity();
	}

	void reserve(size_t count) {
		value.reserve(count);
	}

	void resize(size_t size) {
		value.resize(size);
	}

	void clear() {
		value.clear();
	}

	T& at(size_t i) {
		return value.at(i);
	}

	const T& at(size_t i) const {
		return value.at(i);
	}

	T& operator[](size_t i) {
		return value[i];
	}

	const T& operator[](size_t i) const {
		return value[i];
	}

	auto begin() {
		return value.begin();
	}

	auto begin() const {
		return value.begin();
	}

	auto end() {
		return value.end();
	}

	auto end() const {
		return value.end();
	}

	auto rbegin() {
		return value.rbegin();
	}

	auto rbegin() const {
		return value.rbegin();
	}

	auto rend() {
		return value.end();
	}

	auto rend() const {
		return value.rend();
	}

	template <typename... Args>
	void emplace_back(Args&&... args) {
		value.emplace_back(std::forward<Args>(args)...);
	}

private:
	std::vector<T> value;
};

using ByteArrayTag = ArrayTag<int8_t>;
using IntArrayTag = ArrayTag<int32_t>;
using LongArrayTag = ArrayTag<int64_t>;

struct CompoundTag {
	CompoundTag() = default;
	explicit CompoundTag(std::map<std::string, Tag> value) : value(std::move(value)) {}

	auto size() const {
		return value.size();
	}

	void clear() {
		value.clear();
	}

	auto begin() {
		return value.begin();
	}

	auto begin() const {
		return value.begin();
	}

	auto end() {
		return value.end();
	}

	auto end() const {
		return value.end();
	}

	template <typename T>
	void emplace(std::string name, T&& tag) {
		value.emplace(std::move(name), std::forward<T>(tag));
	}

	Tag& at(const std::string& name) {
		return value.at(name);
	}

	const Tag& at(const std::string& name) const {
		return value.at(name);
	}

	auto erase(const std::string& name) {
		return value.erase(name);
	}

	auto find(const std::string& name) {
		return value.find(name);
	}

	auto find(const std::string& name) const {
		return value.find(name);
	}

	bool contains(const std::string& name) const {
		return value.contains(name);
	}

private:
	std::map<std::string, Tag> value;
};

struct ListTag {
	ListTag() = default;
	explicit ListTag(std::list<Tag> value) : value(std::move(value)) {}

	auto size() const {
		return value.size();
	}

	void clear() {
		value.clear();
	}

	auto begin() {
		return value.begin();
	}

	auto begin() const {
		return value.begin();
	}

	auto end() {
		return value.end();
	}

	auto end() const {
		return value.end();
	}

	template <typename... Args>
	void emplace_back(Args&&... args) {
		value.emplace_back(std::forward<Args>(args)...);
	}

private:
	std::list<Tag> value;
};

struct Tag : std::variant<
	EndTag,
	ByteTag,
	ShortTag,
	IntTag,
	LongTag,
	FloatTag,
	DoubleTag,
	StringTag,
	ByteArrayTag,
	IntArrayTag,
	LongArrayTag,
	ListTag,
	CompoundTag
> {
	using variant::variant;
};

struct NBTFile {
	enum class ID {
		END = 0,
		BYTE = 1,
		SHORT = 2,
		INT = 3,
		LONG = 4,
		FLOAT = 5,
		DOUBLE = 6,
		BYTE_ARRAY = 7,
		STRING = 8,
		LIST = 9,
		COMPOUND = 10,
		INT_ARRAY = 11,
		LONG_ARRAY = 12,
	};
	
	explicit NBTFile(std::span<const std::byte> data) : data(data) {}

	template <typename T>
	std::optional<T> readTag() = delete;

	std::optional<int8_t> readI8() {
		if (pos > data.size()) {
			return std::nullopt;
		}
		return std::bit_cast<int8_t>(data[pos++]);
	}

	std::optional<int16_t> readI16() {
		if (pos + 2 > data.size()) {
			return std::nullopt;
		}

		std::array<std::byte, 2> bytes{};
		bytes[1] = data[pos++];
		bytes[0] = data[pos++];
		return std::bit_cast<int16_t>(bytes);
	}

	std::optional<int32_t> readI32() {
		if (pos + 4 > data.size()) {
			return std::nullopt;
		}

		std::array<std::byte, 4> bytes{};
		bytes[3] = data[pos++];
		bytes[2] = data[pos++];
		bytes[1] = data[pos++];
		bytes[0] = data[pos++];
		return std::bit_cast<int32_t>(bytes);
	}

	std::optional<int64_t> readI64() {
		if (pos + 8 > data.size()) {
			return std::nullopt;
		}

		std::array<std::byte, 8> bytes{};
		bytes[7] = data[pos++];
		bytes[6] = data[pos++];
		bytes[5] = data[pos++];
		bytes[4] = data[pos++];
		bytes[3] = data[pos++];
		bytes[2] = data[pos++];
		bytes[1] = data[pos++];
		bytes[0] = data[pos++];
		return std::bit_cast<int64_t>(bytes);
	}

	std::optional<float> readF32() {
		if (auto i32 = readI32()) {
			return std::bit_cast<float>(*i32);
		}
		return std::nullopt;
	}

	std::optional<double> readF64() {
		if (auto i64 = readI64()) {
			return std::bit_cast<double>(*i64);
		}
		return std::nullopt;
	}

	std::optional<ID> readID() {
		if (auto i8 = readI8()) {
			return static_cast<ID>(*i8);
		}
		return std::nullopt;
	}

	std::optional<std::string> readString() {
		if (auto size = readI16()) {
			if (pos + *size > data.size()) {
				return std::nullopt;
			}
			const auto bytes = data.subspan(pos, static_cast<size_t>(*size));
			pos += bytes.size();

			return std::string(reinterpret_cast<const char*>(bytes.data()), bytes.size());
		}
		return std::nullopt;

	}

	template <>
	std::optional<ByteTag> readTag() {
		if (auto i8 = readI8()) {
			return ByteTag{ *i8 };
		}
		return std::nullopt;
	}

	template <>
	std::optional<ShortTag> readTag() {
		if (auto i16 = readI16()) {
			return ShortTag{ *i16 };
		}
		return std::nullopt;
	}

	template <>
	std::optional<IntTag> readTag() {
		if (auto i32 = readI32()) {
			return IntTag{ *i32 };
		}
		return std::nullopt;
	}

	template <>
	std::optional<LongTag> readTag() {
		if (auto i64 = readI64()) {
			return LongTag{ *i64 };
		}
		return std::nullopt;
	}

	template <>
	std::optional<FloatTag> readTag() {
		if (auto f32 = readF32()) {
			return FloatTag{ *f32 };
		}
		return std::nullopt;
	}

	template <>
	std::optional<DoubleTag> readTag() {
		if (auto f64 = readF64()) {
			return DoubleTag{ *f64 };
		}
		return std::nullopt;
	}

	template <>
	std::optional<StringTag> readTag() {
		if (auto str = readString()) {
			return StringTag{ std::move(*str) };
		}
		return std::nullopt;
	}

	template <>
	std::optional<ListTag> readTag() {
		const auto id = readID();
		if (!id.has_value()) {
			return std::nullopt;
		}
		switch (*id) {
			case ID::END:
				return readListTag<EndTag>();
			case ID::BYTE:
				return readListTag<ByteTag>();
			case ID::SHORT:
				return readListTag<ShortTag>();
			case ID::INT:
				return readListTag<IntTag>();
			case ID::LONG:
				return readListTag<LongTag>();
			case ID::FLOAT:
				return readListTag<FloatTag>();
			case ID::DOUBLE:
				return readListTag<DoubleTag>();
			case ID::BYTE_ARRAY:
				return readListTag<ByteArrayTag>();
			case ID::STRING:
				return readListTag<StringTag>();
			case ID::LIST:
				return readListTag<ListTag>();
			case ID::COMPOUND:
				return readListTag<CompoundTag>();
			case ID::INT_ARRAY:
				return readListTag<IntArrayTag>();
			case ID::LONG_ARRAY:
				return readListTag<LongArrayTag>();
			default:
				return std::nullopt;
		}
	}

	template <typename T>
	std::optional<ListTag> readListTag() {
		if (auto len = readI32()) {
			const auto size = static_cast<size_t>(*len);

			ListTag list{};
			for (size_t i = 0; i < size; ++i) {
				if (auto v = readTag<T>()) {
					list.emplace_back(std::move(*v));
				} else {
					return std::nullopt;
				}
			}
			return list;
		}
		return std::nullopt;
	}

	template <>
	std::optional<ByteArrayTag> readTag() {
		if (auto len = readI32()) {
			const auto size = static_cast<size_t>(*len);

			ByteArrayTag array{};
			array.reserve(size);
			for (size_t i = 0; i < size; ++i) {
				if (auto v = readI8()) {
					array.emplace_back(*v);
				} else {
					return std::nullopt;
				}
			}
			return array;
		}
		return std::nullopt;
	}

	template <>
	std::optional<IntArrayTag> readTag() {
		if (auto len = readI32()) {
			const auto size = static_cast<size_t>(*len);

			IntArrayTag array{};
			array.reserve(size);
			for (size_t i = 0; i < size; ++i) {
				if (auto v = readI32()) {
					array.emplace_back(*v);
				} else {
					return std::nullopt;
				}
			}
			return array;
		}
		return std::nullopt;
	}

	template <>
	std::optional<LongArrayTag> readTag() {
		if (auto len = readI32()) {
			const auto size = static_cast<size_t>(*len);

			LongArrayTag array{};
			array.reserve(size);
			for (size_t i = 0; i < size; ++i) {
				if (auto v = readI64()) {
					array.emplace_back(*v);
				} else {
					return std::nullopt;
				}
			}
			return array;
		}
		return std::nullopt;
	}

	template <>
	std::optional<CompoundTag> readTag() {
		CompoundTag ct{};

		auto emplace = [this]<typename T>(CompoundTag& ct, std::string name, std::optional<T>(NBTFile::*read)()) -> bool {
			if (auto tag = (this->*read)()) {
				ct.emplace(std::move(name), std::move(*tag));
				return true;
			}
			return false;
		};

		while (true) {
			auto id = readID();
			if (!id.has_value()) {
				return std::nullopt;
			}
			if (*id == ID::END) {
				return ct;
			}
			auto name = readString();
			if (!name.has_value()) {
				return std::nullopt;
			}
			switch (*id) {
				case ID::BYTE:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<ByteTag>)) {
						return std::nullopt;
					}
					break;
				case ID::SHORT:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<ShortTag>)) {
						return std::nullopt;
					}
					break;
				case ID::INT:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<IntTag>)) {
						return std::nullopt;
					}
					break;
				case ID::LONG:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<LongTag>)) {
						return std::nullopt;
					}
					break;
				case ID::FLOAT:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<FloatTag>)) {
						return std::nullopt;
					}
					break;
				case ID::DOUBLE:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<DoubleTag>)) {
						return std::nullopt;
					}
					break;
				case ID::BYTE_ARRAY:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<ByteArrayTag>)) {
						return std::nullopt;
					}
					break;
				case ID::STRING:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<StringTag>)) {
						return std::nullopt;
					}
					break;
				case ID::LIST: {
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<ListTag>)) {
						return std::nullopt;
					}
				}
				case ID::COMPOUND:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<CompoundTag>)) {
						return std::nullopt;
					}
					break;
				case ID::INT_ARRAY:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<IntArrayTag>)) {
						return std::nullopt;
					}
					break;
				case ID::LONG_ARRAY:
					if (!emplace(ct, std::move(*name), &NBTFile::readTag<LongArrayTag>)) {
						return std::nullopt;
					}
					break;
				default:
					return std::nullopt;
			}
		}
	}

	template <>
	std::optional<EndTag> readTag() {
		return EndTag{};
	}

	std::optional<CompoundTag> read() {
		if (readID().value_or(ID::END) != ID::COMPOUND) {
			return std::nullopt;
		}

		auto name = readString();
		if (!name.has_value()) {
			return std::nullopt;
		}
		auto tag = readTag<CompoundTag>();
		if (!tag.has_value()) {
			return std::nullopt;
		}

		CompoundTag ct{};
		ct.emplace(std::move(*name), std::move(*tag));
		return ct;
	}

private:
	std::span<const std::byte> data{};
	size_t pos = 0;
};
