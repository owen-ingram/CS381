#pragma once

#define RAYLIB_SUPPORT_OIS
#include <raylib-cpp.hpp>

#include <memory>
#include <vector>
#include <optional>

namespace cs381 {

	/**
	 * @brief Base class for various components that can be attached to an entity.
	 */
	class Component {
		struct Entity* object;
		friend struct Entity;

	public:
		struct Entity& Object() { return *object; }
		struct TransformComponent& Transform();
		bool enabled = true;

		Component(struct Entity& e) : object(&e) {}
		Component(struct Entity& e, bool enabled): object(&e), enabled(enabled) {} // Start
		virtual void Tick(float dt) {}
		virtual ~Component() {} // Cleanup

		template<std::derived_from<Component> T>
		T& as() { return *dynamic_cast<T*>(this); }
	};

	/**
	 * @brief Component storing the positional data of a Entity
	 */
	struct TransformComponent: public Component {
		using Component::Component;
		raylib::Vector3 position = {0, 0, 0};
		raylib::Degree heading = 0;
	};


	/**
	 * @brief Represents an object in the game world.
	 */
	struct Entity {
		std::vector<std::unique_ptr<Component>> components = {};

		Entity() { AddComponent<TransformComponent>(); } // Entities have a transform by default!
		Entity(const Entity&) = delete;
		Entity(Entity&& other) : components(std::move(other.components)) {
			for(auto& component: components)
				component->object = this; // When we are moved make sure the components still point to us!
		}

		Entity& operator=(const Entity&) = delete;
		Entity& operator=(Entity&& other) {
			components = std::move(other.components);
			for(auto& component: components)
				component->object = this; // When we are moved make sure the components still point to us!
			return *this;
		}

		/**
		 * @brief Performs actions during each game update for the entity and its components.
		 * @param dt The time elapsed since the last update.
		 */
		void Tick(float dt) {
			for(auto& componentPtr: components)
				if(componentPtr->enabled)
					componentPtr->Tick(dt);
		}

		/**
		 * @brief Adds a new component of type T to the entity.
		 * @tparam T The type of component to add.
		 * @tparam Ts The types of arguments to pass to the component's constructor.
		 * @param args The arguments to pass to the component's constructor.
		 * @return The index of the added component.
		 */
		template<std::derived_from<Component> T, typename... Ts>
		size_t AddComponent(Ts... args) {
			std::unique_ptr<Component> component = std::make_unique<T>(*this, std::forward<Ts>(args) ...);
			components.push_back(std::move(component));
			return components.size() - 1;
		}

		// How could we remove a component?

		/**
		 * @brief Retrieves a component of type T from the entity.
		 * @tparam T The type of component to retrieve.
		 * @return An optional reference to the component if found, or std::nullopt if not found.
		 */
		template<std::derived_from<Component> T>
		std::optional<std::reference_wrapper<T>> GetComponent() {
			if constexpr(std::is_same_v<T, TransformComponent>){ // This extra step here allows us to skip itterator overhead since we know the transform component should be in slot 0
				T* cast = dynamic_cast<T*>(components[0].get());
				if(cast) return *cast;
			}

			for(auto& componentPtr: components){
				T* cast = dynamic_cast<T*>(componentPtr.get());
				if(cast) return *cast;
			}

			return {};
		}

		/**
		 * @brief Retrieves this object's transform...
		 * @return The object's transform component.
		 * @note this function has undefined behavior (probably a crash...) if the transform component is ever removed!
		 */
		TransformComponent& Transform() { return *GetComponent<TransformComponent>(); }
	};

	/**
	 * @brief Syntax sugar function that provides access to the object's transform component
	 * @return The object's transform component.
	 * @note this function has undefined behavior (probably a crash...) if the transform component is ever removed!
	 */
	TransformComponent& Component::Transform() { return Object().Transform(); }
}