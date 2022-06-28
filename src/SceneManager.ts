import { AssetManager } from "./AssetManager";
import { SubSystem, SubSystemInitProps } from "./SubSystem";

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

type A = SubSystemInitProps<SceneManager>;