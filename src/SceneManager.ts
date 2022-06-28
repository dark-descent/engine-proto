import { AssetManager } from "./AssetManager";
import { Scene } from "./Scene";
import { SubSystem } from "./SubSystem";

@SubSystem.dependsOn(AssetManager)
export class SceneManager extends SubSystem<SceneProps>
{
	private readonly loadedScenes: { [name: string]: Scene } = {};

	public init(props: SceneProps) {  }

	public readonly loadScene = (name: string): Scene =>
	{
		if(!this.loadedScenes[name])
		{
			const sceneData = this.engine.getSubSystem(AssetManager).getAsset("scene", name);
			const scene = new Scene(this.engine, name, sceneData);
			this.loadedScenes[name] = scene;
		}
		return this.loadedScenes[name]!;
	}
}

export type SceneProps = {
	test: string;
};