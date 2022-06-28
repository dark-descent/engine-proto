import { SubSystemsConfig } from "../Engine";
import { Game } from "../Game";
import { Renderer } from "./Renderer";
import { Project } from "./Project";

export class EditorGame extends Game
{
	public readonly project: Project;

	protected get subSystemsConfig(): SubSystemsConfig<{ Renderer: Renderer }>
	{
		return {
			AssetManager: {
				gamePath: this.project.path
			},
			SceneManager: {
				test: ""
			},
			Renderer: {

			},
			ECS: {
				
			}
		}
	}

	public override get subSystemsOverrides()
	{
		return {
			Renderer
		};
	}

	public constructor(project: Project)
	{
		super("Editor");
		this.project = project;
	}
}