#pragma once

#include <cstdint>

namespace infinity{
class Worker{
public:
	Worker(
		std::uint16_t id,
		std::uint16_t cpu_id);

	~Worker() noexcept = default;

	void Execute();

	[[nodiscard]] std::uint16_t CPUID() const noexcept { 
		return cpu_id_; 
	}
private:
	const std::uint16_t id_;
	const std::uint16_t cpu_id_;
};
}