import type { EngineConfig } from "./Engine";
import { Scene } from "./Scene";

const isWorker = !("window" in self && "document" in self);

export namespace Addon
{
	export const module: AddonModule = __non_webpack_require__(isWorker ? "../addon.node" : "./addon.node");
}

type AddonModule = {
	initialize: (config: Required<EngineConfig>) => AddonEngine;
	initializeWorker: () => EngineWorkerSystem;
};

export type AddonEngine = {
	components: ComponentInfo[];
	addEntity: (name?: string, scene?: number) => number;
	addScene: (name: string) => number;
	removeScene: (name: string) => void;
	getScene: (name?: string) => Scene | null; // TODO:
	loadEditorScene: (name: string) => number;
};

type ComponentInfo = {
	name: string;
	index: number;
};

export type EngineWorkerSystem = {
	
};