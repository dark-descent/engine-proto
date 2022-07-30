import { Engine } from "./Engine";
import { Entity } from "./Entity";

export class Scene
{
	public readonly engine: Engine;
	public readonly name: string;
	private _sceneIndex: number;

	public constructor(name: string = "Entity", engine?: Engine, sceneIndex: number = -1)
	{
		if(!engine)
			engine = Engine.get();
		this.name = name
		this.engine = engine;
		this._sceneIndex = sceneIndex < 0 ? engine._addonEngine.addScene(name) : sceneIndex;
		console.log(this._sceneIndex);
	}

	public readonly addEntity = (name?: string): Entity => 
	{
		return new Entity(name, this, this.engine._addonEngine.addEntity(name));
	};
}