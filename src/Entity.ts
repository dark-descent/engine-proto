import { ComponentType } from "react"
import { Component } from "./Component"
import { CoreComponent } from "./CoreComponent"
import { Engine } from "./Engine"
import { Scene } from "./Scene";

export class Entity
{
	private readonly internalHandle_: number;
	public readonly scene: Scene;

	private readonly components: Component[] = [];

	public constructor(name?: string, scene?: Scene, ptr?: number)
	{
		const engine = Engine.get();

		if (!scene)
			scene = engine.activeScene;

		this.scene = scene;
		this.internalHandle_ = ptr || engine._addonEngine.addEntity(name, this.scene["_sceneIndex"]);
		console.log(this.internalHandle_);
	}

	public readonly addComponent = <T extends Component>(type: ComponentType<T>): T =>
	{
		
		return null as any
	}

	public readonly getComponent = <T extends Component>(type: ComponentType<T>) =>
	{
		
	}

	public readonly removeComponent = <T extends Component>(type: ComponentType<T>) =>
	{

	}
}