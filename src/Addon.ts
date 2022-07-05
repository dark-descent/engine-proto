import type { EngineConfig } from "./Engine";
import { IRenderer } from "./System";

export namespace Addon
{
	export const module: AddonModule = __non_webpack_require__("./addon.node");
}

type AddonModule = {
	initialize: (config: Required<EngineConfig>) => EngineSystems;
};

export type EngineSystems = {
	Renderer: IRenderer;
};