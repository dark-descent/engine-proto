import { Component, ComponentType } from "./Component"
import { Transform } from "./components";
import { CoreComponent } from "./CoreComponent"
import { Engine } from "./Engine"
import { Scene } from "./Scene";

export class Entity
{
	private readonly internalHandle_: number;
	public readonly scene: Scene;

	public readonly engine: Engine;

	private readonly components: Component[] = [];

	public constructor(name?: string, scene?: Scene, ptr?: number)
	{
		this.engine = Engine.get();
		if (!scene)
			scene = this.engine.activeScene;

		this.scene = scene;
		this.internalHandle_ = ptr || this.engine.api.addEntity(name, this.scene["_sceneIndex"]);
		this.addComponent(Transform);
	}

	public readonly addComponent = <T extends Component>(type: ComponentType<T>): T =>
	{
		console.log(type);
		// this.engine.api.addComponentToEntity(this.internalHandle_, );
		return null as any
	}

	public readonly getComponent = <T extends Component>(type: ComponentType<T>) =>
	{
		
	}

	public readonly removeComponent = <T extends Component>(type: ComponentType<T>) =>
	{

	}
}