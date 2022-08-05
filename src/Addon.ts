import type { EngineConfig } from "./Engine";

const isWorker = !("window" in self && "document" in self);

export const EngineModule: EngineModule = __non_webpack_require__(isWorker ? "../addon.node" : "./addon.node");

type EngineModule = {
	initialize: (config: Required<EngineConfig>) => AddonEngine;
	initializeWorker: () => EngineWorkerSystem;
};

export type AddonEngine = {
	objects: {
		Entity: typeof Entity;
		Transform: typeof Transform;
		RigidBody: typeof RigidBody;
		BoxCollider: typeof BoxCollider;
		CircleCollider: typeof CircleCollider;
	};
};

export type EngineWorkerSystem = {

};

declare class Entity
{

}

declare class Transform
{

}

declare class BoxCollider
{

}

declare class CircleCollider
{

}

declare class RigidBody
{

}
