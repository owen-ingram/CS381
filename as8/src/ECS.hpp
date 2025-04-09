#ifndef ECS_HPP
#define ECS_HPP

#include <memory>
#include <concepts>
#include <vector>
#include <deque>
#include <iostream>
#include <ranges>
#include <bitset>
#include <span>
#include <variant>
#include <cassert>

extern size_t globalComponentCounter;

namespace cs381 {

	template<typename T>  // Template function for component ID retrieval
	size_t GetComponentID(T reference = {}) {  // Function to get component ID
		static size_t id = globalComponentCounter++;  // Static variable to ensure ID is unique
		return id;  // Return unique component ID
	}

	using Entity = uint8_t;  // Alias for entity type, used for representing entities as uint8_t

	// ComponentStorage structure handles storing components of entities
	struct ComponentStorage {
		size_t elementSize = -1;  // Element size for components
		std::vector<std::byte> data;  // Data vector to store components as bytes

		ComponentStorage() : elementSize(-1), data(1, std::byte{0}) {}  // Default constructor
		ComponentStorage(size_t elementSize) : elementSize(elementSize) { data.reserve(5 * elementSize); }  // Constructor with size

		template<typename Tcomponent>  // Constructor for specific component type
		ComponentStorage(Tcomponent reference = {}) : ComponentStorage(sizeof(Tcomponent)) {}

		template<typename Tcomponent>  // Function to retrieve a component for a specific entity
		Tcomponent& Get(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);  // Ensure element size matches
			assert(e < (data.size() / elementSize));  // Ensure entity index is within bounds
			return *(Tcomponent*)(data.data() + e * elementSize);  // Return the component for the entity
		}

		template<typename Tcomponent>  // Function to allocate memory for components
		std::pair<Tcomponent&, size_t> Allocate(size_t count = 1) {
			assert(sizeof(Tcomponent) == elementSize);  // Ensure element size matches
			assert(count < 100);  // Ensure count is reasonable
			auto originalEnd = data.size();  // Save the current size of data
			data.insert(data.end(), elementSize * count, std::byte{0});  // Insert space for components
			for(size_t i = 0; i < count - 1; i++)  // Initialize all but the last component
				new(data.data() + originalEnd + i * elementSize) Tcomponent();
			return {
				*new(data.data() + data.size() - elementSize) Tcomponent(),  // Construct the last component
				data.size() / elementSize  // Return the index of the newly allocated component
			};
		}

		template<typename Tcomponent>  // Get or allocate a component for an entity
		Tcomponent& GetOrAllocate(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);  // Ensure element size matches
			size_t size = data.size() / elementSize;  // Get current number of components
			if (size <= e)  // If entity index is out of bounds, allocate more components
				Allocate<Tcomponent>(std::max<int64_t>(int64_t(e) - size + 1, 1));
			return Get<Tcomponent>(e);  // Return the component
		}
	};

	// Scene structure manages entities and their components
	template<typename Storage = ComponentStorage>  // Default to using ComponentStorage for the component data
	struct Scene {
		std::vector<std::vector<bool>> entityMasks;  // Masks to track components for each entity
		std::vector<Storage> storages = {Storage()};  // Vector of component storages

		template<typename Tcomponent>  // Get the storage for a specific component
		Storage& GetStorage() {
			size_t id = GetComponentID<Tcomponent>();  // Get the component ID
			if(storages.size() <= id)  // If storage is not large enough, add more
				storages.insert(storages.cend(), std::max<int64_t>(id - storages.size(), 1), Storage());
			if (storages[id].elementSize == std::numeric_limits<size_t>::max())  // If element size is uninitialized, initialize it
				storages[id] = Storage(Tcomponent{});
			return storages[id];  // Return the storage for the component
		}

		Entity CreateEntity() {  // Create a new entity and return its ID
			Entity e = entityMasks.size();  // Use the size of entityMasks as the entity ID
			entityMasks.emplace_back(std::vector<bool>{false});  // Add a new mask for the entity
			return e;  // Return the new entity ID
		}

		template<typename Tcomponent>  // Add a component to an entity
		Tcomponent& AddComponent(Entity e) {
			size_t id = GetComponentID<Tcomponent>();  // Get component ID
			auto& eMask = entityMasks[e];  // Get the entity's mask
			if(eMask.size() <= id)  // If the mask is too small, resize it
				eMask.resize(id + 1, false);
			eMask[id] = true;  // Set the component bit in the mask
			return GetStorage<Tcomponent>().template GetOrAllocate<Tcomponent>(e);  // Return the component
		}

		template<typename Tcomponent>  // Remove a component from an entity
		void RemoveComponent(Entity e) {
			size_t id = GetComponentID<Tcomponent>();  // Get component ID
			auto& eMask = entityMasks[e];  // Get the entity's mask
			if(eMask.size() > id)  // If the component exists, remove it from the mask
				eMask[id] = false;
		}

		template<typename Tcomponent>  // Get a component from an entity
		Tcomponent& GetComponent(Entity e) {
			size_t id = GetComponentID<Tcomponent>();  // Get component ID
			assert(entityMasks[e][id]);  // Ensure the component exists on the entity
			return GetStorage<Tcomponent>().template Get<Tcomponent>(e);  // Return the component
		}

		template<typename Tcomponent>  // Check if an entity has a specific component
		bool HasComponent(Entity e) {
			size_t id = GetComponentID<Tcomponent>();  // Get component ID
			return entityMasks[e].size() > id && entityMasks[e][id];  // Check if component bit is set in the mask
		}
	};

	// SkiplistComponentStorage is an alternative storage for components that uses a skiplist for indexing
	struct SkiplistComponentStorage {
		size_t elementSize = -1;  // Size of each element (component)
		std::vector<size_t> indecies;  // Vector of indices for component locations
		std::vector<std::byte> data;  // Vector for component data

		SkiplistComponentStorage() : elementSize(-1), indecies(1, -1), data(1, std::byte{0}) {}  // Default constructor
		SkiplistComponentStorage(size_t elementSize) : elementSize(elementSize) { data.reserve(5 * elementSize); }  // Constructor with size

		template<typename Tcomponent>  // Constructor for specific component type
		SkiplistComponentStorage(Tcomponent reference = {}) : SkiplistComponentStorage(sizeof(Tcomponent)) {}

		template<typename Tcomponent>  // Retrieve a component from the skiplist storage
		Tcomponent& Get(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);  // Ensure element size matches
			assert(e < indecies.size());  // Ensure entity index is within bounds
			assert(indecies[e] != std::numeric_limits<size_t>::max());  // Ensure index is valid
			return *(Tcomponent*)(data.data() + indecies[e]);  // Return the component
		}

		template<typename Tcomponent>  // Allocate memory for a new component
		std::pair<Tcomponent&, size_t> Allocate() {
			assert(sizeof(Tcomponent) == elementSize);  // Ensure element size matches
			data.insert(data.end(), elementSize, std::byte{0});  // Insert space for the component
			return {
				*new(data.data() + data.size() - elementSize) Tcomponent(),  // Construct a new component
				(data.size() - 1) / elementSize  // Return the index of the new component
			};
		}

		template<typename Tcomponent>  // Allocate a component at a specific entity's index
		Tcomponent& Allocate(Entity e) {
			auto [ret, i] = Allocate<Tcomponent>();  // Allocate the component
			indecies[e] = i * elementSize;  // Store the index for the entity
			return ret;  // Return the component
		}

		template<typename Tcomponent>  // Get or allocate a component for an entity
		Tcomponent& GetOrAllocate(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);  // Ensure element size matches
			if (indecies.size() <= e)  // If entity index is out of bounds, allocate more
				indecies.insert(indecies.end(), std::max<int64_t>(int64_t(e) - indecies.size() + 1, 1), -1);
			if (indecies[e] == std::numeric_limits<size_t>::max())  // If component not allocated, allocate it
				return Allocate<Tcomponent>(e);
			return Get<Tcomponent>(e);  // Return the existing component
		}
	};

	using post_increment_t = int;  // Alias for post-increment type used in iterators

	// SceneView is a view into the scene for iterating over entities with specific components
	template<typename... Tcomponents>  // Template for multiple component types
	struct SceneView {
		Scene<SkiplistComponentStorage>& scene;  // Reference to the scene

		struct Sentinel {};  // Sentinel type to mark the end of an iterator
		struct Iterator {  // Iterator type for iterating over entities
			Scene<SkiplistComponentStorage>* scene = nullptr;  // Pointer to the scene
			Entity e;  // Current entity

			bool valid() { return (scene->HasComponent<Tcomponents>(e) && ...); }  // Check if entity has all required components

			bool operator==(Sentinel) { return scene == nullptr || e >= scene->entityMasks.size(); }  // Check if iterator reached the end

			Iterator& operator++(post_increment_t) {  // Post-increment operator for iterator
				do {
					e++;  // Move to the next entity
				} while(!valid() && e < scene->entityMasks.size());  // Skip invalid entities
				return *this;
			}

			Iterator operator++() {  // Pre-increment operator for iterator
				Iterator old;
				operator++(0);  // Call post-increment
				return old;  // Return the old iterator
			}

			// std::tuple<std::add_lvalue_reference_t<Tcomponents>...> operator*() { return { scene->GetComponent<Tcomponents>(e)... }; }  // Access components of entity
			std::tuple<std::add_lvalue_reference_t<Tcomponents>...> operator*() { return { scene->GetComponent<Tcomponents>(e)... }; }  // Dereference iterator to get components
		};

		Iterator begin() {  // Get the iterator for the beginning of the view
			Iterator out{&scene, 0};  // Create iterator starting at entity 0
			if(!out.valid()) ++out;  // Skip invalid entities
			return out;  // Return iterator
		}
		Sentinel end() { return {}; }  // Return the sentinel for the end of the view
	};
}

#endif // ECS_HPP
