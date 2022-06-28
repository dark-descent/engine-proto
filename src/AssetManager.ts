import { SubSystem } from "./SubSystem";

export class AssetManager extends SubSystem<AssetManagerProps>
{
	private _paths: AssetPaths | null = null;

	public readonly getPath = (target: keyof AssetPaths): string => 
	{
		if(!this._paths)
			throw new Error(`AssetManager is not initialized yet!`);
		return this._paths[target];
	}

	public init(props: AssetManagerProps)
	{
		this._paths = {
			game: props.gamePath
		};

		console.log(this.getPath("game"));
	}
	
}

type AssetManagerProps = {
	gamePath: string;
};

type AssetPaths = {
	game: string;
};