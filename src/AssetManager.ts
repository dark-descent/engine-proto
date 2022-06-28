import { readFileSync } from "fs";
import path from "path";
import { SceneData } from "./Scene";
import { SubSystem } from "./SubSystem";

export class AssetManager extends SubSystem<AssetManagerProps>
{
	private _cachedAssets: { [path: string]: any } = {};

	private _paths: AssetPaths | null = null;
	private readonly _parsers: AssetParsers = {
		scene: (data) => JSON.parse(data),
	};

	public readonly getPath = <K extends keyof AssetPaths>(target: K): AssetPaths[K] => 
	{
		if (!this._paths)
			throw new Error(`AssetManager is not initialized yet!`);
		return this._paths[target];
	}

	public init(props: AssetManagerProps)
	{
		const pkgPath = path.resolve(props.gamePath, env.isEditor ? "project.json" : "game.json");

		const pkg: Package = JSON.parse(readFileSync(pkgPath, "utf-8"));

		if (props.parsers)
			Object.keys(props.parsers).forEach((key) => (this._parsers as any)[key] = (props.parsers as any)[key]);
		
		this._paths = {
			game: props.gamePath,
			scenes: { ...pkg.assets.scenes }
		};
	}

	public getAsset = <K extends keyof AssetDataMap>(target: K, name: string): AssetDataMap[K] =>
	{
		const paths = this.getPath(AssetType[target] as any);
		const gamePath = this.getPath("game");
		if (paths)
		{
			const p = paths[name];
			if (p)
			{
				if (!this._cachedAssets[p])
				{
					const parse = (this._parsers[target] || ((data: string) => data))!;

					this._cachedAssets[p] = parse(readFileSync(path.resolve(gamePath, p), "utf-8"));
				}
				return this._cachedAssets[p];
			}
		}
		throw new Error(`Could not get ${target} ${name}!`);
	}
}

export type AssetDataMap = {
	scene: SceneData;
};

export const AssetType =
{
	scene: "scenes"
};

type AssetManagerProps = {
	gamePath: string;
	parsers?: AssetParsers;
};

type AssetParsers = {
	[K in keyof typeof AssetType]?: (data: string) => any
};

type AssetPaths = {
	game: string;
	scenes: {
		[name: string]: string;
	};
};

type Package = {
	name: string;
	assets: {
		scenes: {
			[name: string]: string;
		}
	}
};