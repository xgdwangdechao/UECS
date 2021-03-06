#pragma once

#include "EntityQuery.h"
#include "Entity.h"
#include "CmptsView.h"
#include "ChunkView.h"

#include <functional>

namespace Ubpa::UECS {
	// [description]
	// system function registered by Schedule in <System>::OnUpdate(Schedule&)
	// name + query + function<...>
	// name must be unique in global
	// query.filter can be change dynamically by other <System> with Schedule
	// [system function kind] (distinguish by argument list)
	// 1. per entity function
	// * [[const] World*]
	// * [[const] Entity]
	// * [size_t indexInQuery]
	// * [[const] CmptsView]
	// * <tagged-component>: {LastFrame|Write|Latest}<Component>
	// 2. chunk: [[const] World*], [const] ChunkView
	// 3. job: [[const] World*]
	class SystemFunc {
	public:
		enum class Mode {
			Entity,
			Chunk,
			Job,
		};

		EntityQuery query;
		
		template<typename Func>
		SystemFunc(Func&& func, std::string name, EntityFilter filter = EntityFilter{});

		// run-time dynamic function
		template<typename Func>
		SystemFunc(Func&& func, std::string name, EntityLocator locator, EntityFilter filter = EntityFilter{});
		
		const std::string& Name() const noexcept { return name; }

		static constexpr size_t HashCode(std::string_view name) { return hash_string(name); }

		size_t HashCode() const noexcept { return hashCode; }

		void operator()(World*, Entity e, size_t entityIndexInQuery, CmptsView rtdcmpts);
		void operator()(World*, ChunkView chunkView);
		void operator()(World*);

		Mode GetMode() const noexcept { return mode; }

		bool operator==(const SystemFunc& func) const noexcept { return name == func.name; }
	private:
		template<typename Func, typename ArgList>
		SystemFunc(Func&& func, std::string name, EntityFilter filter, ArgList);

		std::function<void(World*, Entity, size_t, CmptsView, ChunkView)> func;

		std::string name;
		Mode mode;
		size_t hashCode; // after name
	};
}

#include "detail/SystemFunc.inl"
