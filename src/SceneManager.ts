import { AssetManager } from "./AssetManager";
import { SubSystem } from "./SubSystem";

@SubSystem.dependsOn(AssetManager)
export class SceneManager extends SubSystem<SceneProps>
{
	public init(props: SceneProps)
	{
		console.log("scene manager.init :D")
	}
}

export type SceneProps = {
	test: string;
};