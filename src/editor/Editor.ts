import { Engine } from "../Engine";
import { EditorGame } from "./EditorGame";
import { Project, ProjectProps } from "./Project";

export class Editor
{
	private static _editor: Editor | null = null;

	public static initialize = () =>
	{
		if (this._editor)
			throw new Error(`Editor is already initialized!`);
		this._editor = new Editor();
		return this._editor;
	}

	private _editorEngine: Engine | null = null;
	private _project: Project | null = null;

	public get engine(): Engine
	{
		if (!this._editorEngine)
			throw new Error(`Editor Engine is not initialized yet!`);
		return this._editorEngine;
	}

	public constructor()
	{

	}

	public readonly loadProject = async (projectProps: ProjectProps) =>
	{
		this._project = new Project(projectProps);
		const editorGame = new EditorGame(this._project);
		await editorGame.initialize();
		this._editorEngine = editorGame.engine;
		return this._project;

	}

	public readonly buildGame = () =>
	{
		// return new Game(this.project.name);
	}
}