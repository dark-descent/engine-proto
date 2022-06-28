import { Engine } from "./Engine";

export class Scene
{
	public readonly engine: Engine;
	public readonly name: string
	public readonly data: SceneData;

	public constructor(engine: Engine, name: string, data: SceneData)
	{
		this.engine = engine;
		this.name = name;
		this.data = data;
	}
}

export type SceneData = {

};